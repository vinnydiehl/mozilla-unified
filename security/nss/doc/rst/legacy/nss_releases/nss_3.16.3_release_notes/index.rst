.. _mozilla_projects_nss_nss_3_16_3_release_notes:

NSS 3.16.3 release notes
========================

`Introduction <#introduction>`__
--------------------------------

.. container::

   Network Security Services (NSS) 3.16.3 is a patch release for NSS 3.16. The bug fixes in NSS
   3.16.3 are described in the "Bugs Fixed" section below.



`Distribution Information <#distribution_information>`__
--------------------------------------------------------

.. container::

   The HG tag is NSS_3_16_3_RTM. NSS 3.16.3 requires NSPR 4.10.6 or newer.

   NSS 3.16.3 source distributions are available on ftp.mozilla.org for secure HTTPS download:

   -  Source tarballs:
      https://ftp.mozilla.org/pub/mozilla.org/security/nss/releases/NSS_3_16_3_RTM/src/

.. _new_in_nss_3.16.3:

`New in NSS 3.16.3 <#new_in_nss_3.16.3>`__
------------------------------------------

.. container::

   This release consists primarily of CA certificate changes as listed below, and fixes an issue
   with a recently added utility function.

   .. rubric:: New Functions
      :name: new_functions

   -  *in cert.h*

      -  **CERT_GetGeneralNameTypeFromString** - An utlity function to lookup a value of type
         CERTGeneralNameType given a human readable string. This function was already added in NSS
         3.16.2, however, it wasn't declared in a public header file.

.. _notable_changes_in_nss_3.16.3:

`Notable Changes in NSS 3.16.3 <#notable_changes_in_nss_3.16.3>`__
------------------------------------------------------------------

