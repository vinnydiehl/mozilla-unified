/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// List of standard font families present in a default English install
// of Ubuntu 22.04:
static const char* kBaseFonts_Ubuntu_22_04[] = {
  "aakar",
  "Abyssinica SIL",
  "Ani",
  "AnjaliOldLipi",
  "Bitstream Charter",
  "C059",
  "Chandas",
  "Chilanka",
  "Courier 10 Pitch",
  "D050000L",
  "DejaVu Sans",
  "DejaVu Sans Mono",
  "DejaVu Serif",
  "Dhurjati",
  "Droid Sans Fallback",
  "Dyuthi",
  "FreeMono",
  "FreeSans",
  "FreeSerif",
  "Gargi",
  "Garuda",
  "Gayathri",
  "Gayathri Thin",
  "Gidugu",
  "Gubbi",
  "Gurajada",
  "Jamrul",
  "KacstArt",
  "KacstBook",
  "KacstDecorative",
  "KacstDigital",
  "KacstFarsi",
  "KacstLetter",
  "KacstNaskh",
  "KacstOffice",
  "KacstOne",
  "KacstPen",
  "KacstPoster",
  "KacstQurn",
  "KacstScreen",
  "KacstTitle",
  "KacstTitleL",
  "Kalapi",
  "Kalimati",
  "Karumbi",
  "Keraleeyam",
  "Khmer OS",
  "Khmer OS System",
  "Kinnari",
  "LakkiReddy",
  "Laksaman",
  "Liberation Mono",
  "Liberation Sans",
  "Liberation Sans Narrow",
  "Liberation Serif",
  "Likhan",
  "LKLUG",
  "Lohit Assamese",
  "Lohit Bengali",
  "Lohit Devanagari",
  "Lohit Gujarati",
  "Lohit Gurmukhi",
  "Lohit Kannada",
  "Lohit Malayalam",
  "Lohit Odia",
  "Lohit Tamil",
  "Lohit Tamil Classical",
  "Lohit Telugu",
  "Loma",
  "Mallanna",
  "Mandali",
  "Manjari",
  "Manjari Thin",
  "Meera",
  "Mitra",
  "mry_KacstQurn",
  "Mukti",
  "Nakula",
  "NATS",
  "Navilu",
  "Nimbus Mono PS",
  "Nimbus Roman",
  "Nimbus Sans",
  "Nimbus Sans Narrow",
  "Norasi",
  "Noto Color Emoji",
  "Noto Mono",
  "Noto Sans CJK HK",
  "Noto Sans CJK JP",
  "Noto Sans CJK KR",
  "Noto Sans CJK SC",
  "Noto Sans CJK TC",
  "Noto Sans Mono",
  "Noto Sans Mono CJK HK",
  "Noto Sans Mono CJK JP",
  "Noto Sans Mono CJK KR",
  "Noto Sans Mono CJK SC",
  "Noto Sans Mono CJK TC",
  "Noto Serif CJK HK",
  "Noto Serif CJK JP",
  "Noto Serif CJK KR",
  "Noto Serif CJK SC",
  "Noto Serif CJK TC",
  "NTR",
  "OpenSymbol",
  "ori1Uni",
  "P052",
  "Padauk",
  "Padauk Book",
  "padmaa",
  "padmmaa",
  "Pagul",
  "Peddana",
  "Phetsarath OT",
  "Ponnala",
  "Pothana2000",
  "Potti Sreeramulu",
  "Purisa",
  "Rachana",
  "RaghuMalayalamSans",
  "Ramabhadra",
  "Ramaraja",
  "Rasa",
  "Rasa Light",
  "Rasa Medium",
  "Rasa SemiBold",
  "RaviPrakash",
  "Rekha",
  "Saab",
  "Sahadeva",
  "Samanata",
  "Samyak Devanagari",
  "Samyak Gujarati",
  "Samyak Malayalam",
  "Samyak Tamil",
  "Sarai",
  "Sawasdee",
  "Sree Krushnadevaraya",
  "Standard Symbols PS",
  "Suranna",
  "Suravaram",
  "Suruma",
  "Syamala Ramana",
  "TenaliRamakrishna",
  "Tibetan Machine Uni",
  "Timmana",
  "Tlwg Mono",
  "Tlwg Typewriter",
  "Tlwg Typist",
  "Tlwg Typo",
  "Ubuntu",
  "Ubuntu Condensed",
  "Ubuntu Light",
  "Ubuntu Mono",
  "Ubuntu Thin",
  "Umpush",
  "Uroob",
  "URW Bookman",
  "URW Gothic",
  "utkal",
  "Vemana2000",
  "Waree",
  "Yrsa",
  "Yrsa Light",
  "Yrsa Medium",
  "Yrsa SemiBold",
  "Z003",
  "गार्गी",
  "नालिमाटी",
  "অনি",
  "মুক্তি",
};

