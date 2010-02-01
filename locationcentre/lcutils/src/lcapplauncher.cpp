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
* Description:  Utility class which provides the Application, Service and File
*                launching services to Location Centre.
*
*/


// SYSTEM INCLUDES
#include <apgcli.h>
#include <apgtask.h>
#include <eikenv.h>
#include <lcappexitobserver.h>
#include <SchemeHandler.h>
#include <documenthandler.h>
#include <e32math.h>
#include <eikappui.h>
#include <avkon.hrh>

// USER INCLUDES
#include "lcapplauncher.h"
#include "lcipcparams.h"
#include "lcdebug.h"

// CONSTANT DEFINITIONS
_LIT( KLcFourSpace, "4 " );

// Maximum value that the UID field can take in S60
const TUint32   KUidMaxValue = 0xFFFFFFFF;

// Application Server Service ID. This is the same valuw which AknAppService classes use
const TUid KLcLaunchAppServiceUid = { 0x101F887A };

// -------- Member functions of RLcLaunchAppService --------------------------

// ---------------------------------------------------------
// TUid RLcLaunchAppService::ServiceUid
// ---------------------------------------------------------
//
TUid RLcLaunchAppService::ServiceUid() const
    {
    return KLcLaunchAppServiceUid;
    }

// -------- Member functions of CLcLaunchAppService --------------------------

// ---------------------------------------------------------------------------
// CLcLaunchAppService* CLcLaunchAppService::NewL
// ---------------------------------------------------------------------------
//
CLcLaunchAppService* CLcLaunchAppService::NewL()
	{
	CLcLaunchAppService* self = new ( ELeave ) CLcLaunchAppService();
	return self;
	}

// ---------------------------------------------------------------------------
// CLcLaunchAppService::~CLcLaunchAppService
// ---------------------------------------------------------------------------
//		
CLcLaunchAppService::~CLcLaunchAppService()
	{
    delete iMonitor;
    iService.Close();	
	}

// ---------------------------------------------------------------------------
// Tvoid CLcLaunchAppService::ServerName
// ---------------------------------------------------------------------------
//
void CLcLaunchAppService::ServerName( TName& 	aServerName,
					 				  TUid 		aAppServerUid,
					 				  TUint 	aServerDifferentiator )
	{
	_LIT(KServerNameFormat, "%08x_%08x_AppServer");
	aServerName.Format( KServerNameFormat, 
						aServerDifferentiator, 
						aAppServerUid );
	}

// ---------------------------------------------------------------------------
// TUid RLcLaunchAppService::ServiceUid
// ---------------------------------------------------------------------------
//	
TUint CLcLaunchAppService::GenerateServerDifferentiatorAndName( 
										TName& 		aServerName, 
										TUid 		aAppServerUid )
	{
	TUint r;
	FOREVER
		{
		r = Math::Random();
		if ( r == 0 )
			continue;
		ServerName( aServerName, aAppServerUid, r );
		TFindServer find( aServerName );
		TFullName fullName;
		if ( find.Next( fullName ) == KErrNone )
			{
			continue;
			}			
		break;
		}		
	return r;
	}

