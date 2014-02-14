# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

from io import BytesIO
import struct
import zlib
import os
from zipfile import (
    ZIP_STORED,
    ZIP_DEFLATED,
)
from collections import OrderedDict
from urlparse import urlparse, ParseResult
import mozpack.path

JAR_STORED = ZIP_STORED
JAR_DEFLATED = ZIP_DEFLATED
MAX_WBITS = 15


class JarReaderError(Exception):
    '''Error type for Jar reader errors.'''


class JarWriterError(Exception):
    '''Error type for Jar writer errors.'''


class JarStruct(object):
    '''
    Helper used to define ZIP archive raw data structures. Data structures
    handled by this helper all start with a magic number, defined in
    subclasses MAGIC field as a 32-bits unsigned integer, followed by data
    structured as described in subclasses STRUCT field.

    The STRUCT field contains a list of (name, type) pairs where name is a
    field name, and the type can be one of 'uint32', 'uint16' or one of the
    field names. In the latter case, the field is considered to be a string
    buffer with a length given in that field.
    For example,
        STRUCT = [
            ('version', 'uint32'),
            ('filename_size', 'uint16'),
            ('filename', 'filename_size')
        ]
    describes a structure with a 'version' 32-bits unsigned integer field,
    followed by a 'filename_size' 16-bits unsigned integer field, followed by a
    filename_size-long string buffer 'filename'.

    Fields that are used as other fields size are not stored in objects. In the
    above example, an instance of such subclass would only have two attributes:
        obj['version']
        obj['filename']
    filename_size would be obtained with len(obj['filename']).

    JarStruct subclasses instances can be either initialized from existing data
    (deserialized), or with empty fields.
    '''

    TYPE_MAPPING = {'uint32': ('I', 4), 'uint16': ('H', 2)}

    def __init__(self, data=None):
        '''
        Create an instance from the given data. Data may be omitted to create
        an instance with empty fields.
        '''
        assert self.MAGIC and isinstance(self.STRUCT, OrderedDict)
        self.size_fields = set(t for t in self.STRUCT.itervalues()
                               if not t in JarStruct.TYPE_MAPPING)
        self._values = {}
        if data:
            self._init_data(data)
        else:
            self._init_empty()

    def _init_data(self, data):
        '''
        Initialize an instance from data, following the data structure
        described in self.STRUCT. The self.MAGIC signature is expected at
        data[:4].
        '''
        assert data is not None
        self.signature, size = JarStruct.get_data('uint32', data)
        if self.signature != self.MAGIC:
            raise JarReaderError('Bad magic')
        offset = size
        # For all fields used as other fields sizes, keep track of their value
        # separately.
        sizes = dict((t, 0) for t in self.size_fields)
        for name, t in self.STRUCT.iteritems():
            if t in JarStruct.TYPE_MAPPING:
                value, size = JarStruct.get_data(t, data[offset:])
            else:
                size = sizes[t]
                value = data[offset:offset + size]
                if isinstance(value, memoryview):
                    value = value.tobytes()
            if not name in sizes:
                self._values[name] = value
            else:
                sizes[name] = value
            offset += size

    def _init_empty(self):
        '''
        Initialize an instance with empty fields.
        '''
        self.signature = self.MAGIC
        for name, t in self.STRUCT.iteritems():
            if name in self.size_fields:
                continue
            self._values[name] = 0 if t in JarStruct.TYPE_MAPPING else ''

    @staticmethod
    def get_data(type, data):
        '''
        Deserialize a single field of given type (must be one of
        JarStruct.TYPE_MAPPING) at the given offset in the given data.
        '''
        assert type in JarStruct.TYPE_MAPPING
        assert data is not None
        format, size = JarStruct.TYPE_MAPPING[type]
        data = data[:size]
        if isinstance(data, memoryview):
            data = data.tobytes()
        return struct.unpack('<' + format, data)[0], size

    def serialize(self):
        '''
        Serialize the data structure according to the data structure definition
        from self.STRUCT.
        '''
        serialized = struct.pack('<I', self.signature)
        sizes = dict((t, name) for name, t in self.STRUCT.iteritems()
                     if not t in JarStruct.TYPE_MAPPING)
        for name, t in self.STRUCT.iteritems():
            if t in JarStruct.TYPE_MAPPING:
                format, size = JarStruct.TYPE_MAPPING[t]
                if name in sizes:
                    value = len(self[sizes[name]])
                else:
                    value = self[name]
                serialized += struct.pack('<' + format, value)
            else:
                serialized += self[name]
        return serialized

    @property
    def size(self):
        '''
        Return the size of the data structure, given the current values of all
        variable length fields.
        '''
        size = JarStruct.TYPE_MAPPING['uint32'][1]
        for name, type in self.STRUCT.iteritems():
            if type in JarStruct.TYPE_MAPPING:
                size += JarStruct.TYPE_MAPPING[type][1]
            else:
                size += len(self[name])
        return size

    def __getitem__(self, key):
        return self._values[key]

    def __setitem__(self, key, value):
        if not key in self.STRUCT:
            raise KeyError(key)
        if key in self.size_fields:
            raise AttributeError("can't set attribute")
        self._values[key] = value

    def __contains__(self, key):
        return key in self._values

    def __iter__(self):
        return self._values.iteritems()

    def __repr__(self):
        return "<%s %s>" % (self.__class__.__name__,
                            ' '.join('%s=%s' % (n, v) for n, v in self))


