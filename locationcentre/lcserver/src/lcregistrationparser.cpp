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


// INCLUDE FILES
#include <xml/parser.h>
#include <xml/matchdata.h>
#include <utf.h>
#include <xml/xmlparsererrors.h>
#include <javaregistryincludes.h>

// USER INCLUDE
#include "lcregistrationparser.h"
#include "lcregxmltaginfo.h"
#include "lcregxmlparserutils.h"
#include "lcregxmltagtype.h"
#include "lcregappinfo.h"

#include "lcdebug.h"

// CONSTANTS
/**
 *  Maximum length of UUID of a service can be
 */
const TInt KUUIDMaxLength = 1028;
const TInt KMaxLangCodeLen = 2;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLcRegistrationParser::CLcRegistrationParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLcRegistrationParser::CLcRegistrationParser( RFs& aFs ):
	iFs( aFs )
    {
    iParserError = KErrNone;
    }

// -----------------------------------------------------------------------------
// CPosLmXmlEncoder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLcRegistrationParser* CLcRegistrationParser::NewL( RFs& aFs )
    {
	CLcRegistrationParser* self = new( ELeave ) CLcRegistrationParser( aFs );    
	return self;
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::~CLcRegistrationParser()
// Destructor.
// -----------------------------------------------------------------------------
//
CLcRegistrationParser::~CLcRegistrationParser()
    {
    delete iReadContent;
    
    iParserTreeTags.Reset();
    iParserTreeTags.Close();
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::ParseFileL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CLcRegAppInfo* CLcRegistrationParser::ParseFileL( const TDesC& aFile )
    {
    DEBUG("+ CLcRegistrationParser::ParseFileL" )
    CLcRegAppInfo* regAppInfo = CLcRegAppInfo::NewL();
    CleanupStack::PushL( regAppInfo ); 
    
	HBufC* attributeBuf = HBufC::NewLC( KUUIDMaxLength );
	    
	iRegAppInfo = regAppInfo;
    iAttributeBuf = attributeBuf;
    // Check the file name here.
    if( aFile.Length() <= KMaxFileName )
	    {
    	iFileName.Copy( aFile );	    	
	    }
	else
		{
		DEBUG("Error : Bad file name")
		User::Leave( KErrBadName );	
		}
        
    iParserTreeTags.Reset();
	iParsingTag = ELcXmlNothing;
	iParsedTag = ELcXmlNothing;

	_LIT8(KXmlMimeType, "text/xml");
        
    // Create Symbian XML parser
	// Create parser
	
    Xml::CParser* iRegParser = Xml::CParser::NewLC( KXmlMimeType, *this );
    
    iRegParser->ParseBeginL();
    // check the error code return from the parser
	
	Xml::ParseL( *iRegParser, iFs, aFile );
	// Leave here if Parser framework found some error
	User::LeaveIfError( iParserError );
	
	CleanupStack::PopAndDestroy( 2, attributeBuf );
	CleanupStack::Pop( regAppInfo );
    DEBUG("- CLcRegistrationParser::ParseFileL" )	
	return regAppInfo;		
	}

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnStartDocumentL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnStartDocumentL(
    const Xml::RDocumentParameters& /*aDocParam*/,
    TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnEndDocumentL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnEndDocumentL(TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnStartElementL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnStartElementL(
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes,
    TInt /*aErrorCode*/ )
    {
    DEBUG("+ CLcRegistrationParser::OnStartElementL" )
    delete iReadContent;
    iReadContent = NULL;
	// get the tag name and validate it
    TLcRegXmlTagType startTag =
        TLcRegXmlTagInfo::TagTypeL( aElement.LocalName().DesC() );

	if( startTag == ELcXmlUnknown )
		{
		User::Leave( KErrNotSupported );	
		}

    SetParserTagL( startTag );

    // Check parser status
    CheckMandatoryTagsL( EXmlParserOnStartElement, startTag );
	
	//Collect attribute value for element
	CollectAttributeValueL( aElement, aAttributes );
	
    DEBUG("- CLcRegistrationParser::OnStartElementL" )	
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::SetParserTagL
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::SetParserTagL( TLcRegXmlTagType aTag )
    {
    DEBUG("+ CLcRegistrationParser::SetParserTagL" )
        
	if( !iParserTreeTags.Count() )
		{
		if( aTag == ELcXmlLocationCentreRegistration )
			{
			iParserTreeTags.Append( aTag );				
			}
		else
			{
			DEBUG("CLcRegistrationParser::SetParserTagL Not Supported" )
			
			User::Leave( KErrNotSupported );				
			}			
		}
	else
		{
		if( iParserTreeTags.Find( aTag ) == KErrNotFound )
			{
			User::LeaveIfError( iParserTreeTags.Append( aTag ) );
			CheckSequenceOrderL( aTag );
			}
		else
			{
			User::Leave( EXmlRecursiveEntity );	
			}			
		}		
    DEBUG("- CLcRegistrationParser::SetParserTagL" )				
    }


// -----------------------------------------------------------------------------
// CLcRegistrationParser::CheckSequenceOrderL
//
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::CheckSequenceOrderL( TLcRegXmlTagType aTag )
    {
    DEBUG("+ CLcRegistrationParser::CheckSequenceOrderL" )
    
    switch ( aTag )
        {
        case ELcXmlLocationCentreRegistration:
	        {
	        if( iParserTreeTags.Count() > 1 )
		        {
				User::Leave( KErrNotSupported );		        	
		        }
	        break;
	        }
		case ELcXmlService:
			{
			if( iParserTreeTags.Count()  != 1 &&
				iParserTreeTags.Find( ELcXmlLocationCentreRegistration ) == KErrNotFound )
				{
				User::Leave( KErrNotSupported );	
				}
			break;	
			}
        case ELcXmlRegFileName:
        case ELcXmlFileIcon:
        case ELcXmlFeature:
        case ELcXmlRunMode:
        case ELcXmlWebService:
        case ELcXmlDocument:
        case ELcXmlApplication:
	        {
	        if( iParserTreeTags[1] != ELcXmlService )
		        {
				User::Leave( KErrNotSupported );		        	
		        }
	        break;
	        }
		case ELcXmlApplicationNative:
        case ELcXmlApplicationJava:
        case ELcXmlApplicationParams:
	        {
	        if( iParserTreeTags.Find( ELcXmlApplication ) == KErrNotFound  ||
	        	iParserTreeTags[1] != ELcXmlService  
	        	)
		        {
				User::Leave( KErrNotSupported );		        	
		        }
	        break;
	        }	        	        
        default:
            break;
        }
        
    DEBUG("- CLcRegistrationParser::CheckSequenceOrderL" )        
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::CheckMandatoryTagsL
//
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::CheckMandatoryTagsL(
    TXmlParserElementState aXmlState,
    TLcRegXmlTagType aTag )
    {
    DEBUG("+ CLcRegistrationParser::CheckMandatoryTagsL" )
       
    if ( aXmlState == EXmlParserOnStartElement ) 
        {
        switch ( aTag )
            {
            case ELcXmlApplication:
	            {
	            if( iParsedTag != ELcXmlRunMode )
		            {
		            User::Leave( KErrNotSupported );	
		            }
				iParsingTag = ELcXmlApplication;		            
	            }
	            break;	
            case ELcXmlWebService:
            case ELcXmlDocument:
	            {
	            if( iParsedTag != ELcXmlRunMode )
		            {
		            User::Leave( KErrNotSupported );	
		            }
	            }
	            break;	
            case ELcXmlApplicationNative:
            case ELcXmlApplicationJava:
            case ELcXmlApplicationParams:
	            {
	            if( iParsingTag != ELcXmlApplication )
		            {
		            User::Leave( KErrNotSupported );	
		            }
	            break;	
	            }
            default:
                break;
            }
        }
    else
        {
        // aXmlState == EXmlParserOnEndElement
        TInt count = iParserTreeTags.Count();
        if( count > 0 )
	        {
	        switch ( aTag )
	            {
	            case ELcXmlLocationCentreRegistration:
		            {
		            if( iParsedTag != ELcXmlService )
			            {
			            User::Leave( KErrNotSupported );	
			            }
		            break;	
		            }
	            case ELcXmlService:
		            {
		            if( iParsedTag != ELcXmlApplication )
			            {
			            if( iParsedTag != ELcXmlDocument )
				            {
				            if( iParsedTag != ELcXmlWebService )
					            {
					            User::Leave( KErrNotSupported );	
					            }
				            }
			            }
		            iParsedTag = ELcXmlService;
		            break;	
		            }
	            case ELcXmlRunMode:
		            {
		            iParsedTag = ELcXmlRunMode;
		            break;	
		            }
	            case ELcXmlWebService:
		            {
		            iParsedTag = ELcXmlWebService;
		            break;	
		            }
	            case ELcXmlDocument:
		            {
		            iParsedTag = ELcXmlDocument;
		            break;	
		            }
	            case ELcXmlApplication:
		            {
		            iParsedTag = ELcXmlApplication;
		            break;	
		            }
	            default:
	                break;
	            }
	        iParserTreeTags.Remove( count-1 );
	        }
        }
    DEBUG("- CLcRegistrationParser::CheckMandatoryTagsL" )        
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::CheckAndReturnValidAttrL
// -----------------------------------------------------------------------------
//
TLcRegXmlTagType CLcRegistrationParser::CheckAndReturnValidAttrL(
	TLcRegXmlTagType aElement,
	const Xml::RAttributeArray& aAttributes )
	{
	DEBUG("+ CLcRegistrationParser::CheckAndReturnValidAttrL" )
	
    TInt count( aAttributes.Count() );	
	switch( aElement )
		{
		case ELcXmlLocationCentreRegistration:
		case ELcXmlFeature:
		case ELcXmlApplication:
		case ELcXmlApplicationParams:				
			{
			if( count != 0 )
				{
				User::Leave( KErrNotSupported );	
				}
			break;	
			}		
		case ELcXmlService:
		case ELcXmlRegFileName:
		case ELcXmlRunMode:
		case ELcXmlWebService:
		case ELcXmlDocument:
		case ELcXmlApplicationNative:
		case ELcXmlApplicationJava:
			{
			// If all the above element's doesn't have any attribute
			// invalid registration file.
			if( count == 0 ||
				count > 1 )
				{
				User::Leave( KErrNotSupported );	
				}
			break;
			}
		case ELcXmlFileIcon:
			{
			// If all the above element's doesn't have any attribute
			// invalid registration file.
			if( count != 2 )
				{
				User::Leave( KErrNotSupported );	
				}
			break;
			}	
		default:
			break;			
		}
    
    // Checking attribute name validity    
    TLcRegXmlTagType tagAttr = ELcXmlUnknown;
    // validate the attribute name.        
    for( TInt i = 0; i < count; i++ )    
        {
        tagAttr =
            TLcRegXmlTagInfo::TagAttrTypeL( aElement,
            			aAttributes[i].Attribute().LocalName().DesC() );
        
        // Check the tag attribute value 
        // if it is not according to XML schema then it's a invalid registration file    
    	if( aElement > ELcXmlApplicationParams && tagAttr == ELcXmlUnknown )
    		{
    		User::Leave( KErrNotSupported );	
    		}
        }
    DEBUG("- CLcRegistrationParser::CheckAndReturnValidAttrL" )
            
	return tagAttr;        
	}

// -----------------------------------------------------------------------------
// CLcRegistrationParser::CollectAttributeValueL
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::CollectAttributeValueL(
    const Xml::RTagInfo& aElement,
    const Xml::RAttributeArray& aAttributes )
    {
    DEBUG("+ CLcRegistrationParser::CollectAttributeValueL")
        
    TInt count( aAttributes.Count() );
    TParse parse;
    TBuf<KMaxFileName> fileName( iFileName );
    
    TLcRegXmlTagType tag =
        TLcRegXmlTagInfo::TagTypeL( aElement.LocalName().DesC() );
        
	// Check if the element's defiened in XML registration API
	// schema have any valid attribute if no attribute and attribute
	// is not valid one then don't registered.
	TLcRegXmlTagType tagAttr = CheckAndReturnValidAttrL( tag, aAttributes );
    
    DEBUG1(" Tag : %d", tagAttr)
    
    // If validity done then collect attributa value    
	switch ( tagAttr )
		{
		case ELcXmlServiceAttrUuid:
			{
            TPtrC8 uidValue( aAttributes[0].Value().DesC() );
            if( uidValue.Length() > KUUIDMaxLength ||
                uidValue.Length() <= 0 )
	            {
	            User::Leave( KErrNotSupported );	
	            }
			iAttributeBuf->Des().Copy( uidValue );	
            iRegAppInfo->SetIdL( *iAttributeBuf );
			}
		    break;
		case ELcXmlRegFileNameAttrLang:
			{
            TPtrC8 langValue( aAttributes[0].Value().DesC() );
            if( langValue.Length() > KMaxLangCodeLen )
	            {
	            User::Leave( KErrNotSupported );	
	            }
            iAttributeBuf->Des().Copy( langValue );
            User::LeaveIfError( iRegAppInfo->SetNameLanguage( *iAttributeBuf ) );
			}
		    break;
		case ELcXmlFileIconAttrfile:
		case ELcXmlFileIconAttrFrame:
			{
			// Set the file name and type
			for (TInt i = 0; i< count; i++ )
				{
				if( !aAttributes[i].Attribute().LocalName().DesC().Compare( KLcXmlFileIconAttrfile ) )	
					{
		            TPtrC8 iconfile( aAttributes[i].Value().DesC() );
		            if( iconfile.Length() > KMaxFileName )
			            {
			            User::Leave( KErrNotSupported );	
			            }
		            iAttributeBuf->Des().Copy( iconfile );
		            // Check the drive letter of the document name
		            // if no drive letter then append drive letter of registration file name
					parse.Set( *iAttributeBuf,NULL,NULL );
					if( parse.Drive().Length() == 0 )
						{
						parse.Set(fileName,NULL,NULL);
						fileName.Zero();
						fileName.Append( parse.Drive() );
						fileName.Append( *iAttributeBuf );
						iAttributeBuf->Des().Copy( fileName );	
						}
		            iRegAppInfo->SetIconFileL( *iAttributeBuf );
		            iRegAppInfo->SetIconFileType( CLcAppInfo::EMifFile );		
					}
				else if( !aAttributes[i].Attribute().LocalName().DesC().Compare( KLcXmlFileIconAttrFrame ) )
					{
					// Set the frame no.
		            TPtrC8 iconFrame( aAttributes[i].Value().DesC() );
		            if( iconFrame.Length() > sizeof( TUint32 ) )
			            {
			            User::Leave( KErrNotSupported );	
			            }
		            iAttributeBuf->Des().Copy( iconFrame );
			    	TLex lexer( *iAttributeBuf );
				    TUint32 frameNo = 0;
				    TUint limit = 0xFFFFFFFF; 
				    User::LeaveIfError( lexer.BoundedVal( frameNo, EHex, limit ) );
            		iRegAppInfo->SetFrameNo( frameNo );
					}
				else
					{
					User::Leave( KErrNotSupported );	
					}
				}
			}
		    break;
		case ELcXmlRunModeAttrDefault:
			{
            TPtrC8 runMode( aAttributes[0].Value().DesC() );
            if( runMode.Length() > KLcAppRunModeStandalone.iTypeLength )
	            {
	            User::Leave( KErrNotSupported );	
	            }
            iAttributeBuf->Des().Copy( runMode );
            if( !iAttributeBuf->Des().Compare( KLcAppRunModeStandalone ) )
                {
                iRegAppInfo->SetLaunchMode( CLcLocationAppInfo::EStandAloneMode );			
                }
            else if( !iAttributeBuf->Des().Compare(KLcAppRunModeChild) )
                {
                iRegAppInfo->SetLaunchMode( CLcLocationAppInfo::EChainedMode );			
                }
            else
	            {
				User::Leave( KErrNotSupported );		            	
	            }
			}
		    break;
		case ELcXmlApplicationNativeAttrSid:
			{
            TPtrC8 appSid( aAttributes[0].Value().DesC() );
            // length must be greater than zero
            if( appSid.Length() <= 0 )
	            {
	            User::Leave( KErrNotSupported );	
	            }
            // Check if the SID is in [SID] format.
            // If it is then remove the '[' and ']' from it.
            if( appSid[0] == '[' &&
            	appSid[appSid.Length() - 1 ] == ']' )
	            {
	            // Check SID length.If it is zero or
	            // greater then 10(because sid is 8 and then
	            // two angle brackets equals to 10) dont't registered
	            if( appSid.Length() > 10 )
	                {
	                User::Leave( KErrNotSupported );
	                }
	            iAttributeBuf->Des().Zero();
	            for(TInt i = 0; i < appSid.Length() - 2; i++ )
		            {
		            iAttributeBuf->Des().Append( appSid[i+1] );	
		            }
	            }
			else
				{
				User::Leave( KErrNotSupported );	
				}	            
            iRegAppInfo->SetApplicationDataL( *iAttributeBuf );
            iRegAppInfo->SetApplicationType( ELcNativeApplication );
			}
		    break;
		case ELcXmlApplicationJavaAttrName:
			{


			// Obtain the UID corresponding to the Java Application
			// Midlet name.
            TPtrC8 javaName( aAttributes[0].Value().DesC() );
            if( javaName.Length() > KUUIDMaxLength )
	            {
	            User::Leave( KErrNotSupported );	
	            }	            
            iAttributeBuf->Des().Copy(javaName);
            iAttributeBuf->Des().Format(_L("%x"), JavaAppUidL( *iAttributeBuf ) );
                        
            iRegAppInfo->SetApplicationDataL( *iAttributeBuf );
            iRegAppInfo->SetApplicationType(ELcNativeApplication);
            

			}
		    break;
		case ELcXmlWebServiceAttrUrl:
			{
            TPtrC8 webUri( aAttributes[0].Value().DesC() );
            if( webUri.Length() > KUUIDMaxLength )
	            {
	            User::Leave( KErrNotSupported );	
	            }
            iAttributeBuf->Des().Copy( webUri );
            iRegAppInfo->SetApplicationDataL( *iAttributeBuf );
            iRegAppInfo->SetApplicationType( ELcWebUrl );
			}
		    break;
		case ELcXmlDocumentAttrFile:
			{
            TPtrC8 docFile( aAttributes[0].Value().DesC() );
            if( docFile.Length() > KMaxFileName )
	            {
	            User::Leave( KErrNotSupported );	
	            }
            iAttributeBuf->Des().Copy( docFile );
            // Check the drive letter of the document name
            // if no drive letter then append drive letter of registration file name
			parse.Set( *iAttributeBuf,NULL,NULL );
			if( parse.Drive().Length() == 0 )
				{
				parse.Set(fileName,NULL,NULL);
				fileName.Zero();
				fileName.Append( parse.Drive() );
				fileName.Append( *iAttributeBuf );
				iAttributeBuf->Des().Copy( fileName );	
				}
            iRegAppInfo->SetApplicationDataL( *iAttributeBuf );
            iRegAppInfo->SetApplicationType( ELcDocument );
			}
		    break;
		default:
			break;
		}
		
    DEBUG("- CLcRegistrationParser::CollectAttributeValueL")		
    }

// ---------------------------------------------------------------------------
// void CLcRegistrationParser::JavaAppUidL
// ---------------------------------------------------------------------------
//
TUint32 CLcRegistrationParser::JavaAppUidL( const TDesC&	aMidletName )
	{
	TUint32 uidValue = 0;
	DEBUG("+ CLcRegistrationParser::JavaAppUidL")
	
	// Create a new Registry class
	Java::CJavaRegistry* registry = Java::CJavaRegistry::NewLC();
	
	// Obtain all the UIDs and check if the required UID corresponding
	// to the Midlet name exists
	RArray<TUid> uidArray;
	CleanupClosePushL( uidArray );
	registry->GetRegistryEntryUidsL( uidArray );
	
	for( TInt i = 0; i < uidArray.Count(); i++ )
		{
		// Obtain the Registry entry corresponding to the current UID
		Java::CJavaRegistryEntry* registryEntry = registry->RegistryEntryL( uidArray[i]);
		if( registryEntry )
			{
			if ( !aMidletName.Compare( registryEntry->Name() ) )
			    {
			    uidValue = ( registryEntry->Uid().iUid );
			    delete registryEntry;
			    break;
			    }
			}
		delete registryEntry;
		}
	
	CleanupStack::PopAndDestroy( 2, registry ); // uidArray	
    DEBUG1("UID %d", uidValue)
	DEBUG("- CLcRegistrationParser::JavaAppUidL")
	return uidValue;
	}

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnEndElementL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnEndElementL(
    const Xml::RTagInfo& aElement,
    TInt /*aErrorCode*/ )
    {
    DEBUG("+ CLcRegistrationParser::OnEndElementL" )
    
    // End element
    TLcRegXmlTagType endTag =
        TLcRegXmlTagInfo::TagTypeL( aElement.LocalName().DesC() );
	
	if( endTag == ELcXmlUnknown )
		{
		User::Leave( KErrNotSupported );	
		}
    // Check parser status
    CheckMandatoryTagsL( EXmlParserOnEndElement, endTag );

    // Read property/content for end element
    if (iReadContent)
        {
        HBufC* content = HBufC::NewLC( iReadContent->Length() );
        TPtr16 ptr = content->Des();

        // Convert property/content
        LcRegXmlParserUtils::Utf8ToUnicodeL( *iReadContent, ptr );

        // Set content on appinfo related objects and parser status
        SetContentL( endTag, *content );

        // Reset/delete read/converted content
        CleanupStack::PopAndDestroy( content );

        delete iReadContent;
        iReadContent = NULL;
        }
        
    DEBUG("- CLcRegistrationParser::OnEndElementL" )
    }
// -----------------------------------------------------------------------------
// CLcRegistrationParser::SetContentL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::SetContentL(
    TLcRegXmlTagType aTag,
    const TDesC& aContent )
    {
    DEBUG("+ CLcRegistrationParser::SetContentL" )
    
	switch( aTag )
		{
		case ELcXmlRegFileName:
			{
			iRegAppInfo->SetAppNameL( aContent );
			break;	
			}
		case ELcXmlFeature:
			{
			if( !aContent.Compare( KLcAppCharSettings ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharSettings );	
				}
			else if( !aContent.Compare( KLcAppCharPersonalData ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharPersonalDataManagement );	
				}
			else if( !aContent.Compare( KLcAppCharNavigation ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharNavigation );	
				}
			else if( !aContent.Compare( KLcAppCharMaps ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharMaps );	
				}
			else if( !aContent.Compare( KLcAppCharLocationEvents ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharLocationBasedEvents );	
				}
			else if( !aContent.Compare( KLcAppCharRemotePositioning ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharRemotePositioning );	
				}
			else if( !aContent.Compare( KLcAppCharNetworkService ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharNetworkService );	
				}
			else if( !aContent.Compare( KLcAppCharLocationMessaging ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharLocationEnhancedMessages );	
				}
			else if( !aContent.Compare( KLcAppCharLocalContent ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharOnDeviceLocationBasedContent );	
				}
			else if( !aContent.Compare( KLcAppCharRemoteContent ) )
				{
				iRegAppInfo->SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharRemoteLocationBasedContent );	
				}
			else
				{
				User::Leave( KErrNotSupported );					
				}
			break;	
			}
		case ELcXmlApplicationParams:
			{
			iRegAppInfo->SetCommandLineParametersL( aContent );
			break;	
			}
		default:
			break;			
		}
		
    DEBUG("- CLcRegistrationParser::SetContentL" )
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnContentL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnContentL(
    const TDesC8& aBytes, TInt /*aErrorCode*/ )
    {
    DEBUG("+ CLcRegistrationParser::OnContentL")
    // If there are content from a previous parsing
    // then we need to concatenate data/strings.
    if (iReadContent)
        {
        HBufC8* prevContent = iReadContent;
        CleanupStack::PushL( prevContent );
        iReadContent = NULL;

        iReadContent = HBufC8::NewL( prevContent->Length() + aBytes.Length() );
        iReadContent->Des().Append( *prevContent );
        iReadContent->Des().Append( aBytes );
        CleanupStack::PopAndDestroy( prevContent );
        }
	else
		{
		iReadContent = aBytes.AllocL();	
		}
    DEBUG("- CLcRegistrationParser::OnContentL")
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnStartPrefixMappingL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnStartPrefixMappingL(
    const RString& /*aPrefix*/,
    const RString& /*aUri*/,
    TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnEndPrefixMappingL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnEndPrefixMappingL(
    const RString& /*aPrefix*/, TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnIgnorableWhiteSpaceL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnIgnorableWhiteSpaceL(
    const TDesC8& /*aBytes*/, TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnSkippedEntityL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnSkippedEntityL(
    const RString& /*aName*/, TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnProcessingInstructionL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/, const TDesC8& /*aData*/,
    TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnOutOfData
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnOutOfData()
    {
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::OnError
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLcRegistrationParser::OnError( TInt aErrorCode )
    {
    iParserError = aErrorCode;
    }

// -----------------------------------------------------------------------------
// CLcRegistrationParser::GetExtendedInterface
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CLcRegistrationParser::GetExtendedInterface( const TInt32 /*aUid*/ )
    {
    return NULL;
    }
    
//  End of File