// Additional font families installed when all languages are enabled via the
// Language Support utility on Ubuntu 22.04:
static const char* kLangFonts_Ubuntu_22_04[] = {
  "AR PL UKai CN",
  "AR PL UKai HK",
  "AR PL UKai TW",
  "AR PL UKai TW MBE",
  "AR PL UMing CN",
  "AR PL UMing HK",
  "AR PL UMing TW",
  "AR PL UMing TW MBE",
  "Ezra SIL",
  "Ezra SIL SR",
  "Homa",
  "Khmer OS Battambang",
  "Khmer OS Bokor",
  "Khmer OS Content",
  "Khmer OS Fasthand",
  "Khmer OS Freehand",
  "Khmer OS Metal Chrieng",
  "Khmer OS Muol",
  "Khmer OS Muol Light",
  "Khmer OS Muol Pali",
  "Khmer OS Siemreap",
  "Nazli",
  "Noto Kufi Arabic",
  "Noto Looped Lao",
  "Noto Looped Lao Bold",
  "Noto Looped Lao Regular",
  "Noto Looped Lao UI",
  "Noto Looped Lao UI Bold",
  "Noto Looped Lao UI Regular",
  "Noto Looped Thai",
  "Noto Looped Thai Bold",
  "Noto Looped Thai Regular",
  "Noto Looped Thai UI",
  "Noto Looped Thai UI Bold",
  "Noto Looped Thai UI Regular",
  "Noto Music",
  "Noto Naskh Arabic",
  "Noto Naskh Arabic UI",
  "Noto Nastaliq Urdu",
  "Noto Rashi Hebrew",
  "Noto Sans",
  "Noto Sans Adlam",
  "Noto Sans Adlam Unjoined",
  "Noto Sans AnatoHiero",
  "Noto Sans Anatolian Hieroglyphs",
  "Noto Sans Arabic",
  "Noto Sans Arabic UI",
  "Noto Sans Armenian",
  "Noto Sans Avestan",
  "Noto Sans Balinese",
  "Noto Sans Bamum",
  "Noto Sans Bassa Vah",
  "Noto Sans Batak",
  "Noto Sans Bengali",
  "Noto Sans Bengali UI",
  "Noto Sans Bhaiksuki",
  "Noto Sans Brahmi",
  "Noto Sans Buginese",
  "Noto Sans Buhid",
  "Noto Sans CanAborig",
  "Noto Sans Canadian Aboriginal",
  "Noto Sans Carian",
  "Noto Sans CaucAlban",
  "Noto Sans Caucasian Albanian",
  "Noto Sans Chakma",
  "Noto Sans Cham",
  "Noto Sans Cherokee",
  "Noto Sans CJK HK Black",
  "Noto Sans CJK HK DemiLight",
  "Noto Sans CJK HK Light",
  "Noto Sans CJK HK Medium",
  "Noto Sans CJK HK Thin",
  "Noto Sans CJK JP Black",
  "Noto Sans CJK JP DemiLight",
  "Noto Sans CJK JP Light",
  "Noto Sans CJK JP Medium",
  "Noto Sans CJK JP Thin",
  "Noto Sans CJK KR Black",
  "Noto Sans CJK KR DemiLight",
  "Noto Sans CJK KR Light",
  "Noto Sans CJK KR Medium",
  "Noto Sans CJK KR Thin",
  "Noto Sans CJK SC Black",
  "Noto Sans CJK SC DemiLight",
  "Noto Sans CJK SC Light",
  "Noto Sans CJK SC Medium",
  "Noto Sans CJK SC Thin",
  "Noto Sans CJK TC Black",
  "Noto Sans CJK TC DemiLight",
  "Noto Sans CJK TC Light",
  "Noto Sans CJK TC Medium",
  "Noto Sans CJK TC Thin",
  "Noto Sans Coptic",
  "Noto Sans Cuneiform",
  "Noto Sans Cypriot",
  "Noto Sans Deseret",
  "Noto Sans Devanagari",
  "Noto Sans Devanagari UI",
  "Noto Sans Display",
  "Noto Sans Duployan",
  "Noto Sans EgyptHiero",
  "Noto Sans Egyptian Hieroglyphs",
  "Noto Sans Elbasan",
  "Noto Sans Elymaic",
  "Noto Sans Ethiopic",
  "Noto Sans Georgian",
  "Noto Sans Glagolitic",
  "Noto Sans Gothic",
  "Noto Sans Grantha",
  "Noto Sans Gujarati",
  "Noto Sans Gujarati UI",
  "Noto Sans Gunjala Gondi",
  "Noto Sans Gurmukhi",
  "Noto Sans Gurmukhi UI",
  "Noto Sans Hanifi Rohingya",
  "Noto Sans Hanunoo",
  "Noto Sans Hatran",
  "Noto Sans Hebrew",
  "Noto Sans ImpAramaic",
  "Noto Sans Imperial Aramaic",
  "Noto Sans Indic Siyaq Numbers",
  "Noto Sans Inscriptional Pahlavi",
  "Noto Sans Inscriptional Parthian",
  "Noto Sans InsPahlavi",
  "Noto Sans InsParthi",
  "Noto Sans Javanese",
  "Noto Sans Kaithi",
  "Noto Sans Kannada",
  "Noto Sans Kannada UI",
  "Noto Sans Kayah Li",
  "Noto Sans Kharoshthi",
  "Noto Sans Khmer",
  "Noto Sans Khmer UI",
  "Noto Sans Khojki",
  "Noto Sans Khudawadi",
  "Noto Sans Lao",
  "Noto Sans Lao UI",
  "Noto Sans Lepcha",
  "Noto Sans Limbu",
  "Noto Sans Linear A",
  "Noto Sans Linear B",
  "Noto Sans Lisu",
  "Noto Sans Lycian",
  "Noto Sans Lydian",
  "Noto Sans Mahajani",
  "Noto Sans Malayalam",
  "Noto Sans Malayalam UI",
  "Noto Sans Mandaic",
  "Noto Sans Manichaean",
  "Noto Sans Marchen",
  "Noto Sans Masaram Gondi",
  "Noto Sans Math",
  "Noto Sans Mayan Numerals",
  "Noto Sans Medefaidrin",
  "Noto Sans Meetei Mayek",
  "Noto Sans Mende Kikakui",
  "Noto Sans Meroitic",
  "Noto Sans Miao",
  "Noto Sans Modi",
  "Noto Sans Mongolian",
  "Noto Sans Mro",
  "Noto Sans Multani",
  "Noto Sans Myanmar",
  "Noto Sans Myanmar UI",
  "Noto Sans Nabataean",
  "Noto Sans New Tai Lue",
  "Noto Sans Newa",
  "Noto Sans NKo",
  "Noto Sans Nushu",
  "Noto Sans Ogham",
  "Noto Sans Ol Chiki",
  "Noto Sans Old Hungarian",
  "Noto Sans Old Italic",
  "Noto Sans Old North Arabian",
  "Noto Sans Old Permic",
  "Noto Sans Old Persian",
  "Noto Sans Old Sogdian",
  "Noto Sans Old South Arabian",
  "Noto Sans Old Turkic",
  "Noto Sans OldHung",
  "Noto Sans OldNorArab",
  "Noto Sans OldSouArab",
  "Noto Sans Oriya",
  "Noto Sans Oriya UI",
  "Noto Sans Osage",
  "Noto Sans Osmanya",
  "Noto Sans Pahawh Hmong",
  "Noto Sans Palmyrene",
  "Noto Sans Pau Cin Hau",
  "Noto Sans PhagsPa",
  "Noto Sans Phoenician",
  "Noto Sans Psalter Pahlavi",
  "Noto Sans PsaPahlavi",
  "Noto Sans Rejang",
  "Noto Sans Runic",
  "Noto Sans Samaritan",
  "Noto Sans Saurashtra",
  "Noto Sans Sharada",
  "Noto Sans Shavian",
  "Noto Sans Siddham",
  "Noto Sans SignWrit",
  "Noto Sans SignWriting",
  "Noto Sans Sinhala",
  "Noto Sans Sinhala UI",
  "Noto Sans Sogdian",
  "Noto Sans Sora Sompeng",
  "Noto Sans Soyombo",
  "Noto Sans Sundanese",
  "Noto Sans Syloti Nagri",
  "Noto Sans Symbols",
  "Noto Sans Symbols2",
  "Noto Sans Syriac",
  "Noto Sans Tagalog",
  "Noto Sans Tagbanwa",
  "Noto Sans Tai Le",
  "Noto Sans Tai Tham",
  "Noto Sans Tai Viet",
  "Noto Sans Takri",
  "Noto Sans Tamil",
  "Noto Sans Tamil Supplement",
  "Noto Sans Tamil UI",
  "Noto Sans Telugu",
  "Noto Sans Telugu UI",
  "Noto Sans Thaana",
  "Noto Sans Thai",
  "Noto Sans Thai UI",
  "Noto Sans Tifinagh",
  "Noto Sans Tifinagh Adrar",
  "Noto Sans Tifinagh Agraw Imazighen",
  "Noto Sans Tifinagh Ahaggar",
  "Noto Sans Tifinagh Air",
  "Noto Sans Tifinagh APT",
  "Noto Sans Tifinagh Azawagh",
  "Noto Sans Tifinagh Ghat",
  "Noto Sans Tifinagh Hawad",
  "Noto Sans Tifinagh Rhissa Ixa",
  "Noto Sans Tifinagh SIL",
  "Noto Sans Tifinagh Tawellemmet",
  "Noto Sans Tirhuta",
  "Noto Sans Ugaritic",
  "Noto Sans Vai",
  "Noto Sans Wancho",
  "Noto Sans Warang Citi",
  "Noto Sans Yi",
  "Noto Sans Zanabazar",
  "Noto Sans Zanabazar Square",
  "Noto Serif",
  "Noto Serif Ahom",
  "Noto Serif Armenian",
  "Noto Serif Balinese",
  "Noto Serif Bengali",
  "Noto Serif CJK HK Black",
  "Noto Serif CJK HK ExtraLight",
  "Noto Serif CJK HK Light",
  "Noto Serif CJK HK Medium",
  "Noto Serif CJK HK SemiBold",
  "Noto Serif CJK JP Black",
  "Noto Serif CJK JP ExtraLight",
  "Noto Serif CJK JP Light",
  "Noto Serif CJK JP Medium",
  "Noto Serif CJK JP SemiBold",
  "Noto Serif CJK KR Black",
  "Noto Serif CJK KR ExtraLight",
  "Noto Serif CJK KR Light",
  "Noto Serif CJK KR Medium",
  "Noto Serif CJK KR SemiBold",
  "Noto Serif CJK SC Black",
  "Noto Serif CJK SC ExtraLight",
  "Noto Serif CJK SC Light",
  "Noto Serif CJK SC Medium",
  "Noto Serif CJK SC SemiBold",
  "Noto Serif CJK TC Black",
  "Noto Serif CJK TC ExtraLight",
  "Noto Serif CJK TC Light",
  "Noto Serif CJK TC Medium",
  "Noto Serif CJK TC SemiBold",
  "Noto Serif Devanagari",
  "Noto Serif Display",
  "Noto Serif Dogra",
  "Noto Serif Ethiopic",
  "Noto Serif Georgian",
  "Noto Serif Grantha",
  "Noto Serif Gujarati",
  "Noto Serif Gurmukhi",
  "Noto Serif Hebrew",
  "Noto Serif Hmong Nyiakeng",
  "Noto Serif Kannada",
  "Noto Serif Khmer",
  "Noto Serif Khojki",
  "Noto Serif Lao",
  "Noto Serif Malayalam",
  "Noto Serif Myanmar",
  "Noto Serif Sinhala",
  "Noto Serif Tamil",
  "Noto Serif Tamil Slanted",
  "Noto Serif Tangut",
  "Noto Serif Telugu",
  "Noto Serif Thai",
  "Noto Serif Tibetan",
  "Noto Serif Yezidi",
  "Noto Traditional Nushu",
  "Scheherazade",
  "Titr",
  "UKIJ 3D",
  "UKIJ Basma",
  "UKIJ Bom",
  "UKIJ Chechek",
  "UKIJ Chiwer Kesme",
  "UKIJ CJK",
  "UKIJ Diwani",
  "UKIJ Diwani Kawak",
  "UKIJ Diwani Tom",
  "UKIJ Diwani Yantu",
  "UKIJ Ekran",
  "UKIJ Elipbe",
  "UKIJ Elipbe_Chekitlik",
  "UKIJ Esliye",
  "UKIJ Esliye Chiwer",
  "UKIJ Esliye Neqish",
  "UKIJ Esliye Qara",
  "UKIJ Esliye Tom",
  "UKIJ Imaret",
  "UKIJ Inchike",
  "UKIJ Jelliy",
  "UKIJ Junun",
  "UKIJ Kawak",
  "UKIJ Kawak 3D",
  "UKIJ Kesme",
  "UKIJ Kesme Tuz",
  "UKIJ Kufi",
  "UKIJ Kufi 3D",
  "UKIJ Kufi Chiwer",
  "UKIJ Kufi Gul",
  "UKIJ Kufi Kawak",
  "UKIJ Kufi Tar",
  "UKIJ Kufi Uz",
  "UKIJ Kufi Yay",
  "UKIJ Kufi Yolluq",
  "UKIJ Mejnun",
  "UKIJ Mejnuntal",
  "UKIJ Merdane",
  "UKIJ Moy Qelem",
  "UKIJ Nasq",
  "UKIJ Nasq Zilwa",
  "UKIJ Orqun Basma",
  "UKIJ Orqun Yazma",
  "UKIJ Orxun-Yensey",
  "UKIJ Qara",
  "UKIJ Qolyazma",
  "UKIJ Qolyazma Tez",
  "UKIJ Qolyazma Tuz",
  "UKIJ Qolyazma Yantu",
  "UKIJ Ruqi",
  "UKIJ Saet",
  "UKIJ Sulus",
  "UKIJ Sulus Tom",
  "UKIJ Teng",
  "UKIJ Tiken",
  "UKIJ Title",
  "UKIJ Tor",
  "UKIJ Tughra",
  "UKIJ Tuz",
  "UKIJ Tuz Basma",
  "UKIJ Tuz Gezit",
  "UKIJ Tuz Kitab",
  "UKIJ Tuz Neqish",
  "UKIJ Tuz Qara",
  "UKIJ Tuz Tom",
  "UKIJ Tuz Tor",
  "UKIJ Zilwa",
  "UKIJ_Mac Basma",
  "UKIJ_Mac Ekran",
};

