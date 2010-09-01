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
* Description:  Handler for notifier on Event
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <AknGlobalNote.h>  // CAknGlobalNote

#include "evtdebug.h"
#include "evtnotifierhandler.h"
#include "evtnotifierobserver.h"
#include "evttoneaction.h"
#include "evtglobalquery.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
//S60 Event Notifier plugin id
const TUid KEvtNotifierUidS60 = { 0x2001E66C };

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEvtNotifierHandler::NewL
// -----------------------------------------------------------------------------
CEvtNotifierHandler* CEvtNotifierHandler::NewL(
        	MEvtNotifierObserver& aNotifierObserver,
        	TEvtEventInfo aEventInfo )
    {
    CEvtNotifierHandler* self = new ( ELeave ) CEvtNotifierHandler( aNotifierObserver, aEventInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CEvtNotifierHandler::CEvtNotifierHandler
// -----------------------------------------------------------------------------
CEvtNotifierHandler::CEvtNotifierHandler( 
	MEvtNotifierObserver& aNotifierObserver, TEvtEventInfo aEventInfo )
    : CActive( EPriorityStandard ), 
      iNotifierObserver( aNotifierObserver ),iEventInfo(aEventInfo)
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CEvtNotifierHandler::~CEvtNotifierHandler
// -----------------------------------------------------------------------------
CEvtNotifierHandler::~CEvtNotifierHandler()
    {
    delete iInputParam;
    delete iInputBuffer;
    
    Cancel();
    
    if(iEvtGlobalQuery)
        {
        delete iEvtGlobalQuery;
        iEvtGlobalQuery = NULL;
        }
        
    iNotifier.Close();
    
    delete iEvent;
    delete iEventManager;
    }

// -----------------------------------------------------------------------------
// CEvtNotifierHandler::ConstructL
// -----------------------------------------------------------------------------
void CEvtNotifierHandler::ConstructL()
    {
    EVTUIDEBUG("+ CEvtNotifierHandler::ConstructL");    
    iInputBuffer = CBufFlat::NewL( KEvtNotifierBufferLength );        
    iInputParam  = CEvtInfoNoteInputParam::NewL();    
    iResponseBuffer.Zero();
    iResponseBuffer.SetLength( KEventIdLength );  
    iEventManager = CEvtEventManager::NewL();
    EVTUIDEBUG( "- CEvtNotifierHandler::ConstructL" );
    }

// -----------------------------------------------------------------------------
// CEvtNotifierHandler::IsLaunchable
// -----------------------------------------------------------------------------
TBool CEvtNotifierHandler::IsLaunchable()
	{    
    EVTUIDEBUG( "+ CEvtNotifierHandler::IsLaunchable" );	
	TBool launchable = EFalse;
	
    TRAPD( evtErr, iEvent = iEventManager->GetEventL( iEventInfo.iEventId ) );
    if( !evtErr && iEvent && iEvent->EventStatus() == EActive )
		{
		launchable = ETrue;
		}
		
    return launchable;
	}

// -----------------------------------------------------------------------------
// CEvtNotifierHandler::StartEvtNotifier
// -----------------------------------------------------------------------------
void CEvtNotifierHandler::StartEvtNotifierL()
	{    
    EVTUIDEBUG( "+ CEvtNotifierHandler::StartEvtNotifierL" );	
	
    if( iEvent )
    	{
    	CEvtInfoNoteInputParam::TEvtTriggerAccuracy accuracy;
    	accuracy = static_cast<	CEvtInfoNoteInputParam::TEvtTriggerAccuracy>(iEventInfo.iAccuracy);
		// set the subject and description of notifier
	    iInputParam->SetSubjectL( iEvent->Subject() );
	    iInputParam->SetDescriptionL( iEvent->Description() );
	    iInputParam->SetTrigerringAccuracy( accuracy );    
	    iInputParam->SetToneEnabled( iEventInfo.iPlayTone );    	      
	    iInputParam->SetShowSnooze( iEventInfo.iSnooze );
        if( iEventInfo.iPlayTone && iEvent->HasAction() )
            {      
            // Populate the Tone.
       		CEvtToneAction* tone = CEvtToneAction::NewLC();
            tone->InternalizeL( iEvent->Action() );
            iInputParam->SetToneL( tone->FileName() );
            iInputParam->SetToneRepeat( tone->ToneLoop() );    
        	CleanupStack::PopAndDestroy(tone);      
            }
        
        EVTUIDEBUG( "Trigger Fired Event" );
        
        if(iEvent->Subject().Length())
	        {
	        TPtrC subject( iEvent->Subject() );        
        	EVTUIDEBUG1( "Subject  = %S", &subject );	
	        }
        EVTUIDEBUG1( "Tigger ID  = %d", iEvent->EventId() );
                
	    RBufWriteStream writeStream( *iInputBuffer, 0 );
	    CleanupClosePushL( writeStream );                
	    
	    iInputParam->ExternalizeL( writeStream );
	    
	    TPtr8 bufPtr( const_cast< TUint8 *>( iInputBuffer->Ptr( 0 ).Ptr() ),
	                  iInputBuffer->Ptr( 0 ).Length(),
	                  iInputBuffer->Ptr( 0 ).Length() );
	                  
	    iHandlerType = EHandlerNotifier;
	    User::LeaveIfError( iNotifier.Connect());
	    iNotifier.StartNotifierAndGetResponse( iStatus, KEvtNotifierUidS60, bufPtr, iResponseBuffer );
	    SetActive();

	    CleanupStack::Pop(); // writeStream	    	
    	}	    
    EVTUIDEBUG( "- CEvtNotifierHandler::StartEvtNotifierL" );
	}

// -----------------------------------------------------------------------------
// CEvtNotifierHandler::RunL
// -----------------------------------------------------------------------------
void CEvtNotifierHandler::RunL()
    {
    EVTUIDEBUG1( "+ CEvtNotifierHandler::RunL %d", iStatus.Int() );
    TEvtInfoNoteResult result = ECloseInfoNote; 
    switch( iStatus.Int() )
        {
        case KErrNone:
            {
    	    TInt retVal = 0;
    	    TLex8  lexer( iResponseBuffer );
    	    TInt   error = lexer.Val( retVal );    
    	    
    	    EVTUIDEBUG1( "Event %d", retVal ); 
    	    switch( retVal )
    	    	{
    			case EOpenTrigger:
    				{
    				result = EOpenTrigger;
    				break;				
    				}			
    			case EShowDetails:
    				{
    				result = EShowDetails;
    				break;				
    				}		 
                case ESnoozeTrigger:
                    {
                    result = ESnoozeTrigger;
                    break;              
                    }   
    			case ECloseInfoNote:
    			default:
    				{
    				result = ECloseInfoNote;
    				break;				
    				}  			
    	    	}
            // Complete the event note launching
            iNotifierObserver.HandlerCompleteL( result, iEvent ); 
            break;
            }
		default:
			{
			if(iHandlerType == EHandlerNotifier)
			    {
			    iHandlerType = EHandlerGlobalNote;
                iStatus = KRequestPending;
				if(iEvtGlobalQuery)
					{
					delete iEvtGlobalQuery;
					iEvtGlobalQuery = NULL;
					}
                iEvtGlobalQuery = CEvtGlobalQuery::NewL(*iInputParam);
                iEvtGlobalQuery->StartGlobalQueryL( iStatus, iResponseBuffer );
                SetActive();
			    }
			else
			    {
			    // Complete the event note launching
		        iNotifierObserver.HandlerCompleteL( result, iEvent ); 
			    }
			break;				
			}
        }
        
    EVTUIDEBUG( "- CEvtNotifierHandler::RunL" );
    }

// -----------------------------------------------------------------------------
// CEvtNotifierHandler::DoCancel
// -----------------------------------------------------------------------------
void CEvtNotifierHandler::DoCancel()
    {
    EVTUIDEBUG( "+ CEvtNotifierHandler::DoCancel()" );
	if(iHandlerType == EHandlerNotifier)
	    {
    	iNotifier.CancelNotifier( KEvtNotifierUidS60 );
		}
	else
		{
		iEvtGlobalQuery->CancelDialog();
		}
    EVTUIDEBUG( "- CEvtNotifierHandler::DoCancel()" );
    }

// -----------------------------------------------------------------------------
// CEvtNotifierHandler::RunError
// -----------------------------------------------------------------------------
TInt CEvtNotifierHandler::RunError( TInt /*aError*/ )
    {
    return KErrNone;    
    }

//  End of File
