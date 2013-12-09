# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

r"""Data structures representing Mozilla's source tree.

The frontend files are parsed into static data structures. These data
structures are defined in this module.

All data structures of interest are children of the TreeMetadata class.

Logic for populating these data structures is not defined in this class.
Instead, what we have here are dumb container classes. The emitter module
contains the code for converting executed mozbuild files into these data
structures.
"""

from __future__ import unicode_literals

import os

from collections import OrderedDict
from mozbuild.util import (
    shell_quote,
    StrictOrderingOnAppendList,
)
from .sandbox_symbols import compute_final_target


class TreeMetadata(object):
    """Base class for all data being captured."""

    def __init__(self):
        self._ack = False

    def ack(self):
        self._ack = True


class ReaderSummary(TreeMetadata):
    """A summary of what the reader did."""

    def __init__(self, total_file_count, total_execution_time):
        TreeMetadata.__init__(self)
        self.total_file_count = total_file_count
        self.total_execution_time = total_execution_time


class SandboxDerived(TreeMetadata):
    """Build object derived from a single MozbuildSandbox instance.

    It holds fields common to all sandboxes. This class is likely never
    instantiated directly but is instead derived from.
    """

    __slots__ = (
        'objdir',
        'relativedir',
        'sandbox_all_paths',
        'sandbox_path',
        'srcdir',
        'topobjdir',
        'topsrcdir',
    )

    def __init__(self, sandbox):
        TreeMetadata.__init__(self)

        # Capture the files that were evaluated to build this sandbox.
        self.sandbox_main_path = sandbox.main_path
        self.sandbox_all_paths = sandbox.all_paths

        # Basic directory state.
        self.topsrcdir = sandbox['TOPSRCDIR']
        self.topobjdir = sandbox['TOPOBJDIR']

        self.relativedir = sandbox['RELATIVEDIR']
        self.srcdir = sandbox['SRCDIR']
        self.objdir = sandbox['OBJDIR']


class DirectoryTraversal(SandboxDerived):
    """Describes how directory traversal for building should work.

    This build object is likely only of interest to the recursive make backend.
    Other build backends should (ideally) not attempt to mimic the behavior of
    the recursive make backend. The only reason this exists is to support the
    existing recursive make backend while the transition to mozbuild frontend
    files is complete and we move to a more optimal build backend.

    Fields in this class correspond to similarly named variables in the
    frontend files.
    """
    __slots__ = (
        'dirs',
        'parallel_dirs',
        'tool_dirs',
        'test_dirs',
        'test_tool_dirs',
        'tier_dirs',
        'tier_static_dirs',
        'external_make_dirs',
        'parallel_external_make_dirs',
    )

    def __init__(self, sandbox):
        SandboxDerived.__init__(self, sandbox)

        self.dirs = []
        self.parallel_dirs = []
        self.tool_dirs = []
        self.test_dirs = []
        self.test_tool_dirs = []
        self.tier_dirs = OrderedDict()
        self.tier_static_dirs = OrderedDict()
        self.external_make_dirs = []
        self.parallel_external_make_dirs = []


class BaseConfigSubstitution(SandboxDerived):
    """Base class describing autogenerated files as part of config.status."""

    __slots__ = (
        'input_path',
        'output_path',
        'relpath',
    )

    def __init__(self, sandbox):
        SandboxDerived.__init__(self, sandbox)

        self.input_path = None
        self.output_path = None
        self.relpath = None


class ConfigFileSubstitution(BaseConfigSubstitution):
    """Describes a config file that will be generated using substitutions."""


class HeaderFileSubstitution(BaseConfigSubstitution):
    """Describes a header file that will be generated using substitutions."""


class VariablePassthru(SandboxDerived):
    """A dict of variables to pass through to backend.mk unaltered.

    The purpose of this object is to facilitate rapid transitioning of
    variables from Makefile.in to moz.build. In the ideal world, this class
    does not exist and every variable has a richer class representing it.
    As long as we rely on this class, we lose the ability to have flexibility
    in our build backends since we will continue to be tied to our rules.mk.
    """
    __slots__ = ('variables')

    def __init__(self, sandbox):
        SandboxDerived.__init__(self, sandbox)
        self.variables = {}

