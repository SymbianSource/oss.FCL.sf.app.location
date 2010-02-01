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
#include <eikedwin.h>
#include <AknUtils.h>
#include <eikcapc.h>
#include <AknPopupField.h>
#include <StringLoader.h>
#include <lmkui.rsg>
#include "LmkConsts.h"
#include "CLmkDbUtils.h"
#include "MLmkEditorUiBuilder.h"
#include "CLmkEditorListField.h"

// CONSTANTS
_LIT(KEmptyCatName, " ");

/// Unnamed namespace for local definitions
namespace {
const TInt KNumberOfLines( 4 );
}  // namespace

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CLmkEditorListField::CLmkEditorListField
// ----------------------------------------------------
//
CLmkEditorListField::CLmkEditorListField
        ( MLmkFieldData& aField,
		  MLmkEditorUiBuilder& aUiBuilder ) :
    CLmkEditorFieldBase( aField, aUiBuilder )
    {
    }

// ----------------------------------------------------
// CLmkEditorListField::ConstructL
// ----------------------------------------------------
//
void CLmkEditorListField::ConstructL()
    {

    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>( iUiBuilder.CreateLineL(
            FieldLabel(), LandmarkItemField().UniqueFieldIdentity(),
            EEikCtEdwin ) );

    // ctrl is now owned by the dialog
    AknEditUtils::ConstructEditingL( iControl,
                                     KMaxCategoryNameFieldLen,
                                     KNumberOfLines,
            EEikEdwinDisplayOnly | EEikEdwinResizable
            | EEikEdwinNoAutoSelection, EEikEdwinIgnoreVirtualCursor
            | EEikEdwinAvkonDisableCursor,
            EFalse, EFalse, EFalse );

    // Get text
    HBufC* textBuf = HBufC::NewLC( KMaxCategoryNameFieldLen);
    TPtr text= textBuf->Des();
    RArray<TPosLmItemId>& categories = LandmarkItemField().Categories();
    TInt count( categories.Count() );

    HBufC* catName;
    //if only one category
    if ( count == 1 )
        {
        catName = CLmkDbUtils::CategoryNameL( iUiBuilder.Database(),
                                    categories[0] );
        }
    else
        {
        if (count > 1)
	        {
	        catName = StringLoader::LoadL( R_LMK_CATEGORIES_LIST_FORMAT,
	                                       count,
	                                       const_cast< CEikonEnv *>( iEnv ));
	        }
	    else // No Category, Don't display the string
		    {
		    catName	=  HBufC::NewL(20);
			*catName = KEmptyCatName;
		    }
        }
    text.Append( *catName );
    delete catName;
    catName = NULL;
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( text );
    iControl->SetAknEditorCase( EAknEditorTextCase );

    // Set formatted text to editor control
   	iControl->SetTextL( &text );
    // SetTextL method above copied the text to the control,
    // so it is safe to destroy the buffer
    CleanupStack::PopAndDestroy( textBuf );

	// readonly and no cursor
	iControl->AddFlagToUserFlags( CEikEdwin::EReadOnly | CEikEdwin::EDisplayOnly |
                                  CEikEdwin::EAvkonDisableCursor );

    // CreateTextViewL() is flagged as deprecated but if it is not
    // called here the ActivateL() below crashes sometimes.
    iControl->CreateTextViewL();
    iCaptionedCtrl = iUiBuilder.LineControl( ControlId() );
    }

