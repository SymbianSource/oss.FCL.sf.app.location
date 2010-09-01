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
* Description:    LandmarksUi Content File -
*           Methods for Landmark editor field array.
*
*/







// INCLUDE FILES
#include "CLmkEditorFieldArray.h"  // this class
#include "MLmkEditorUiBuilder.h"
#include "MLmkEditorField.h"
#include "LmkEditorFieldFactory.h"
#include "CLmkFields.h"
#include "landmarks.hrh"
#include "CLmkUiUtils.h"
#include <bldvariant.hrh>
#include <EPos_CPosLandmark.h>
// Code added
#include <e32math.h>
#include <e32des16.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
const TInt KNumberOfFields( 5 );
}  // namespace
// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CLmkEditorFieldArray::CLmkEditorFieldArray
// ----------------------------------------------------
//
CLmkEditorFieldArray::CLmkEditorFieldArray( CLmkFields& aLmkFields,
        MLmkEditorUiBuilder& aUiBuilder ) :
    iLmkFields( aLmkFields ),
    iUiBuilder( aUiBuilder ),
    iFieldArray(KNumberOfFields)
    {
    }

// ----------------------------------------------------
// CLmkEditorFieldArray::ConstructL
// ----------------------------------------------------
//
void CLmkEditorFieldArray::ConstructL()
    {
	    iCategoryFlag = EFalse; // Used for showing categories
    }

// ----------------------------------------------------
// CLmkEditorFieldArray::NewL
// ----------------------------------------------------
//
CLmkEditorFieldArray* CLmkEditorFieldArray::NewL
        ( CLmkFields& aLmkFields,
        MLmkEditorUiBuilder& aUiBuilder )
    {
	CLmkEditorFieldArray* dlg = new(ELeave) CLmkEditorFieldArray( aLmkFields,
                                                                  aUiBuilder );
    CleanupStack::PushL( dlg );
    dlg->ConstructL();
    CleanupStack::Pop();  // dlg
    return dlg;
    }

// ----------------------------------------------------
// CLmkEditorFieldArray::~CLmkEditorFieldArray
// ----------------------------------------------------
//
CLmkEditorFieldArray::~CLmkEditorFieldArray()
    {
    iFieldArray.ResetAndDestroy();
    }

// ----------------------------------------------------
// CLmkEditorFieldArray::UpdateFieldsL
// ----------------------------------------------------
//
void CLmkEditorFieldArray::UpdateFieldsL( const TUint16 aFieldType )
    {
    TInt count = iFieldArray.Count();
    for ( TInt i=0; i < count; i++ )
        {
        if ( ( iFieldArray[i]->LandmarkItemField().FieldType() ==
               aFieldType ) || ( aFieldType == EAllEditorItems ) )
            {
            iFieldArray[i]->UpdateL();
            }
        }
    }

// ----------------------------------------------------
// CLmkEditorFieldArray::CreateFieldsFromLmkL
// ----------------------------------------------------
//
void CLmkEditorFieldArray::CreateFieldsFromLmkL()
    {
    CArrayPtrFlat<MLmkFieldData>& fields = iLmkFields.Fields();
    TInt count = fields.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        MLmkFieldData& lmkItemField = *fields[i];
        TBool tight = ETrue;
        if(iUiBuilder.EditMode())
	        {
	        tight =  CheckIfFieldIsEmpty(lmkItemField);
	        }
	        MLmkEditorField* editorField = NULL;
	        if( tight && iCategoryFlag && lmkItemField.EditorType() == EListFieldEditor )
	        {	//if there are
		        editorField = LmkEditorFieldFactory::CreateFieldL(
	                                        lmkItemField, iUiBuilder,iCategoryName);
	            iCategoryFlag = EFalse; // Safty purpose....
	        }
	        else if(tight)
			{
	        	editorField = LmkEditorFieldFactory::CreateFieldL(
	                                        lmkItemField, iUiBuilder );
			}
        CleanupStack::PushL( editorField );
        if ( editorField )
            {
            iFieldArray.AppendL( editorField );
            }
        CleanupStack::Pop(); //editorField
        }
    }

