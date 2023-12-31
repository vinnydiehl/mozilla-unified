.. _mozilla_projects_nss_nss_3_30_2_release_notes:

NSS 3.30.2 release notes
========================

`Introduction <#introduction>`__
--------------------------------

.. container::

   Network Security Services (NSS) 3.30.2 is a patch release for NSS 3.30. The bug fixes in NSS
   3.30.2 are described in the "Bugs Fixed" section below.



`Distribution Information <#distribution_information>`__
--------------------------------------------------------

.. container::

   The HG tag is NSS_3_30_2_RTM. NSS 3.30.2 requires NSPR 4.14 or newer.

   NSS 3.30.2 source distributions are available on ftp.mozilla.org for secure HTTPS download:

   -  Source tarballs:
      https://ftp.mozilla.org/pub/mozilla.org/security/nss/releases/NSS_3_30_2_RTM/src/

.. _new_in_nss_3.30.2:

`New in NSS 3.30.2 <#new_in_nss_3.30.2>`__
------------------------------------------

.. container::

   No new functionality is introduced in this release. This is a patch release to update the list of
   root CA certificates.

.. _notable_changes_in_nss_3.30.2:

`Notable Changes in NSS 3.30.2 <#notable_changes_in_nss_3.30.2>`__
------------------------------------------------------------------

.. container::

   -  The following CA certificates were **Removed**:

      -  O = Japanese Government, OU = ApplicationCA

         -  SHA-256 Fingerprint:
            2D:47:43:7D:E1:79:51:21:5A:12:F3:C5:8E:51:C7:29:A5:80:26:EF:1F:CC:0A:5F:B3:D9:DC:01:2F:60:0D:19

      -  CN = WellsSecure Public Root Certificate Authority

         -  SHA-256 Fingerprint:
            A7:12:72:AE:AA:A3:CF:E8:72:7F:7F:B3:9F:0F:B3:D1:E5:42:6E:90:60:B0:6E:E6:F1:3E:9A:3C:58:33:CD:43

      -  CN=TÜRKTRUST Elektronik Sertifika Hizmet Sağlayıcısı H6

         -  SHA-256 Fingerprint:
            8D:E7:86:55:E1:BE:7F:78:47:80:0B:93:F6:94:D2:1D:36:8C:C0:6E:03:3E:7F:AB:04:BB:5E:B9:9D:A6:B7:00

      -  CN=Microsec e-Szigno Root

         -  SHA-256 Fingerprint:
            32:7A:3D:76:1A:BA:DE:A0:34:EB:99:84:06:27:5C:B1:A4:77:6E:FD:AE:2F:DF:6D:01:68:EA:1C:4F:55:67:D0

   -  The following CA certificates were **Added**:

      -  CN = D-TRUST Root CA 3 2013

         -  SHA-256 Fingerprint:
            A1:A8:6D:04:12:1E:B8:7F:02:7C:66:F5:33:03:C2:8E:57:39:F9:43:FC:84:B3:8A:D6:AF:00:90:35:DD:94:57
         -  Trust Flags: Email

      -  CN = TUBITAK Kamu SM SSL Kok Sertifikasi - Surum 1

         -  SHA-256 Fingerprint:
            46:ED:C3:68:90:46:D5:3A:45:3F:B3:10:4A:B8:0D:CA:EC:65:8B:26:60:EA:16:29:DD:7E:86:79:90:64:87:16
         -  Trust Flags: Websites
         -  Technically constrained to: gov.tr, k12.tr, pol.tr, mil.tr, tsk.tr, kep.tr, bel.tr,
            edu.tr, org.tr

   -  The version number of the updated root CA list has been set to 2.14
      (The version numbers 2.12 and 2.13 for the root CA list have been skipped.)

.. _bugs_fixed_in_nss_3.30.2:

`Bugs fixed in NSS 3.30.2 <#bugs_fixed_in_nss_3.30.2>`__
--------------------------------------------------------

.. container::

   -  `Bug 1350859 <https://bugzilla.mozilla.org/show_bug.cgi?id=1350859>`__ - March 2017 batch of
      root CA changes
   -  `Bug 1349705 <https://bugzilla.mozilla.org/show_bug.cgi?id=1349705>`__ - Implemented domain
      name constraints for CA: TUBITAK Kamu SM SSL Kok Sertifikasi - Surum 1

`Compatibility <#compatibility>`__
----------------------------------

.. container::

   NSS 3.30.2 shared libraries are backward compatible with all older NSS 3.x shared libraries. A
   program linked with older NSS 3.x shared libraries will work with NSS 3.30.2 shared libraries
   without recompiling or relinking. Furthermore, applications that restrict their use of NSS APIs
   to the functions listed in NSS Public Functions will remain compatible with future versions of
   the NSS shared libraries.

`Feedback <#feedback>`__
------------------------

.. container::

   Bugs discovered should be reported by filing a bug report with
   `bugzilla.mozilla.org <https://bugzilla.mozilla.org/enter_bug.cgi?product=NSS>`__ (product NSS).