class JarCdirEnd(JarStruct):
    '''
    End of central directory record.
    '''
    MAGIC = 0x06054b50
    STRUCT = OrderedDict([
        ('disk_num', 'uint16'),
        ('cdir_disk', 'uint16'),
        ('disk_entries', 'uint16'),
        ('cdir_entries', 'uint16'),
        ('cdir_size', 'uint32'),
        ('cdir_offset', 'uint32'),
        ('comment_size', 'uint16'),
        ('comment', 'comment_size'),
    ])

CDIR_END_SIZE = JarCdirEnd().size


class JarCdirEntry(JarStruct):
    '''
    Central directory file header
    '''
    MAGIC = 0x02014b50
    STRUCT = OrderedDict([
        ('creator_version', 'uint16'),
        ('min_version', 'uint16'),
        ('general_flag', 'uint16'),
        ('compression', 'uint16'),
        ('lastmod_time', 'uint16'),
        ('lastmod_date', 'uint16'),
        ('crc32', 'uint32'),
        ('compressed_size', 'uint32'),
        ('uncompressed_size', 'uint32'),
        ('filename_size', 'uint16'),
        ('extrafield_size', 'uint16'),
        ('filecomment_size', 'uint16'),
        ('disknum', 'uint16'),
        ('internal_attr', 'uint16'),
        ('external_attr', 'uint32'),
        ('offset', 'uint32'),
        ('filename', 'filename_size'),
        ('extrafield', 'extrafield_size'),
        ('filecomment', 'filecomment_size'),
    ])


class JarLocalFileHeader(JarStruct):
    '''
    Local file header
    '''
    MAGIC = 0x04034b50
    STRUCT = OrderedDict([
        ('min_version', 'uint16'),
        ('general_flag', 'uint16'),
        ('compression', 'uint16'),
        ('lastmod_time', 'uint16'),
        ('lastmod_date', 'uint16'),
        ('crc32', 'uint32'),
        ('compressed_size', 'uint32'),
        ('uncompressed_size', 'uint32'),
        ('filename_size', 'uint16'),
        ('extra_field_size', 'uint16'),
        ('filename', 'filename_size'),
        ('extra_field', 'extra_field_size'),
    ])


