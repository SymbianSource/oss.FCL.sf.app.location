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
* Description:  Location Centre Server object.
*
*/



#ifndef T_LCREGXMLTAGTYPE_H
#define T_LCREGXMLTAGTYPE_H

//  INCLUDES
#include <e32def.h>
#include <e32std.h>

// DATA TYPES

enum TLcRegXmlTagType
    {
    /*
	 * All parent elements
     */
    ELcXmlNothing              							= 0,
    ELcXmlLocationCentreRegistration,
    ELcXmlFeature,
    ELcXmlApplication,
    ELcXmlApplicationParams,
    ELcXmlService,

    
	//childs
   	ELcXmlRegFileName,
    ELcXmlFileIcon,
    ELcXmlRunMode,
	ELcXmlWebService,
	ELcXmlDocument,
    ELcXmlApplicationNative,
    ELcXmlApplicationJava,

    
   /*
	* Attributes for different elements
	*/
    ELcXmlServiceAttrUuid,
    ELcXmlRegFileNameAttrLang,
    ELcXmlFileIconAttrfile,
    ELcXmlFileIconAttrFrame,
    ELcXmlRunModeAttrDefault,
    ELcXmlApplicationNativeAttrSid,
    ELcXmlApplicationJavaAttrName,
    ELcXmlWebServiceAttrUrl,
    ELcXmlDocumentAttrFile,
    ELcXmlUnknown
    };


// CONSTANTS

_LIT(KLcXmlVersionNum,         							"1.0");
_LIT(KLcXmlEncNameUTF8,        							"UTF-8");
_LIT(KLcXmlEncNameUTT16,       							"UTF-16");

_LIT(KLcXmlLocationCentreRegistration,                	"location_centre_registration");
_LIT(KLcXmlService,       								"service");
_LIT(KLcXmlServiceAttrUuid,        						"uuid");
_LIT(KLcXmlRegFileName,  								"name");
_LIT(KLcXmlRegFileNameAttrLang,        					"lang");
_LIT(KLcXmlFileIcon,                 					"icon");
_LIT8(KLcXmlFileIconAttrfile,      						"file");
_LIT8(KLcXmlFileIconAttrFrame,        					"frame");
_LIT(KLcXmlIconAttrfile,      						    "file");
_LIT(KLcXmlIconAttrFrame,        					    "frame");
_LIT(KLcXmlFeature,             						"feature");
_LIT(KLcXmlRunMode,      								"runmode");
_LIT(KLcXmlRunModeAttrDefault,    						"default");
_LIT(KLcXmlApplication,      							"application");
_LIT(KLcXmlApplicationNative,          					"native");
_LIT(KLcXmlApplicationNativeAttrSid,       				"sid");
_LIT(KLcXmlApplicationJava,         					"java");
_LIT(KELcXmlApplicationJavaAttrName,     				"name");
_LIT(KLcXmlApplicationParams,           				"params");
_LIT(KLcXmlWebService,   								"webService");
_LIT(KLcXmlWebServiceAttrUrl,         					"url");
_LIT(KLcXmlDocument,      								"document");
_LIT(KLcXmlDocumentAttrFile,      						"file");

//default run mode
_LIT(KLcAppRunModeStandalone,      						"standalone");
_LIT(KLcAppRunModeChild,      							"child");


//application character
_LIT(KLcAppCharSettings,      							"Settings");
_LIT(KLcAppCharPersonalData,      						"PersonalData");
_LIT(KLcAppCharNavigation,      						"Navigation");
_LIT(KLcAppCharMaps,      								"Maps");
_LIT(KLcAppCharLocationEvents,      					"LocationEvents");
_LIT(KLcAppCharRemotePositioning,      					"RemotePositioning");
_LIT(KLcAppCharNetworkService,      					"NetworkService");
_LIT(KLcAppCharLocationMessaging,      					"LocationMessaging");
_LIT(KLcAppCharLocalContent,      						"LocalContent");
_LIT(KLcAppCharRemoteContent,      						"RemoteContent");

#endif      // T_LCREGXMLTAGTYPE_H

// End of File