// ----------------------------------------------------
// CLmkEditorListField::NewL
// ----------------------------------------------------
//
CLmkEditorListField* CLmkEditorListField::NewL
        ( MLmkFieldData& aField,
		 MLmkEditorUiBuilder& aUiBuilder  )
    {
    CLmkEditorListField* self =
        new(ELeave) CLmkEditorListField( aField, aUiBuilder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CLmkEditorListField::~CLmkEditorListField
// ----------------------------------------------------
//
CLmkEditorListField::~CLmkEditorListField()
    {
    }

// ----------------------------------------------------
// CLmkEditorListField::SaveFieldL
// ----------------------------------------------------
//
TBool CLmkEditorListField::SaveFieldL()
    {
    return ETrue;
    }

// ----------------------------------------------------
// CLmkEditorListField::ActivateL
// ----------------------------------------------------
//
void CLmkEditorListField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// ----------------------------------------------------
// CLmkEditorListField::UpdateL
// ----------------------------------------------------
//
void CLmkEditorListField::UpdateL()
    {
    HBufC* textBuf = HBufC::NewLC( KMaxCategoryNameFieldLen );
    TPtr text= textBuf->Des();
    RArray<TPosLmItemId> categories = LandmarkItemField().Categories();
    TInt count( categories.Count() );

    HBufC* catName;
    if ( count == 1 )
        {
        catName = CLmkDbUtils::CategoryNameL( iUiBuilder.Database(),
                                    categories[0] );
        }
    else
        {
   		if ( count > 1 )
	        {
	        catName = StringLoader::LoadL( R_LMK_CATEGORIES_LIST_FORMAT,
		                                   count,
		                                   const_cast< CEikonEnv *>( iEnv ) );
		    }
		else // No Category, Don't display the string
		    {
		    catName	=  HBufC::NewL(20);
			*catName = KEmptyCatName;
		    }
        }

    text.Append( *catName );
    delete catName;
    catName = NULL;
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( text );
    iControl->SetTextL( &text );
    iControl->DrawDeferred();
    CleanupStack::PopAndDestroy( textBuf );

    HBufC* label;
    if ( count > 1 )
        {
        label = iEnv->AllocReadResourceL( R_LMK_CATEGORIES_LABEL );
        }
    else
        {
        label = iEnv->AllocReadResourceL( R_LMK_CATEGORY_LABEL );
        }

    CleanupStack::PushL( label );
    LandmarkItemField().SetLabel( label );
    SetFieldLabelL( label );
    CleanupStack::PopAndDestroy();
    }

// Added newly............ for supporting Single Category Name in Editor

// ----------------------------------------------------
// CLmkEditorListField::ConstructL
// ----------------------------------------------------
//
void CLmkEditorListField::ConstructL(const TDesC& aCategoryName)
    {

    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>( iUiBuilder.CreateLineL(
            FieldLabel(), LandmarkItemField().UniqueFieldIdentity(),
            EEikCtEdwin ) );

    // ctrl is now owned by the dialog
    AknEditUtils::ConstructEditingL( iControl,
                                     KMaxCategoryNameFieldLen,
                                     KNumberOfLines,
            EEikEdwinDisplayOnly | EEikEdwinResizable
            | EEikEdwinNoAutoSelection, EEikEdwinIgnoreVirtualCursor
            | EEikEdwinAvkonDisableCursor,
            EFalse, EFalse, EFalse );

    // Get text
    HBufC* textBuf = HBufC::NewLC( KMaxCategoryNameFieldLen );
    TPtr text= textBuf->Des();
    RArray<TPosLmItemId>& categories = LandmarkItemField().Categories();
    TInt count( categories.Count() );

    HBufC* catName;
    //if only one category
    if ( count == 1 )
        {
		text.Append(aCategoryName);
        }
    else
        {
        if ( count > 1 )
	        {
	        catName = StringLoader::LoadL( R_LMK_CATEGORIES_LIST_FORMAT,
	                                       count,
	                                       const_cast< CEikonEnv *>( iEnv ));
			text.Append( *catName );
			delete catName;
			catName = NULL;
	        }
	    else // No Category, Don't display the string
		    {
		    catName	=  HBufC::NewL(20);
			*catName = KEmptyCatName;
		    }
        }

    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( text );
    iControl->SetAknEditorCase( EAknEditorTextCase );

    // Set formatted text to editor control
   	iControl->SetTextL( &text );
    // SetTextL method above copied the text to the control,
    // so it is safe to destroy the buffer
    CleanupStack::PopAndDestroy( textBuf );

	// readonly and no cursor
	iControl->AddFlagToUserFlags( CEikEdwin::EReadOnly | CEikEdwin::EDisplayOnly |
                                  CEikEdwin::EAvkonDisableCursor );

    // CreateTextViewL() is flagged as deprecated but if it is not
    // called here the ActivateL() below crashes sometimes.
    iControl->CreateTextViewL();
    iCaptionedCtrl = iUiBuilder.LineControl( ControlId() );
    }

// ----------------------------------------------------
// CLmkEditorListField::NewL
// ----------------------------------------------------
//
CLmkEditorListField* CLmkEditorListField::NewL
        ( MLmkFieldData& aField,
		 MLmkEditorUiBuilder& aUiBuilder,
		 const TDesC& aCategoryName  )
    {
    CLmkEditorListField* self =
        new(ELeave) CLmkEditorListField( aField, aUiBuilder );
    CleanupStack::PushL( self );
    self->ConstructL(aCategoryName);
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CLmkEditorListField::Control
// ----------------------------------------------------
//

CEikEdwin* CLmkEditorListField::Control()
	{
	return iControl;
	}
// End of File

