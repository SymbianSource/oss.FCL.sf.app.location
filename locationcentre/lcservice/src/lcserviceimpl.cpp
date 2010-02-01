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
* Description:  Location Centre API interface.
*
*/


// SYSTEM INCLUDES
#include <s32mem.h>
#include <aknsutils.h>
#include <gulicon.h>
#include <aknlists.h>
#include <aknpopup.h>
#include <avkon.rsg>

// USER INCLUDES
#include "lcserviceimpl.h"
#include "lclocationappfilter.h"
#include "lcappexitobserver.h"
#include "lcnotification.h"
#include "lcapplauncher.h"
#include "lcipcparams.h"
#include "lcnotificationao.h"
#include "lciconloader.h"
#include "lcsyncoperation.h"
#include "lcpopuplistbox.h"
#include "lcdebug.h"

// ----- Member funtions for CLcServiceImpl ---------------------------------

// ---------------------------------------------------------------------------
// CLcServiceImpl::CLcServiceImpl
// ---------------------------------------------------------------------------
//
CLcServiceImpl::CLcServiceImpl()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
         
// ---------------------------------------------------------------------------
// CLcServiceImpl::~CLcServiceImpl
// ---------------------------------------------------------------------------
//
CLcServiceImpl::~CLcServiceImpl()
    {
    // C++ Destructor. Free all resources associated with this class.
    if ( iPopUp )
        {
        iPopUp->CancelPopup();
        }
    
    // Delele the Pop-up list    
    delete iPopUpListBox;
    
    // Delete the Selected ID buffer   
    delete iSelectedAppId;
                
    // Delete filter parameters
    delete iFilter;
    
    // Delete the Application information array
    iAppArray.Reset();
    iAppArray.Close();

    // Delete the Notification Active object
    delete iNotificationAO;
    
    // Delete the Application launcher
    delete iAppLauncher;
    
    // If there is an outstanding request then complete the same with KErrCancel.
    if ( iAsyncOperation )
    	{
    	iAsyncOperation->CancelGetLocationApplications();
    	User::RequestComplete( iStatus, KErrCompletion );
    	
    	// We can now delete the asynchronous operation
    	delete iAsyncOperation;
    	iAsyncOperation = NULL;
    	} 
    	    
    // Close the session handle. The session handle is active throughout the
    // lifetime of this class
    iSession.Close();
    
    }
        
