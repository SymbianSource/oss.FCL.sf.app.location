/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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

// User Include
#include "evtmgmtuimodel.h"
#include "evtbasiceventinfo.h"
#include "evtdebug.h"
		  					   	  
// Constants
const TInt KPreDefinedItems = 1;
const TInt KMaxConversionBufferLength = 0x200;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupLBModel::CEvtMgmtUiStartupLBModel
// ---------------------------------------------------------------------------
//
CEvtMgmtUiModel::CEvtMgmtUiModel()
	{		
	}

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::~CEvtMgmtUiModel
// ---------------------------------------------------------------------------
//
CEvtMgmtUiModel::~CEvtMgmtUiModel()
	{	    
	// Delete the Buffers
	delete iBuffer;
	iBuffer = NULL;
	
	delete iCreateNew;
	iCreateNew = NULL;
	
	delete iUnnamed;
	iUnnamed = NULL;
	
	iEventsArray.ResetAndDestroy();
	iEventsArray.Close();
	}
	
// ---------------------------------------------------------------------------
// CEvtMgmtUiModel* CEvtMgmtUiModel::NewL
// ---------------------------------------------------------------------------
//
CEvtMgmtUiModel* CEvtMgmtUiModel::NewL()
	{
	CEvtMgmtUiModel* self = CEvtMgmtUiModel::NewLC();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel* CEvtMgmtUiModel::NewLC
// ---------------------------------------------------------------------------
//
CEvtMgmtUiModel* CEvtMgmtUiModel::NewLC()
	{
	CEvtMgmtUiModel* self = new( ELeave )CEvtMgmtUiModel();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
		
// ---------------------------------------------------------------------------
// void CEvtMgmtUiModel::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiModel::ConstructL()
	{
	EVTUIDEBUG( "+ CEvtMgmtUiModel::ConstructL()" );
	    
    // Allocate the Conversion Buffer
	iBuffer = HBufC16::NewL( KMaxConversionBufferLength );
        
    // Open the Resource File
  //  OpenLocalizedResourceFileL( KEvtEngineResFileName, iResourceLoader );
	
	// Allocate and set the access point title
 	iCreateNew = StringLoader::LoadL( R_EVTUI_CREATENEW_LIST );
 	iUnnamed = StringLoader::LoadL( R_EVTUI_UNNAMED ); 
 	
 	iStatusFilter = CEvtMgmtUiEngine::EEvtFilterAll;
 	
 	EVTUIDEBUG( "- CEvtMgmtUiModel::ConstructL()" );
 	    
	}
        
// ---------------------------------------------------------------------------
// TInt CEvtMgmtUiModel::MdcaCount() const
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiModel::MdcaCount() const
	{
	// Return the Model count based on the Status Filter
  	switch( iStatusFilter )
  		{
  		case CEvtMgmtUiEngine::EEvtFilterAll:
  			return KPreDefinedItems + iActiveTriggers + iDraftTriggers + iCompletedTriggers;
  		case CEvtMgmtUiEngine::EEvtFilterActive:
  			return KPreDefinedItems + iActiveTriggers;
  		case CEvtMgmtUiEngine::EEvtFilterDraft:
  			return KPreDefinedItems + iDraftTriggers;
        case CEvtMgmtUiEngine::EEvtFilterCompleted:
            return KPreDefinedItems + iCompletedTriggers;
  		default:
  			return KPreDefinedItems + iActiveTriggers + iDraftTriggers + iCompletedTriggers;
		}
	}
	
// ---------------------------------------------------------------------------
// TPtrC16 CEvtMgmtUiModel::MdcaPoint() const
// ---------------------------------------------------------------------------
//
TPtrC16 CEvtMgmtUiModel::MdcaPoint( TInt  aIndex ) const
	{
	// Zero the internal buffer
	TPtr16 ptr( iBuffer->Des());
	TPtrC subject(KNullDesC);
	TPtrC place(KNullDesC);
	ptr.Zero();
	
	// Format "%d\t%S\t%S\t%d"
	
	// Append the Icon ID
	if( aIndex==0 )
	    ptr.AppendNum( 1 );
	else
	    {	
		// Get the Index based on the Status Filter
		aIndex = IndexBasedOnFilter( aIndex );    
	    switch(iEventsArray[aIndex-1]->EventStatus())
            {
            case EActive:
                {
                ptr.AppendNum( 2 );
                break;
                }
            case EDraft:
                {
                ptr.AppendNum( 3 );
                break;
                }           
            case ECompleted:
                {
                ptr.AppendNum( 4 );
                break;
                }
            }
	    }
	
	// Append the first tab
	ptr.Append( EKeyTab );
	
	// Subject
	if(aIndex==0)
	    {
	    ptr.Append( *iCreateNew );
	    ptr.Append( EKeyTab );
	    ptr.Append( EKeyTab );
	    }
	else
	    {
	    subject.Set(iEventsArray[aIndex-1]->Subject());
	    place.Set(iEventsArray[aIndex-1]->Place());
	    if(subject.Compare(KNullDesC)==0 && place.Compare(KNullDesC)==0)
            {
            ptr.Append(*iUnnamed);            
            }
        else
            ptr.Append(subject);
	    
    	//Place
    	ptr.Append( EKeyTab );
    	
    	    {
    	    HBufC* localPlace = place.Alloc();   
			if( localPlace )
				{
	            TPtr placePtr = localPlace->Des();
	    	    ptr.Append(placePtr);
	    	    delete localPlace;   
				} 
    	    }
    	ptr.Append( EKeyTab );	
    	if( iEventsArray[aIndex-1]->Repeat() )
    	    ptr.AppendNum(5);
	    }	
	
    return *iBuffer;
	}
        
// ---------------------------------------------------------------------------
// TInt CEvtMgmtUiModel::IndexBasedOnFilter() const
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiModel::IndexBasedOnFilter( TInt aIndex ) const
	{
	// We calculate the Index based on the Status Filter
	// This is calculated based on the assumption of the Order of the Events
	// which is in the order of Active->Draft->Completed
  	switch( iStatusFilter )
  		{
  		case CEvtMgmtUiEngine::EEvtFilterAll:
  		case CEvtMgmtUiEngine::EEvtFilterActive:
  			return aIndex;
  		case CEvtMgmtUiEngine::EEvtFilterDraft:
  			return iActiveTriggers + aIndex;
  		case CEvtMgmtUiEngine::EEvtFilterCompleted:
  			return iActiveTriggers + iDraftTriggers + aIndex;
  		default:
  			return aIndex;
		}
	}

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::IndexWithoutFilter()
// It will Index without filter provided the index based on filter.
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiModel::IndexWithoutFilter( TInt aIndex ) const
    {
	// The Actual Index in the model is calculated based on the Status Filter
  	switch( iStatusFilter )
  		{
  		case CEvtMgmtUiEngine::EEvtFilterAll:
  		case CEvtMgmtUiEngine::EEvtFilterActive:
  			return KPreDefinedItems + aIndex;
  		case CEvtMgmtUiEngine::EEvtFilterDraft:
  			return KPreDefinedItems + aIndex - iActiveTriggers;
        case CEvtMgmtUiEngine::EEvtFilterCompleted:
            return KPreDefinedItems + aIndex - ( iActiveTriggers + iDraftTriggers );
  		default:
  			return KPreDefinedItems + aIndex;
		}
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::EventsArray()
// It will return Events array object reference.
// ---------------------------------------------------------------------------
//
RPointerArray<CEvtBasicEventInfo>& CEvtMgmtUiModel::EventsArray()
    {
    return iEventsArray;
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::TotalEvents()
// It will return total events
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiModel::TotalEvents()
	{
	return iEventsArray.Count();
	}
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::StatusFilter()
// It will Show Events based on Filter
// ---------------------------------------------------------------------------
//
CEvtMgmtUiEngine::TEvtEventStatusFilter CEvtMgmtUiModel::StatusFilter( )
	{
	return iStatusFilter;
	}
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::SetStatusFilter()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiModel::SetStatusFilter( CEvtMgmtUiEngine::TEvtEventStatusFilter
                                        aFilter )
	{
	iStatusFilter = aFilter;
	}
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::EventFromModel()
// It will return  eventinfo object.
// ---------------------------------------------------------------------------
//
CEvtBasicEventInfo& CEvtMgmtUiModel::EventFromModel( TInt aIndex )
    {
    return *(iEventsArray[IndexBasedOnFilter( aIndex )-1]);
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::MandatoryFieldsFilled()
// ---------------------------------------------------------------------------
//
TBool CEvtMgmtUiModel::MandatoryFieldsFilled(  TInt aIndex  )
    {
	// Check the Array boundary conditions
    if( aIndex == 0 || aIndex > iEventsArray.Count())
        return EFalse;
    
	// Check the Status of the event considering the current Status Filter
    if( iEventsArray[IndexBasedOnFilter( aIndex )-1]->Subject().Compare(KNullDesC)==0 || 
    			iEventsArray[IndexBasedOnFilter( aIndex )-1]->Place().Compare(KNullDesC)==0 )
       return EFalse;
    else
        return ETrue;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::EventId()
// It will return  event id.
// ---------------------------------------------------------------------------
//
TEvtEventId CEvtMgmtUiModel::EventId( TInt aIndex )
    {  
	// Check the Array boundary conditions
    if(aIndex<1 || aIndex > iEventsArray.Count() )
        return 0;
    return iEventsArray[IndexBasedOnFilter( aIndex )-1]->EventId();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::EventStatus()
// It will return  event status.
// ---------------------------------------------------------------------------
//
TEvtEventStatus CEvtMgmtUiModel::EventStatus( TInt aIndex )
    {
	// Check the Array boundary conditions
    if(aIndex<1 || aIndex > iEventsArray.Count() )
        return EActive;
    return iEventsArray[IndexBasedOnFilter( aIndex )-1]->EventStatus();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::UpdatedCurrentItemIndex()
// It will return  updated current item index
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiModel::UpdatedCurrentItemIndex( )
    {
  	switch( iStatusFilter )
  		{
  		case CEvtMgmtUiEngine::EEvtFilterAll:
  			if(iCurrentEventIndex<0 || iCurrentEventIndex > (iActiveTriggers+iDraftTriggers+iCompletedTriggers) )
  				iCurrentEventIndex = KPreDefinedItems + iActiveTriggers + iDraftTriggers + iCompletedTriggers;
  			break;
  		case CEvtMgmtUiEngine::EEvtFilterActive:
  			if(iCurrentEventIndex<0 || iCurrentEventIndex > iActiveTriggers )
  				iCurrentEventIndex = KPreDefinedItems + iActiveTriggers;
  			break;
  		case CEvtMgmtUiEngine::EEvtFilterDraft:
  			if(iCurrentEventIndex<0 || iCurrentEventIndex > iDraftTriggers )
  				iCurrentEventIndex = KPreDefinedItems + iDraftTriggers;
  			break;
        case CEvtMgmtUiEngine::EEvtFilterCompleted:
            if(iCurrentEventIndex<0 || iCurrentEventIndex > iCompletedTriggers )
                iCurrentEventIndex = KPreDefinedItems + iCompletedTriggers;
            break;
  		default:
  			iCurrentEventIndex = 0;
  			break;
		}
	return iCurrentEventIndex;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::SetCurrentItemId()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiModel::SetCurrentItemId( TInt aItemIndex )
    {
    if(aItemIndex > 0 && aItemIndex <= iEventsArray.Count() )
 		{
        iCurrentEventId = iEventsArray[IndexBasedOnFilter( aItemIndex )-1]->EventId();
		iCurrentEventIndex = aItemIndex - 1;
    	}
    else
    	{
        iCurrentEventId = 0; //if focus is on 'create new event'
		iCurrentEventIndex = 0;
    	}
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::UpdateEventsStatus()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiModel::UpdateEventsStatus()
    {
	// Maintain the previous count to compare with the updated model
    TInt prevCount = iActiveTriggers + iDraftTriggers + iCompletedTriggers;
    iActiveTriggers = 0;
    iDraftTriggers = 0;
    iCompletedTriggers = 0;
    TInt cnt = iEventsArray.Count();
	TEvtModelUpdateState updateState;
    
	// Get the update type based on the previous and current model count.
    if( prevCount < cnt )
    	{
    	updateState = EEvtCreate;
    	}
    else if( prevCount > cnt )
    	{
    	updateState = EEvtDelete;
    	iCurrentEventId = 0;
    	}
    else
    	{
    	updateState = EEvtModify;
    	}
    
	// Loop through the model to update the Event Status mask.
    TInt index;
    iEventsStatusMask = 0;
    for( index = 0; index<cnt; index++ )
        {
        
        if( updateState == EEvtCreate && iCurrentEventId < iEventsArray[index]->EventId() )	
        	{
        	iCurrentEventId = iEventsArray[index]->EventId();
        	iCurrentEventIndex = index;
        	}
        	
        if( updateState == EEvtModify && iCurrentEventId == iEventsArray[index]->EventId() )	
        	{
        	iCurrentEventId = iEventsArray[index]->EventId();
        	iCurrentEventIndex = index;
        	}
        	
		// Append the Event status to Event status mask.	
        switch(iEventsArray[index]->EventStatus())
            {
            case EActive:
                {
                iActiveTriggers++;
                iEventsStatusMask|= EEvtShowAttributeActive;
                break;
                }
            case EDraft:
                {
                iDraftTriggers++;
                iEventsStatusMask|= EEvtShowAttributeDraft;
                break;
                }
            case ECompleted:
                {
                iCompletedTriggers++;
                iEventsStatusMask|= EEvtShowAttributeCompleted;
                break;
                }
            }        
        }   
    
    // If the current view(show) is empty, then switch to Filter All View
    switch(iStatusFilter)
        {
        case CEvtMgmtUiEngine::EEvtFilterActive:
            {
            if( !iActiveTriggers )
                iStatusFilter = CEvtMgmtUiEngine::EEvtFilterAll;
            break;
            }
        case CEvtMgmtUiEngine::EEvtFilterDraft:
            {
            if( !iDraftTriggers )
                iStatusFilter = CEvtMgmtUiEngine::EEvtFilterAll;
            break;
            }
        case CEvtMgmtUiEngine::EEvtFilterCompleted:
            {
            if( !iCompletedTriggers )
                iStatusFilter = CEvtMgmtUiEngine::EEvtFilterAll;
            break;
            }
        } 
    
	// Update the index for create and modify
	if( updateState == EEvtCreate || updateState == EEvtModify )
		iCurrentEventIndex = IndexWithoutFilter( iCurrentEventIndex );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::EventsStatus()
// It will return type of events
// ---------------------------------------------------------------------------
//
TEvtEventsStatusMask CEvtMgmtUiModel::EventsStatus()
    {
    return iEventsStatusMask;   
    }
