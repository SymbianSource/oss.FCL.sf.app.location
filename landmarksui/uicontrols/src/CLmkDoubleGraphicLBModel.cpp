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
#include "CLmkDoubleGraphicLBModel.h"
#include "CLmkListProviderBase.h"
#include "CLmkSelectorCache.h"
#include "CLmkUiItemBase.h"
#include <lmkui.rsg>
#include <EPos_Landmarks.h>
#include <e32keys.h>
#include <eikenv.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkDoubleGraphicLBModel::CLmkDoubleGraphicLBModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDoubleGraphicLBModel::CLmkDoubleGraphicLBModel()
    {
    }

// -----------------------------------------------------------------------------
// CLmkDoubleGraphicLBModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkDoubleGraphicLBModel::ConstructL(CLmkListProviderBase& aListProvider)
    {
    BaseConstructL( aListProvider );

    iEmpty = CEikonEnv::Static()->AllocReadResourceL(
                R_LMK_CATEGORY_SETTINGS_EMPTY );
    }

// -----------------------------------------------------------------------------
// CLmkDoubleGraphicLBModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkDoubleGraphicLBModel* CLmkDoubleGraphicLBModel::NewL(
    CLmkListProviderBase& aListProvider )
    {
    CLmkDoubleGraphicLBModel* self = new( ELeave )
    				CLmkDoubleGraphicLBModel();

    CleanupStack::PushL( self );
    self->ConstructL( aListProvider );
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkDoubleGraphicLBModel::~CLmkDoubleGraphicLBModel
// ----------------------------------------------------
//
CLmkDoubleGraphicLBModel::~CLmkDoubleGraphicLBModel()
    {
    delete iEmpty;
    }

// -----------------------------------------------------------------------------
// CLmkDoubleGraphicLBModel::MdcaPointL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLmkDoubleGraphicLBModel::MdcaPointL( TInt aIndex ) const
    {
    CLmkUiItemBase& item = iCache->ItemAtL( aIndex );
    TPtr16 modBuffer = iBuffer->Des();
    modBuffer.Zero();
    modBuffer.AppendNum( item.GetIconId() );
    modBuffer.Append( EKeyTab );
    modBuffer.Append( item.Text() );
    modBuffer.Append( EKeyTab );
    if ( item.ExtensionIntExists() && !item.ExtensionInt() )
        {
        modBuffer.Append( *iEmpty );
        }
    modBuffer.Append( EKeyTab );
    }
//  End of File