// ---------------------------------------------------------------------------
// void CLcLaunchAppService::LaunchAppL
// ---------------------------------------------------------------------------
//		
void CLcLaunchAppService::LaunchAppL( const TUid& 						aAppUid, 
    					   				    MAknServerAppExitObserver& 	aObserver, 
    				 				  const TDesC&               		aCmdLineArg )
	{	
	DEBUG("+ CLcLaunchAppService::LaunchAppL")
		
	CEikonEnv* eikEnv = CEikonEnv::Static();		
	RWindowGroup& wg = eikEnv->RootWin();
	
	TName notUsed;
	const TUint differentiator = GenerateServerDifferentiatorAndName( notUsed, aAppUid );
	TRequestStatus requestStatusForRendezvous;
	
	RApaLsSession apa;
	User::LeaveIfError( apa.Connect());
	CleanupClosePushL( apa );
	
    // Check for the Application capabilities
    TApaAppCapabilityBuf capabilityBuf;
    User::LeaveIfError( apa.GetAppCapability( capabilityBuf, aAppUid ));
    TApaAppCapability& caps = capabilityBuf();
	
	// Launch only those applications can be embedded and will not be run the background
	
	if( caps.iLaunchInBackground || 
	    caps.iEmbeddability == TApaAppCapability::ENotEmbeddable )
		{
		User::Leave( KErrNotSupported );
		}
			
	TApaAppInfo info;
	User::LeaveIfError( apa.GetAppInfo( info, aAppUid ));

    // Pack the command line arguments
	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
	cmdLine->SetExecutableNameL(info.iFullName);
	cmdLine->SetServerRequiredL(differentiator);

    // Set the Command line arguments.
    if ( aCmdLineArg.Length())
        {
        // These arguments would be sent as Tail end arguments to the launched
        // Application. Its left to the application to handle those
        HBufC8* tailEnd = HBufC8::NewLC( aCmdLineArg.Length());
        tailEnd->Des().Copy( aCmdLineArg );
        
        // The Tail end buffer is copied to the Command line structure. Hence,
        // can delete this variable after the set operation succeeds.
        cmdLine->SetTailEndL( tailEnd->Des());
        
        CleanupStack::PopAndDestroy( tailEnd );
        }

	cmdLine->SetCommandL( EApaCommandRun );

	// Window chain the client with the current application
	const TInt parentWindowGroupID = wg.Identifier();
	if ( parentWindowGroupID )
		{
		cmdLine->SetParentWindowGroupID( parentWindowGroupID );
		wg.AllowProcessToCreateChildWindowGroups( aAppUid );
		}

	// Launch the requested Application
	TThreadId threadNotUsed;
	User::LeaveIfError( apa.StartApp( *cmdLine, 
								      threadNotUsed, 
								      &requestStatusForRendezvous ));

	CleanupStack::PopAndDestroy(2, &apa);	// cmdLine and apa	
	
	CEikAppUi* appUi = eikEnv->EikAppUi();
	if ( appUi )
		{
		appUi->DeactivateActiveViewL();
		}

	User::WaitForRequest( requestStatusForRendezvous );
	User::LeaveIfError( requestStatusForRendezvous.Int());
	
	// Establish a connection to the running server
	TName serverName;
	ServerName( serverName, aAppUid, differentiator );
	
	// Though the server had been successfully created the application
	// has can shutdown before we can establish a connection. There is nothing
	// that we can do here. Ignore the error.	
	iService.ConnectExistingByNameL(serverName );

	// Set the observer for receiving notifications of the application exit.
    iMonitor = CApaServerAppExitMonitor::NewL( iService, 
    										   aObserver,
    										   CActive::EPriorityStandard );
    										   
    DEBUG("- CLcLaunchAppService::LaunchAppL")    										   
	}
    				 

// ----- Member funtions for CLcAppLauncher ----------------------------------

// ---------------------------------------------------------------------------
// CLcAppLauncher::CLcAppLauncher
// ---------------------------------------------------------------------------
//
CLcAppLauncher::CLcAppLauncher()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
         
// ---------------------------------------------------------------------------
// CLcAppLauncher::~CLcAppLauncher
// ---------------------------------------------------------------------------
//
CLcAppLauncher::~CLcAppLauncher()
    {
	
	// Delete the Broswer launcher.
	delete iBrowserLauncher;
	
	// Delete the document handler.
	delete iDocHandler;
	
	// Delete the Application Service if it exists
	delete iAppService;    	
    }
        
