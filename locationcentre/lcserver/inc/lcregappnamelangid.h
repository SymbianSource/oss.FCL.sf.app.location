/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Registration information for a Location based Application.
*
*/


#ifndef T_LCREGAPPNAMELANGID_H
#define T_LCREGAPPNAMELANGID_H

// SYSTEM INCLUDES
#include <e32const.h>

// USER INCLUDE 

// FORWARD DECLARATIONS

// CONSTANT DEFINATION
//Language code  _LIT defination
_LIT( KLangEn, "en" ); 
_LIT( KLangFr, "fr" );
_LIT( KLangGe, "ge" );
_LIT( KLangSp, "sp" );
_LIT( KLangIt, "it" );
_LIT( KLangSw, "sw" );
_LIT( KLangDa, "da" );
_LIT( KLangNo, "no" );
_LIT( KLangFi, "fi" );
_LIT( KLangAm, "am" );
_LIT( KLangSf, "sf" );
_LIT( KLangSg, "sg" );
_LIT( KLangPo, "po" );
_LIT( KLangTu, "tu" );
_LIT( KLangIc, "ic" );
_LIT( KLangRu, "ru" );
_LIT( KLangHu, "hu" );
_LIT( KLangDu, "du" );
_LIT( KLangBl, "bl" );
_LIT( KLangAu, "au" );
_LIT( KLangBf, "bf" );
_LIT( KLangAs, "as" );
_LIT( KLangNz, "nz" );
_LIT( KLangIf, "if" );
_LIT( KLangCs, "cs" );
_LIT( KLangSk, "sk" );
_LIT( KLangPl, "pl" );
_LIT( KLangSl, "sl" );
_LIT( KLangTc, "tc" );
_LIT( KLangHk, "hk" );
_LIT( KLangZh, "zh" );
_LIT( KLangJa, "ja" );
_LIT( KLangTh, "th" );
_LIT( KLangAf, "af" );
_LIT( KLangSq, "sq" );
_LIT( KLangAh, "ah" );
_LIT( KLangAr, "ar" );
_LIT( KLangHy, "hy" );
_LIT( KLangTl, "tl" );
_LIT( KLangBe, "be" );
_LIT( KLangBn, "bn" );
_LIT( KLangBg, "bg" );
_LIT( KLangMy, "my" );
_LIT( KLangCa, "ca" );
_LIT( KLangHr, "hr" );
_LIT( KLangCe, "ce" );
_LIT( KLangIe, "ie" );
_LIT( KLangSa, "sa" );
_LIT( KLangEt, "et" );
_LIT( KLangFa, "fa" );
_LIT( KLangCf, "cf" );
_LIT( KLangGd, "gd" );
_LIT( KLangKa, "ka" );
_LIT( KLangEl, "el" );
_LIT( KLangCg, "cg" );
_LIT( KLangGu, "gu" );
_LIT( KLangHe, "he" );
_LIT( KLangHi, "hi" );
_LIT( KLangIn, "in" );
_LIT( KLangGa, "ga" );
_LIT( KLangSz, "sz" );
_LIT( KLangKn, "kn" );
_LIT( KLangKk, "kk" );
_LIT( KLangKm, "km" );
_LIT( KLangKo, "ko" );
_LIT( KLangLo, "lo" );
_LIT( KLangLv, "lv" );
_LIT( KLangLt, "lt" );
_LIT( KLangMk, "mk" );
_LIT( KLangMs, "ms" );
_LIT( KLangMl, "ml" );
_LIT( KLangMr, "mr" );
_LIT( KLangMo, "mo" );
_LIT( KLangMn, "mn" );
_LIT( KLangNn, "nn" );
_LIT( KLangBp, "bp" );
_LIT( KLangPa, "pa" );
_LIT( KLangRo, "ro" );
_LIT( KLangSr, "sr" );
_LIT( KLangSi, "si" );
_LIT( KLangSo, "so" );
_LIT( KLangOs, "os" );
_LIT( KLangLs, "ls" );
_LIT( KLangSh, "sh" );
_LIT( KLangFs, "fs" );
_LIT( KLangReserv1, "Reserved1" );
_LIT( KLangTa, "ta" );
_LIT( KLangTe, "te" );
_LIT( KLangBo, "bo" );
_LIT( KLangTi, "ti" );
_LIT( KLangCt, "ct" );
_LIT( KLangTk, "tk" );
_LIT( KLangUk, "uk" );
_LIT( KLangUr, "ur" );
_LIT( KLangReserv2, "Reserved2" );
_LIT( KLangVi, "vi" );
_LIT( KLangCy, "cy" );
_LIT( KLangZu, "zu" );


/**
 *  Structure of Language name table.
 *
 *	This structure stores the 
 *	language name and the language id for a service.
 *
 *  @since S60 v5.0
 */