// ---------------------------------------------------------------------------
// CLcServiceImpl* CLcServiceImpl::NewL
// ---------------------------------------------------------------------------
//
CLcServiceImpl* CLcServiceImpl::NewL()
    {
    CLcServiceImpl* self = NewLC();
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcServiceImpl* CLcServiceImpl::NewLC
// ---------------------------------------------------------------------------
//
CLcServiceImpl* CLcServiceImpl::NewLC()
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcServiceImpl* self = 
                new ( ELeave )CLcServiceImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcServiceImpl::ConstructL
// ---------------------------------------------------------------------------
//
void CLcServiceImpl::ConstructL()
    {
    // Second phase of the two phase constructor.
    
    // Open a connection to the Location Centre Server
    // The connection is opened at the class construction and Closed at the
    // class deletion. This is done inorder to avoid delays during every
    // operation
    User::LeaveIfError( iSession.CreateSession());
    
    // Create the Location application launcher.
    iAppLauncher = CLcAppLauncher::NewL();
    
    // Create the Notification AO
    iNotificationAO = CLcNotificationAO::NewL( *this, iSession );
    }
                    
// ---------------------------------------------------------------------------
// void CLcServiceImpl::LaunchLocationCentreL
// ---------------------------------------------------------------------------
//
void CLcServiceImpl::LaunchLocationCentreL( 
              const TLcLocationAppFilter&             aLocationAppFilter,
                    CLcLocationAppInfo::TLcLaunchMode aLaunchMode,                        
                    MLcAppExitObserver*               aChainedAppExitObserver )
    {
    DEBUG("+ CLcServiceImpl::LaunchLocationCentreL")
    // First select a Location based Application from the list
    TPtrC identifier = SelectLocationApplicationL( aLocationAppFilter );
    if ( !identifier.Length())
        {
        return;
        }    
    
    // Launch the corresponding application.
    LaunchLocationApplicationL( identifier, aLaunchMode, aChainedAppExitObserver );
    DEBUG("- CLcServiceImpl::LaunchLocationCentreL")
    }
    
// ---------------------------------------------------------------------------
// void CLcServiceImpl::LaunchLocationCentreL
// ---------------------------------------------------------------------------
//
void CLcServiceImpl::LaunchLocationCentreL(
      const RPointerArray<CLcService::CLcLaunchParam>& aIncludeAppArray,
      MLcAppExitObserver*                              aChainedAppExitObserver )
    {
    // Form an array of applications identifiers which can be passed to the
    // selection function
    RArray<TPtrC>  appArray;
    CleanupClosePushL( appArray );
    
    TInt count = aIncludeAppArray.Count();    
    for( TInt i = 0; i < count; i++ )
        {
        User::LeaveIfError( appArray.Append( aIncludeAppArray[i]->Id()));
        }
        
    // First select a Location based Application from the list
    TPtrC identifier = SelectLocationApplicationL( appArray, ETrue );    
    if ( !identifier.Length())
        {
        return;
        }
        
    CLcLocationAppInfo::TLcLaunchMode launchmode = CLcLocationAppInfo::EDefaultMode;
    // Obtain the launch mode for that application            
    for ( TInt i = 0; i < count; i++ )
        {
        if ( !appArray[i].Compare( identifier ))
            {
            launchmode = aIncludeAppArray[i]->LaunchMode();
            break;
            }
        }
        
    // Launch the corresponding application.
    LaunchLocationApplicationL( identifier, launchmode, aChainedAppExitObserver ); 
    
    // Pop and Destroy the application array        
    CleanupStack::PopAndDestroy();
    }
    
// ---------------------------------------------------------------------------
// void CLcServiceImpl::LaunchLocationCentreL
// ---------------------------------------------------------------------------
//
void CLcServiceImpl::LaunchLocationCentreL( 
                    const RArray<TPtrC>&              aExcludeAppArray,
                    CLcLocationAppInfo::TLcLaunchMode aLaunchMode,
                    MLcAppExitObserver*               aChainedAppExitObserver )
    {
    // First select a Location based Application from the list
    TPtrC identifier = SelectLocationApplicationL( aExcludeAppArray, EFalse );
    if ( !identifier.Length())
        {
        return;
        }    
    
    // Launch the corresponding application.
    LaunchLocationApplicationL( identifier, aLaunchMode, aChainedAppExitObserver );    
    }

// ---------------------------------------------------------------------------
// void CLcServiceImpl::SelectLocationApplicationL
// ---------------------------------------------------------------------------
//
TPtrC CLcServiceImpl::SelectLocationApplicationL(
                        const TLcLocationAppFilter&     aLocationAppFilter )
    {
    // If the pop-up dialog is currently being displayed then cannot proceed with
    // the request.
    if ( iPopUp )
        {
        User::Leave( KErrInUse );
        }
    
    iPopupOutStanding = ELcPopupWithFiltering;
    
    // Delete any existing filter. This condition should never occur. Just a safe
    // check.
    delete iFilter;
    iFilter = NULL;
    
    // Copy the filter contents
    iFilter = new ( ELeave ) TLcLocationAppFilter( aLocationAppFilter );
      
    // Obtain the list of applications from the Location Centre Server and
    // launch the Location Centre pop-up with that list.    
    CLcLocationAppInfoArray* array = GetLocationApplicationsL( aLocationAppFilter );
                                  
    // The ownership of the array is transferred.
    TPtrC  appId(KNullDesC());
    TRAPD( error, appId.Set( SelectLocationApplicationL( array )));
    
    // Delete the filter before any operation on the result        
    delete iFilter;
    iFilter = NULL;
     
    // Handle Error 
    if ( error )
        {
        if ( iPopUp )
            {
            iPopUp->CancelPopup();            
            }
        iPopUp = NULL;
        
        delete iPopUpListBox;
        iPopUpListBox = NULL;
        
        User::Leave( error );        
        }        
    return appId;        
    }
    
// ---------------------------------------------------------------------------
// void CLcServiceImpl::SelectLocationApplicationL
// ---------------------------------------------------------------------------
//
TPtrC CLcServiceImpl::SelectLocationApplicationL(
                            const RArray<TPtrC>&      aAppArray,
                                  TBool               aIncludeFlag )
    {
    // If the pop-up dialog is currently being displayed then cannot proceed with
    // the request.
    if ( iPopUp )
        {
        User::Leave( KErrInUse );
        }
     
    // Reset and Destroy the existing array.
    iAppArray.Reset();
    
    if ( aIncludeFlag )
        {
        iPopupOutStanding = ELcPopupWithSpecifiedApps;
        }
    else
        {
        iPopupOutStanding = ELcPopupWithoutSpecifedApps;
        }
        
    // Create the array with the new elements
    TInt count = aAppArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        User::LeaveIfError( iAppArray.Append( aAppArray[i] ));
        }
        
    // Obtain the list of applications from the Location Centre Server and
    // launch the Location Centre pop-up with that list.    
    CLcLocationAppInfoArray* array = 
                    LcSyncOperation::GetLocationApplicationsL( iSession,
                                                               aAppArray,
                                                               aIncludeFlag );
                                  
    // The ownership of the array is transferred.
    TPtrC  appId(KNullDesC());
    TRAPD( error, appId.Set( SelectLocationApplicationL( array )));
    
    // Reset and destroy the array
    iAppArray.Reset();
      
    // Handle Error    
    if ( error )
        {
        if ( iPopUp )
            {
            iPopUp->CancelPopup();            
            }
        iPopUp = NULL;
        
        delete iPopUpListBox;
        iPopUpListBox = NULL;
        
        User::Leave( error );
        }
    return appId;         
    }
    
// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray* CLcServiceImpl::GetLocationApplicationsL
// ---------------------------------------------------------------------------
//    
CLcLocationAppInfoArray* CLcServiceImpl::GetLocationApplicationsL(
            const TLcLocationAppFilter&         aLocationAppFilter )
    {
    return LcSyncOperation::GetLocationApplicationsL( iSession,
    												  aLocationAppFilter );
    }
    
// ---------------------------------------------------------------------------
// void CLcServiceImpl::GetLocationApplicationsL
// ---------------------------------------------------------------------------
//    
void CLcServiceImpl::GetLocationApplications(
                            TRequestStatus&             aStatus,
                      const TLcLocationAppFilter&       aLocationAppFilter,                            
                            CLcLocationAppInfoArray*&   aAppInfoArray)
    {
    TRAPD( error, GetLocationApplicationsL( aStatus,
    										aLocationAppFilter,
    										aAppInfoArray ));
	if ( error )
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete( status , error );
		}
    }

// ---------------------------------------------------------------------------
// void CLcServiceImpl::CancelGetLocationApplications
// ---------------------------------------------------------------------------
//     
void CLcServiceImpl::CancelGetLocationApplications()
    {
    // If there is an outstanding request then complete the same with KErrCancel.
    if ( iAsyncOperation )
    	{
    	iAsyncOperation->CancelGetLocationApplications();
    	User::RequestComplete( iStatus, KErrCancel );
    	
    	// We can now delete the asynchronous operation
    	delete iAsyncOperation;
    	iAsyncOperation = NULL;
    	} 
    }

