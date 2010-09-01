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
* Description:  Handler for Snoozing of Event
*
*/


// INCLUDE FILES
#include <centralrepository.h>

#include "evthandlerserverconsts.h"
#include "evtdebug.h"
#include "evtsnoozehandler.h"
#include "evtstoragedomaincrkeys.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEvtSnoozeHandler::NewL
// -----------------------------------------------------------------------------
CEvtSnoozeHandler* CEvtSnoozeHandler::NewL(
        	MEvtSnoozeObserver& aSnoozeObserver )
    {
    CEvtSnoozeHandler* self = new ( ELeave ) CEvtSnoozeHandler( aSnoozeObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CEvtSnoozeHandler::CEvtSnoozeHandler
// -----------------------------------------------------------------------------
CEvtSnoozeHandler::CEvtSnoozeHandler( 
	MEvtSnoozeObserver& aSnoozeObserver )
    : CTimer( EPriorityStandard ), iSnoozeObserver( aSnoozeObserver ), 
		iSnoozeQueue( KEvtItemArrayGranularity ), 
		iSnoozeCompleteQueue( KEvtItemArrayGranularity )
    {
    }

// -----------------------------------------------------------------------------
// CEvtSnoozeHandler::~CEvtSnoozeHandler
// -----------------------------------------------------------------------------
CEvtSnoozeHandler::~CEvtSnoozeHandler()
    {
	if( iIntervalNotifyHandler )
		{
		iIntervalNotifyHandler->StopListening();
		}

	if( iPeriodNotifyHandler )
		{
		iPeriodNotifyHandler->StopListening();
		}

    delete iIntervalNotifyHandler;
	delete iPeriodNotifyHandler;
	delete iSettingSession;
	
	iSnoozeQueue.Reset();
    iSnoozeQueue.Close();
	iSnoozeCompleteQueue.Reset();
    iSnoozeCompleteQueue.Close();
    }

// -----------------------------------------------------------------------------
// CEvtSnoozeHandler::ConstructL
// -----------------------------------------------------------------------------
void CEvtSnoozeHandler::ConstructL()
    {
    EVTUIDEBUG("+ CEvtSnoozeHandler::ConstructL");    
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    
	// Create the Session to EventsUi Settings
	iSettingSession = CRepository::NewL( TUid::Uid( KCRUidEvtStorageDb ) );
	
	// Create the Cenrep Key observer for Interval and Period.
    iIntervalNotifyHandler = CCenRepNotifyHandler::NewL(*this,
                                                     *iSettingSession,
                                                     CCenRepNotifyHandler::EIntKey,
                                                     KEvtSnoozeInterval);

	iPeriodNotifyHandler = CCenRepNotifyHandler::NewL(*this,
                                                     *iSettingSession,
                                                     CCenRepNotifyHandler::EIntKey,
                                                     KEvtSnoozePeriod);

	// Get the Initial values from settings.
    TInt period;
	User::LeaveIfError( iSettingSession->Get( KEvtSnoozeInterval, iSnoozeInterval ) );
    User::LeaveIfError( iSettingSession->Get( KEvtSnoozePeriod, period ) );

	// Start Listening.
    iIntervalNotifyHandler->StartListeningL();
    iPeriodNotifyHandler->StartListeningL();
    
    iSnoozeCount = 1;
    if( period && iSnoozeInterval/period )
        iSnoozeCount = iSnoozeInterval/period;
        
    EVTUIDEBUG( "- CEvtSnoozeHandler::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CEvtSnoozeHandler::RemoveIfPresent
// -----------------------------------------------------------------------------
void CEvtSnoozeHandler::RemoveIfPresent(const TEvtSnoozeItem& aSnoozeItem)
	{    
    EVTUIDEBUG( "+ CEvtSnoozeHandler::RemoveIfPresent" );	
    
    // Remove if already present
    TIdentityRelation<TEvtSnoozeItem> matcher(TEvtSnoozeItem::Match);
    TInt index = iSnoozeCompleteQueue.Find(aSnoozeItem,matcher);

    if( index != KErrNotFound )
        { 
        iSnoozeCompleteQueue.Remove( index );
        }
    else
        {
        index = iSnoozeQueue.Find(aSnoozeItem,matcher);
        if( index != KErrNotFound )
            { 
            iSnoozeQueue.Remove( index );
            
            // Cancel the timer if it is already snoozing for
            // requested event id
            if( index == 0 )
                {
                Cancel();
                
                // Start the Timer for next Item
                if( !IsActive() && iSnoozeQueue.Count() )
                    At( iSnoozeQueue[ 0 ].iTime );
                }
            }
        }
    
    EVTUIDEBUG( "- CEvtSnoozeHandler::RemoveIfPresent" );
	}

// -----------------------------------------------------------------------------
// CEvtSnoozeHandler::IsSnoozeQueEmpty
// -----------------------------------------------------------------------------
TBool CEvtSnoozeHandler::IsSnoozeQueEmpty()
	{    
    EVTUIDEBUG( "+ CEvtSnoozeHandler::IsSnoozeQueEmpty" );	
    
    if( IsActive() || iSnoozeCompleteQueue.Count() || iSnoozeCompleteQueue.Count() )
        { 
        return EFalse;
        }
    return ETrue;
	}

// -----------------------------------------------------------------------------
// CEvtSnoozeHandler::Snooze
// -----------------------------------------------------------------------------
void CEvtSnoozeHandler::Snooze(TEvtSnoozeItem aSnoozeItem)
	{    
    EVTUIDEBUG( "+ CEvtSnoozeHandler::Snooze" );	
    
    TTimeIntervalMinutes interval(iSnoozeInterval); 
    aSnoozeItem.iTime.HomeTime();
    
    // Add the Snooze Time
    aSnoozeItem.iTime = aSnoozeItem.iTime + interval;
     
    TIdentityRelation<TEvtSnoozeItem> matcher(TEvtSnoozeItem::Match);
    TInt index = iSnoozeCompleteQueue.Find(aSnoozeItem,matcher);

    if( index != KErrNotFound )
        { 
        // Increment Snooze count
        aSnoozeItem.iCount = iSnoozeCompleteQueue[index].iCount + 1;
        
        // Remove the already present item
        iSnoozeCompleteQueue.Remove( index );
        }
    else
        {
        index = iSnoozeQueue.Find(aSnoozeItem,matcher);
        if( index != KErrNotFound )
            { 
            // Cancel the timer if it is already snoozing for
            // requested event id
            if( index == 0 )
                {
                Cancel();
                }
            
            // Increment Snooze count
            aSnoozeItem.iCount = iSnoozeQueue[index].iCount + 1;
            
            // Remove the already present item
            iSnoozeQueue.Remove( index );
            }
        else
            {
            // First Snooze
            aSnoozeItem.iCount = 1;
            }
        }
    
    // Append this element to the queue
    iSnoozeQueue.Append( aSnoozeItem );
    
    // Start the Timer if it is not already started
    if(!IsActive())
        At( iSnoozeQueue[ 0 ].iTime );
    
    EVTUIDEBUG( "- CEvtSnoozeHandler::Snooze" );
	}

// -----------------------------------------------------------------------------
// CEvtSnoozeHandler::RunL
// -----------------------------------------------------------------------------
void CEvtSnoozeHandler::RunL()
    {
    EVTUIDEBUG1( "+ CEvtSnoozeHandler::RunL %d", iStatus.Int() );
    
    if( iSnoozeQueue.Count() > 1 )
        At( iSnoozeQueue[ 1 ].iTime );
        
    if( iSnoozeQueue[ 0 ].iCount < iSnoozeCount )
        {
        // Notify on Snooze complete
        iSnoozeObserver.SnoozeCompleteL( iSnoozeQueue[ 0 ], ETrue ); 
        
        // Move the snooze item to complete
        iSnoozeCompleteQueue.Append( iSnoozeQueue[ 0 ] );
        iSnoozeQueue.Remove(0);
        }
    else
        {
        // Notify on Snooze complete
        iSnoozeObserver.SnoozeCompleteL( iSnoozeQueue[ 0 ], EFalse); 
        
        // Remove Snoozed Item
        iSnoozeQueue.Remove(0);
        }
        
    EVTUIDEBUG( "- CEvtSnoozeHandler::RunL" );
    }

// -----------------------------------------------------------------------------
// CEvtSnoozeHandler::HandleNotifyInt()
// -----------------------------------------------------------------------------
//
void CEvtSnoozeHandler::HandleNotifyInt(TUint32 aId, TInt aNewValue)
    {

   	if ( aId == KEvtSnoozeInterval )
   		{
   		iSnoozeInterval = aNewValue;
   		}
   	else if ( aId == KEvtSnoozePeriod )
		{
        if( aNewValue && iSnoozeInterval/aNewValue )
            iSnoozeCount = iSnoozeInterval/aNewValue;
   		}
    }

//  End of File