// Standard fonts from previous LTS version Ubuntu 20.04:
static const char* kBaseFonts_Ubuntu_20_04[] = {
  "aakar",
  "Abyssinica SIL",
  "Ani",
  "AnjaliOldLipi",
  "Bitstream Charter",
  "C059",
  "Century Schoolbook L",
  "Chandas",
  "Chilanka",
  "Courier 10 Pitch",
  "D050000L",
  "DejaVu Sans",
  "DejaVu Sans Mono",
  "DejaVu Serif",
  "Dingbats",
  "Droid Sans Fallback",
  "Dyuthi",
  "FreeMono",
  "FreeSans",
  "FreeSerif",
  "Gargi",
  "Garuda",
  "Gayathri",
  "Gayathri Thin",
  "Gubbi",
  "Jamrul",
  "KacstArt",
  "KacstBook",
  "KacstDecorative",
  "KacstDigital",
  "KacstFarsi",
  "KacstLetter",
  "KacstNaskh",
  "KacstOffice",
  "KacstOne",
  "KacstPen",
  "KacstPoster",
  "KacstQurn",
  "KacstScreen",
  "KacstTitle",
  "KacstTitleL",
  "Kalapi",
  "Kalimati",
  "Karumbi",
  "Keraleeyam",
  "Khmer OS",
  "Khmer OS System",
  "Kinnari",
  "Laksaman",
  "Liberation Mono",
  "Liberation Sans",
  "Liberation Sans Narrow",
  "Liberation Serif",
  "Likhan",
  "LKLUG",
  "Lohit Assamese",
  "Lohit Bengali",
  "Lohit Devanagari",
  "Lohit Gujarati",
  "Lohit Gurmukhi",
  "Lohit Kannada",
  "Lohit Malayalam",
  "Lohit Odia",
  "Lohit Tamil",
  "Lohit Tamil Classical",
  "Lohit Telugu",
  "Loma",
  "Manjari",
  "Manjari Thin",
  "Meera",
  "Mitra Mono",
  "mry_KacstQurn",
  "Mukti Narrow",
  "Mukti Narrow Bold",
  "Nakula",
  "Navilu",
  "Nimbus Mono L",
  "Nimbus Mono PS",
  "Nimbus Roman",
  "Nimbus Roman No9 L",
  "Nimbus Sans",
  "Nimbus Sans L",
  "Nimbus Sans Narrow",
  "Norasi",
  "Noto Color Emoji",
  "Noto Mono",
  "Noto Sans CJK HK",
  "Noto Sans CJK JP",
  "Noto Sans CJK KR",
  "Noto Sans CJK SC",
  "Noto Sans CJK TC",
  "Noto Sans Mono CJK HK",
  "Noto Sans Mono CJK JP",
  "Noto Sans Mono CJK KR",
  "Noto Sans Mono CJK SC",
  "Noto Sans Mono CJK TC",
  "Noto Serif CJK JP",
  "Noto Serif CJK KR",
  "Noto Serif CJK SC",
  "Noto Serif CJK TC",
  "OpenSymbol",
  "ori1Uni",
  "P052",
  "Padauk",
  "Padauk Book",
  "padmaa",
  "padmaa-Bold.1.1",
  "padmmaa",
  "Pagul",
  "Phetsarath OT",
  "Pothana2000",
  "Purisa",
  "Rachana",
  "RaghuMalayalamSans",
  "Rasa",
  "Rasa Light",
  "Rasa Medium",
  "Rasa SemiBold",
  "Rekha",
  "Saab",
  "Sahadeva",
  "Samanata",
  "Samyak Devanagari",
  "Samyak Gujarati",
  "Samyak Malayalam",
  "Samyak Tamil",
  "Sarai",
  "Sawasdee",
  "Standard Symbols L",
  "Standard Symbols PS",
  "Suruma",
  "Tibetan Machine Uni",
  "Tlwg Mono",
  "Tlwg Typewriter",
  "Tlwg Typist",
  "Tlwg Typo",
  "Ubuntu",
  "Ubuntu Condensed",
  "Ubuntu Light",
  "Ubuntu Mono",
  "Ubuntu Thin",
  "Umpush",
  "Uroob",
  "URW Bookman",
  "URW Bookman L",
  "URW Chancery L",
  "URW Gothic",
  "URW Gothic L",
  "URW Palladio L",
  "utkal",
  "Vemana2000",
  "Waree",
  "Yrsa",
  "Yrsa Light",
  "Yrsa Medium",
  "Yrsa SemiBold",
  "Z003",
  "गार्गी",
  "नालिमाटी",
  "অনি  Dvf",
  "মিত্র",
  "মুক্তি",
  "মুক্তি  পাতনা",
};