// ---------------------------------------------------------------------------
// CLcAppLauncher* CLcAppLauncher::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CLcAppLauncher* CLcAppLauncher::NewL()
    {
    CLcAppLauncher* self = NewLC();
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcAppLauncher* CLcAppLauncher::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CLcAppLauncher* CLcAppLauncher::NewLC()
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcAppLauncher* self = new ( ELeave ) CLcAppLauncher();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcAppLauncher::ConstructL
// ---------------------------------------------------------------------------
//
void CLcAppLauncher::ConstructL()
    {
    // Second phase of the two phase constructor.    
    iEnv = CEikonEnv::Static();
    }

// ---------------------------------------------------------------------------
// void LcAppLauncher::LaunchLocationApplicationL
// ---------------------------------------------------------------------------
//
EXPORT_C void CLcAppLauncher::LaunchLocationApplicationL(
                                TInt                 	aAppType,
                          const TDesC&                  aIdentifer,
                                TBool                   aStandAlone,
                          const TDesC&                  aCmdLineArg,
                                MLcAppExitObserver*     aExitObserver )
    {  
    DEBUG("+ CLcLaunchAppService::LaunchLocationApplicationL")
    
    // Check for the validity of the request
    
    // Check if there is an outstanding Embedded launch request
    if(( isEmbeddedLaunchOutstanding && !aStandAlone ) ||
       ( !aStandAlone && !iEnv ))
    	{
        User::Leave( KErrNotSupported );    	
    	}
    
    // At any instant of time this class can one outstanding Exit observer.
    // There is currently no provision of storing a list of observers and
    // mapping them correspondingly to each of the assciated applications
    // Hence, Leave if there exists an Exit observer.
    if (( aExitObserver && iExitObserver ))
        {
        User::Leave( KErrAlreadyExists );
        }
        
    // Launch the Applications based on their type and Mode of launching
    switch ( aAppType )
        {
        case ELcNativeApplication:
            {
            // Since the request is for a Native S60 application, the Identifer
            // contains an UID. Obtain it using Lexer
            TLex lexer( aIdentifer );
            TUint32 uidValue;
            
            User::LeaveIfError( lexer.BoundedVal( uidValue, EHex, KUidMaxValue ));
            
            if ( aStandAlone )
            	{
	            // Launch the Location based Application based standalone mode
	            LaunchAppL( TUid::Uid( uidValue ), aCmdLineArg );   	
            	}
			else
				{
				// Launch Application in the chained mode
				LaunchAppAsChainedL( TUid::Uid( uidValue ), aCmdLineArg, aExitObserver );
				}
                
            break;
            }
        case ELcWebUrl:
            {
            LaunchWebBroserL( aIdentifer, aStandAlone, aExitObserver );
            break;
            }
        case ELcDocument:
            {
            LaunchContentL( aIdentifer, aStandAlone, aExitObserver );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
        
    DEBUG("+ CLcLaunchAppService::LaunchLocationApplicationL")        
    }
        
// ---------------------------------------------------------------------------
// void LcAppLauncher::LaunchAppL
// ---------------------------------------------------------------------------
//
void CLcAppLauncher::LaunchAppL(
                                TUid                    aApplicationUid,
                          const TDesC&                  aCmdLineArg )
    {  
    DEBUG("+ CLcAppLauncher::LaunchAppL")
    
    // Create the Session handle and connect to the Application architecture
    // server. This is required to obtain the details of the application from
    // the Application UID.
    RApaLsSession lsSession;
    User::LeaveIfError( lsSession.Connect() );
    CleanupClosePushL( lsSession );
        
    // Get the Application's capability.
    TApaAppCapabilityBuf capabilityBuf;
    User::LeaveIfError( lsSession.GetAppCapability( capabilityBuf, aApplicationUid ));
    TApaAppCapability& caps = capabilityBuf();
	
	if ( caps.iEmbeddability == TApaAppCapability::EEmbeddableOnly ||
	     caps.iEmbeddability == TApaAppCapability::EEmbeddableUiNotStandAlone )
	    {
	    User::Leave( KErrNotSupported );
	    }
		            
    // Get the Application Information.
    TApaAppInfo appInfo;    
    User::LeaveIfError( lsSession.GetAppInfo( appInfo, aApplicationUid ) );
    
    // Fill the command line argument structure
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    
    // Set the Application name.    
    TFileName appName = appInfo.iFullName;
    cmdLine->SetExecutableNameL( appName );
    
    // Set the Command
    if ( caps.iLaunchInBackground )
        {
        cmdLine->SetCommandL( EApaCommandBackground );
        }
    else
        {
        cmdLine->SetCommandL( EApaCommandRun );
        }

    // Set the Command line arguments.
    if ( aCmdLineArg.Length())
        {
        // These arguments would be sent as Tail end arguments to the launched
        // Application. Its left to the application to handle those
        HBufC8* tailEnd = HBufC8::NewLC( aCmdLineArg.Length());
        tailEnd->Des().Copy( aCmdLineArg );
        
        // The Tail end buffer is copied to the Command line structure. Hence,
        // can delete this variable after the set operation succeeds.
        cmdLine->SetTailEndL( tailEnd->Des());
        
        CleanupStack::PopAndDestroy( tailEnd );
        }

	// Start the Application
	User::LeaveIfError( lsSession.StartApp( *cmdLine ));
    
    // Clean up all the allocated heap variables.
    CleanupStack::PopAndDestroy( cmdLine );
    CleanupStack::PopAndDestroy(); //lsSession 		         
    
    DEBUG("- CLcAppLauncher::LaunchAppL")
    }

// ---------------------------------------------------------------------------
// void LcAppLauncher::LaunchAppAsChainedL
// ---------------------------------------------------------------------------
//
void CLcAppLauncher::LaunchAppAsChainedL( 	  
							  TUid                    aApplicationUid,                               
                 		const TDesC&                  aCmdLineArg,
                 			  MLcAppExitObserver*     aExitObserver )
	{
	DEBUG("+ CLcAppLauncher::LaunchAppAsChainedL" )
	
	// Createt the App service object and launch the application
	CLcLaunchAppService* appService = CLcLaunchAppService::NewL();
	CleanupStack::PushL( appService );
	appService->LaunchAppL( aApplicationUid, *this, aCmdLineArg );
	CleanupStack::Pop( appService );
	
	// Transfer the ownership to this class.
	iAppService = appService;	
	isEmbeddedLaunchOutstanding = ETrue;
	
	// Store the Observer for future reference
	iExitObserver = aExitObserver;
	
	DEBUG("- CLcAppLauncher::LaunchAppAsChainedL" )	
	}
	
// ---------------------------------------------------------------------------
// void LcAppLauncher::LaunchWebBroserL
// ---------------------------------------------------------------------------
// 
void CLcAppLauncher::LaunchWebBroserL( const TDesC&              aUrl,
                                             TBool               aStandAlone,
                                             MLcAppExitObserver* aObserver )
    {
    // The parameters need to be packed in the order specified by the browser.
    // Parameters are separated by space
    // 1st parameter: Type of the second parameter. In this case 4 to indicate
    //                that the second parameter is a URL.
    // 2nd parameter: URL that needs to be launched in the browser.
    // Since we have to pack "4 " before the URL name, the total length of
    // the parameter string should be Length of the URL + 2.
    HBufC* param = HBufC::NewLC(( 2 + aUrl.Length()));
    param->Des().Append( KLcFourSpace );
    param->Des().Append( aUrl );
    
    if ( aStandAlone )
        {
        // The URL is launched in the Browser. Set the Browser UID as the UID
        // of the application which needs to be launched.
        const TInt KBrowserUid = 0x10008D39;
        TUid id( TUid::Uid( KBrowserUid ) );
        
        // Check if the Browser application is already running. Incase its
        // running Open this URL in the browser.
        TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
        TApaTask task = taskList.FindApp( id );
        if ( task.Exists() )
            {
            // The browser is running. Launch the URL.
            HBufC8* param8 = HBufC8::NewLC( param->Length() );
            param8->Des().Append( *param );
            task.SendMessage( TUid::Uid( 0 ), *param8 ); // UID is not used
            CleanupStack::PopAndDestroy();
            }
        else 
            {
            // The browser is not running. Start the browser application with
            // the specified parameter. This is done by launching the browser with
            // a document name as the URL.
            RApaLsSession appArcSession;
            
            // Connect to the Application architecture server.
            User::LeaveIfError( appArcSession.Connect());
            CleanupClosePushL( appArcSession );
            TThreadId id;
            
            // Start the browser with the URL
            User::LeaveIfError( appArcSession.StartDocument( *param, 
                                                             TUid::Uid( KBrowserUid ),
                                                             id ));
            CleanupStack::PopAndDestroy( &appArcSession );
            }        
        }
    else
        {
        
        if ( !iBrowserLauncher )
            {
            // Create the Browser instance    
            iBrowserLauncher = CSchemeHandler::NewL( aUrl );
            }
            
        // Launch the Web URL in a browser in the Embedded mode.
        iBrowserLauncher->HandleUrlEmbeddedL();
        iBrowserLauncher->Observer( this );
            
        // Set the Observer incase we need to notify the termination of the
        // browser                                              
        aObserver ? ( iExitObserver = aObserver ) : NULL;
        
        isEmbeddedLaunchOutstanding = ETrue;      
        }
    CleanupStack::PopAndDestroy( param ); // param
    
    }

// ---------------------------------------------------------------------------
// void LcAppLauncher::LaunchContentL
// ---------------------------------------------------------------------------
//    
void CLcAppLauncher::LaunchContentL( const TDesC&       		aFileName,
    						   			   TBool                aStandAlone,
    						   			   MLcAppExitObserver*  aObserver )
    {
    if ( !iDocHandler )
    	{
    	iDocHandler = CDocumentHandler::NewL();
    	iDocHandler->SetExitObserver( this );
    	}
   
    TDataType dataType;    
    if ( aStandAlone )
    	{
	   	// The return values are ignored since, the User Cancelling the event doesnt
	   	// make any difference to the Client Application	
	    User::LeaveIfError( iDocHandler->OpenFileL( aFileName, dataType ));      	
    	}
	else
		{
		iExitObserver = aObserver;
	   	// The return values are ignored since, the User Cancelling the event doesnt
	   	// make any difference to the Client Application	
	    User::LeaveIfError( iDocHandler->OpenFileEmbeddedL( aFileName, dataType ));
	    
        isEmbeddedLaunchOutstanding = ETrue;	    	
		}    	
    }

// ---------------------------------------------------------------------------
// void LcAppLauncher::HandleServerAppExit
// ---------------------------------------------------------------------------
//    
void CLcAppLauncher::HandleServerAppExit( TInt aReason )
    {
    isEmbeddedLaunchOutstanding = EFalse;
    
    // Delete the App Service
  	delete iAppService;
  	iAppService = NULL;    
  	
    MLcAppExitObserver* observer = iExitObserver;        
    if ( observer )
    	{
    	// Once the request has been issued we can NULLify the observer. Nullifying
	    // it before the actual callback to faciliate the launching of another app
	    // with an exit observer when the call is executed.
	    iExitObserver = NULL;
    
    	TRAP_IGNORE( observer->HandleChainedLocationAppExitL( aReason ));
    	}
    	
	MAknServerAppExitObserver::HandleServerAppExit( aReason );	  
    }

// End of File

