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
*                Centre Application
*
*/


// SYSTEM INCLUDES 

// USER INCLUDES
#include "lclistboxmodel.h"
#include "lclocationappinfo.h"
#include "lcerrors.h"

// CONSTANT DEFINTION
const TInt KLcBufferSize = 128;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CLcListBoxModel::CLcListBoxModel
// ---------------------------------------------------------------------------
//
CLcListBoxModel::CLcListBoxModel( CLcLocationAppInfoArray*	aAppArray )
	:iAppArray( aAppArray )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }

// ---------------------------------------------------------------------------
// CLcListBoxModel::~CLcListBoxModel
// ---------------------------------------------------------------------------
//
CLcListBoxModel::~CLcListBoxModel()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete the MdcaPoint buffer
    delete iBuffer;
    
    // Delete the Application information array
    delete iAppArray;
    }

// ---------------------------------------------------------------------------
// void CLcListBoxModel::ConstructL
// ---------------------------------------------------------------------------
//
void CLcListBoxModel::ConstructL()
    {
    // Create the Buffer for packing the MdcaPoint
    iBuffer = HBufC16::NewL( KLcBufferSize );
    }

// ---------------------------------------------------------------------------
// CLcListBoxModel* CLcListBoxModel::NewL
// ---------------------------------------------------------------------------
//
CLcListBoxModel* CLcListBoxModel::NewL( CLcLocationAppInfoArray*	aAppArray )
    {
    CLcListBoxModel* self = new ( ELeave ) CLcListBoxModel( aAppArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// void CLcListBoxModel::UpdateModel
// ---------------------------------------------------------------------------
//
void CLcListBoxModel::UpdateModel( CLcLocationAppInfoArray*	aAppArray )
	{
	// The model contents have changed. We have to remove the old App array
	// and assign the new one.
	
	delete iAppArray;
	
	// Now assign the new one
	iAppArray = aAppArray;
	}

// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray& CLcListBoxModel::AppArray
// ---------------------------------------------------------------------------
// 
CLcLocationAppInfoArray& CLcListBoxModel::AppArray()
	{
    // If the Array is not set then there is a programming error from which
    // we cannot recover. So Panic the Application
    if( !iAppArray )
        {
        User::Panic( KLcPanicCategory, ELcInvalidIndex );
        }
	return *iAppArray;
	}

// ---------------------------------------------------------------------------
// TInt CLcListBoxModel::AppInfoId
// ---------------------------------------------------------------------------
//
TPtrC CLcListBoxModel::AppInfoId( TInt aIndex ) const
	{
    // Check if the element requested is out of array bounds
    if( aIndex < 0 || aIndex >= iAppArray->Count())
        {
        User::Panic( KLcPanicCategory, ELcInvalidIndex );
        }
        	
	return (*iAppArray)[ aIndex ].Id();
	}
	           
// ---------------------------------------------------------------------------
// TInt CLcListBoxModel::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CLcListBoxModel::MdcaCount() const
    {
    if ( iAppArray )
    	{
    	return iAppArray->Count();    	
    	}
	else
		{
		return 0;
		}    	
    }
    
// ---------------------------------------------------------------------------
// TPtrC16 CLcListBoxModel::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC16 CLcListBoxModel::MdcaPoint(  TInt aIndex  ) const
    {
    // If the Array is not set then there is a programming error from which
    // we cannot recover. So Panic the Application
    if( !iAppArray )
        {
        User::Panic( KLcPanicCategory, ELcInvalidIndex );
        }
            
    // Pack the contents into the Buffer. We dont have to re-allocate the buffer
    // everytime because the length of the name field is restricted in the
    // Location Centre Registration API
		    
    // Tabulator Defintion
    _LIT( KTab, "\t" );
    
    // Create formatted item string.
    // list_single_large_pane:
    // list item string format: "1\tTextLabel\t0\t0"
    // where 0 is an index to icon array
	TPtr16	buf( iBuffer->Des());
	buf.Zero();
	
	// Append the Icon Index
	buf.AppendNum( aIndex );
	
	// Append the Tab Key
	buf.Append( KTab );
	
	// Append the Application Information
	buf.Append( (*iAppArray)[aIndex].Name());
	
	// Append the Tab Key
	buf.Append( KTab );
	
	// Append the Tab Key
	buf.Append( KTab );   
    return buf;
    }			
