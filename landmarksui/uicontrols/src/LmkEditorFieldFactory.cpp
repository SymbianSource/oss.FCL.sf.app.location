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
* Description:    Methods for Landmark field factory class.
*
*/







// INCLUDE FILES
#include "LmkEditorFieldFactory.h"  // this class
#include "CLmkEditorTextField.h"
#include "CLmkEditorNumberField.h"
#include "CLmkEditorListField.h"
#include "CLmkEditorCoordinateField.h"
#include "MLmkFieldData.h"
#include "landmarks.hrh"
#include <e32std.h>
#include <lmkerrors.h>


/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "LmkEditorFieldFactory");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// LmkEditorFieldFactory::CreateFieldL
// ----------------------------------------------------
//
MLmkEditorField* LmkEditorFieldFactory::CreateFieldL
        ( MLmkFieldData& aField,
          MLmkEditorUiBuilder& aUiBuilder )
    {
    MLmkEditorField* editorField = NULL;
    switch( aField.EditorType() )
        {
        //case ETextFieldEditor:
        // Added for rel3.1
        case ETextGenericFieldEditor:
        case ETextPhoneNumberFieldEditor:
       	case ETextUriFieldEditor :
            {
            editorField = CLmkEditorTextField::NewL( aField, aUiBuilder );
            break;
            }
        case ENumberFieldEditor:
            {
            editorField = CLmkEditorNumberField::NewL( aField, aUiBuilder );
            break;
            }
        case EListFieldEditor:
            {
            editorField = CLmkEditorListField::NewL( aField, aUiBuilder );
            break;
            }
        case ECoordinateFieldEditor:
            {
            editorField = CLmkEditorCoordinateField::NewL( aField,
                                                           aUiBuilder );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic(
                KLmkPanicUnknownControlType ) );
            break;
            }
        }

    return editorField;
    }


MLmkEditorField* LmkEditorFieldFactory::CreateFieldL
        ( MLmkFieldData& aField,
          MLmkEditorUiBuilder& aUiBuilder,const TDesC& aCategoryName )
    {

    MLmkEditorField* editorField = NULL;
    switch( aField.EditorType() )
        {
        //case ETextFieldEditor:
        //case ETextFieldEditor:
        // Added for rel3.1
        case ETextGenericFieldEditor:
        case ETextPhoneNumberFieldEditor:
       	case ETextUriFieldEditor :
            {
            editorField = CLmkEditorTextField::NewL( aField, aUiBuilder );
            break;
            }
        case ENumberFieldEditor:
            {
            editorField = CLmkEditorNumberField::NewL( aField, aUiBuilder );
            break;
            }
        case EListFieldEditor:
            {
            editorField = CLmkEditorListField::NewL( aField, aUiBuilder,aCategoryName );
            break;
            }
        case ECoordinateFieldEditor:
            {
            editorField = CLmkEditorCoordinateField::NewL( aField,
                                                           aUiBuilder );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic(
                KLmkPanicUnknownControlType ) );
            break;
            }
        }

    return editorField;
    }




// End of File
