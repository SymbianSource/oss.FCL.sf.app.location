/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  List box model class for Events Management UI.
*
*/

// INCLUDES
#include <aknsfld.h> // For SearchField
#include "evtmgmtuilistboxmodel.h"
#include "evtmgmtuimodel.h"
#include "evtmgmtuifiltermodel.h"

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::~CEvtMgmtUiListBoxModel
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CEvtMgmtUiListBoxModel::~CEvtMgmtUiListBoxModel()
    {        
    delete iEvtMgmtUiFilterModel;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::NewLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CEvtMgmtUiListBoxModel* CEvtMgmtUiListBoxModel::NewL( )
    {
	CEvtMgmtUiListBoxModel* self = NewLC( );
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::NewLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CEvtMgmtUiListBoxModel* CEvtMgmtUiListBoxModel::NewLC( )
    {
	CEvtMgmtUiListBoxModel* self = new ( ELeave )CEvtMgmtUiListBoxModel( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::CEvtMgmtUiListBoxModel
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CEvtMgmtUiListBoxModel::CEvtMgmtUiListBoxModel()
    {    
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::ConstructL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiListBoxModel::ConstructL()
    {    
    iEvtMgmtUiFilterModel = CEvtMgmtUiFilterModel::NewL();
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::MatchableTextArray
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const MDesCArray* CEvtMgmtUiListBoxModel::MatchableTextArray() const
    {    
    return this;
    }
        
// ---------------------------------------------------------------------------
// TInt CEvtMgmtUiListBoxModel::MdcaCount() const
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiListBoxModel::MdcaCount() const
    {
    return iEvtMgmtUiFilterModel->MdcaCount();
    }
    
// ---------------------------------------------------------------------------
// TPtrC16 CEvtMgmtUiListBoxModel::MdcaPoint() const
// ---------------------------------------------------------------------------
//
TPtrC16 CEvtMgmtUiListBoxModel::MdcaPoint( TInt  aIndex ) const
    {
    if (Filter())
        {
        return Filter()->DefaultMatchableItemFromItem(iEvtMgmtUiFilterModel->MdcaPoint(aIndex));
        }   
    else
        {
        return iEvtMgmtUiFilterModel->MdcaPoint(aIndex);
        }
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::NumberOfItems
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CEvtMgmtUiListBoxModel::NumberOfItems() const
    {
    return Filter() ? 
           Filter()->FilteredNumberOfItems(): CTextListBoxModel::NumberOfItems();
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::ItemText
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CEvtMgmtUiListBoxModel::ItemText(TInt aItemIndex) const
    {
    if(aItemIndex==0)
        {
        return iEvtMgmtUiFilterModel->GetZeroIndex();
        }
    else
        {
        return CTextListBoxModel::ItemText( Filter() ? 
                          Filter()->FilteredItemIndex(aItemIndex) : aItemIndex);
        }
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::NumberOfItems
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiListBoxModel::SetItemTextArrayToFilterModel()
    {
    iEvtMgmtUiFilterModel->SetItemTextArray(ItemTextArray());
    SetItemTextArray(iEvtMgmtUiFilterModel);
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiListBoxModel::NumberOfItems
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiListBoxModel::SetFindToFilterModel(CAknSearchField* aSearchField)
    {
    iEvtMgmtUiFilterModel->SetSearchField(aSearchField);
    }

// End of File

