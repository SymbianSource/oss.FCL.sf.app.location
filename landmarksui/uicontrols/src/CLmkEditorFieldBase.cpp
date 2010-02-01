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
* Description:    Methods for landmark editor base class.
*
*/







// INCLUDE FILES
#include <eikcapc.h>
#include <eikimage.h>

#include <AknIconUtils.h>
#include <eikedwin.h>
#include <eikenv.h>
#include "LmkConsts.h"
#include "landmarks.hrh"
#include "CLmkUiUtils.h"

#include "CLmkEditorFieldBase.h"
#include "MLmkEditorUiBuilder.h"

#include "avkon.loc"


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CLmkEditorFieldBase::CLmkEditorFieldBase
// ----------------------------------------------------
//
CLmkEditorFieldBase::CLmkEditorFieldBase
        ( MLmkFieldData& aField,
				MLmkEditorUiBuilder& aUiBuilder ) :
    iLmkItemField( aField ), iUiBuilder( aUiBuilder )
    {
    iEnv = CEikonEnv::Static();
    }

// ----------------------------------------------------
// CLmkEditorFieldBase::BaseConstructL
// ----------------------------------------------------
//
void CLmkEditorFieldBase::BaseConstructL()
    {
    }

// ----------------------------------------------------
// CLmkEditorFieldBase::~CLmkEditorFieldBase
// ----------------------------------------------------
//
CLmkEditorFieldBase::~CLmkEditorFieldBase()
    {
    }

// ----------------------------------------------------
// CLmkEditorFieldBase::ControlId
// ----------------------------------------------------
//
TInt CLmkEditorFieldBase::ControlId() const
    {
    return iLmkItemField.UniqueFieldIdentity();
    }

// ----------------------------------------------------
// CLmkEditorFieldBase::FieldId
// ----------------------------------------------------
//
TInt CLmkEditorFieldBase::FieldId() const
    {
    return 0;
    }

// ----------------------------------------------------
// CLmkEditorFieldBase::FieldLabel
// ----------------------------------------------------
//
TPtrC CLmkEditorFieldBase::FieldLabel() const
    {
    return iLmkItemField.Label();
    }

// ----------------------------------------------------
// CLmkEditorFieldBase::SetFieldLabelL
// ----------------------------------------------------
//
void CLmkEditorFieldBase::SetFieldLabelL
		(HBufC* aLabel)
    {
	iUiBuilder.SetCurrentLineCaptionL(ControlId(),*aLabel);
    }

// ----------------------------------------------------
// CLmkEditorFieldBase::ControlTextL
// ----------------------------------------------------
//
HBufC* CLmkEditorFieldBase::ControlTextL() const
	{
	HBufC* textBuf = NULL;
	CCoeControl* ctrl = iUiBuilder.Control(ControlId());
	if (ctrl)
		{
        textBuf = static_cast<CEikEdwin*>(ctrl)->GetTextInHBufL();
		}
	return textBuf;
	}

// ----------------------------------------------------
// CLmkEditorFieldBase::LandmarkItemField
// ----------------------------------------------------
//
MLmkFieldData& CLmkEditorFieldBase::LandmarkItemField() const
    {
    return iLmkItemField;
    }

// -----------------------------------------------------------------------------
//CLmkEditorFieldBase::GetNumberFieldValue()
// -----------------------------------------------------------------------------
//
TReal CLmkEditorFieldBase::GetNumberFieldValue() const
	{
	return 0;
	}

// -----------------------------------------------------------------------------
//CLmkEditorFieldBase::FieldValue()
// -----------------------------------------------------------------------------
//
TReal CLmkEditorFieldBase::FieldValue() const
	{
	return 0;
	}
// -----------------------------------------------------------------------------
//CLmkEditorFieldBase::LoadBitmapToFieldL
// -----------------------------------------------------------------------------
//
void CLmkEditorFieldBase::LoadBitmapToFieldL
			(MLmkEditorUiBuilder& aUiBuilder)
	{
	// Get the landmark field to which the Icon should be added
	MLmkFieldData& nameField = LandmarkItemField();

	// Implemented only for Name field
	if ( nameField.FieldType() == EName )
		{
	    CEikImage* image = new(ELeave) CEikImage();
	    CleanupStack::PushL( image );
	    //TFileName defaultIconFile( CLmkUiUtils::LmkUiIconFileL() );
		// No need to add iconFile to cleanupstack since it is a member variable from CLmkFieldData class
	    HBufC* iconFile = nameField.IconPath();
		TInt err = KErrNotFound;
		CFbsBitmap* bitmap = NULL;
		CFbsBitmap* mask = NULL;

		if (iconFile) // landmark has a defined icon
			{
			if ( nameField.IconId() != KErrNotFound )
		        {
				TRAP(err,AknIconUtils::CreateIconL( bitmap, mask,
									   *iconFile, nameField.IconId(),
									   nameField.IconId()+1 ););
		        }
			image->SetPictureOwnedExternally(EFalse);
			image->SetPicture(bitmap, mask);
			image->CopyControlContextFrom(aUiBuilder.LineControl(ControlId()));
			image->SetContainerWindowL(*aUiBuilder.LineControl(ControlId()));
			image->SetNonFocusing();
			image->SetBrushStyle(CGraphicsContext::ENullBrush);

			delete aUiBuilder.LineControl(ControlId())->iBitmap;
			aUiBuilder.LineControl(ControlId())->iBitmap = image;

			TRect rect = aUiBuilder.LineControl(ControlId())->Rect();
			rect.Resize(-1, -1);
			aUiBuilder.LineControl(ControlId())->SetRect(rect);
			rect.Resize(1, 1);
			aUiBuilder.LineControl(ControlId())->SetRect(rect);
			CleanupStack::Pop(image);
			}
		else
    		{
			CleanupStack::PopAndDestroy(image);
    		}

		}
	}

// End of File