// ---------------------------------------------------------------------------
// void CLcServiceImpl::LaunchLocationApplicationL
// ---------------------------------------------------------------------------
// 
void CLcServiceImpl::LaunchLocationApplicationL( 
                const TDesC&                      aAppIdentifier,
                CLcLocationAppInfo::TLcLaunchMode aLaunchMode,
                MLcAppExitObserver*               aChainedAppExitObserver )
    {
    DEBUG("+ CLcServiceImpl::LaunchLocationApplicationL" )
    
    // Obtain the Location Application Information from Location Centre Server.
    CLcBasicAppInfo* appInfo = 
    	LcSyncOperation::GetLocationAppInfoL( iSession, aAppIdentifier );
    CleanupStack::PushL( appInfo );
   
    // Resolve the model in which the Application must be launched.
    if ( aLaunchMode == CLcLocationAppInfo::EDefaultMode )
    	{
    	// Since this is a Local copy we can use the same for handling
    	// Launch mode changes.
    	aLaunchMode = 
    		static_cast<CLcLocationAppInfo::TLcLaunchMode>( appInfo->LaunchMode());
    	}
    	    	
	// Now Launch the Location based Application or Content/Service    	
    iAppLauncher->LaunchLocationApplicationL( 
			appInfo->ApplicationType(),
            appInfo->ApplicationData(),
            ( aLaunchMode == CLcLocationAppInfo::EStandAloneMode ) ? ETrue : EFalse,
            appInfo->CommandLineParamters(),
            aChainedAppExitObserver );

	CleanupStack::PopAndDestroy( appInfo );                                          
	
	DEBUG("- CLcServiceImpl::LaunchLocationApplicationL" )                                          
    }

// ---------------------------------------------------------------------------
// void CLcServiceImpl::SetObserverL
// ---------------------------------------------------------------------------
// 
void CLcServiceImpl::SetObserverL( MLcNotification&     aObserver )
    {
    if ( iObserver )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    iObserver = &aObserver;
    
    iNotificationAO->IssueRequest();
    }

