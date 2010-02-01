/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  List box model for the list-box used to display the Location
*                Centre pop-up.
*
*/


// SYSTEM INCLUDES 

// USER INCLUDES
#include "lcpopuplistboxmodel.h"
#include "lclocationappinfo.h"
#include "lcerrors.h"

// CONSTANT DEFINTION
const TInt KLcBufferSize = 128;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CLcPopupListBoxModel::CLcPopupListBoxModel
// ---------------------------------------------------------------------------
//
CLcPopupListBoxModel::CLcPopupListBoxModel( CLcLocationAppInfoArray&	aAppArray )
	:iAppArray( &aAppArray )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }

// ---------------------------------------------------------------------------
// CLcPopupListBoxModel::~CLcPopupListBoxModel
// ---------------------------------------------------------------------------
//
CLcPopupListBoxModel::~CLcPopupListBoxModel()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete the MdcaPoint buffer
    delete iBuffer;
    }

// ---------------------------------------------------------------------------
// void CLcPopupListBoxModel::ConstructL
// ---------------------------------------------------------------------------
//
void CLcPopupListBoxModel::ConstructL()
    {
    // Create the Buffer for packing the MdcaPoint
    iBuffer = HBufC16::NewL( KLcBufferSize );
    }

// ---------------------------------------------------------------------------
// CLcPopupListBoxModel* CLcPopupListBoxModel::NewL
// ---------------------------------------------------------------------------
//
CLcPopupListBoxModel* CLcPopupListBoxModel::NewL( CLcLocationAppInfoArray&	aAppArray )
    {
    CLcPopupListBoxModel* self = new ( ELeave ) CLcPopupListBoxModel( aAppArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// void CLcPopupListBoxModel::UpdateModel
// ---------------------------------------------------------------------------
//
void CLcPopupListBoxModel::UpdateModel( CLcLocationAppInfoArray&	aAppArray )
	{	
	// Now assign the new one
	iAppArray = &aAppArray;
	}
	           
// ---------------------------------------------------------------------------
// TInt CLcPopupListBoxModel::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CLcPopupListBoxModel::MdcaCount() const
    {
    return iAppArray->Count();    	
    }
    
// ---------------------------------------------------------------------------
// TPtrC16 CLcPopupListBoxModel::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC16 CLcPopupListBoxModel::MdcaPoint(  TInt aIndex  ) const
    {            
    // Pack the contents into the Buffer. We dont have to re-allocate the buffer
    // everytime because the length of the name field is restricted in the
    // Location Centre Registration API
		    
    // Tabulator Defintion
    _LIT( KTab, "\t" );
    
    // Create formatted item string.
    // list_single_large_pane:
    // list item string format: "0\tTextLabel"
    // where 0 is an index to icon array
	TPtr16	buf( iBuffer->Des());
	buf.Zero();
	
	// Append the Icon Index
	buf.AppendNum( aIndex );
	
	// Append the Tab Key
	buf.Append( KTab );
	
	// Append the Application Information
	buf.Append(( *iAppArray )[aIndex].Name());
	
    return buf;
    }			
