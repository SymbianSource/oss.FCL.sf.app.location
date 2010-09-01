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
#include "CLmkMultiSelectionLBModel.h"
#include "CLmkListProviderBase.h"
#include "CLmkSelectorCache.h"
#include "CLmkUiItemBase.h"
#include <EPos_Landmarks.h>
#include <e32keys.h>
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
const TInt KCheckboxOffIcon = 1;
}  // namespace
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkMultiSelectionLBModel::CLmkMultiSelectionLBModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkMultiSelectionLBModel::CLmkMultiSelectionLBModel()
    {

    }

// -----------------------------------------------------------------------------
// CLmkMultiSelectionLBModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkMultiSelectionLBModel::ConstructL(
    CLmkListProviderBase& aListProvider )
    {
    BaseConstructL( aListProvider );
    }

// -----------------------------------------------------------------------------
// CLmkMultiSelectionLBModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkMultiSelectionLBModel* CLmkMultiSelectionLBModel::NewL(
    CLmkListProviderBase& aListProvider )
    {
    CLmkMultiSelectionLBModel* self = new( ELeave ) CLmkMultiSelectionLBModel;

    CleanupStack::PushL( self );
    self->ConstructL( aListProvider );
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkMultiSelectionLBModel::~CLmkMultiSelectionLBModel
// ----------------------------------------------------
//
CLmkMultiSelectionLBModel::~CLmkMultiSelectionLBModel()
    {
    }

// -----------------------------------------------------------------------------
// CLmkMultiSelectionLBModel::MdcaPointL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLmkMultiSelectionLBModel::MdcaPointL( TInt aIndex ) const
    {
    CLmkUiItemBase& item = iCache->ItemAtL( aIndex );
    TPtr16 modBuffer = iBuffer->Des();
    modBuffer.Zero();
    modBuffer.AppendNum( KCheckboxOffIcon );
    modBuffer.Append( EKeyTab );
    modBuffer.Append( item.Text() );
    }

//  End of File
