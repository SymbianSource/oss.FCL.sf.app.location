/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Methods for landmark editor text field.
*
*/







// INCLUDE FILES
#include <featmgr.h>
#include <eikedwin.h>
#include <AknUtils.h>
#include <eikcapc.h>
#include "landmarks.hrh"
#include "CLmkEditorTextField.h"
#include "MLmkEditorUiBuilder.h"
#include "CLmkUiUtils.h"
#include "LmkConsts.h"
#include "CLmkFields.h"
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
const TInt KNumberOfLines( 8 );
}  // namespace

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CLmkEditorTextField::CLmkEditorTextField
// ----------------------------------------------------
//
CLmkEditorTextField::CLmkEditorTextField
        ( MLmkFieldData& aField,
		MLmkEditorUiBuilder& aUiBuilder ) :
    CLmkEditorFieldBase( aField, aUiBuilder )
    {
    }

// ----------------------------------------------------
// CLmkEditorTextField::ConstructL
// ----------------------------------------------------
//
void CLmkEditorTextField::ConstructL()
    {
    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>( iUiBuilder.CreateLineL(
            FieldLabel(), LandmarkItemField().UniqueFieldIdentity(),
            EEikCtEdwin ) );
    // ctrl is now owned by the dialog
    AknEditUtils::ConstructEditingL( iControl,
                                    LandmarkItemField().FieldLength(),
                                    KNumberOfLines,
            EAknEditorTextCase | EAknEditorCharactersUpperCase
            | EAknEditorCharactersLowerCase, EAknEditorAlignLeft,
            ETrue, ETrue, EFalse );

   // Set Edwin properties for numeric editor (Phone number field)
	if (ETextPhoneNumberFieldEditor == iLmkItemField.EditorType())
		{
		iControl->SetAknEditorNumericKeymap(EAknEditorStandardNumberModeKeymap);
		iControl->SetAknEditorInputMode(EAknEditorNumericInputMode);
		iControl->SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
		iControl->SetAknEditorSpecialCharacterTable(0);
		}

    // Set the editor case for URI editor
    if ( ETextUriFieldEditor == iLmkItemField.EditorType() )
	    {
	    iControl->SetAknEditorCase( EAknEditorLowerCase );
	    iControl->SetAknEditorFlags( EAknEditorFlagLatinInputModesOnly );
	    }
	else
		{
		iControl->SetAknEditorCase( EAknEditorTextCase );
		}

    // Get text
    HBufC* textBuf = HBufC::NewLC( LandmarkItemField().TextData().Length() );
    TPtr text= textBuf->Des();
    text = LandmarkItemField().TextData();

    // No Japanese input modes tolerated in Japanese variant,
    // set the allowed onces manually
    // Only for postal/zip code field

    if (LandmarkItemField().FieldType() == EPostCode)
	    {
			iControl->SetAknEditorInputMode(EAknEditorNumericInputMode);
			iControl->SetAknEditorAllowedInputModes(EAknEditorAllInputModes);
	    if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
	        {
	        iControl->SetAknEditorFlags(EAknEditorFlagLatinInputModesOnly);
	        }
	    else
	        {
	        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
	        }
	    }
    else
	    {
	    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
	    }


	if(LandmarkItemField().FieldType() == EWebAddress && text.Length() > 0)
	   	{
	   	//only for web address field
	   	TInt position;
		if((position =  IsWebUrlProtocolPreFixedL(text))!=
			KErrNotFound)
			{
			TrimPrefixesFromWebUrlL( text, position +1 );
			}
	   	}

	TPtrC ptr (text);
    SetControlTextL(ptr);

    // SetTextL method above copied the text to the control,
    // so it is safe to destroy the buffer
    CleanupStack::PopAndDestroy( textBuf );

    LoadBitmapToFieldL(iUiBuilder);

	// Place cursor to the end of the line
	iControl->AddFlagToUserFlags( CEikEdwin::EJustAutoCurEnd );

    // CreateTextViewL() is flagged as deprecated but if it is not
    // called here the ActivateL() below crashes sometimes.
    iControl->CreateTextViewL();
    iCaptionedCtrl = iUiBuilder.LineControl( ControlId() );
    iCaptionedCtrl->SetTakesEnterKey( ETrue );
    }