.. container::

   -  The following **1024-bit** CA certificates were **Removed**

      -  CN = Entrust.net Secure Server Certification Authority

         -  SHA1 Fingerprint: 99:A6:9B:E6:1A:FE:88:6B:4D:2B:82:00:7C:B8:54:FC:31:7E:15:39

      -  CN = GTE CyberTrust Global Root

         -  SHA1 Fingerprint: 97:81:79:50:D8:1C:96:70:CC:34:D8:09:CF:79:44:31:36:7E:F4:74

      -  OU = ValiCert Class 1 Policy Validation Authority

         -  SHA1 Fingerprint: E5:DF:74:3C:B6:01:C4:9B:98:43:DC:AB:8C:E8:6A:81:10:9F:E4:8E

      -  OU = ValiCert Class 2 Policy Validation Authority

         -  SHA1 Fingerprint: 31:7A:2A:D0:7F:2B:33:5E:F5:A1:C3:4E:4B:57:E8:B7:D8:F1:FC:A6

      -  OU = ValiCert Class 3 Policy Validation Authority

         -  SHA1 Fingerprint: 69:BD:8C:F4:9C:D3:00:FB:59:2E:17:93:CA:55:6A:F3:EC:AA:35:FB

   -  Additionally, the following CA certificate was **Removed** as requested by the CA

      -  OU = TDC Internet Root CA

         -  SHA1 Fingerprint: 21:FC:BD:8E:7F:6C:AF:05:1B:D1:B3:43:EC:A8:E7:61:47:F2:0F:8A

   -  The following CA certificates were **Added**

      -  CN = Certification Authority of WoSign

         -  SHA1 Fingerprint: B9:42:94:BF:91:EA:8F:B6:4B:E6:10:97:C7:FB:00:13:59:B6:76:CB

      -  CN = CA 沃通根证书

         -  SHA1 Fingerprint: 16:32:47:8D:89:F9:21:3A:92:00:85:63:F5:A4:A7:D3:12:40:8A:D6

      -  CN = DigiCert Assured ID Root G2

         -  SHA1 Fingerprint: A1:4B:48:D9:43:EE:0A:0E:40:90:4F:3C:E0:A4:C0:91:93:51:5D:3F

      -  CN = DigiCert Assured ID Root G3

         -  SHA1 Fingerprint: F5:17:A2:4F:9A:48:C6:C9:F8:A2:00:26:9F:DC:0F:48:2C:AB:30:89

      -  CN = DigiCert Global Root G2

         -  SHA1 Fingerprint: DF:3C:24:F9:BF:D6:66:76:1B:26:80:73:FE:06:D1:CC:8D:4F:82:A4

      -  CN = DigiCert Global Root G3

         -  SHA1 Fingerprint: 7E:04:DE:89:6A:3E:66:6D:00:E6:87:D3:3F:FA:D9:3B:E8:3D:34:9E

      -  CN = DigiCert Trusted Root G4

         -  SHA1 Fingerprint: DD:FB:16:CD:49:31:C9:73:A2:03:7D:3F:C8:3A:4D:7D:77:5D:05:E4

      -  CN = QuoVadis Root CA 1 G3

         -  SHA1 Fingerprint: 1B:8E:EA:57:96:29:1A:C9:39:EA:B8:0A:81:1A:73:73:C0:93:79:67

      -  CN = QuoVadis Root CA 2 G3

         -  SHA1 Fingerprint: 09:3C:61:F3:8B:8B:DC:7D:55:DF:75:38:02:05:00:E1:25:F5:C8:36

      -  CN = QuoVadis Root CA 3 G3

         -  SHA1 Fingerprint: 48:12:BD:92:3C:A8:C4:39:06:E7:30:6D:27:96:E6:A4:CF:22:2E:7D

   -  The **Trust Bits were changed** for the following CA certificates

      -  OU = Class 3 Public Primary Certification Authority

         -  SHA1 Fingerprint: A1:DB:63:93:91:6F:17:E4:18:55:09:40:04:15:C7:02:40:B0:AE:6B
         -  Turned off websites and code signing trust bits (1024-bit root)

      -  OU = Class 3 Public Primary Certification Authority

         -  SHA1 Fingerprint: 74:2C:31:92:E6:07:E4:24:EB:45:49:54:2B:E1:BB:C5:3E:61:74:E2
         -  Turned off websites and code signing trust bits (1024-bit root)

      -  OU = Class 2 Public Primary Certification Authority - G2

         -  SHA1 Fingerprint: B3:EA:C4:47:76:C9:C8:1C:EA:F2:9D:95:B6:CC:A0:08:1B:67:EC:9D
         -  Turned off code signing trust bit (change requested by CA)

      -  CN = VeriSign Class 2 Public Primary Certification Authority - G3

         -  SHA-1 Fingerprint: 61:EF:43:D7:7F:CA:D4:61:51:BC:98:E0:C3:59:12:AF:9F:EB:63:11
         -  Turned off code signing trust bit (change requested by CA)

      -  CN = AC Raíz Certicámara S.A.

         -  SHA1 Fingerprint: CB:A1:C5:F8:B0:E3:5E:B8:B9:45:12:D3:F9:34:A2:E9:06:10:D3:36
         -  Turned off websites trust bit (change requested by CA)

      -  CN = NetLock Uzleti (Class B) Tanusitvanykiado

         -  SHA1 Fingerprint: 87:9F:4B:EE:05:DF:98:58:3B:E3:60:D6:33:E7:0D:3F:FE:98:71:AF
         -  Turned off websites and code signing trust bits (1024-bit root)

      -  CN = NetLock Expressz (Class C) Tanusitvanykiado

         -  SHA1 Fingerprint: E3:92:51:2F:0A:CF:F5:05:DF:F6:DE:06:7F:75:37:E1:65:EA:57:4B
         -  Turned off websites and code signing trust bits (1024-bit root)

.. _bugs_fixed_in_nss_3.16.3:

`Bugs fixed in NSS 3.16.3 <#bugs_fixed_in_nss_3.16.3>`__
--------------------------------------------------------

.. container::

   This Bugzilla query returns all the bugs fixed in NSS 3.16.3:

   | https://bugzilla.mozilla.org/buglist.cgi?resolution=FIXED&classification=Components&query_format=advanced&product=NSS&target_milestone=3.16.3
   |