class JarFileReader(object):
    '''
    File-like class for use by JarReader to give access to individual files
    within a Jar archive.
    '''
    def __init__(self, header, data):
        '''
        Initialize a JarFileReader. header is the local file header
        corresponding to the file in the jar archive, data a buffer containing
        the file data.
        '''
        assert header['compression'] in [JAR_DEFLATED, JAR_STORED]
        self._data = data
        # Copy some local file header fields.
        for name in ['filename', 'compressed_size',
                     'uncompressed_size', 'crc32']:
            setattr(self, name, header[name])
        self.compressed = header['compression'] == JAR_DEFLATED

    def read(self, length=-1):
        '''
        Read some amount of uncompressed data.
        '''
        return self.uncompressed_data.read(length)

    def readlines(self):
        '''
        Return a list containing all the lines of data in the uncompressed
        data.
        '''
        return self.read().splitlines(True)

    def __iter__(self):
        '''
        Iterator, to support the "for line in fileobj" constructs.
        '''
        return iter(self.readlines())

    def seek(self, pos, whence=os.SEEK_SET):
        '''
        Change the current position in the uncompressed data. Subsequent reads
        will start from there.
        '''
        return self.uncompressed_data.seek(pos, whence)

    def close(self):
        '''
        Free the uncompressed data buffer.
        '''
        self.uncompressed_data.close()

    @property
    def compressed_data(self):
        '''
        Return the raw compressed data.
        '''
        return self._data[:self.compressed_size]

    @property
    def uncompressed_data(self):
        '''
        Return the uncompressed data.
        '''
        if hasattr(self, '_uncompressed_data'):
            return self._uncompressed_data
        data = self.compressed_data
        if self.compressed:
            data = zlib.decompress(data.tobytes(), -MAX_WBITS)
        else:
            data = data.tobytes()
        if len(data) != self.uncompressed_size:
            raise JarReaderError('Corrupted file? %s' % self.filename)
        self._uncompressed_data = BytesIO(data)
        return self._uncompressed_data


class JarReader(object):
    '''
    Class with methods to read Jar files. Can open standard jar files as well
    as Mozilla jar files (see further details in the JarWriter documentation).
    '''
    def __init__(self, file=None, fileobj=None):
        '''
        Opens the given file as a Jar archive. Use the given file-like object
        if one is given instead of opening the given file name.
        '''
        if fileobj:
            data = fileobj.read()
        else:
            data = open(file, 'rb').read()
        self._data = memoryview(data)
        # The End of Central Directory Record has a variable size because of
        # comments it may contain, so scan for it from the end of the file.
        offset = -CDIR_END_SIZE
        while True:
            signature = JarStruct.get_data('uint32', self._data[offset:])[0]
            if signature == JarCdirEnd.MAGIC:
                break
            if offset == -len(self._data):
                raise JarReaderError('Not a jar?')
            offset -= 1
        self._cdir_end = JarCdirEnd(self._data[offset:])

    def close(self):
        '''
        Free some resources associated with the Jar.
        '''
        del self._data

    @property
    def entries(self):
        '''
        Return an ordered dict of central directory entries, indexed by
        filename, in the order they appear in the Jar archive central
        directory. Directory entries are skipped.
        '''
        if hasattr(self, '_entries'):
            return self._entries
        preload = 0
        if self.is_optimized:
            preload = JarStruct.get_data('uint32', self._data)[0]
        entries = OrderedDict()
        offset = self._cdir_end['cdir_offset']
        for e in xrange(self._cdir_end['cdir_entries']):
            entry = JarCdirEntry(self._data[offset:])
            offset += entry.size
            # Creator host system. 0 is MSDOS, 3 is Unix
            host = entry['creator_version'] >> 8
            # External attributes values depend on host above. On Unix the
            # higher bits are the stat.st_mode value. On MSDOS, the lower bits
            # are the FAT attributes.
            xattr = entry['external_attr']
            # Skip directories
            if (host == 0 and xattr & 0x10) or (host == 3 and
                                                xattr & (040000 << 16)):
                continue
            entries[entry['filename']] = entry
            if entry['offset'] < preload:
                self._last_preloaded = entry['filename']
        self._entries = entries
        return entries

    @property
    def is_optimized(self):
        '''
        Return whether the jar archive is optimized.
        '''
        # In optimized jars, the central directory is at the beginning of the
        # file, after a single 32-bits value, which is the length of data
        # preloaded.
        return self._cdir_end['cdir_offset'] == \
            JarStruct.TYPE_MAPPING['uint32'][1]

    @property
    def last_preloaded(self):
        '''
        Return the name of the last file that is set to be preloaded.
        See JarWriter documentation for more details on preloading.
        '''
        if hasattr(self, '_last_preloaded'):
            return self._last_preloaded
        self._last_preloaded = None
        self.entries
        return self._last_preloaded

    def _getreader(self, entry):
        '''
        Helper to create a JarFileReader corresponding to the given central
        directory entry.
        '''
        header = JarLocalFileHeader(self._data[entry['offset']:])
        for key, value in entry:
            if key in header and header[key] != value:
                raise JarReaderError('Central directory and file header ' +
                                     'mismatch. Corrupted archive?')
        return JarFileReader(header,
                             self._data[entry['offset'] + header.size:])

    def __iter__(self):
        '''
        Iterate over all files in the Jar archive, in the form of
        JarFileReaders.
            for file in jarReader:
                ...
        '''
        for entry in self.entries.itervalues():
            yield self._getreader(entry)

    def __getitem__(self, name):
        '''
        Get a JarFileReader for the given file name.
        '''
        return self._getreader(self.entries[name])

    def __contains__(self, name):
        '''
        Return whether the given file name appears in the Jar archive.
        '''
        return name in self.entries