// ----------------------------------------------------
// CLmkEditorTextField::NewL
// ----------------------------------------------------
//
CLmkEditorTextField* CLmkEditorTextField::NewL
        ( MLmkFieldData& aField,
		  MLmkEditorUiBuilder& aUiBuilder  )
    {
    CLmkEditorTextField* self =
        new(ELeave) CLmkEditorTextField( aField, aUiBuilder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CLmkEditorTextField::~CLmkEditorTextField
// ----------------------------------------------------
//
CLmkEditorTextField::~CLmkEditorTextField()
    {
    delete iText;
    delete iUrlPrefix;
    }

// ----------------------------------------------------
// CLmkEditorTextField::SaveFieldL
// ----------------------------------------------------
//
TBool CLmkEditorTextField::SaveFieldL()
    {
    TBool result = EFalse;
    HBufC* text = iControl->GetTextInHBufL();
    if ( text )
        {
        if ( LandmarkItemField().FieldType() == EWebAddress )
            {
            TInt totalLength = text->Length();
            if (totalLength > 0)
            	{
            	if(CLmkUiUtils::FindIfWebAddrFieldIsEmpty(text->Des()))
            		{
            		//nothing is there after :// e.g http://
            		// do not save
            		text->Des().Zero();
                	LandmarkItemField().SetText( text );
                	text = NULL;
                	result = ETrue;

            		}
            	else
            		{
            		//something is there after :// e.g http://www.ebay.com

	                if( iText && *iText != *text)
	                    {
	                    // store the text
	                    // Prepend the webaddress text with "http://" by default
	                    TInt httpProtoPos = text->Find( KProtocol );
	                    if ( httpProtoPos == KErrNotFound)
	                        {
							if(text->Length() <= KMaxUrlFieldLenWithDefaultProtocol)
								{
								text = text->ReAllocL( KHttp().Length() +  text->Length() );
								text->Des().Insert(0, KHttp);
								}
	                        }

	                    if( isUrlPrefixed )
		                    {
		                    TPtr text1= text->Des();
	                    	if(IsWebUrlProtocolPreFixedL(text1)== KErrNotFound)
		                    	{
		                    	//insert the pre-fix back to the url, if it was removed.
			                    //since for ui , prefixes are not shown, but prefixes are
			                    //retained in its original form.
			                    text = text->ReAllocL( iUrlPrefix->Length() +  text->Length() );
								text->Des().Insert(0, iUrlPrefix->Des());
		                    	}
		                    }
	                    LandmarkItemField().SetText( text );
	                     text = NULL;
	                     result = ETrue;
	                    }
            		}//else
            	}//end of if statement
            }// not a web address field
        else
            {
            if ( LandmarkItemField().TextData() != *text )
                {
                // store the text
                LandmarkItemField().SetText( text );
                text = NULL;
                result = ETrue;
                }
            }
            delete text;
        }
    else
        {
        // Ensure field text is empty
        if ( LandmarkItemField().TextData().Length() > 0)
            {
            text = HBufC::New(0);
            LandmarkItemField().SetText( text );
            result = ETrue;
            }
        }

    return result;
    }

// ----------------------------------------------------
// CLmkEditorTextField::ActivateL
// ----------------------------------------------------
//
void CLmkEditorTextField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }


// ----------------------------------------------------
// CLmkEditorTextField::Control
// ----------------------------------------------------
//
CEikEdwin* CLmkEditorTextField::Control()
	{
	return iControl;
	}

// ----------------------------------------------------
// CLmkEditorTextField::Control
// ----------------------------------------------------
//
CEikCaptionedControl* CLmkEditorTextField::CaptionedControl()
	{
	return iCaptionedCtrl;
	}

// ----------------------------------------------------
// CLmkEditorTextField::IsWebUrlProtocolPreFixedL()
// ----------------------------------------------------
//
TInt CLmkEditorTextField::IsWebUrlProtocolPreFixedL( TPtr& aWebUrl )
	{
	TChar ch = '/';
	// e.g image/jpeg/http://www.google.com
	TInt protocolPos = aWebUrl.Find(KProtocol);
	TInt posPrefix = KErrNotFound;
	if( protocolPos != KErrNotFound)
		{
		//find the left string from ':" character e.g image/jpeg/http
		TPtrC ptrPrefix = aWebUrl.Left( protocolPos );
		//search '/' from right handside.
		posPrefix = ptrPrefix.LocateReverse(ch);
		if( posPrefix != KErrNotFound)
			{
			/*if( !iUrlPrefix )
			    {*/
			    delete iUrlPrefix;
			    iUrlPrefix = NULL;
			    //prefix e.g image/jpeg.
			    iUrlPrefix = HBufC::NewL( posPrefix + 1 );
			    iUrlPrefix->Des().Copy(ptrPrefix.Left( posPrefix + 1 ));
			    //}
			isUrlPrefixed = ETrue;
			}
		}
	//returns the position of the prefix.
	return posPrefix;
	}

// ----------------------------------------------------
// CLmkEditorTextField::TrimPrefixesFromWebUrlL()
// ----------------------------------------------------
//
void CLmkEditorTextField::TrimPrefixesFromWebUrlL( TPtr& aWebUrl,
												    TInt aTrimPos )
	{
	//deletes the prefix from url
	//e.g deletes "image/jpeg" from image/jpeg/http://www.google.com
	aWebUrl.Delete(0,aTrimPos);
	}
// ----------------------------------------------------
// CLmkEditorTextField::SetControlTextL()
// ----------------------------------------------------
//
void CLmkEditorTextField::SetControlTextL( TPtrC& aText)
	{
	TPtrC dispTxt;
	dispTxt.Set(aText);
	// Set formatted text to editor control
    if ( LandmarkItemField().FieldType() == EWebAddress &&
    	 aText.Length() == 0 && (!iUiBuilder.EditMode()))
        {
        //only for web address field
         // Insert http:// prefix to URL field only in edit mode
         dispTxt.Set(KHttp());
        }
    else
        {
        //if it is editor, remove the enter character, if at all present.
        if (iUiBuilder.EditMode())
	        {
	        TPtr des1 = CLmkFields::RemoveEnterCharacter( aText );
	        dispTxt.Set(des1);
	        }
        }

    if( !iText )
	    {
	    //currently this member is used only for url field.
	    //it can be used for other fields also, later.
	    iText = HBufC::NewL( dispTxt.Length() );
	    }
	else if( dispTxt.Length() > iText->Length())
		{
		iText = iText->ReAllocL( dispTxt.Length() );
		}
	iText->Des().Copy( dispTxt);
    iControl->SetTextL( &dispTxt );
    iControl->DrawDeferred();
	}
// ----------------------------------------------------
// CLmkEditorTextField::SetEditableL()
// ----------------------------------------------------
//
void CLmkEditorTextField::SetEditableL( TBool aEditable )
	{
	if(iCaptionedCtrl)
		iCaptionedCtrl->SetEditableL(aEditable);
	}
// End of File