class XPIDLFile(SandboxDerived):
    """Describes an XPIDL file to be compiled."""

    __slots__ = (
        'basename',
        'source_path',
    )

    def __init__(self, sandbox, source, module):
        SandboxDerived.__init__(self, sandbox)

        self.source_path = source
        self.basename = os.path.basename(source)
        self.module = module

class Defines(SandboxDerived):
    """Sandbox container object for DEFINES, which is an OrderedDict.
    """
    __slots__ = ('defines')

    def __init__(self, sandbox, defines):
        SandboxDerived.__init__(self, sandbox)
        self.defines = defines

    def get_defines(self):
        for define, value in self.defines.iteritems():
            if value is True:
                defstr = define
            else:
                defstr = '%s=%s' % (define, shell_quote(value))
            yield('-D%s' % defstr)

class Exports(SandboxDerived):
    """Sandbox container object for EXPORTS, which is a HierarchicalStringList.

    We need an object derived from SandboxDerived for use in the backend, so
    this object fills that role. It just has a reference to the underlying
    HierarchicalStringList, which is created when parsing EXPORTS.
    """
    __slots__ = ('exports', 'dist_install')

    def __init__(self, sandbox, exports, dist_install=True):
        SandboxDerived.__init__(self, sandbox)
        self.exports = exports
        self.dist_install = dist_install


class IPDLFile(SandboxDerived):
    """Describes an individual .ipdl source file."""

    __slots__ = (
        'basename',
    )

    def __init__(self, sandbox, path):
        SandboxDerived.__init__(self, sandbox)

        self.basename = path

class WebIDLFile(SandboxDerived):
    """Describes an individual .webidl source file."""

    __slots__ = (
        'basename',
    )

    def __init__(self, sandbox, path):
        SandboxDerived.__init__(self, sandbox)

        self.basename = path

class GeneratedEventWebIDLFile(SandboxDerived):
    """Describes an individual .webidl source file."""

    __slots__ = (
        'basename',
    )

    def __init__(self, sandbox, path):
        SandboxDerived.__init__(self, sandbox)

        self.basename = path

class TestWebIDLFile(SandboxDerived):
    """Describes an individual test-only .webidl source file."""

    __slots__ = (
        'basename',
    )

    def __init__(self, sandbox, path):
        SandboxDerived.__init__(self, sandbox)

        self.basename = path

class PreprocessedTestWebIDLFile(SandboxDerived):
    """Describes an individual test-only .webidl source file that requires
    preprocessing."""

    __slots__ = (
        'basename',
    )

    def __init__(self, sandbox, path):
        SandboxDerived.__init__(self, sandbox)

        self.basename = path

class PreprocessedWebIDLFile(SandboxDerived):
    """Describes an individual .webidl source file that requires preprocessing."""

    __slots__ = (
        'basename',
    )

    def __init__(self, sandbox, path):
        SandboxDerived.__init__(self, sandbox)

        self.basename = path

class GeneratedWebIDLFile(SandboxDerived):
    """Describes an individual .webidl source file that is generated from
    build rules."""

    __slots__ = (
        'basename',
    )

    def __init__(self, sandbox, path):
        SandboxDerived.__init__(self, sandbox)

        self.basename = path

class BaseProgram(SandboxDerived):
    """Sandbox container object for programs, which is a unicode string.

    This class handles automatically appending a binary suffix to the program
    name.
    If the suffix is not defined, the program name is unchanged.
    Otherwise, if the program name ends with the given suffix, it is unchanged
    Otherwise, the suffix is appended to the program name.
    """
    __slots__ = ('program')

    def __init__(self, sandbox, program, bin_suffix):
        SandboxDerived.__init__(self, sandbox)

        bin_suffix = bin_suffix or ''
        if not program.endswith(bin_suffix):
            program += bin_suffix
        self.program = program


class Program(BaseProgram):
    """Sandbox container object for PROGRAM"""


class HostProgram(BaseProgram):
    """Sandbox container object for HOST_PROGRAM"""


class SimpleProgram(BaseProgram):
    """Sandbox container object for each program in SIMPLE_PROGRAMS"""


class HostSimpleProgram(BaseProgram):
    """Sandbox container object for each program in HOST_SIMPLE_PROGRAMS"""


