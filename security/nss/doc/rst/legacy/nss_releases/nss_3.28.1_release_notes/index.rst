.. _mozilla_projects_nss_nss_3_28_1_release_notes:

NSS 3.28.1 release notes
========================

`Introduction <#introduction>`__
--------------------------------

.. container::

   Network Security Services (NSS) 3.28.1 is a patch release for NSS 3.28. The bug fixes in NSS
   3.28.1 are described in the "Bugs Fixed" section below.



`Distribution Information <#distribution_information>`__
--------------------------------------------------------

.. container::

   The HG tag is NSS_3_28_1_RTM. NSS 3.28.1 requires NSPR 4.13.1 or newer.

   NSS 3.28.1 source distributions are available on ftp.mozilla.org for secure HTTPS download:

   -  Source tarballs:
      https://ftp.mozilla.org/pub/mozilla.org/security/nss/releases/NSS_3_28_1_RTM/src/

.. _new_in_nss_3.28.1:

`New in NSS 3.28.1 <#new_in_nss_3.28.1>`__
------------------------------------------

.. container::

   No new functionality is introduced in this release. This is a patch release to update the list of
   root CA certificates, and address a minor TLS compatibility issue, that some applications
   experienced with NSS 3.28.

.. _notable_changes_in_nss_3.28.1:

`Notable Changes in NSS 3.28.1 <#notable_changes_in_nss_3.28.1>`__
------------------------------------------------------------------

.. container::

   -  The following CA certificates were **Removed**

      -  CN = Buypass Class 2 CA 1

         -  SHA-256 Fingerprint:
            0F:4E:9C:DD:26:4B:02:55:50:D1:70:80:63:40:21:4F:E9:44:34:C9:B0:2F:69:7E:C7:10:FC:5F:EA:FB:5E:38

      -  CN = Root CA Generalitat Valenciana

         -  SHA-256 Fingerprint:
            8C:4E:DF:D0:43:48:F3:22:96:9E:7E:29:A4:CD:4D:CA:00:46:55:06:1C:16:E1:B0:76:42:2E:F3:42:AD:63:0E

      -  OU = RSA Security 2048 V3

         -  SHA-256 Fingerprint:
            AF:8B:67:62:A1:E5:28:22:81:61:A9:5D:5C:55:9E:E2:66:27:8F:75:D7:9E:83:01:89:A5:03:50:6A:BD:6B:4C

   -  The following CA certificates were **Added**

      -  OU = AC RAIZ FNMT-RCM

         -  SHA-256 Fingerprint:
            EB:C5:57:0C:29:01:8C:4D:67:B1:AA:12:7B:AF:12:F7:03:B4:61:1E:BC:17:B7:DA:B5:57:38:94:17:9B:93:FA

      -  CN = Amazon Root CA 1

         -  SHA-256 Fingerprint:
            8E:CD:E6:88:4F:3D:87:B1:12:5B:A3:1A:C3:FC:B1:3D:70:16:DE:7F:57:CC:90:4F:E1:CB:97:C6:AE:98:19:6E

      -  CN = Amazon Root CA 2

         -  SHA-256 Fingerprint:
            1B:A5:B2:AA:8C:65:40:1A:82:96:01:18:F8:0B:EC:4F:62:30:4D:83:CE:C4:71:3A:19:C3:9C:01:1E:A4:6D:B4

      -  CN = Amazon Root CA 3

         -  SHA-256 Fingerprint:
            18:CE:6C:FE:7B:F1:4E:60:B2:E3:47:B8:DF:E8:68:CB:31:D0:2E:BB:3A:DA:27:15:69:F5:03:43:B4:6D:B3:A4

      -  CN = Amazon Root CA 4

         -  SHA-256 Fingerprint:
            E3:5D:28:41:9E:D0:20:25:CF:A6:90:38:CD:62:39:62:45:8D:A5:C6:95:FB:DE:A3:C2:2B:0B:FB:25:89:70:92

      -  CN = LuxTrust Global Root 2

         -  SHA-256 Fingerprint:
            54:45:5F:71:29:C2:0B:14:47:C4:18:F9:97:16:8F:24:C5:8F:C5:02:3B:F5:DA:5B:E2:EB:6E:1D:D8:90:2E:D5

      -  CN = Symantec Class 1 Public Primary Certification Authority - G4

         -  SHA-256 Fingerprint:
            36:3F:3C:84:9E:AB:03:B0:A2:A0:F6:36:D7:B8:6D:04:D3:AC:7F:CF:E2:6A:0A:91:21:AB:97:95:F6:E1:76:DF

      -  CN = Symantec Class 1 Public Primary Certification Authority - G6

         -  SHA-256 Fingerprint:
            9D:19:0B:2E:31:45:66:68:5B:E8:A8:89:E2:7A:A8:C7:D7:AE:1D:8A:AD:DB:A3:C1:EC:F9:D2:48:63:CD:34:B9

      -  CN = Symantec Class 2 Public Primary Certification Authority - G4

         -  SHA-256 Fingerprint:
            FE:86:3D:08:22:FE:7A:23:53:FA:48:4D:59:24:E8:75:65:6D:3D:C9:FB:58:77:1F:6F:61:6F:9D:57:1B:C5:92

      -  CN = Symantec Class 2 Public Primary Certification Authority - G6

         -  SHA-256 Fingerprint:
            CB:62:7D:18:B5:8A:D5:6D:DE:33:1A:30:45:6B:C6:5C:60:1A:4E:9B:18:DE:DC:EA:08:E7:DA:AA:07:81:5F:F0

   -  The version number of the updated root CA list has been set to 2.11
   -  A misleading assertion/alert has been removed, when NSS tries to flush data to the peer but
      the connection was already reset.

.. _bugs_fixed_in_nss_3.28.1:

`Bugs fixed in NSS 3.28.1 <#bugs_fixed_in_nss_3.28.1>`__
--------------------------------------------------------

.. container::

   | `Bug 1296697 - December 2016 batch of root CA
     changes <https://bugzilla.mozilla.org/show_bug.cgi?id=1296697>`__
   | `Bug 1322496 - Internal error assert when the other side closes connection before reading
     EOED <https://bugzilla.mozilla.org/show_bug.cgi?id=1322496>`__

`Compatibility <#compatibility>`__
----------------------------------

.. container::

   NSS 3.28.1 shared libraries are backward compatible with all older NSS 3.x shared libraries. A
   program linked with older NSS 3.x shared libraries will work with NSS 3.28.1 shared libraries
   without recompiling or relinking. Furthermore, applications that restrict their use of NSS APIs
   to the functions listed in NSS Public Functions will remain compatible with future versions of
   the NSS shared libraries.

`Feedback <#feedback>`__
------------------------

.. container::

   Bugs discovered should be reported by filing a bug report with
   `bugzilla.mozilla.org <https://bugzilla.mozilla.org/enter_bug.cgi?product=NSS>`__ (product NSS).