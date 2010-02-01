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


// SYSTEM INCLUDES
#include    <e32base.h>

// USER INCLUDES
#include "lcregxmltagtype.h"
#include "lcregxmltaginfo.h"
#include "lcregxmlparserutils.h"

// CONSTANTS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TLcRegXmlTagInfo::TLcRegXmlTagInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TLcRegXmlTagInfo::TLcRegXmlTagInfo()
    {
    }

// -----------------------------------------------------------------------------
// TLcRegXmlTagInfo::TagTypeL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLcRegXmlTagType TLcRegXmlTagInfo::TagTypeL(
    const TDesC8& aTagName)
    {
    TLcRegXmlTagType toReturn = ELcXmlUnknown;

    if (EqualsTagL(aTagName, KLcXmlLocationCentreRegistration))
        {
        toReturn = ELcXmlLocationCentreRegistration;
        }
    else if (EqualsTagL(aTagName, KLcXmlService))
        {
        toReturn = ELcXmlService;
        }
    else if (EqualsTagL(aTagName, KLcXmlRegFileName))
        {
        toReturn = ELcXmlRegFileName;
        }
    else if (EqualsTagL(aTagName, KLcXmlFileIcon))
        {
        toReturn = ELcXmlFileIcon;
        }
    else if (EqualsTagL(aTagName, KLcXmlFeature))
        {
        toReturn = ELcXmlFeature;
        }
    else if (EqualsTagL(aTagName, KLcXmlRunMode))
        {
        toReturn = ELcXmlRunMode;
        }
    else if (EqualsTagL(aTagName, KLcXmlApplication))
        {
        toReturn = ELcXmlApplication;
        }
    else if (EqualsTagL(aTagName, KLcXmlWebService))
        {
        toReturn = ELcXmlWebService;
        }
    else if (EqualsTagL(aTagName, KLcXmlDocument))
        {
        toReturn = ELcXmlDocument;
        }
    else if (EqualsTagL(aTagName, KLcXmlApplicationNative))
        {
        toReturn = ELcXmlApplicationNative;
        }
    else if (EqualsTagL(aTagName, KLcXmlApplicationJava))
        {
        toReturn = ELcXmlApplicationJava;
        }
    else if (EqualsTagL(aTagName, KLcXmlApplicationParams))
        {
        toReturn = ELcXmlApplicationParams;
        }
    return toReturn;
    }

// -----------------------------------------------------------------------------
// TLcRegXmlTagInfo::TagAttrTypeL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLcRegXmlTagType TLcRegXmlTagInfo::TagAttrTypeL(TLcRegXmlTagType aTagType, 
    const TDesC8& aAttrName )
	{
	TLcRegXmlTagType toReturn = ELcXmlUnknown;
	
	if (EqualsTagL(aAttrName, KLcXmlServiceAttrUuid))
	    {
	    toReturn = ELcXmlServiceAttrUuid;
	    }
    else if( EqualsTagL(aAttrName, KLcXmlRegFileNameAttrLang) )	    
        {
        toReturn = ELcXmlRegFileNameAttrLang;
        }
    else if( EqualsTagL(aAttrName, KLcXmlIconAttrfile) )	    
        {
        if( aTagType == ELcXmlDocument )
	        {
	        toReturn = ELcXmlDocumentAttrFile;	
	        }
	    else
		    {
        	toReturn = ELcXmlFileIconAttrfile;
		    }
        }
    else if( EqualsTagL(aAttrName, KLcXmlIconAttrFrame) )	    
        {
        toReturn = ELcXmlFileIconAttrFrame;
        }        
    else if( EqualsTagL(aAttrName, KLcXmlRunModeAttrDefault) )	    
        {
        toReturn = ELcXmlRunModeAttrDefault;
        }
    else if( EqualsTagL(aAttrName, KLcXmlApplicationNativeAttrSid) )	    
        {
        toReturn = ELcXmlApplicationNativeAttrSid;
        }
    else if( EqualsTagL(aAttrName, KELcXmlApplicationJavaAttrName) )	    
        {
        toReturn = ELcXmlApplicationJavaAttrName;
        }
    else if( EqualsTagL(aAttrName, KLcXmlWebServiceAttrUrl) )	    
        {
        toReturn = ELcXmlWebServiceAttrUrl;
        }
    else if( EqualsTagL(aAttrName, KLcXmlDocumentAttrFile) )	    
        {
        toReturn = ELcXmlDocumentAttrFile;
        }
	return toReturn;		
	}
	
// -----------------------------------------------------------------------------
// TLcRegXmlTagInfo::EqualsTagL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool TLcRegXmlTagInfo::EqualsTagL(
    const TDesC8& aTag,
    const TDesC& aDefinedTag)
    {
    HBufC* tag = HBufC::NewLC(aTag.Length());
    TPtr16 tagptr = tag->Des();

    LcRegXmlParserUtils::Utf8ToUnicodeL(aTag, tagptr);

    TBool answer = (tag->CompareF(aDefinedTag) == 0);
    CleanupStack::PopAndDestroy(tag);
    return answer;
    }
//  End of File