class JarWriter(object):
    '''
    Class with methods to write Jar files. Can write more-or-less standard jar
    archives as well as jar archives optimized for Gecko. See the documentation
    for the close() member function for a description of both layouts.
    '''
    def __init__(self, file=None, fileobj=None, compress=True, optimize=True):
        '''
        Initialize a Jar archive in the given file. Use the given file-like
        object if one is given instead of opening the given file name.
        The compress option determines the default behavior for storing data
        in the jar archive. The optimize options determines whether the jar
        archive should be optimized for Gecko or not.
        '''
        if fileobj:
            self._data = fileobj
        else:
            self._data = open(file, 'wb')
        self._compress = compress
        self._contents = OrderedDict()
        self._last_preloaded = None
        self._optimize = optimize

    def __enter__(self):
        '''
        Context manager __enter__ method for JarWriter.
        '''
        return self

    def __exit__(self, type, value, tb):
        '''
        Context manager __exit__ method for JarWriter.
        '''
        self.finish()

    def finish(self):
        '''
        Flush and close the Jar archive.

        Standard jar archives are laid out like the following:
            - Local file header 1
            - File data 1
            - Local file header 2
            - File data 2
            - (...)
            - Central directory entry pointing at Local file header 1
            - Central directory entry pointing at Local file header 2
            - (...)
            - End of central directory, pointing at first central directory
              entry.

        Jar archives optimized for Gecko are laid out like the following:
            - 32-bits unsigned integer giving the amount of data to preload.
            - Central directory entry pointing at Local file header 1
            - Central directory entry pointing at Local file header 2
            - (...)
            - End of central directory, pointing at first central directory
              entry.
            - Local file header 1
            - File data 1
            - Local file header 2
            - File data 2
            - (...)
            - End of central directory, pointing at first central directory
              entry.
        The duplication of the End of central directory is to accomodate some
        Zip reading tools that want an end of central directory structure to
        follow the central directory entries.
        '''
        offset = 0
        headers = {}
        preload_size = 0
        # Prepare central directory entries
        for entry, content in self._contents.itervalues():
            header = JarLocalFileHeader()
            for name in entry.STRUCT:
                if name in header:
                    header[name] = entry[name]
            entry['offset'] = offset
            offset += len(content) + header.size
            if entry['filename'] == self._last_preloaded:
                preload_size = offset
            headers[entry] = header
        # Prepare end of central directory
        end = JarCdirEnd()
        end['disk_entries'] = len(self._contents)
        end['cdir_entries'] = end['disk_entries']
        end['cdir_size'] = reduce(lambda x, y: x + y[0].size,
                                  self._contents.values(), 0)
        # On optimized archives, store the preloaded size and the central
        # directory entries, followed by the first end of central directory.
        if self._optimize:
            end['cdir_offset'] = 4
            offset = end['cdir_size'] + end['cdir_offset'] + end.size
            if preload_size:
                preload_size += offset
            self._data.write(struct.pack('<I', preload_size))
            for entry, _ in self._contents.itervalues():
                entry['offset'] += offset
                self._data.write(entry.serialize())
            self._data.write(end.serialize())
        # Store local file entries followed by compressed data
        for entry, content in self._contents.itervalues():
            self._data.write(headers[entry].serialize())
            self._data.write(content)
        # On non optimized archives, store the central directory entries.
        if not self._optimize:
            end['cdir_offset'] = offset
            for entry, _ in self._contents.itervalues():
                self._data.write(entry.serialize())
        # Store the end of central directory.
        self._data.write(end.serialize())
        self._data.close()

    def add(self, name, data, compress=None, mode=None):
        '''
        Add a new member to the jar archive, with the given name and the given
        data.
        The compress option indicates if the given data should be compressed
        (True), not compressed (False), or compressed according to the default
        defined when creating the JarWriter (None).
        When the data should be compressed (True or None with self.compress ==
        True), it is only really compressed if the compressed size is smaller
        than the uncompressed size.
        The mode option gives the unix permissions that should be stored
        for the jar entry.
        The given data may be a buffer, a file-like instance, a Deflater or a
        JarFileReader instance. The latter two allow to avoid uncompressing
        data to recompress it.
        '''
        if name in self._contents:
            raise JarWriterError("File %s already in JarWriter" % name)
        if compress is None:
            compress = self._compress
        if (isinstance(data, JarFileReader) and data.compressed == compress) \
                or (isinstance(data, Deflater) and data.compress == compress):
            deflater = data
        else:
            deflater = Deflater(compress)
            if isinstance(data, basestring):
                deflater.write(data)
            elif hasattr(data, 'read'):
                data.seek(0)
                deflater.write(data.read())
            else:
                raise JarWriterError("Don't know how to handle %s" %
                                     type(data))
        # Fill a central directory entry for this new member.
        entry = JarCdirEntry()
        entry['creator_version'] = 20
        if mode is not None:
            # Set creator host system (upper byte of creator_version)
            # to 3 (Unix) so mode is honored when there is one.
            entry['creator_version'] |= 3 << 8
            entry['external_attr'] = (mode & 0xFFFF) << 16L
        if deflater.compressed:
            entry['min_version'] = 20  # Version 2.0 supports deflated streams
            entry['general_flag'] = 2  # Max compression
            entry['compression'] = JAR_DEFLATED
        else:
            entry['min_version'] = 10  # Version 1.0 for stored streams
            entry['general_flag'] = 0
            entry['compression'] = JAR_STORED
        # January 1st, 2010. See bug 592369.
        entry['lastmod_date'] = ((2010 - 1980) << 9) | (1 << 5) | 1
        entry['lastmod_time'] = 0
        entry['crc32'] = deflater.crc32
        entry['compressed_size'] = deflater.compressed_size
        entry['uncompressed_size'] = deflater.uncompressed_size
        entry['filename'] = name
        self._contents[name] = entry, deflater.compressed_data

    def preload(self, files):
        '''
        Set which members of the jar archive should be preloaded when opening
        the archive in Gecko. This reorders the members according to the order
        of given list.
        '''
        new_contents = OrderedDict()
        for f in files:
            if not f in self._contents:
                continue
            new_contents[f] = self._contents[f]
            self._last_preloaded = f
        for f in self._contents:
            if not f in new_contents:
                new_contents[f] = self._contents[f]
        self._contents = new_contents