// Additional font families installed when all languages are enabled via the
// Language Support utility on Ubuntu 20.04:
static const char* kLangFonts_Ubuntu_20_04[] = {
  "Aharoni CLM",
  "AlArabiya",
  "AlBattar",
  "AlHor",
  "AlManzomah",
  "AlYarmook",
  "Amiri",
  "Amiri Quran",
  "Amiri Quran Colored",
  "AR PL UKai CN",
  "AR PL UKai HK",
  "AR PL UKai TW",
  "AR PL UKai TW MBE",
  "AR PL UMing CN",
  "AR PL UMing HK",
  "AR PL UMing TW",
  "AR PL UMing TW MBE",
  "Arab",
  "Caladings CLM",
  "Cortoba",
  "David CLM",
  "Dimnah",
  "Drugulin CLM",
  "Electron",
  "Ellinia CLM",
  "Ezra SIL",
  "Ezra SIL SR",
  "Frank Ruehl CLM",
  "Furat",
  "Granada",
  "Graph",
  "Hadasim CLM",
  "Hani",
  "Haramain",
  "Homa",
  "Hor",
  "Japan",
  "Jet",
  "Kayrawan",
  "Keter YG",
  "Khalid",
  "Khmer OS Battambang",
  "Khmer OS Bokor",
  "Khmer OS Content",
  "Khmer OS Fasthand",
  "Khmer OS Freehand",
  "Khmer OS Metal Chrieng",
  "Khmer OS Muol",
  "Khmer OS Muol Light",
  "Khmer OS Muol Pali",
  "Khmer OS Siemreap",
  "Mashq",
  "Mashq-Bold",
  "Metal",
  "Miriam CLM",
  "Miriam Mono CLM",
  "Nachlieli CLM",
  "Nada",
  "Nagham",
  "Nazli",
  "Nice",
  "Noto Sans CJK HK Black",
  "Noto Sans CJK HK DemiLight",
  "Noto Sans CJK HK Light",
  "Noto Sans CJK HK Medium",
  "Noto Sans CJK HK Thin",
  "Noto Sans CJK JP Black",
  "Noto Sans CJK JP DemiLight",
  "Noto Sans CJK JP Light",
  "Noto Sans CJK JP Medium",
  "Noto Sans CJK JP Thin",
  "Noto Sans CJK KR Black",
  "Noto Sans CJK KR DemiLight",
  "Noto Sans CJK KR Light",
  "Noto Sans CJK KR Medium",
  "Noto Sans CJK KR Thin",
  "Noto Sans CJK SC Black",
  "Noto Sans CJK SC DemiLight",
  "Noto Sans CJK SC Light",
  "Noto Sans CJK SC Medium",
  "Noto Sans CJK SC Thin",
  "Noto Sans CJK TC Black",
  "Noto Sans CJK TC DemiLight",
  "Noto Sans CJK TC Light",
  "Noto Sans CJK TC Medium",
  "Noto Sans CJK TC Thin",
  "Noto Serif CJK JP Black",
  "Noto Serif CJK JP ExtraLight",
  "Noto Serif CJK JP Light",
  "Noto Serif CJK JP Medium",
  "Noto Serif CJK JP SemiBold",
  "Noto Serif CJK KR Black",
  "Noto Serif CJK KR ExtraLight",
  "Noto Serif CJK KR Light",
  "Noto Serif CJK KR Medium",
  "Noto Serif CJK KR SemiBold",
  "Noto Serif CJK SC Black",
  "Noto Serif CJK SC ExtraLight",
  "Noto Serif CJK SC Light",
  "Noto Serif CJK SC Medium",
  "Noto Serif CJK SC SemiBold",
  "Noto Serif CJK TC Black",
  "Noto Serif CJK TC ExtraLight",
  "Noto Serif CJK TC Light",
  "Noto Serif CJK TC Medium",
  "Noto Serif CJK TC SemiBold",
  "Ostorah",
  "Ouhod",
  "Ouhod-Bold",
  "Petra",
  "Rasheeq",
  "Rasheeq-Bold",
  "Rehan",
  "Salem",
  "Scheherazade",
  "Shado",
  "Sharjah",
  "Shofar",
  "Simple CLM",
  "Sindbad",
  "Stam Ashkenaz CLM",
  "Stam Sefarad CLM",
  "Tarablus",
  "Tholoth",
  "Titr",
  "UKIJ 3D",
  "UKIJ Basma",
  "UKIJ Bom",
  "UKIJ Chechek",
  "UKIJ Chiwer Kesme",
  "UKIJ CJK",
  "UKIJ Diwani",
  "UKIJ Diwani Kawak",
  "UKIJ Diwani Tom",
  "UKIJ Diwani Yantu",
  "UKIJ Ekran",
  "UKIJ Elipbe",
  "UKIJ Elipbe_Chekitlik",
  "UKIJ Esliye",
  "UKIJ Esliye Chiwer",
  "UKIJ Esliye Neqish",
  "UKIJ Esliye Qara",
  "UKIJ Esliye Tom",
  "UKIJ Imaret",
  "UKIJ Inchike",
  "UKIJ Jelliy",
  "UKIJ Junun",
  "UKIJ Kawak",
  "UKIJ Kawak 3D",
  "UKIJ Kesme",
  "UKIJ Kesme Tuz",
  "UKIJ Kufi",
  "UKIJ Kufi 3D",
  "UKIJ Kufi Chiwer",
  "UKIJ Kufi Gul",
  "UKIJ Kufi Kawak",
  "UKIJ Kufi Tar",
  "UKIJ Kufi Uz",
  "UKIJ Kufi Yay",
  "UKIJ Kufi Yolluq",
  "UKIJ Mejnun",
  "UKIJ Mejnuntal",
  "UKIJ Merdane",
  "UKIJ Moy Qelem",
  "UKIJ Nasq",
  "UKIJ Nasq Zilwa",
  "UKIJ Orqun Basma",
  "UKIJ Orqun Yazma",
  "UKIJ Orxun-Yensey",
  "UKIJ Qara",
  "UKIJ Qolyazma",
  "UKIJ Qolyazma Tez",
  "UKIJ Qolyazma Tuz",
  "UKIJ Qolyazma Yantu",
  "UKIJ Ruqi",
  "UKIJ Saet",
  "UKIJ Sulus",
  "UKIJ Sulus Tom",
  "UKIJ Teng",
  "UKIJ Tiken",
  "UKIJ Title",
  "UKIJ Tor",
  "UKIJ Tughra",
  "UKIJ Tuz",
  "UKIJ Tuz Basma",
  "UKIJ Tuz Gezit",
  "UKIJ Tuz Kitab",
  "UKIJ Tuz Neqish",
  "UKIJ Tuz Qara",
  "UKIJ Tuz Tom",
  "UKIJ Tuz Tor",
  "UKIJ Zilwa",
  "UKIJ_Mac Basma",
  "UKIJ_Mac Ekran",
  "Yehuda CLM",
  "מרים",
};

