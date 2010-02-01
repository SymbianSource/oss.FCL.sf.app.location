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
* Description:  Engine class for Events Management UI.
*
*/

#include <lbt.h> 

#include "evtevent.h"
#include "evtmgmtuilbtadapter.h"
#include "evteventmanager.h"
#include "evtmgmtuiengine.h"
#include "evtdebug.h"
#include "evttoneaction.h"

// Constants
static const TInt KHighAccValue = 500;
static const TInt KMidAccValue = 1500;

// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::ConstructL()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEngine::ConstructL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiEngine::ConstructL()" );
    
    iEventManager = CEvtEventManager::NewL();
    iEventManager->SetObserver(this);
           
    EVTUIDEBUG("Create lbtadapter object" );      
    iLbtAdapter = CEvtMgmtUiLbtAdapter::NewL(*this);
    
    EVTUIDEBUG("- CEvtMgmtUiEngine::ConstructL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtMgmtUiEngine* CEvtMgmtUiEngine::NewL()
    {
    CEvtMgmtUiEngine* self = CEvtMgmtUiEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtMgmtUiEngine* CEvtMgmtUiEngine::NewLC()
    {
    CEvtMgmtUiEngine* self = new( ELeave ) CEvtMgmtUiEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::CEvtMgmtUiEngine()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiEngine::CEvtMgmtUiEngine()
    {       
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::~CEvtMgmtUiEngine()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiEngine::~CEvtMgmtUiEngine()
    {        
    // Reseting the Observer array
    iObserverArray.Reset();
    iObserverArray.Close();
    
    delete iEventManager;   
    iEventManager = NULL;
    
    delete iLbtAdapter;
    iLbtAdapter = NULL;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::SetObserver()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::SetObserver(MEvtMgmtUiEngineObserver* aObserver)
    {
    // Append the observer to the observer array waiting for notifications
    iObserverArray.Append( aObserver );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::RemoveObserver()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::RemoveObserver(MEvtMgmtUiEngineObserver* aObserver)
    {
    // Remove the observer from receiving notifications
    for( TInt i = 0; i < iObserverArray.Count(); i++ )
        {
        if ( iObserverArray[i] == aObserver )
            {
            iObserverArray.Remove( i );
            break;   
            }
        }
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::EventL()
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtEvent* CEvtMgmtUiEngine::EventL( TEvtEventId aId )
    {        
    EVTUIDEBUG1("+ CEvtMgmtUiEngine::EventL() event id = %d", aId);
    
    CEvtEvent* evt = NULL;
    if( aId!= 0 )
	    {
	    evt = iEventManager->GetEventL( aId ); 
	    CleanupStack::PushL( evt );
	    RetrieveLocationL( aId, evt->Location() );
	    CleanupStack::Pop();
	    }
    else
	    {
	    evt = CEvtEvent::NewL();	
	    }
    
    EVTUIDEBUG("- CEvtMgmtUiEngine::EventL()" );
    return evt;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::AddEventL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::AddEventL( CEvtEvent& aEvtEvent )
    {  
    EVTUIDEBUG("+ CEvtMgmtUiEngine::AddEventL()" );
   
    CLbtTriggerEntry::TLbtTriggerState state;
    if(aEvtEvent.EventStatus()==EActive)
        {
        state = CLbtTriggerEntry::EStateEnabled;
        }
    else
        {
        state = CLbtTriggerEntry::EStateDisabled;
        }
        
    iEvent = &aEvtEvent;
     
    //create trigger
    if(iEvent->Subject().Compare(KNullDesC) == 0)
       iLbtAdapter->CreateTrigger(iEvent->Location(),TReal(iEvent->Radius()),_L("Un Named Location Event"), state );
    else
       iLbtAdapter->CreateTrigger(iEvent->Location(),TReal(iEvent->Radius()),iEvent->Subject(), state );

    EVTUIDEBUG("- CEvtMgmtUiEngine::AddEventL()" );  
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::ModifyEventL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::ModifyEventL( CEvtEvent& aEvtEvent, TEvtEventAttributeMask aEventAttributeMask )
    {  
    EVTUIDEBUG("+ CEvtMgmtUiEngine::ModifyEventL()" );
  
    //if loc and/or state information are changed
    if( (aEventAttributeMask & EEvtEventAttributeStatus) == EEvtEventAttributeStatus ||
       (aEventAttributeMask & EEvtEventAttributePlace) == EEvtEventAttributePlace ||
       (aEventAttributeMask & EEvtEventAttributeRadius) == EEvtEventAttributeRadius )
        {
        CLbtTriggerEntry::TLbtTriggerState state;
        if( aEvtEvent.EventStatus() == EActive )
          {
          state = CLbtTriggerEntry::EStateEnabled;
          }
        else
          {
          state = CLbtTriggerEntry::EStateDisabled;
          }
        
        iEvent = &aEvtEvent;
        
        iLbtAdapter->UpdateTriggerL(aEvtEvent.EventId(), aEvtEvent.Location(), 
                                aEvtEvent.Radius(), state );
        } 
    else
        {
        iEventManager->UpdateEventL(aEvtEvent);
        
        if ( iObserverArray.Count())
            {
            for( TInt i = 0; i < iObserverArray.Count(); i++ )
                {
                iObserverArray[i]->NotifyEventChangeL( KErrNone,CEvtMgmtUiLbtAdapter::ELbtAdapterUpdate );
                }
            }
        }

    EVTUIDEBUG("- CEvtMgmtUiEngine::ModifyEventL()" );  
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::DeleteEventL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::DeleteEventL( TEvtEventId aEventId )
    {   
    EVTUIDEBUG1("+ CEvtMgmtUiEngine::DeleteEventL() event id =%d", aEventId );   
    TInt err = KErrNone;
    if( aEventId!=0 ) //if it is present in lbt database        
        {      
        iLbtAdapter->DeleteTriggerL( aEventId );
        iEventManager->RemoveEventL( aEventId );
        }
        
    if ( iObserverArray.Count())
        {
        for( TInt i = 0; i < iObserverArray.Count(); i++ )
            {
            iObserverArray[i]->NotifyEventChangeL( err,CEvtMgmtUiLbtAdapter::ELbtAdapterDelete );
            }
        }
    EVTUIDEBUG("- CEvtMgmtUiEngine::DeleteEventL()" );  
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::DeleteEventsL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::DeleteEventsL( const RArray<TEvtEventId>* aEvtIdArray )
    {    
    iEvtIdArray = aEvtIdArray;
    iLbtAdapter->DeleteTriggersL( *aEvtIdArray );  
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::SetEventStateL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::SetEventStateL( TEvtEventStatus aEvtStatus, TEvtEventId aEventId)
    {
    EVTUIDEBUG("+ CEvtMgmtUiEngine::SetEventStateL()" );  
    TInt err = KErrNone;
    if( aEventId != 0 )
    	{ 
	    switch(aEvtStatus)
	        {
	        case EActive:
	            {            
	            iLbtAdapter->SetTriggerStateL( aEventId, CLbtTriggerEntry::EStateEnabled);
                iEventManager->UpdateEventStatusL( aEventId, aEvtStatus );
	            break;
	            }
	        case EDraft:
	            { 
	            iLbtAdapter->SetTriggerStateL( aEventId, CLbtTriggerEntry::EStateDisabled );
                iEventManager->UpdateEventStatusL( aEventId, aEvtStatus ); 
	            break;
	            }  
	        case ECompleted:
	            { 
                iEventManager->UpdateEventStatusL( aEventId, aEvtStatus ); 
	            iLbtAdapter->SetTriggerStateL( aEventId, CLbtTriggerEntry::EStateDisabled );
	            break;
	            }  
	        default:
	        	break;                  
	        } 
        }   
        
    if ( iObserverArray.Count())
        {
        for( TInt i = 0; i < iObserverArray.Count(); i++ )
            {
            iObserverArray[i]->NotifyEventChangeL( err,CEvtMgmtUiLbtAdapter::ELbtAdapterSetTriggerState );
            }
        }
	EVTUIDEBUG("- CEvtMgmtUiEngine::SetEventStateL()" ); 
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::SetEventsStateL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::SetEventsStateL( TEvtEventStatus aEvtStatus, const RArray<TEvtEventId>* aEvtIdArray )
    {
    EVTUIDEBUG("+ CEvtMgmtUiEngine::SetEventsStateL()" );
    iEvtIdArray = aEvtIdArray;
    iEvtStatus = aEvtStatus;
    switch(aEvtStatus)
        {
        case EActive:
            {            
            iLbtAdapter->SetTriggersStateL( *aEvtIdArray, CLbtTriggerEntry::EStateEnabled);
            break;
            }
        case EDraft:
        case ECompleted:
            {  
            iLbtAdapter->SetTriggersStateL( *aEvtIdArray, CLbtTriggerEntry::EStateDisabled);
            break;
            }  
        default:
            break;                  
        }                
    EVTUIDEBUG("- CEvtMgmtUiEngine::SetEventsStateL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::UpdateModelL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::UpdateModelL( RPointerArray<CEvtBasicEventInfo>& aEventsArray,
												TEvtEventStatusFilter  aEventStatusFilter )
    {
    EVTUIDEBUG("+ CEvtMgmtUiEngine::UpdateModelL()" );
  
    if( aEventsArray.Count() )
         {
         aEventsArray.ResetAndDestroy();         
         } 
         
    if( aEventStatusFilter == EEvtFilterAll )
        iEventManager->GetEventsL( aEventsArray );
    else
        iEventManager->GetEventsL( static_cast<TEvtEventStatus>(aEventStatusFilter), aEventsArray );
    
    EVTUIDEBUG("- CEvtMgmtUiEngine::UpdateModelL()" );  
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::SyncDbsL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::SyncDbsL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiEngine::SyncDbsL()" );
	TInt err;
    
    // Handle the Enabled and Disabled Trigger list from LBT 
    RArray<TLbtTriggerId> enabledtriggerList;
    RArray<TLbtTriggerId> disabledtriggerList;
    
	TRAP(err, iLbtAdapter->GetTriggerListL(disabledtriggerList, CLbtTriggerEntry::EStateDisabled ) );
	if(err != KErrNone)
		{
		EVTUIDEBUG1("CEvtMgmtUiEngine::SyncDbsL:Error while retrieving trigger ids = %d", err);
		disabledtriggerList.Close();
		enabledtriggerList.Close();
		User::Leave( err );
		}
		
	TRAP(err, iLbtAdapter->GetTriggerListL(enabledtriggerList, CLbtTriggerEntry::EStateEnabled ) );
	if(err != KErrNone)
		{
		EVTUIDEBUG1("CEvtMgmtUiEngine::SyncDbsL:Error while retrieving trigger ids = %d", err);
		disabledtriggerList.Close();
		enabledtriggerList.Close();
		User::Leave( err );
		}
				
	// Get Events list from EvtStorage
	RPointerArray<CEvtBasicEventInfo> eventsArray;
	RArray<TEvtEventId> removeArray;
	iEventManager->GetEventsL( eventsArray );
	
	// Compute
	TInt count = eventsArray.Count();
	TInt index=KErrNotFound;
	for(TInt i=0; i<count; i++)
	{
    if( eventsArray[i]->EventStatus() == EActive )
		{
        if( KErrNotFound != (index = disabledtriggerList.Find(eventsArray[i]->EventId())) )
            {
			iEventManager->UpdateEventStatusL( eventsArray[i]->EventId(), EDraft );
			disabledtriggerList.Remove(index);
            }
		else if( KErrNotFound == (index = enabledtriggerList.Find(eventsArray[i]->EventId())) )
		    {
			removeArray.Append(eventsArray[i]->EventId());
		    }
		else
		    {
		    enabledtriggerList.Remove(index);
		    }
		}
	else
		{
        if( KErrNotFound != (index = enabledtriggerList.Find(eventsArray[i]->EventId())) )
            {
			iEventManager->UpdateEventStatusL( eventsArray[i]->EventId(), EActive );
			enabledtriggerList.Remove(index);
            }
		else if( KErrNotFound == (index = disabledtriggerList.Find(eventsArray[i]->EventId())) )
		    {
			removeArray.Append(eventsArray[i]->EventId());
		    }
        else
            {
            disabledtriggerList.Remove(index);
            }
		}
	}

	// Update the Storage Db for removed triggers.
	if( removeArray.Count() != 0 )
		iEventManager->RemoveEventsL( removeArray );
	
	// Update the LBT Db for non existant triggers.
	count = enabledtriggerList.Count();
	for(TInt i=0; i<count; i++)
	    disabledtriggerList.Append(enabledtriggerList[i]);
	
	// We Nullify iEvtIdArray to ensure that observers are not notified of this delete triggers
	iEvtIdArray = NULL;
	if( disabledtriggerList.Count() != 0 )
    	iLbtAdapter->DeleteTriggersL( disabledtriggerList );  

	removeArray.Close();
	disabledtriggerList.Close();
	enabledtriggerList.Close();
	eventsArray.ResetAndDestroy();
	eventsArray.Close();
	EVTUIDEBUG("- CEvtMgmtUiEngine::SyncDbsL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::CalculateFiredAccuracyL()
// ---------------------------------------------------------------------------
//
EXPORT_C TEvtFireAccuracy CEvtMgmtUiEngine::CalculateFiredAccuracyL( 
											const TEvtEventId aEventId )
	{
	EVTUIDEBUG1("+ CEvtMgmtUiEngine::CalculateFiredAccuracyL() - %d", aEventId );
	TEvtFireAccuracy accuracy = ELowAccuracy;
	TReal32 distance(KMidAccValue+1);
	iLbtAdapter->GetFiredTriggerAccuracyL(aEventId, distance);
	
	EVTUIDEBUG1("= CalculateFiredAccuracyL() distance = %d", distance );
	
	if( distance <= KHighAccValue )
		accuracy = EHighAccuracy;
	else if( distance <= KMidAccValue )
		accuracy = EMidAccuracy;
	else
		accuracy = ELowAccuracy;
	
	EVTUIDEBUG("- CEvtMgmtUiEngine::CalculateFiredAccuracyL()" );
	return accuracy;
	}

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::HandleFiredTriggerStateL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::HandleFiredTriggerStateL( 
											const TEvtEventId aEventId )
	{	
	EVTUIDEBUG1("+ CEvtMgmtUiEngine::HandleFiredTriggerStateL() - %d", aEventId );    
    CEvtEvent* evt = NULL;
    if( aEventId!= 0 )
	    {
	    evt = iEventManager->GetEventL( aEventId ); 
	    if( evt )
		    {
		    CleanupStack::PushL( evt );	
			if( !evt->Repeat() )
				{
				SetEventStateL( ECompleted, aEventId );
				}
		    CleanupStack::PopAndDestroy( evt );
		    }	
	    }
	EVTUIDEBUG("- CEvtMgmtUiEngine::HandleFiredTriggerStateL()" );
	}

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::CancelRequest()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::CancelRequest()
    {     
    EVTUIDEBUG("+ CEvtMgmtUiEngine::CancelRequest()" );
	if( iLbtAdapter->IsActive() )
    	iLbtAdapter->CancelRequest();
    EVTUIDEBUG("- CEvtMgmtUiEngine::CancelRequest()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::HandleStorageDbChangedL()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEngine::HandleStorageDbChangedL()
    {     
    // Notify all observers
    if ( iObserverArray.Count())
        {
        for( TInt i = 0; i < iObserverArray.Count(); i++ )
            {
            iObserverArray[i]->NotifyModelChangeL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::RetrieveLocationL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtMgmtUiEngine::RetrieveLocationL( TEvtEventId aEventId, TCoordinate& aCoordinate )
    {
    EVTUIDEBUG("+ CEvtMgmtUiEngine::RetrieveLocationL()" );
    iLbtAdapter->RetrieveLocationL( aEventId, aCoordinate);
    EVTUIDEBUG("- CEvtMgmtUiEngine::RetrieveLocationL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::NotifyLbtChangeL()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEngine::NotifyLbtChangeL(TLbtTriggerId &  aTriggerId, TInt aState )
    {   
    EVTUIDEBUG("+ CEvtMgmtUiEngine::NotifyLbtChangeL()" );
    
    // Notify all observers
    if ( iObserverArray.Count() )
        {
        for( TInt i = 0; i < iObserverArray.Count(); i++ )
            {
            iObserverArray[i]->NotifyEventChangeL( KErrNone,aState );
            }
        }
    
    //iState = aState;
    switch(aState)
        {
        case CEvtMgmtUiLbtAdapter::ELbtAdapterCreate:       
            {
            EVTUIDEBUG1("Trigger is created TriggerId=%d", aTriggerId );
            
            // Update the the command Line argument (Temporarily commented)
            iLbtAdapter->UpdateCmdLineArgL( aTriggerId, aTriggerId );
            
			// Add the new Event to the Local Db.
			if( iEvent->EventId() == 0 )
			   {
			   iEvent->SetEventId( aTriggerId );
			   
			   EVTUIDEBUG("========== Event Details ============================" );
			   TBuf<256>debug;
			   debug.Copy( iEvent->Subject() );
			   EVTUIDEBUG1("Subject: %S", &debug);
			   debug.Copy( iEvent->Place() );
			   EVTUIDEBUG1("Place: %S", &debug);
			   EVTUIDEBUG1("Radius: %f", iEvent->Radius());
			   EVTUIDEBUG1("Repeat: %d", iEvent->Repeat());
			   
			   CEvtToneAction* tone = CEvtToneAction::NewLC();
				if( iEvent->HasAction() )
					{              
					tone->InternalizeL( iEvent->Action() );
					debug.Copy( tone->FileName() );
					EVTUIDEBUG1("Ringtone: %S", &debug);
					EVTUIDEBUG1("Audio Loop: %d", tone->ToneLoop());    
					}
			    CleanupStack::PopAndDestroy(tone);
			    
			   EVTUIDEBUG("==========End Event Details ============================" );    
			   iEventManager->AddEventL( *iEvent );       
			   
			   EVTUIDEBUG1( "Event is added to local database eventid = %d" , iEvent->EventId() );
			   }
            break;
            }
        case CEvtMgmtUiLbtAdapter::ELbtAdapterUpdate:       
            {         
            iEventManager->UpdateEventL(*iEvent);
            break;
            }    
        case CEvtMgmtUiLbtAdapter::ELbtAdapterDeleteTriggers:
            {           
            if(iEvtIdArray && iEvtIdArray->Count())
                {
                iEventManager->RemoveEventsL( *iEvtIdArray );
                iEvtIdArray = NULL;
                }
            else
                {
                return; // This condition is satisfied by DeleteTriggers in SyncDbL
                }
            break;
            }
        case CEvtMgmtUiLbtAdapter::ELbtAdapterSetTriggersState:
            {
            if(iEvtIdArray && iEvtIdArray->Count())
                {
                iEventManager->UpdateEventsStatusL( *iEvtIdArray, iEvtStatus );
                iEvtIdArray = NULL;
                }
            }
            break;
        default:
            {
            //error
            break;
            }
        }
		
    EVTUIDEBUG("- CEvtMgmtUiEngine::NotifyLbtChangeL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::NotifyLbtError()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEngine::NotifyLbtError(TInt aErrorCode, TInt aState )
    {   
    EVTUIDEBUG1("+ CEvtMgmtUiEngine::NotifyLbtErrorL() error =%d", aErrorCode);
    // Notify all observers
    if ( iObserverArray.Count())
        {
        for( TInt i = 0; i < iObserverArray.Count(); i++ )
            {
            TRAP_IGNORE( iObserverArray[i]->NotifyEventChangeL( aErrorCode, aState ));
            }
        }
    
    EVTUIDEBUG("- CEvtMgmtUiEngine::NotifyLbtErrorL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEngine::NotifyTriggerChangeL()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEngine::NotifyTriggerChangeL( const TLbtTriggerId &  aTriggerId,
    								TLbtTriggerChangeType aType )
	{
	EVTUIDEBUG("+ CEvtMgmtUiEngine::NotifyTriggerChangeL()" );
	CLbtTriggerEntry::TLbtTriggerState state = CLbtTriggerEntry::EStateDisabled;
	CEvtEvent* evt = NULL;
	switch( aType )
		{
		case ETriggerMultipleModified:
			SyncDbsL();
			break;
		case ETriggerModified:
			state = iLbtAdapter->GetTriggerStateL( aTriggerId );
			
			evt = iEventManager->GetEventL( aTriggerId );
			CleanupStack::PushL( evt );
			if( evt->EventStatus() == EActive )
				{
				if( CLbtTriggerEntry::EStateDisabled == state )
					{
					iEventManager->UpdateEventStatusL( evt->EventId(), ECompleted );
					}
				}
			else
				{
				if( CLbtTriggerEntry::EStateEnabled == state )
					{
					iEventManager->UpdateEventStatusL( evt->EventId(), EActive );
					}
				}
			CleanupStack::PopAndDestroy( evt );
			break;
		case ETriggerDeleted:
			iEventManager->RemoveEventL( aTriggerId );
			break;
		default:
			break;
		}
	EVTUIDEBUG("- CEvtMgmtUiEngine::NotifyTriggerChangeL()" );
	}
//End of file
