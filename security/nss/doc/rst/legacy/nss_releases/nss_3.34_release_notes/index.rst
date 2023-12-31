.. _mozilla_projects_nss_nss_3_34_release_notes:

NSS 3.34 release notes
======================

`Introduction <#introduction>`__
--------------------------------

.. container::

   The Network Security Services (NSS) team has released NSS 3.34, which is a minor release.



`Distribution information <#distribution_information>`__
--------------------------------------------------------

.. container::

   The hg tag is NSS_3_34_RTM. NSS 3.34 requires Netscape Portable Runtime (NSPR) 4.17, or newer.

   NSS 3.34 source distributions are available on ftp.mozilla.org for secure HTTPS download:

   -  Source tarballs:
      https://ftp.mozilla.org/pub/mozilla.org/security/nss/releases/NSS_3_34_RTM/src/

.. _notable_changes_in_nss_3.34:

`Notable Changes in NSS 3.34 <#notable_changes_in_nss_3.34>`__
--------------------------------------------------------------

.. container::

   -  The following CA certificates were **Added**:

      -  CN = GDCA TrustAUTH R5 ROOT

         -  SHA-256 Fingerprint:
            BF:FF:8F:D0:44:33:48:7D:6A:8A:A6:0C:1A:29:76:7A:9F:C2:BB:B0:5E:42:0F:71:3A:13:B9:92:89:1D:38:93
         -  Trust Flags: Websites

      -  CN = SSL.com Root Certification Authority RSA

         -  SHA-256 Fingerprint:
            85:66:6A:56:2E:E0:BE:5C:E9:25:C1:D8:89:0A:6F:76:A8:7E:C1:6D:4D:7D:5F:29:EA:74:19:CF:20:12:3B:69
         -  Trust Flags: Websites, Email

      -  CN = SSL.com Root Certification Authority ECC

         -  SHA-256 Fingerprint:
            34:17:BB:06:CC:60:07:DA:1B:96:1C:92:0B:8A:B4:CE:3F:AD:82:0E:4A:A3:0B:9A:CB:C4:A7:4E:BD:CE:BC:65
         -  Trust Flags: Websites, Email

      -  CN = SSL.com EV Root Certification Authority RSA R2

         -  SHA-256 Fingerprint:
            2E:7B:F1:6C:C2:24:85:A7:BB:E2:AA:86:96:75:07:61:B0:AE:39:BE:3B:2F:E9:D0:CC:6D:4E:F7:34:91:42:5C
         -  Trust Flags: Websites

      -  CN = SSL.com EV Root Certification Authority ECC

         -  SHA-256 Fingerprint:
            22:A2:C1:F7:BD:ED:70:4C:C1:E7:01:B5:F4:08:C3:10:88:0F:E9:56:B5:DE:2A:4A:44:F9:9C:87:3A:25:A7:C8
         -  Trust Flags: Websites

      -  CN = TrustCor RootCert CA-1

         -  SHA-256 Fingerprint:
            D4:0E:9C:86:CD:8F:E4:68:C1:77:69:59:F4:9E:A7:74:FA:54:86:84:B6:C4:06:F3:90:92:61:F4:DC:E2:57:5C
         -  Trust Flags: Websites, Email

      -  CN = TrustCor RootCert CA-2

         -  SHA-256 Fingerprint:
            07:53:E9:40:37:8C:1B:D5:E3:83:6E:39:5D:AE:A5:CB:83:9E:50:46:F1:BD:0E:AE:19:51:CF:10:FE:C7:C9:65
         -  Trust Flags: Websites, Email

      -  CN = TrustCor ECA-1

         -  SHA-256 Fingerprint:
            5A:88:5D:B1:9C:01:D9:12:C5:75:93:88:93:8C:AF:BB:DF:03:1A:B2:D4:8E:91:EE:15:58:9B:42:97:1D:03:9C
         -  Trust Flags: Websites, Email

   -  The following CA certificates were **Removed**:

      -  CN = Certum CA, O=Unizeto Sp. z o.o.

         -  SHA-256 Fingerprint:
            D8:E0:FE:BC:1D:B2:E3:8D:00:94:0F:37:D2:7D:41:34:4D:99:3E:73:4B:99:D5:65:6D:97:78:D4:D8:14:36:24

      -  CN = StartCom Certification Authority

         -  SHA-256 Fingerprint:
            C7:66:A9:BE:F2:D4:07:1C:86:3A:31:AA:49:20:E8:13:B2:D1:98:60:8C:B7:B7:CF:E2:11:43:B8:36:DF:09:EA

      -  CN = StartCom Certification Authority

         -  SHA-256 Fingerprint:
            E1:78:90:EE:09:A3:FB:F4:F4:8B:9C:41:4A:17:D6:37:B7:A5:06:47:E9:BC:75:23:22:72:7F:CC:17:42:A9:11

      -  CN = StartCom Certification Authority G2

         -  SHA-256 Fingerprint:
            C7:BA:65:67:DE:93:A7:98:AE:1F:AA:79:1E:71:2D:37:8F:AE:1F:93:C4:39:7F:EA:44:1B:B7:CB:E6:FD:59:95

      -  CN = TÜBİTAK UEKAE Kök Sertifika Hizmet Sağlayıcısı - Sürüm 3

         -  SHA-256 Fingerprint:
            E4:C7:34:30:D7:A5:B5:09:25:DF:43:37:0A:0D:21:6E:9A:79:B9:D6:DB:83:73:A0:C6:9E:B1:CC:31:C7:C5:2A

      -  CN = ACEDICOM Root

         -  SHA-256 Fingerprint:
            03:95:0F:B4:9A:53:1F:3E:19:91:94:23:98:DF:A9:E0:EA:32:D7:BA:1C:DD:9B:C8:5D:B5:7E:D9:40:0B:43:4A

      -  CN = Certinomis - Autorité Racine

         -  SHA-256 Fingerprint:
            FC:BF:E2:88:62:06:F7:2B:27:59:3C:8B:07:02:97:E1:2D:76:9E:D1:0E:D7:93:07:05:A8:09:8E:FF:C1:4D:17

      -  CN = TÜRKTRUST Elektronik Sertifika Hizmet Sağlayıcısı

         -  SHA-256 Fingerprint:
            97:8C:D9:66:F2:FA:A0:7B:A7:AA:95:00:D9:C0:2E:9D:77:F2:CD:AD:A6:AD:6B:A7:4A:F4:B9:1C:66:59:3C:50

      -  CN = PSCProcert

         -  SHA-256 Fingerprint:
            3C:FC:3C:14:D1:F6:84:FF:17:E3:8C:43:CA:44:0C:00:B9:67:EC:93:3E:8B:FE:06:4C:A1:D7:2C:90:F2:AD:B0

      -  CN = CA 沃通根证书, O=WoSign CA Limited

         -  SHA-256 Fingerprint:
            D6:F0:34:BD:94:AA:23:3F:02:97:EC:A4:24:5B:28:39:73:E4:47:AA:59:0F:31:0C:77:F4:8F:DF:83:11:22:54

      -  CN = Certification Authority of WoSign

         -  SHA-256 Fingerprint:
            4B:22:D5:A6:AE:C9:9F:3C:DB:79:AA:5E:C0:68:38:47:9C:D5:EC:BA:71:64:F7:F2:2D:C1:D6:5F:63:D8:57:08

      -  CN = Certification Authority of WoSign G2

         -  SHA-256 Fingerprint:
            D4:87:A5:6F:83:B0:74:82:E8:5E:96:33:94:C1:EC:C2:C9:E5:1D:09:03:EE:94:6B:02:C3:01:58:1E:D9:9E:16

      -  CN = CA WoSign ECC Root

         -  SHA-256 Fingerprint:
            8B:45:DA:1C:06:F7:91:EB:0C:AB:F2:6B:E5:88:F5:FB:23:16:5C:2E:61:4B:F8:85:56:2D:0D:CE:50:B2:9B:02

   -  libfreebl no longer requires SSE2 instructions.

