.. _mozilla_projects_nss_nss_3_37_release_notes:

NSS 3.37 release notes
======================

`Introduction <#introduction>`__
--------------------------------

.. container::

   The NSS team has released Network Security Services (NSS) 3.37, which is a minor release.



`Distribution Information <#distribution_information>`__
--------------------------------------------------------

.. container::

   The HG tag is NSS_3_37_RTM. NSS 3.37 requires NSPR 4.19 or newer.

   NSS 3.37 source distributions are available on ftp.mozilla.org for secure HTTPS download:

   -  Source tarballs:
      https://ftp.mozilla.org/pub/mozilla.org/security/nss/releases/NSS_3_37_RTM/src/

.. _notable_changes_in_nss_3.37:

`Notable Changes in NSS 3.37 <#notable_changes_in_nss_3.37>`__
--------------------------------------------------------------

.. container::

   -  The TLS 1.3 implementation was updated to Draft 28.

   -  An issue where NSS erroneously accepted HRR requests was resolved. This issue was found by
      `OSS fuzz <https://bugs.chromium.org/p/oss-fuzz/issues/detail?id=7159>`__.

   -  Added HACL\* Poly1305 32-bit

   -  The code to support the NPN protocol, which had already been disabled in a previous release,
      has been fully removed.

   -  NSS allows servers now to register ALPN handling callbacks to select a protocol.

   -  NSS supports opening SQL databases in read-only mode. NSS now requires the SQLite APIs of
      version 3.5.0 or newer.

   -  Starting with NSS version 3.31, an alternative implementation for RNG seeding on the
      Linux/UNIX platform was available (bug 1346735), which performed seeding exclusively based on
      /dev/urandom. This alternative implementation is selected at build time by defining the
      SEED_ONLY_DEV_URANDOM symbol.

      (The classic implementation for RNG seeding on the Linux/Unix platform, which may use
      additional sources for the default seeding, is still available and will be used if
      SEED_ONLY_DEV_URANDOM is undefined.)

      With NSS 3.37, this alternative implementation for Linux/Unix can be selected in "make" builds
      by defining the environment variable NSS_SEED_ONLY_DEV_URANDOM.

      With NSS 3.37, this alternative implementation for Linux has been enhanced to use the glibc
      function getentropy(), instead of reading from /dev/urandom directly, if the build and runtime
      Linux platform supports it.

   -  The CA certificates list was updated to version 2.24.

   -  The following CA certificates were **Removed**:

      -  CN = S-TRUST Universal Root CA

         -  SHA-256 Fingerprint:
            D8:0F:EF:91:0A:E3:F1:04:72:3B:04:5C:EC:2D:01:9F:44:1C:E6:21:3A:DF:15:67:91:E7:0C:17:90:11:0A:31

      -  CN = TC TrustCenter Class 3 CA II

         -  SHA-256 Fingerprint:
            8D:A0:84:FC:F9:9C:E0:77:22:F8:9B:32:05:93:98:06:FA:5C:B8:11:E1:C8:13:F6:A1:08:C7:D3:36:B3:40:8E

      -  CN = TÜRKTRUST Elektronik Sertifika Hizmet Sağlayıcısı H5

         -  SHA-256 Fingerprint:
            49:35:1B:90:34:44:C1:85:CC:DC:5C:69:3D:24:D8:55:5C:B2:08:D6:A8:14:13:07:69:9F:4A:F0:63:19:9D:78

.. _bugs_fixed_in_nss_3.37:

`Bugs fixed in NSS 3.37 <#bugs_fixed_in_nss_3.37>`__
----------------------------------------------------

.. container::

   This Bugzilla query returns all the bugs fixed in NSS 3.37:

   https://bugzilla.mozilla.org/buglist.cgi?resolution=FIXED&classification=Components&query_format=advanced&product=NSS&target_milestone=3.37

`Compatibility <#compatibility>`__
----------------------------------

.. container::

   NSS 3.37 shared libraries are backward compatible with all older NSS 3.x shared libraries. A
   program linked with older NSS 3.x shared libraries will work with NSS 3.37 shared libraries
   without recompiling or relinking. Furthermore, applications that restrict their use of NSS APIs
   to the functions listed in NSS Public Functions will remain compatible with future versions of
   the NSS shared libraries.

`Feedback <#feedback>`__
------------------------

.. container::

   Bugs discovered should be reported by filing a bug report with
   `bugzilla.mozilla.org <https://bugzilla.mozilla.org/enter_bug.cgi?product=NSS>`__ (product NSS).