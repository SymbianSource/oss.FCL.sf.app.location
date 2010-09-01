/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It is adapter class to handle lbt operations
*
*/

#include <lbtgeocircle.h> 
#include <lbttriggerconditionarea.h> 
#include <lbttriggerfilterbyattribute.h> 
#include <lbtlisttriggeroptions.h>
#include <lbtstartuptrigger.h> 
#include <lbsrequestor.h> 
#include <lbttriggerinfo.h> 
#include <lbttriggerchangeeventnotifier.h>
#include <e32cmn.h>

#include "evtmgmtuilbtadapter.h"
#include "evtdebug.h"

// CONSTANTS
const TInt KUnitConversion = 1000;                        // Unit Converter
_LIT( KTriggerHandlingProcessName, "evthandler.exe" );    // Handler Process Name
_LIT_SECURE_ID( KTriggerHandlingProcessSID,  0x2001E667); // Handler Process UID
_LIT_SECURE_ID( KManagerProcessSID,  0x2001E668);         // Manager Process UID
_LIT( KMyRequestor , "Requestor" );                       //Set requestor information

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter ::CEvtMgmtUiLbtAdapter()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiLbtAdapter::CEvtMgmtUiLbtAdapter(MEvtMgmtUiLbtObserver&  aObserver)
:CActive( EPriorityStandard ), iObserver(aObserver)
    {   
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter ::ConstructL()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::ConstructL()
    {   
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::ConstructL()" );
    
    CActiveScheduler::Add(this);  
    EVTUIDEBUG("scheduler added" );
    
    // Create a Connection to LBT Sever.
    User::LeaveIfError( iLbtServer.Connect() );
    EVTUIDEBUG("lbtserver connected" );
    
    // Open 2 Sessions - One for LBT operation and Other for LBT observer 
    User::LeaveIfError( iLbt.Open(iLbtServer) );
    User::LeaveIfError( iLbtObserver.Open(iLbtServer) );
    EVTUIDEBUG("lbt is opened" );
    
    // Create a Trigger change notifier to listen to any changes to triggers. 
    iEventChangeObserver = CLbtTriggerChangeEventNotifier::NewL( iLbtObserver, *this );
    iEventChangeObserver->Start();
    EVTUIDEBUG("Trigger Change Notifier is started" );
    
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::ConstructL()" );
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter ::NewL()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiLbtAdapter* CEvtMgmtUiLbtAdapter::NewL(MEvtMgmtUiLbtObserver&  aObserver)
    {
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::NewL()" );
    CEvtMgmtUiLbtAdapter * self = new (ELeave) CEvtMgmtUiLbtAdapter(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::NewL()" );
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter ::~CEvtMgmtUiLbtAdapter()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiLbtAdapter::~CEvtMgmtUiLbtAdapter()
    {     
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::~CEvtMgmtUiLbtAdapter()" );
    Cancel();
    if(iAttrFilter)
        {
        delete iAttrFilter;
        iAttrFilter = NULL;
        }
        
    if(iTrigger)
        {
        delete iTrigger;
        iTrigger = NULL;
        }
    	
    delete iEventChangeObserver;
    
    //Close lbt session and server
    iLbtObserver.Close();
    iLbt.Close();
    iLbtServer.Close();
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::~CEvtMgmtUiLbtAdapter()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::CreateTrigger
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::CreateTrigger(const TCoordinate& aCenter,
        TReal aRadiusInKiloMeters, const TPtrC& aTriggerName,
        CLbtTriggerEntry::TLbtTriggerState aState )
    {       
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::CreateOrUpdateTrigger()" );
    
    TRAPD(err, CreateTriggerL( aCenter, aRadiusInKiloMeters, aTriggerName, 
                            aState ) );
    
    if( err!=KErrNone )
        iObserver.NotifyLbtError(err, iState);
    
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::CreateOrUpdateTrigger()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::CreateTriggerL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::CreateTriggerL(const TCoordinate& aCenter,
        TReal aRadiusInKiloMeters, const TPtrC& aTriggerName,
        CLbtTriggerEntry::TLbtTriggerState aState )
    {        
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::CreateTriggerL()");
    
    if ( IsActive() )
        {
        User::Leave( KErrInUse );            
        }
	
    //set state of the machine
    iState = ELbtAdapterCreate;
    
    EVTUIDEBUG("========== Trigger Parameter Details ===============" );   
    CLbtTriggerConditionArea* cond = GetConditionAreaL(aCenter, aRadiusInKiloMeters);
    CleanupStack::PushL(cond);     
   
    /*********************** Debug ******************************************/
    TBuf<256> debug;
    debug.Copy( aTriggerName );
    EVTUIDEBUG1("Trigger Name = %S", &debug );
    EVTUIDEBUG1("Event State = %d", aState );
    EVTUIDEBUG1("Radius = %f", aRadiusInKiloMeters*KUnitConversion );
	debug.Copy( KTriggerHandlingProcessName() );
	EVTUIDEBUG1("Trigger handling process name: %S", &debug );
	EVTUIDEBUG1("Trigger handling process SID: %x", KTriggerHandlingProcessSID);
	EVTUIDEBUG1("Manager process SID: %x", KManagerProcessSID);
    /*********************** Debug ******************************************/
    EVTUIDEBUG("========== Trigger Parameter Details end ===============" );   
    
    if(iTrigger)
       {
       delete iTrigger;
       iTrigger = NULL;
       }
   
    // Create the Startup trigegr without the command line.
    iTrigger = CLbtStartupTrigger::NewL(          
                            aTriggerName,                           // Trigger name
                            aState,                                 // Trigger state
                            CRequestorBase::ERequestorService,      // Requestor type
                            CRequestorBase::EFormatApplication,     // Requestor format
                            KMyRequestor,                           // Requestor data
                            KManagerProcessSID,                     // Manager Uid
                            cond,                                   // Trigger condition
                            KTriggerHandlingProcessName,            // Trigger handler app
                            KTriggerHandlingProcessSID
                            );          //Trigger state
   
  	CleanupStack::Pop( cond ); //ownership of cond is transferred.    
	
    SetActive();
    iStatus = KRequestPending;
    
    EVTUIDEBUG("Before CreateTrigger" );      
    iLbt.CreateTrigger( *iTrigger, iTriggerId, EFalse, iStatus );            
    EVTUIDEBUG("After CreateTrigger" );      

    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::CreateTriggerL()");
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::GetConditionAreaL
// ---------------------------------------------------------------------------
//
CLbtTriggerConditionArea* CEvtMgmtUiLbtAdapter::GetConditionAreaL( 
                const TCoordinate& aCenter, TReal aRadiusInKiloMeters )
    {
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::GetConditionAreaL()");
    // Create the Condition Area object depending on the trigger type.
    CLbtTriggerConditionArea* cond = NULL;

    if( Math::IsNaN( aCenter.Latitude() ) || Math::IsNaN( aCenter.Longitude() ) )
        {
        //Define the triggering area   
        CLbtGeoCircle * trigArea = CLbtGeoCircle ::NewLC( TCoordinate(TReal(0), TReal(0), TReal(0) ),1*KUnitConversion );
        //Construct trigger condition
        cond = CLbtTriggerConditionArea::NewL
                    (trigArea,CLbtTriggerConditionArea::EFireOnEnter );
        CleanupStack::Pop( trigArea ); //ownership of trigArea is transferred.
        }
    else
        {
        //Define the triggering area   
        CLbtGeoCircle * trigArea = CLbtGeoCircle ::NewLC( aCenter,aRadiusInKiloMeters*KUnitConversion );
        //Construct trigger condition
        cond = CLbtTriggerConditionArea::NewL
                    (trigArea,CLbtTriggerConditionArea::EFireOnEnter );
        CleanupStack::Pop( trigArea ); //ownership of trigArea is transferred.                
        }
    EVTUIDEBUG1("Lat = %f", aCenter.Latitude() );
    EVTUIDEBUG1("Long = %f", aCenter.Longitude() );
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::GetConditionAreaL()");
    return cond;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::UpdateCmdLineArgL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::UpdateCmdLineArgL( TLbtTriggerId aTriggerId, 
                                                        TEvtEventId aEventId )
    {
    EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::UpdateCmdLineArgL() - %d", aTriggerId );
    //set commandline argument
    const TInt KTriggerIdLength(132); 
    _LIT(KDelimiter, ":");
    TBuf<KTriggerIdLength> cmdArg;
    cmdArg.Zero();
    cmdArg.AppendNumUC(EEvtActionFired);
    cmdArg.Append(KDelimiter());
    cmdArg.AppendNumUC(aEventId);
       
    CLbtStartupTrigger* trigger = CLbtStartupTrigger::NewLC();
    trigger->SetId( aTriggerId );
    trigger->SetCommandLineL( cmdArg );    
    iState = ELbtAdapterUpdateCmdLineArg;
    
	EVTUIDEBUG1("Before UpdateTriggerL  - %d", aTriggerId );
    iLbt.UpdateTriggerL( *trigger, CLbtTriggerEntry::EAttributeStartUpCommandLine, ELbtFalse );  
	EVTUIDEBUG1("After UpdateTriggerL  - %d", aTriggerId ); 
    
    CleanupStack::PopAndDestroy(trigger);
    EVTUIDEBUG1("- CEvtMgmtUiLbtAdapter::UpdateCmdLineArgL() - %d", aTriggerId );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::UpdateTriggerL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::UpdateTriggerL( TLbtTriggerId aTriggerId,
        const TCoordinate& aCenter, TReal aRadiusInKiloMeters,
        CLbtTriggerEntry::TLbtTriggerState aState )
    {
    EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::UpdateTriggerL() - %d", aTriggerId )
    EVTUIDEBUG1("Event State = %d", aState );
    
    if ( IsActive() )
       {    
       User::Leave( KErrInUse );            
       }
    
    //Set state of the machine
    iState = ELbtAdapterUpdate;   

    // Create the Condition Area object depending on the trigger type.
    CLbtTriggerConditionArea* cond = GetConditionAreaL(aCenter, aRadiusInKiloMeters );
    CleanupStack::PushL(cond); 
       
    //create startup trigger object
    if(iTrigger)
       {
       delete iTrigger;
       iTrigger = NULL;
       }
    iTrigger = CLbtStartupTrigger::NewL();
    iTrigger->SetId( aTriggerId );
    iTrigger->SetCondition(cond);    
    CleanupStack::Pop(cond);    //ownership is transferred to trigger.
    iTrigger->SetState(aState);
    
    // Create the Trigger update attributes mask. 
    TLbtTriggerAttributeFieldsMask fieldsMask;
    fieldsMask = CLbtTriggerEntry::EAttributeState | CLbtTriggerEntry::EAttributeCondition;
    
    SetActive();
    iStatus = KRequestPending;
    
    EVTUIDEBUG1("Before UpdateTriggerL  - %d", aTriggerId );
    iLbt.UpdateTrigger( *iTrigger, fieldsMask, ELbtFalse, iStatus);  
    EVTUIDEBUG1("After UpdateTriggerL  - %d", aTriggerId ); 
    
    EVTUIDEBUG1("- CEvtMgmtUiLbtAdapter::UpdateTriggerL() - %d", aTriggerId );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::SetTriggerStateL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::SetTriggerStateL( TLbtTriggerId  aTriggerId,
									CLbtTriggerEntry::TLbtTriggerState aState )  
    {
    EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::SetTriggerStateL() - %d", aTriggerId );
    EVTUIDEBUG2("SetTriggerState for Triggerid =%d eventstatus = %d ", aTriggerId, aState);
    
    //Set state of the machine
    iState = ELbtAdapterSetTriggerState;
    iTriggerId = aTriggerId;
    
	EVTUIDEBUG1("Before SetTriggerStateL  - %d", aTriggerId );
    iLbt.SetTriggerStateL(aTriggerId, aState, ELbtFalse );
	EVTUIDEBUG1("After SetTriggerStateL  - %d", aTriggerId );
	
    EVTUIDEBUG1("- CEvtMgmtUiLbtAdapter::SetTriggerStateL() - %d", aTriggerId );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::SetTriggersStateL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::SetTriggersStateL( const RArray<TLbtTriggerId>& aTriggerIdList,
                                    CLbtTriggerEntry::TLbtTriggerState aState )  
    {
    EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::SetTriggersStateL() Count - %d", aTriggerIdList.Count() );
    
    if ( IsActive() )
       {    
       User::Leave( KErrInUse );            
       }
    
    //Set state of the machine
    iState = ELbtAdapterSetTriggersState;  
    
    if( iAttrFilter )
    	{
    	delete iAttrFilter;
    	iAttrFilter = NULL;
    	}
        
    // Create an attribute filter to include all the trigger ids
    iAttrFilter = CLbtTriggerFilterByAttribute::NewL();
    
    for( TInt index=0; index<aTriggerIdList.Count(); index++ )
        {
        iAttrFilter->AddTriggerIdL(aTriggerIdList[index]);
        }
            
    SetActive();
    iStatus = KRequestPending;
    iLbt.SetTriggersState( iStatus, aState, ELbtFalse, iAttrFilter );
    
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::SetTriggersState()");
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::DeleteTriggerL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::DeleteTriggerL( TLbtTriggerId  aTriggerId )  
    {
    EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::DeleteTriggerL() TriggerID = %d" , aTriggerId);
    EVTUIDEBUG2("SetTriggerState for Triggerid =%d eventstatus = %d ", aTriggerId, iState);
      
    //set state 
    iState = ELbtAdapterDelete;
    
    //Set it to member variable so that it can be used in RunL to delete trigger
    iTriggerId = aTriggerId;
    
	EVTUIDEBUG1("Before DeleteTriggerL  - %d", aTriggerId );
    iLbt.DeleteTriggerL( iTriggerId );
	EVTUIDEBUG1("After DeleteTriggerL  - %d", aTriggerId );
    
    EVTUIDEBUG1("- CEvtMgmtUiLbtAdapter::DeleteTriggerL() TriggerID = %d" , aTriggerId);
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::DeleteTriggersL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::DeleteTriggersL( const RArray<TLbtTriggerId>& aTriggerIdList )  
    {
    EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::DeleteTriggersL() Count - %d", aTriggerIdList.Count() );
    
    if ( IsActive() )
        {
        User::Leave( KErrInUse );            
        }
    iState = ELbtAdapterDeleteTriggers;
    SetActive();
    iStatus = KRequestPending;
        
    iLbt.DeleteTriggers( aTriggerIdList, iStatus );
    
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::DeleteTriggersL()");
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::GetTriggerStateL
// ---------------------------------------------------------------------------
//
CLbtTriggerEntry::TLbtTriggerState CEvtMgmtUiLbtAdapter::GetTriggerStateL( TLbtTriggerId  aTriggerId )
    {
    EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::GetTriggerStateL = %d ", aTriggerId);
    CLbtTriggerEntry::TLbtTriggerState state = CLbtTriggerEntry::EStateDisabled;
    
	EVTUIDEBUG1("Before GetTriggerLC  - %d", aTriggerId );
    CLbtTriggerInfo * info = iLbt.GetTriggerLC( aTriggerId );
	EVTUIDEBUG1("After GetTriggerLC  - %d", aTriggerId );
    
    if(info)
        {
        CLbtTriggerEntry* entry = info->TriggerEntry();
        state = entry->State();
        CleanupStack::PopAndDestroy(info);
        }
    
    EVTUIDEBUG1("- CEvtMgmtUiLbtAdapter::GetTriggerStateL = %d ", aTriggerId);
    
    return state;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::CancelRequest()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::CancelRequest()
    {     
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::CancelRequest()" );
    Cancel();
    if(iStatus.Int()==KErrCancel)
        {       
		iObserver.NotifyLbtError(KErrCancel, iState);
		}
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::CancelRequest()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::RetrieveLocationL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::RetrieveLocationL( TLbtTriggerId aTriggerId, TCoordinate& aCenter )
    {
    EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::RetrieveLocationL() TriggerId = %d ", aTriggerId );   
    
	EVTUIDEBUG1("Before GetTriggerLC  - %d", aTriggerId );
    CLbtTriggerInfo * info = iLbt.GetTriggerLC( aTriggerId );
	EVTUIDEBUG1("After GetTriggerLC  - %d", aTriggerId );
    
    if(info)
           {
           CLbtTriggerEntry* entry = info->TriggerEntry();
           CLbtTriggerConditionArea* triggArea =  static_cast< CLbtTriggerConditionArea*>( entry->GetCondition());
           CLbtGeoAreaBase* triggGeoArea = static_cast< CLbtGeoAreaBase*>( triggArea->TriggerArea());
           if( triggGeoArea->Type() == CLbtGeoAreaBase::ECircle)
               {
               CLbtGeoCircle* loc = static_cast<CLbtGeoCircle*>( triggArea->TriggerArea() );          
               aCenter = loc->Center();
               EVTUIDEBUG1("GetTriggerLC: Latitude = %f", aCenter.Latitude() );
               EVTUIDEBUG1("GetTriggerLC: Longitude = %f", aCenter.Longitude() );
               }
           else
               {
               User::Leave(KErrNotSupported);
               }
    	   CleanupStack::PopAndDestroy(); //info
           }   
    
    EVTUIDEBUG1("- CEvtMgmtUiLbtAdapter::RetrieveLocationL() TriggerId = %d ", aTriggerId );  
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::GetFiredTriggerAccuracyL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::GetFiredTriggerAccuracyL( TLbtTriggerId aTriggerId, TReal32& aDistance )
    {
    EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::GetFiredTriggerAccuracyL() TriggerId = %d ", aTriggerId );   
    
	EVTUIDEBUG1("Before GetTriggerLC  - %d", aTriggerId );
    CLbtTriggerInfo * info = iLbt.GetTriggerLC( aTriggerId );
	EVTUIDEBUG1("After GetTriggerLC  - %d", aTriggerId );
    
    if(info)
		{
        CLbtTriggerEntry* entry = info->TriggerEntry();
        CLbtTriggerConditionArea* triggArea =  static_cast< CLbtTriggerConditionArea*>( entry->GetCondition());
        CLbtGeoAreaBase* triggGeoArea = static_cast< CLbtGeoAreaBase*>( triggArea->TriggerArea());
        if( triggGeoArea->Type() == CLbtGeoAreaBase::ECircle)
            {
            TLbtTriggerDynamicInfo* dynInfo = info->DynInfo();
            CLbtGeoCircle* circle = static_cast<CLbtGeoCircle*> (triggGeoArea);
            TCoordinate cord = circle->Center();
            cord.Distance(dynInfo->iFiredLocality, aDistance);
            aDistance = aDistance - circle->Radius();
            aDistance = Abs(aDistance);
            }
        EVTUIDEBUG1( "Fired Trigger Accuracy in Distance = %f", aDistance );
        CleanupStack::PopAndDestroy(); //info
		}   
    
    EVTUIDEBUG1("- CEvtMgmtUiLbtAdapter::GetFiredTriggerAccuracyL() TriggerId = %d ", aTriggerId );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::RunError
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiLbtAdapter::RunError( TInt aError )
    {
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::RunError()" );
    if( aError != KErrNone )
        {
        //if error, notify it to the engine.      
        EVTUIDEBUG1("CEvtMgmtUiLbtAdapter RunError error = %d", aError );
        iObserver.NotifyLbtError( aError, iState);
        }
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::RunError()" );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter ::RunL()
// Inherited from CActive
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::RunL() 
    {
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::RunL()" );
        
    if(iStatus.Int()!=KErrNone)
        {       
        EVTUIDEBUG1("- CEvtMgmtUiLbtAdapter::RunL() error =%d", iStatus.Int());
        User::Leave( iStatus.Int() );
        return;
        }
    
    EVTUIDEBUG("LbtAdapter error = 0" );
    
    switch(iState)
        {
        case ELbtAdapterCreate: 
            {
			 iState = ELbtAdapterCreate;
			 
            //delete trigger object when trigger is successfuly created
            if(iTrigger)            
                {
        		EVTUIDEBUG("Deleting trigger entry after Create" );
                delete iTrigger;
                iTrigger = NULL;
                }
            break;
            }
        case ELbtAdapterUpdate:
            {			 
            //delete trigger object when trigger is successfuly updated
            if(iTrigger)            
                {
        		EVTUIDEBUG("Deleting trigger entry after Update" );
                delete iTrigger;
                iTrigger = NULL;
                }
            break;
            }
        case ELbtAdapterDeleteTriggers:
            {
            EVTUIDEBUG("Triggers are deleted successfully");
            }
            break;
        case ELbtAdapterSetTriggersState:
            {
		    if(iAttrFilter)
		        {
		        delete iAttrFilter;
		        iAttrFilter = NULL;
		        }
            EVTUIDEBUG("State of triggers are changed successfully");
            break;
            }
        default:
            { 
            break;
            }
        }
    //Notify engine about lbt changes
    iObserver.NotifyLbtChangeL(iTriggerId, iState);               
    
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::RunL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter ::DoCancel()
// Inherited from CActive
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::DoCancel()
    {    
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::DoCancel()" );
    iLbt.CancelAll();
    //iObserver.NotifyLbtError(KErrCancel, iState);
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::DoCancel()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::GetTriggerListL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::GetTriggerListL( RArray<TLbtTriggerId>& aTriggerList,
								CLbtTriggerEntry::TLbtTriggerState aState )
    {
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::GetTriggerListL()" );
	CLbtListTriggerOptions *listOptions = CLbtListTriggerOptions::NewL();
    CleanupStack::PushL(listOptions);  
      
	CLbtTriggerFilterByAttribute* filter = CLbtTriggerFilterByAttribute::NewLC();
	
    // List only valid triggers and whose state is aState.
	filter->AddTriggerStateL( aState );
    filter->AddTriggerValidityL( TLbtTriggerDynamicInfo::EValid );
    
	listOptions->SetFilter( filter );
	
	EVTUIDEBUG("Before ListTriggerIdsL");
	iLbt.ListTriggerIdsL( aTriggerList, listOptions );
	EVTUIDEBUG("After ListTriggerIdsL");

    CleanupStack::PopAndDestroy(2);// filter, listOptions
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::GetTriggerListL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::GetTriggerListL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::GetTriggerListL( RArray<TLbtTriggerId>& aTriggerList,
                TLbtTriggerDynamicInfo::TLbtTriggerValidity aValid )
    {
    EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::GetTriggerListL()" );
    CLbtListTriggerOptions *listOptions = CLbtListTriggerOptions::NewL();
    CleanupStack::PushL(listOptions);  
      
    CLbtTriggerFilterByAttribute* filter = CLbtTriggerFilterByAttribute::NewLC();
    
    // List only triggers of status aValid
    filter->AddTriggerValidityL( aValid );
    
    listOptions->SetFilter( filter );
    
    EVTUIDEBUG("Before ListTriggerIdsL");
    iLbt.ListTriggerIdsL( aTriggerList, listOptions );
    EVTUIDEBUG("After ListTriggerIdsL");

    CleanupStack::PopAndDestroy(2);// filter, listOptions
    EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::GetTriggerListL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::TriggerChangedL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLbtAdapter::TriggerChangedL(
							const TLbtTriggerChangeEvent &aEvent)
	{
	EVTUIDEBUG("+ CEvtMgmtUiLbtAdapter::TriggerChangedL()" );
	switch( aEvent.iEventType )
		{
		case ELbtTriggerChangeEventMultiple:
		iObserver.NotifyTriggerChangeL(aEvent.iTriggerId, MEvtMgmtUiLbtObserver::ETriggerMultipleModified);
		break;
		case ELbtTriggerChangeEventDeleted:
		iObserver.NotifyTriggerChangeL(aEvent.iTriggerId, MEvtMgmtUiLbtObserver::ETriggerDeleted);
		break;
		case ELbtTriggerChangeEventUpdated:
		iObserver.NotifyTriggerChangeL(aEvent.iTriggerId, MEvtMgmtUiLbtObserver::ETriggerModified);
		break;
		default:
		break;
		}
	EVTUIDEBUG("- CEvtMgmtUiLbtAdapter::TriggerChangedL()" );
	}

// ---------------------------------------------------------------------------
// CEvtMgmtUiLbtAdapter::GetFiredPositionL
// ---------------------------------------------------------------------------
//
TBool CEvtMgmtUiLbtAdapter::GetFiredPositionL( TPosition& aPosition,
											const TLbtTriggerId aTriggerId )
	{
	EVTUIDEBUG1("+ CEvtMgmtUiLbtAdapter::GetFiredPositionL() - %d", aTriggerId );
	RArray<TLbtTriggerFireInfo> aTriggerInfoList;
	
	EVTUIDEBUG1("Before GetFiredTriggersL  - %d", aTriggerId );
	iLbt.GetFiredTriggersL( aTriggerInfoList );
	EVTUIDEBUG1("After GetFiredTriggersL  - %d", aTriggerId );
	
	TInt count = aTriggerInfoList.Count();
	TBool found(EFalse);
	
	for( TInt i=0; i<count; i++ )
		{
		if ( aTriggerInfoList[i].iTriggerId == aTriggerId )
			{
			if(aTriggerInfoList[i].iAreaType == CLbtGeoAreaBase::ECircle)
				{ 
				found = ETrue;
				aTriggerInfoList[i].iFiredPositionInfo.GetPosition( aPosition );
				}
			}
		}
	aTriggerInfoList.Close();
	EVTUIDEBUG1("- CEvtMgmtUiLbtAdapter::GetFiredPositionL() - %d", aTriggerId );
	return found;
	}
//end of file