// The following Fedora font lists were obtained by installing @fonts and
// fontconfig into a container, then running this command:
// fc-list : family | tr ',' '\n' | sort -fu

// List of standard font families installed on Fedora 39 Workstation.
static const char* kBaseFonts_Fedora_39[] = {
  "Cantarell",
  "Jomolhari",
  "Madan",
  "Noto Color Emoji",
  "Noto Naskh Arabic",
  "Noto Sans",
  "Noto Sans Arabic",
  "Noto Sans Armenian",
  "Noto Sans Bengali",
  "Noto Sans Canadian Aboriginal",
  "Noto Sans Canadian Aboriginal Light",
  "Noto Sans Cherokee",
  "Noto Sans CJK HK",
  "Noto Sans CJK JP",
  "Noto Sans CJK KR",
  "Noto Sans CJK SC",
  "Noto Sans CJK TC",
  "Noto Sans Devanagari",
  "Noto Sans Ethiopic",
  "Noto Sans Georgian",
  "Noto Sans Gujarati",
  "Noto Sans Gurmukhi",
  "Noto Sans Hebrew",
  "Noto Sans Kannada",
  "Noto Sans Khmer",
  "Noto Sans Lao",
  "Noto Sans Math",
  "Noto Sans Mono",
  "Noto Sans Mono CJK HK",
  "Noto Sans Mono CJK JP",
  "Noto Sans Mono CJK KR",
  "Noto Sans Mono CJK SC",
  "Noto Sans Mono CJK TC",
  "Noto Sans Oriya",
  "Noto Sans Sinhala",
  "Noto Sans Symbols",
  "Noto Sans Symbols 2",
  "Noto Sans Tamil",
  "Noto Sans Telugu",
  "Noto Sans Thaana",
  "Noto Sans Thai",
  "Noto Serif",
  "Noto Serif Armenian",
  "Noto Serif Bengali",
  "Noto Serif CJK HK",
  "Noto Serif CJK JP",
  "Noto Serif CJK KR",
  "Noto Serif CJK SC",
  "Noto Serif CJK TC",
  "Noto Serif Devanagari",
  "Noto Serif Georgian",
  "Noto Serif Gujarati",
  "Noto Serif Gurmukhi",
  "Noto Serif Hebrew",
  "Noto Serif Kannada",
  "Noto Serif Khmer",
  "Noto Serif Lao",
  "Noto Serif Oriya",
  "Noto Serif Sinhala",
  "Noto Serif Tamil",
  "Noto Serif Telugu",
  "Noto Serif Thai",
  "Nuosu SIL",
  "Padauk",
  "PakType Naskh Basic",
  "RIT Meera New",
  "RIT Rachana",
  "STIX",
  "STIX Two Math",
  "STIX Two Text",
  "STIX Two Text Medium",
  "STIX Two Text SemiBold",
  "Vazirmatn",
  "ആർഐടി രചന",
  "രചന",
};