.. _new_in_nss_3.34:

`New in NSS 3.34 <#new_in_nss_3.34>`__
--------------------------------------

.. _new_functionality:

`New Functionality <#new_functionality>`__
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. container::

   -  When listing an NSS database. using ``certutil -L``, and the database hasn't yet been
      initialized with any non-empty or empty password, the text "Database needs user init" will be
      included in the listing.
   -  When using certutil, to set an inacceptable password in FIPS mode, a correct explanation of
      acceptable passwords will be printed.
   -  SSLKEYLOGFILE is now supported with TLS 1.3, see `Bug
      1287711 <https://bugzilla.mozilla.org/show_bug.cgi?id=1287711>`__ for details.
   -  ``SSLChannelInfo`` has two new fields (Bug
      `1396525 <https://bugzilla.mozilla.org/show_bug.cgi?id=1396525>`__)

      -  ``SSLNamedGroup originalKeaGroup`` holds the key exchange group of the original handshake,
         when the session was resumed.
      -  ``PRBool resumed`` is ``PR_TRUE`` when the session is resumed, and ``PR_FALSE`` otherwise.

   -  RSA-PSS signatures are now supported on certificates.  Certificates with RSA-PSS or
      RSA-PKCS#1v1.5 keys can be used to create an RSA-PSS signature on a certificate, using the
      ``--pss-sign`` argument to ``certutil``.

   .. rubric:: New Functions
      :name: new_functions

.. _bugs_fixed_in_nss_3.34:

`Bugs fixed in NSS 3.34 <#bugs_fixed_in_nss_3.34>`__
----------------------------------------------------

.. container::

   This Bugzilla query returns all the bugs fixed in NSS 3.34:

   https://bugzilla.mozilla.org/buglist.cgi?resolution=FIXED&classification=Components&query_format=advanced&product=NSS&target_milestone=3.34

`Compatibility <#compatibility>`__
----------------------------------

.. container::

   NSS 3.34 shared libraries are backward compatible with all older NSS 3.x shared libraries. A
   program linked with older NSS 3.x shared libraries will work with NSS 3.34 shared libraries,
   without recompiling, or relinking. Furthermore, applications that restrict their use of NSS APIs
   to the functions listed in NSS Public Functions will remain compatible with future versions of
   the NSS shared libraries.

`Feedback <#feedback>`__
------------------------

.. container::

   Bugs discovered should be reported by filing a bug report with
   `bugzilla.mozilla.org <https://bugzilla.mozilla.org/enter_bug.cgi?product=NSS>`__ (select product
   'NSS').