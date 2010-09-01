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
* Description:  Model class for Events management UI start-up view.
*
*/


// System Includes
#include <e32keys.h>
#include <barsread.h>               // For TResourceReader
#include <bautils.h>    
#include <StringLoader.h>
#include <evtmgmtui.rsg>
#include <eikenv.h>
#include <AknUtils.h>
#include <aknsfld.h> // For SearchField
	
// User Includes	  	
#include "evtmgmtuifiltermodel.h"
				   	  
// Constants
const TInt KMaxConversionBufferLength = 0x200;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupLBModel::CEvtMgmtUiStartupLBModel
// ---------------------------------------------------------------------------
//
CEvtMgmtUiFilterModel::CEvtMgmtUiFilterModel()
	{		
	}

// ---------------------------------------------------------------------------
// CEvtMgmtUiFilterModel::~CEvtMgmtUiFilterModel
// ---------------------------------------------------------------------------
//
CEvtMgmtUiFilterModel::~CEvtMgmtUiFilterModel()
	{	    
	// Delete the Buffers
	delete iBuffer;
	iBuffer = NULL;
	}
	
// ---------------------------------------------------------------------------
// CEvtMgmtUiFilterModel* CEvtMgmtUiFilterModel::NewL
// ---------------------------------------------------------------------------
//
CEvtMgmtUiFilterModel* CEvtMgmtUiFilterModel::NewL()
	{
	CEvtMgmtUiFilterModel* self = CEvtMgmtUiFilterModel::NewLC();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// CEvtMgmtUiFilterModel* CEvtMgmtUiFilterModel::NewLC
// ---------------------------------------------------------------------------
//
CEvtMgmtUiFilterModel* CEvtMgmtUiFilterModel::NewLC()
	{
	CEvtMgmtUiFilterModel* self = new( ELeave )CEvtMgmtUiFilterModel();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
		
// ---------------------------------------------------------------------------
// void CEvtMgmtUiFilterModel::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiFilterModel::ConstructL()
	{
    // Allocate the Conversion Buffer
	iBuffer = HBufC16::NewL( KMaxConversionBufferLength );
	}
        
// ---------------------------------------------------------------------------
// TInt CEvtMgmtUiFilterModel::MdcaCount() const
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiFilterModel::MdcaCount() const
	{
  	return iActualItemTextArray->MdcaCount();
	}
	
// ---------------------------------------------------------------------------
// TPtrC16 CEvtMgmtUiFilterModel::MdcaPoint() const
// ---------------------------------------------------------------------------
//
TPtrC16 CEvtMgmtUiFilterModel::MdcaPoint( TInt  aIndex ) const
	{
	if( !aIndex )
	    {
	    TRAP_IGNORE( UpdateTextL() );
        return *iBuffer;
	    }
	else
	    {
	    return iActualItemTextArray->MdcaPoint(aIndex);
	    }
	}

// ---------------------------------------------------------------------------
// CEvtMgmtUiFilterModel::GetZeroIndex()
// It will return Search Text
// ---------------------------------------------------------------------------
//
TPtrC16 CEvtMgmtUiFilterModel::GetZeroIndex()
    {
    return iActualItemTextArray->MdcaPoint(0);
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiFilterModel::GetSearchTextL()
// It will return Search Text
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiFilterModel::SetItemTextArray(MDesCArray* aActualItemTextArray)
    {
    iActualItemTextArray = NULL;
    iActualItemTextArray = aActualItemTextArray;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiFilterModel::GetSearchTextL()
// It will return Search Text
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiFilterModel::SetSearchField(CAknSearchField* aSearchField)
    {
    iSearchField = NULL;
    iSearchField = aSearchField;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiFilterModel::GetSearchTextL()
// It will return Search Text
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiFilterModel::GetSearchTextL(HBufC*& aString) const
    {
    if( aString )
        {
        delete aString;
        aString = NULL;
        }
    
    if(!iSearchField)
		{
        aString = KNullDesC().AllocL();
        return;
		}
    
    // Fetch filter from search field
    TInt searchTextLength = iSearchField->TextLength();
        
    if( searchTextLength )
        {
        aString = HBufC::NewL(searchTextLength);
        TPtr filter = aString->Des();
        iSearchField->GetSearchText(filter);
        }
    else
        {
        aString = KNullDesC().AllocL();
        }
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiFilterModel::UpdateTextL()
// It will return type of events
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiFilterModel::UpdateTextL() const
    {
    // Allocate the Conversion Buffer
	HBufC* buffer = NULL;
	
	GetSearchTextL(buffer);
	
    // Zero the internal buffer
    TPtr16 ptr( iBuffer->Des());
    ptr.Zero();
    
    if( buffer->Length() )
        {
        // Format "%d\t%S\t%S\t%d"
        ptr.AppendNum( 0 );
        ptr.Append( EKeyTab );
        ptr.Append( *buffer );
        ptr.Append( EKeyTab );
        ptr.Append( EKeyTab );
        }
    else
        {
        if( iActualItemTextArray->MdcaCount() > 1 )
			{
            ptr.Append( iActualItemTextArray->MdcaPoint(1) );
			}
		else
			{
	        // Format "%d\t%S\t%S\t%d"
	        ptr.AppendNum( 0 );
	        ptr.Append( EKeyTab );
	        ptr.Append( *buffer );
	        ptr.Append( EKeyTab );
	        ptr.Append( EKeyTab );
			}
        }
        
    delete buffer; 
    }