class Deflater(object):
    '''
    File-like interface to zlib compression. The data is actually not
    compressed unless the compressed form is smaller than the uncompressed
    data.
    '''
    def __init__(self, compress=True):
        '''
        Initialize a Deflater. The compress argument determines whether to
        try to compress at all.
        '''
        self._data = BytesIO()
        self.compress = compress
        if compress:
            self._deflater = zlib.compressobj(9, zlib.DEFLATED, -MAX_WBITS)
            self._deflated = BytesIO()
        else:
            self._deflater = None

    def write(self, data):
        '''
        Append a buffer to the Deflater.
        '''
        self._data.write(data)
        if self.compress:
            if self._deflater:
                if isinstance(data, memoryview):
                    data = data.tobytes()
                self._deflated.write(self._deflater.compress(data))
            else:
                raise JarWriterError("Can't write after flush")

    def close(self):
        '''
        Close the Deflater.
        '''
        self._data.close()
        if self.compress:
            self._deflated.close()

    def _flush(self):
        '''
        Flush the underlying zlib compression object.
        '''
        if self.compress and self._deflater:
            self._deflated.write(self._deflater.flush())
            self._deflater = None

    @property
    def compressed(self):
        '''
        Return whether the data should be compressed.
        '''
        return self._compressed_size < self.uncompressed_size

    @property
    def _compressed_size(self):
        '''
        Return the real compressed size of the data written to the Deflater. If
        the Deflater is set not to compress, the uncompressed size is returned.
        Otherwise, the actual compressed size is returned, whether or not it is
        a win over the uncompressed size.
        '''
        if self.compress:
            self._flush()
            return self._deflated.tell()
        return self.uncompressed_size

    @property
    def compressed_size(self):
        '''
        Return the compressed size of the data written to the Deflater. If the
        Deflater is set not to compress, the uncompressed size is returned.
        Otherwise, if the data should not be compressed (the real compressed
        size is bigger than the uncompressed size), return the uncompressed
        size.
        '''
        if self.compressed:
            return self._compressed_size
        return self.uncompressed_size

    @property
    def uncompressed_size(self):
        '''
        Return the size of the data written to the Deflater.
        '''
        return self._data.tell()

    @property
    def crc32(self):
        '''
        Return the crc32 of the data written to the Deflater.
        '''
        return zlib.crc32(self._data.getvalue()) & 0xffffffff

    @property
    def compressed_data(self):
        '''
        Return the compressed data, if the data should be compressed (real
        compressed size smaller than the uncompressed size), or the
        uncompressed data otherwise.
        '''
        if self.compressed:
            return self._deflated.getvalue()
        return self._data.getvalue()