// ---------------------------------------------------------------------------
// TInt CLcServiceImpl::RemoveObserver
// ---------------------------------------------------------------------------
//                                         
TInt CLcServiceImpl::RemoveObserver()
    {
    if ( !iObserver )
        {
        return KErrNotFound;
        }
    
    iObserver = NULL;
    
    // If there is a popup outstanding then do nothing. If not remove the
    // notification
    if ( !iPopUp )
        {
        iNotificationAO->CancelRequest();        
        }
        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// void CLcServiceImpl::LcStatusChangedL
// ---------------------------------------------------------------------------
// 
void CLcServiceImpl::LcStatusChangedL( TInt aErrorCode )
    {
    // If we are displaying the pop-up then we need to transfer the changes
    // to the pop-up first
    if ( iPopUp )
        {
        // Obtain the new list of applications from the Location Centre Server and
        // update the Location Centre pop-up with that list.    
        CLcLocationAppInfoArray* array = NULL;
        TInt error = KErrNone;
                        
        switch( iPopupOutStanding )
            {
            case ELcPopupWithFiltering:
                {                
                // Incase we face any errors in retrival then we can Ignore the
                // updations
                TRAP( error, array = GetLocationApplicationsL( *iFilter ));
                break;
                }
            case ELcPopupWithSpecifiedApps:
            case ELcPopupWithoutSpecifedApps:            
                {
                TBool includeflag = EFalse;
                if ( iPopupOutStanding == ELcPopupWithSpecifiedApps )
                    {
                    includeflag = ETrue;
                    }
                    
                // Incase we face any errors in retrival then we can Ignore the
                // updations
                TRAP( error, array = LcSyncOperation::GetLocationApplicationsL( 
                                                        iSession, iAppArray, includeflag ));
                break;
                }
            default:
                {
                break;
                }
            }
           
        if ( !error && array )
            {
            // The ownership of the array is transferred to the list box.
            TRAP_IGNORE( CleanupStack::PushL( array ); 
                         iPopUpListBox->UpdateListBoxL( array );
                         CleanupStack::Pop( array );
                       );
            }
        else if ( KErrNotFound == error )
            {
            if ( iPopUp )
                {
                iPopUp->CancelPopup();            
                }
            iPopUp = NULL;
            
            delete iPopUpListBox;
            iPopUpListBox = NULL;            
            }
        }
        
    // Notify the client regarding the changes to Location Centre Registry
    if ( iObserver )
        {
        iObserver->LcStatusChangedL( aErrorCode );
        }   
    }

// ---------------------------------------------------------------------------
// void CLcServiceImpl::OperationComplete
// ---------------------------------------------------------------------------
// 
void CLcServiceImpl::OperationComplete( TInt aError )
	{
	User::RequestComplete( iStatus, aError );
	
	// We can now delete the asynchronous operation
	delete iAsyncOperation;
	iAsyncOperation = NULL;	
	}

// ---------------------------------------------------------------------------
// void CLcServiceImpl::GetLocationApplicationsL
// ---------------------------------------------------------------------------
// 	
void CLcServiceImpl::GetLocationApplicationsL( 
                TRequestStatus&             aStatus,
          const TLcLocationAppFilter&       aLocationAppFilter,
                CLcLocationAppInfoArray*&   aAppInfoArray )
	{
    aStatus = KRequestPending;
    
    // If there is an asynchronous operation then leave with KErrInUse
    if ( iAsyncOperation )
    	{
    	User::Leave( KErrInUse );
    	}
    	
	// Create the Asynchronous Operations class
	iAsyncOperation = CLcAsyncOperation::NewL( iSession, *this );
	    	
    // Set the status to KRequestPending
    aStatus = KRequestPending;
    iStatus = &aStatus;
    
    // Issue the request to the ASP
    iAsyncOperation->GetLocationApplicationsL( aLocationAppFilter,
    										   aAppInfoArray );	
	}

// ---------------------------------------------------------------------------
// void CLcServiceImpl::SelectLocationApplicationL
// ---------------------------------------------------------------------------
//
TPtrC CLcServiceImpl::SelectLocationApplicationL( 
                                CLcLocationAppInfoArray*   aAppInfoArray )
    {
    // The App Info arrays ownership is tranferred to the List. So push it
    // to the Cleanup stack until the ownership is completely transferred.
    CleanupStack::PushL( aAppInfoArray );
    
    iPopUpListBox = new ( ELeave ) CLcPopupListBox( aAppInfoArray );
    CleanupStack::Pop( aAppInfoArray );
    
    // Construct the popup with the list
    iPopUp = CAknPopupList::NewL( iPopUpListBox, 
                                  R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                                  AknPopupLayouts::EMenuGraphicWindow );  
    iPopUpListBox->ConstructL( iPopUp );    
    // iPopUp->SetBorder( AknBorderId::EAknBorderMenuPopup );								                          
    
    // Set up notification events
    iNotificationAO->IssueRequest();
                             
    // Display the pop-up and let user selection.                             
    TInt popupOk = iPopUp->ExecuteLD();
    iPopUp = NULL;
    
    // Only we are observing for notifications. We dont need it anymore
    // Cancel it
    if ( !iObserver )
        {
        iNotificationAO->CancelRequest();
        }
        
    if ( popupOk )
        {
        delete iSelectedAppId;        
        iSelectedAppId = NULL;
                
        iSelectedAppId = iPopUpListBox->AppInfoId().AllocL();
        
        delete iPopUpListBox;
        iPopUpListBox = NULL;
        
        return *iSelectedAppId;
        }
    else
        {
        delete iPopUpListBox;
        iPopUpListBox = NULL;
        
        return TPtrC();
        }
    }
    
// End of File