// ----------------------------------------------------
// CLmkEditorFieldArray::AreAllFieldsEmptyL
// ----------------------------------------------------
//
TBool CLmkEditorFieldArray::AreAllFieldsEmptyL() const
   {
   TBool result = ETrue;
   TReal localVar;

   const TInt fieldCount = iFieldArray.Count();
   for ( TInt i=0; i < fieldCount; i++ )
        {
        switch( iFieldArray[i]->LandmarkItemField().EditorType() )
            {
            //case ETextFieldEditor:
            case ETextGenericFieldEditor:
            case ETextPhoneNumberFieldEditor:
                {
                HBufC* fieldText = iFieldArray[i]->ControlTextL();
                if ( fieldText && fieldText->Length() > 0 )
                    {
					//Check if any active character or not
					TPtr16 ptr = fieldText->Des();
					ptr.Trim();
					if (ptr.Length() == 0)
						{
						result = ETrue;
						}
					else
						{
						result = EFalse;
						}
                    }
                delete fieldText;
                break;
                }
            case ETextUriFieldEditor:
                {
                HBufC* fieldText = iFieldArray[i]->ControlTextL();

                if( fieldText && fieldText->Length() > 0 )
                    {
                    //Check if any active character or not
					TPtr16 ptr = fieldText->Des();
					ptr.Trim();
					// Checking if the String present is only "http:\\"
					// If that is the case then the field is still
					// considered empty
					if(!CLmkUiUtils::FindIfWebAddrFieldIsEmpty(ptr))
					    {
					    result = EFalse;
					    }
                    }
                delete fieldText;
                break;
                }
            case EListFieldEditor:
                {
                if ( iFieldArray[i]->LandmarkItemField().Categories().Count() )
                    {
                    result = EFalse;
                    }
                break;
                }
           case ENumberFieldEditor:
   		        {
                localVar = iFieldArray[i]->FieldValue();
                if (!Math::IsNaN(localVar))
			        result = EFalse;
			    break;
		        }

           case ECoordinateFieldEditor:
   		        {
                localVar = iFieldArray[i]->FieldValue();
                if (!Math::IsNaN(localVar))
                    result = EFalse;
			    break;
		        }


            default:
                {
                break;
                }
            }

        if ( !result )
            {
            break;
            }
        }
    return result;
    }

// ----------------------------------------------------
// CLmkEditorFieldArray::FieldAt
// ----------------------------------------------------
//
const MLmkEditorField& CLmkEditorFieldArray::FieldAt
		( TInt aFieldIndex )
	{
    return *iFieldArray[aFieldIndex];
	}

// ----------------------------------------------------
// CLmkEditorFieldArray::Find
// ----------------------------------------------------
//
MLmkEditorField* CLmkEditorFieldArray::Find( TInt aControlId, TInt* aPos,
											 TInt* aPrevControlId ) const
    {
    MLmkEditorField* field = NULL;
	const TInt count = iFieldArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        MLmkEditorField* temp = iFieldArray.At(i);
        if (temp && temp->ControlId() == aControlId )
            {
            field = temp;
            if(aPos)
	            {
	            *aPos =i;
	            }
            break;
            }
        if( (temp && aPrevControlId) && temp->ControlId() < aControlId)
	        {
	        *aPrevControlId = temp->ControlId();
	        }
        }
    return field;
    }

// ----------------------------------------------------
// CLmkEditorFieldArray::SaveFieldsL
// ----------------------------------------------------
//
TBool CLmkEditorFieldArray::SaveFieldsL()
    {
	TBool result = EFalse;
    for ( TInt i(0); i < iFieldArray.Count(); ++i )
        {
        if ( iFieldArray[i]->SaveFieldL() )
			{
			result = ETrue;
			}
        }

    iLmkFields.SaveFieldsL();
    return result;
	}

// ----------------------------------------------------
// CLmkEditorFieldArray::SetCategoryName
// ----------------------------------------------------
//
void CLmkEditorFieldArray::SetCategoryName(const TDesC& aCategoryName)
{
	iCategoryName.Copy(aCategoryName);
	iCategoryFlag = ETrue;
}

// ----------------------------------------------------
// CLmkEditorFieldArray::SaveFieldsToBufferL
// ----------------------------------------------------
//
void CLmkEditorFieldArray::SaveFieldsToBufferL()
	{
    for ( TInt i(0); i < iFieldArray.Count(); ++i )
        {
        iFieldArray[i]->SaveFieldL();
        }
	}

// ----------------------------------------------------
// CLmkEditorFieldArray::GetFieldArray
// ----------------------------------------------------
//
CArrayPtrFlat<MLmkEditorField>& CLmkEditorFieldArray::GetFieldArray()
{
	return iFieldArray;
}

// ----------------------------------------------------
// CLmkEditorFieldArray::CheckIfFieldIsEmpty
// ----------------------------------------------------
//
TBool CLmkEditorFieldArray::CheckIfFieldIsEmpty(MLmkFieldData& a)
	{
	TBool result = EFalse;
	switch( a.EditorType() )
	    {
	    //case ETextFieldEditor:
	    case ETextGenericFieldEditor:
	    case ETextPhoneNumberFieldEditor:
	    case ETextUriFieldEditor:
	        {
	        TPtrC buf = a.TextData();
	        if ( buf.Length() > 0 )
	            {
	            result = ETrue;
	            }
	        break;
	        }
	    case EListFieldEditor:
	    	{
	    	RArray<TPosLmItemId>& t = a.Categories();
	    	if ( t.Count() > 0 )
	            {
	            result = ETrue;
	            }
	        break;
	        }
	    case ENumberFieldEditor:
	        {
	        TReal32 y = a.FloatData();
	        if(!Math::IsNaN(y))
	            {
	            result = ETrue;
	            }
	        break;
	        }
	    case ECoordinateFieldEditor:
	    	{
	    	TReal64 z = a.DoubleData();
	    	if(!Math::IsNaN(z))
	            {
	            result = ETrue;
	            }
	        break;
	        }
	    }
	  return result;
	}
// End of File