class LibraryDefinition(SandboxDerived):
    """Partial definition for a library

    The static_libraries member tracks the list of relative directory and
    library names of static libraries that are meant to be linked into
    the library defined by an instance of this class.
    """
    __slots__ = (
        'basename',
        'static_libraries',
        'refcount',
    )

    def __init__(self, sandbox, basename):
        SandboxDerived.__init__(self, sandbox)

        self.basename = basename
        self.refcount = 0
        self.static_libraries = []

    def link_static_lib(self, objdir, basename):
        self.static_libraries.append((objdir, basename))


class TestManifest(SandboxDerived):
    """Represents a manifest file containing information about tests."""

    __slots__ = (
        # The type of test manifest this is.
        'flavor',

        # Maps source filename to destination filename. The destination
        # path is relative from the tests root directory.
        'installs',

        # A list of pattern matching installs to perform. Entries are
        # (base, pattern, dest).
        'pattern_installs',

        # Where all files for this manifest flavor are installed in the unified
        # test package directory.
        'install_prefix',

        # Set of files provided by an external mechanism.
        'external_installs',

        # The full path of this manifest file.
        'path',

        # The directory where this manifest is defined.
        'directory',

        # The parsed manifestparser.TestManifest instance.
        'manifest',

        # List of tests. Each element is a dict of metadata.
        'tests',

        # The relative path of the parsed manifest within the srcdir.
        'manifest_relpath',

        # If this manifest is a duplicate of another one, this is the
        # manifestparser.TestManifest of the other one.
        'dupe_manifest',
    )

    def __init__(self, sandbox, path, manifest, flavor=None,
            install_prefix=None, relpath=None, dupe_manifest=False):
        SandboxDerived.__init__(self, sandbox)

        self.path = path
        self.directory = os.path.dirname(path)
        self.manifest = manifest
        self.flavor = flavor
        self.install_prefix = install_prefix
        self.manifest_relpath = relpath
        self.dupe_manifest = dupe_manifest
        self.installs = {}
        self.pattern_installs = []
        self.tests = []
        self.external_installs = set()


class LocalInclude(SandboxDerived):
    """Describes an individual local include path."""

    __slots__ = (
        'path',
    )

    def __init__(self, sandbox, path):
        SandboxDerived.__init__(self, sandbox)

        self.path = path

class GeneratedInclude(SandboxDerived):
    """Describes an individual generated include path."""

    __slots__ = (
        'path',
    )

    def __init__(self, sandbox, path):
        SandboxDerived.__init__(self, sandbox)

        self.path = path


class SandboxWrapped(SandboxDerived):
    """Generic sandbox container object for a wrapped rich object.

    Use this wrapper class to shuttle a rich build system object
    completely defined in moz.build files through the tree metadata
    emitter to the build backend for processing as-is.
    """

    __slots__ = (
        'wrapped',
    )

    def __init__(self, sandbox, wrapped):
        SandboxDerived.__init__(self, sandbox)

        self.wrapped = wrapped


class JavaJarData(object):
    """Represents a Java JAR file.

    A Java JAR has the following members:
        * sources - strictly ordered list of input java sources
        * generated_sources - strictly ordered list of generated input
          java sources
        * extra_jars - list of JAR file dependencies to include on the
          javac compiler classpath
        * javac_flags - list containing extra flags passed to the
          javac compiler
    """

    __slots__ = (
        'name',
        'sources',
        'generated_sources',
        'extra_jars',
        'javac_flags',
    )

    def __init__(self, name, sources=[], generated_sources=[],
            extra_jars=[], javac_flags=[]):
        self.name = name
        self.sources = StrictOrderingOnAppendList(sources)
        self.generated_sources = StrictOrderingOnAppendList(generated_sources)
        self.extra_jars = list(extra_jars)
        self.javac_flags = list(javac_flags)


class InstallationTarget(SandboxDerived):
    """Describes the rules that affect where files get installed to."""

    __slots__ = (
        'xpiname',
        'subdir',
        'target',
        'enabled'
    )

    def __init__(self, sandbox):
        SandboxDerived.__init__(self, sandbox)

        self.xpiname = sandbox.get('XPI_NAME', '')
        self.subdir = sandbox.get('DIST_SUBDIR', '')
        self.target = sandbox['FINAL_TARGET']
        self.enabled = not sandbox.get('NO_DIST_INSTALL', False)

    def is_custom(self):
        """Returns whether or not the target is not derived from the default
        given xpiname and subdir."""

        return compute_final_target(dict(
            XPI_NAME=self.xpiname,
            DIST_SUBDIR=self.subdir)) == self.target