struct TNameTable
	{
	const TDesC&		iName;
	TLanguage			iNumber;
	};

/**
 *
 *	This array contains all the 
 *	double character standard language code.
 *
 *  @since S60 v5.0
 */
const TNameTable KLangOptions[] = 
	{
		{KLangEn, ELangEnglish},
		{KLangFr, ELangFrench},
		{KLangGe, ELangGerman},
		{KLangSp, ELangSpanish},
		{KLangIt, ELangItalian},
		{KLangSw, ELangSwedish},
		{KLangDa, ELangDanish},
		{KLangNo, ELangNorwegian},
		{KLangFi, ELangFinnish},
		{KLangAm, ELangAmerican},
		{KLangSf, ELangSwissFrench},
		{KLangSg, ELangSwissGerman},
		{KLangPo, ELangPortuguese},
		{KLangTu, ELangTurkish},
		{KLangIc, ELangIcelandic},
		{KLangRu, ELangRussian},
		{KLangHu, ELangHungarian},
		{KLangDu, ELangDutch},
		{KLangBl, ELangBelgianFlemish},
		{KLangAu, ELangAustralian},
		{KLangBf, ELangBelgianFrench},
		{KLangAs, ELangAustrian},
		{KLangNz, ELangNewZealand},
		{KLangIf, ELangInternationalFrench},
		{KLangCs, ELangCzech},
		{KLangSk, ELangSlovak},
		{KLangPl, ELangPolish},
		{KLangSl, ELangSlovenian},
		{KLangTc, ELangTaiwanChinese},
		{KLangHk, ELangHongKongChinese},
		{KLangZh, ELangPrcChinese},
		{KLangJa, ELangJapanese},
		{KLangTh, ELangThai},
		{KLangAf, ELangAfrikaans },
		{KLangSq, ELangAlbanian },
		{KLangAh, ELangAmharic },
		{KLangAr, ELangArabic },
		{KLangHy, ELangArmenian },
		{KLangTl, ELangTagalog },
		{KLangBe, ELangBelarussian },
		{KLangBn, ELangBengali },
		{KLangBg, ELangBulgarian },
		{KLangMy, ELangBurmese },
		{KLangCa, ELangCatalan },
		{KLangHr, ELangCroatian },
		{KLangCe, ELangCanadianEnglish },
		{KLangIe, ELangInternationalEnglish },
		{KLangSa, ELangSouthAfricanEnglish },
		{KLangEt, ELangEstonian },
		{KLangFa, ELangFarsi },
		{KLangCf, ELangCanadianFrench },
		{KLangGd, ELangScotsGaelic },
		{KLangKa, ELangGeorgian },
		{KLangEl, ELangGreek },
		{KLangCg, ELangCyprusGreek },
		{KLangGu, ELangGujarati },
		{KLangHe, ELangHebrew },
		{KLangHi, ELangHindi },
		{KLangIn, ELangIndonesian },
		{KLangGa, ELangIrish },
		{KLangSz, ELangSwissItalian },
		{KLangKn, ELangKannada },
		{KLangKk, ELangKazakh },
		{KLangKm, ELangKhmer },
		{KLangKo, ELangKorean },
		{KLangLo, ELangLao },
		{KLangLv, ELangLatvian },
		{KLangLt, ELangLithuanian },
		{KLangMk, ELangMacedonian },
		{KLangMs, ELangMalay },
		{KLangMl, ELangMalayalam },
		{KLangMr, ELangMarathi },
		{KLangMo, ELangMoldavian },
		{KLangMn, ELangMongolian },
		{KLangNn, ELangNorwegianNynorsk },
		{KLangBp, ELangBrazilianPortuguese },
		{KLangPa, ELangPunjabi },
		{KLangRo, ELangRomanian },
		{KLangSr, ELangSerbian },
		{KLangSi, ELangSinhalese },
		{KLangSo, ELangSomali },
		{KLangOs, ELangInternationalSpanish },
		{KLangLs, ELangLatinAmericanSpanish },
		{KLangSh, ELangSwahili },
		{KLangFs, ELangFinlandSwedish },
		{KLangReserv1, ELangReserved1 },
		{KLangTa, ELangTamil },
		{KLangTe, ELangTelugu },
		{KLangBo, ELangTibetan },
		{KLangTi, ELangTigrinya },
		{KLangCt, ELangCyprusTurkish },
		{KLangTk, ELangTurkmen },
		{KLangUk, ELangUkrainian },
		{KLangUr, ELangUrdu },
		{KLangReserv2, ELangReserved2 },
		{KLangVi, ELangVietnamese },
		{KLangCy, ELangWelsh },
		{KLangZu, ELangZulu }
	};

#define NUMLANGOPTIONS (sizeof(KLangOptions)/sizeof(TNameTable))

#endif // T_LCREGAPPNAMELANGID_H