class JarLog(dict):
    '''
    Helper to read the file Gecko generates when setting MOZ_JAR_LOG_FILE.
    The jar log is then available as a dict with the jar path as key (see
    canonicalize for more details on the key value), and the corresponding
    access log as a list value. Only the first access to a given member of
    a jar is stored.
    '''
    def __init__(self, file=None, fileobj=None):
        if not fileobj:
            fileobj = open(file, 'r')
        urlmap = {}
        for line in fileobj:
            url, path = line.strip().split(None, 1)
            if not url or not path:
                continue
            if url not in urlmap:
                urlmap[url] = JarLog.canonicalize(url)
            jar = urlmap[url]
            entry = self.setdefault(jar, [])
            if path not in entry:
                entry.append(path)

    @staticmethod
    def canonicalize(url):
        '''
        The jar path is stored in a MOZ_JAR_LOG_FILE log as a url. This method
        returns a unique value corresponding to such urls.
        - file:///{path} becomes {path}
        - jar:file:///{path}!/{subpath} becomes ({path}, {subpath})
        - jar:jar:file:///{path}!/{subpath}!/{subpath2} becomes
           ({path}, {subpath}, {subpath2})
        '''
        if not isinstance(url, ParseResult):
            # Assume that if it doesn't start with jar: or file:, it's a path.
            if not url.startswith(('jar:', 'file:')):
                url = 'file:///' + os.path.abspath(url)
            url = urlparse(url)
        assert url.scheme
        assert url.scheme in ('jar', 'file')
        if url.scheme == 'jar':
            path = JarLog.canonicalize(url.path)
            if isinstance(path, tuple):
                return path[:-1] + tuple(path[-1].split('!/', 1))
            return tuple(path.split('!/', 1))
        if url.scheme == 'file':
            assert os.path.isabs(url.path)
            path = url.path
            # On Windows, url.path will be /drive:/path ; on Unix systems,
            # /path. As we want drive:/path instead of /drive:/path on Windows,
            # remove the leading /.
            if os.path.isabs(path[1:]):
                path = path[1:]
            path = os.path.realpath(path)
            return mozpack.path.normsep(os.path.normcase(path))
