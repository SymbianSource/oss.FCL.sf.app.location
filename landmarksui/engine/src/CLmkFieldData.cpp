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
*
*/








// INCLUDE FILES
#include "CLmkFieldData.h"
#include <lmkerrors.h>


#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkFieldData");

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CLmkFieldData::CLmkFieldData
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLmkFieldData::CLmkFieldData() : iTitleField( EFalse )
    {
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkFieldData::ConstructL( HBufC* aLabel )
    {
    SetLabel( aLabel );
    iTextData = HBufC::NewL(0);
    iUniqueFieldIdentity = reinterpret_cast<TInt>(this);
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLmkFieldData* CLmkFieldData::NewL( HBufC* aLabel )
    {
    CLmkFieldData* self = new ( ELeave ) CLmkFieldData;
    CleanupStack::PushL( self );
    self->ConstructL( aLabel );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::~CLmkFieldData
// ----------------------------------------------------------------------------
//
CLmkFieldData::~CLmkFieldData()
    {
    delete iTextData;
    delete iIconPath;
    iCategories.Close();
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetLabel
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetLabel( HBufC* aLabel )
    {
    __ASSERT_DEBUG( aLabel, Panic( KLmkPanicNullMember ) );
    iLabel = aLabel;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::Label
// ----------------------------------------------------------------------------
//
const HBufC& CLmkFieldData::Label() const
    {
    return *iLabel;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetEditorType
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetEditorType( const TUint16 aType )
    {
    iEditorType = aType;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::EditorType
// ----------------------------------------------------------------------------
//
TUint16 CLmkFieldData::EditorType() const
    {
    return iEditorType;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetText
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetTextL( const TPtrC aText )
    {
    delete iTextData;
    iTextData = NULL;
    iTextData = aText.AllocL();
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetText
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetText( HBufC* aText )
    {
    if ( aText )
        {
        delete iTextData;
        iTextData = aText;
        }
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::TextData
// ----------------------------------------------------------------------------
//
const HBufC& CLmkFieldData::TextData() const
    {
    return *iTextData;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetInt
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetInt( const TInt aTInt )
    {
    iIntData = aTInt;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::IntegerData
// ----------------------------------------------------------------------------
//
TInt CLmkFieldData::IntegerData() const
    {
    return iIntData;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetTReal
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetTReal( const TReal32 aTReal )
    {
    iTRealData32 = aTReal;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::FloatData
// ----------------------------------------------------------------------------
//
TReal32 CLmkFieldData::FloatData() const
    {
    return iTRealData32;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetTReal
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetTReal( const TReal64 aTReal )
    {
    iTRealData64 = aTReal;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::DoubleData
// ----------------------------------------------------------------------------
//
TReal64 CLmkFieldData::DoubleData() const
    {
    return iTRealData64;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::Categories
// ----------------------------------------------------------------------------
//
RArray<TPosLmItemId>& CLmkFieldData::Categories()
    {
    return iCategories;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::UniqueFieldIdentity
// ----------------------------------------------------------------------------
//
TInt CLmkFieldData::UniqueFieldIdentity() const
    {
    return iUniqueFieldIdentity;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetFieldType
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetFieldType( const TUint16 aFieldType )
    {
    iFieldType = aFieldType;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::FieldType
// ----------------------------------------------------------------------------
//
TUint16 CLmkFieldData::FieldType() const
    {
    return iFieldType;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetPositionFieldId
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetPositionFieldId( const TUint16 aPositionFieldId )
    {
    iPositionFieldId = aPositionFieldId;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::PositionFieldId
// ----------------------------------------------------------------------------
//
TUint16 CLmkFieldData::PositionFieldId() const
    {
    return iPositionFieldId;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetFieldLength
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetFieldLength( const TInt aLength )
    {
    iFieldLength = aLength;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::FieldLength
// ----------------------------------------------------------------------------
//
TInt CLmkFieldData::FieldLength() const
    {
    return iFieldLength;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetTitleField
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetTitleField( const TBool aTitleField )
    {
    iTitleField = aTitleField;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::IsTitleField
// ----------------------------------------------------------------------------
//
TBool CLmkFieldData::IsTitleField() const
    {
    return iTitleField;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetIconId
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetIconId( const TInt aIconId )
    {
    iIconId = aIconId;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::IconId
// ----------------------------------------------------------------------------
//
TInt CLmkFieldData::IconId() const
    {
    return iIconId;
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::SetIconPathL
// ----------------------------------------------------------------------------
//
void CLmkFieldData::SetIconPathL( const TDesC& aIconPath )
    {
    if ( aIconPath.Length() > 0 )
        {
        delete iIconPath;
        iIconPath = NULL;
        iIconPath = aIconPath.AllocL();
        }
    }

// ----------------------------------------------------------------------------
// CLmkFieldData::IconPath
// ----------------------------------------------------------------------------
//
HBufC* CLmkFieldData::IconPath()
    {
    return iIconPath;
    }
//  End of File