// List of standard font families installed on Fedora 38 Workstation.
static const char* kBaseFonts_Fedora_38[] = {
  "Cantarell",
  "Jomolhari",
  "Liberation Mono",
  "Liberation Sans",
  "Liberation Serif",
  "Lohit Assamese",
  "Lohit Bengali",
  "Lohit Devanagari",
  "Lohit Gujarati",
  "Lohit Kannada",
  "Lohit Marathi",
  "Lohit Odia",
  "Lohit Tamil",
  "Lohit Telugu",
  "Mingzat",
  "Noto Color Emoji",
  "Noto Naskh Arabic",
  "Noto Sans",
  "Noto Sans Arabic",
  "Noto Sans Armenian",
  "Noto Sans Canadian Aboriginal",
  "Noto Sans Cherokee",
  "Noto Sans CJK HK",
  "Noto Sans CJK JP",
  "Noto Sans CJK KR",
  "Noto Sans CJK SC",
  "Noto Sans CJK TC",
  "Noto Sans Ethiopic",
  "Noto Sans Georgian",
  "Noto Sans Gurmukhi",
  "Noto Sans Hebrew",
  "Noto Sans Khmer",
  "Noto Sans Lao",
  "Noto Sans Math",
  "Noto Sans Mono",
  "Noto Sans Mono CJK HK",
  "Noto Sans Mono CJK JP",
  "Noto Sans Mono CJK KR",
  "Noto Sans Mono CJK SC",
  "Noto Sans Mono CJK TC",
  "Noto Sans Sinhala",
  "Noto Sans Thaana",
  "Noto Sans Thai",
  "Noto Serif",
  "Nuosu SIL",
  "Padauk",
  "PakType Naskh Basic",
  "RIT Meera New",
  "STIX",
  "STIX Two Math",
  "STIX Two Text",
  "STIX Two Text Medium",
  "STIX Two Text SemiBold",
  "Vazirmatn",
};
