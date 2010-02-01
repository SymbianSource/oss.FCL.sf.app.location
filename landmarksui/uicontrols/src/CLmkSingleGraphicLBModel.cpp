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
#include "CLmkSingleGraphicLBModel.h"
#include "CLmkListProviderBase.h"
#include "CLmkSelectorCache.h"
#include "CLmkUiItemBase.h"
#include <EPos_Landmarks.h>
#include <e32keys.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkSingleGraphicLBModel::CLmkSingleGraphicLBModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkSingleGraphicLBModel::CLmkSingleGraphicLBModel()
    {

    }

// -----------------------------------------------------------------------------
// CLmkSingleGraphicLBModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkSingleGraphicLBModel::ConstructL(
                               CLmkListProviderBase& aListProvider )
    {
    BaseConstructL( aListProvider );
    }

// -----------------------------------------------------------------------------
// CLmkSingleGraphicLBModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkSingleGraphicLBModel* CLmkSingleGraphicLBModel::NewL(
    CLmkListProviderBase& aListProvider )
    {
    CLmkSingleGraphicLBModel* self = new ( ELeave ) CLmkSingleGraphicLBModel;

    CleanupStack::PushL( self );
    self->ConstructL( aListProvider );
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkSingleGraphicLBModel::~CLmkSingleGraphicLBModel
// ----------------------------------------------------
//
CLmkSingleGraphicLBModel::~CLmkSingleGraphicLBModel()
    {
    }

// -----------------------------------------------------------------------------
// CLmkSingleGraphicLBModel::MdcaPointL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLmkSingleGraphicLBModel::MdcaPointL( TInt aIndex ) const
    {
    CLmkUiItemBase& item = iCache->ItemAtL( aIndex );
    TPtr16 modBuffer = iBuffer->Des();
    modBuffer.Zero();
    modBuffer.AppendNum( item.GetIconId() );
    modBuffer.Append( EKeyTab );
    modBuffer.Append( item.Text() );
    }

//  End of File
