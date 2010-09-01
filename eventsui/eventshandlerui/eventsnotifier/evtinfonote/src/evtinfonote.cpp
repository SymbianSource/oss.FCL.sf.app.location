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
* Description:  Info Note implementation
*
*/


// System Includes
#include <eikenv.h>
#include <e32std.h>
#include <aknmessagequerydialog.h>
#include <bautils.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <AknNotiferAppServerApplication.h>
#include <aknPopupHeadingPane.h>
#include <evtinfonote.rsg>
#include <s32mem.h>
#include <eikbtgpc.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <eikimage.h>

#include <aknnotedialog.h>

// User Includes
#include "evtinfonoteparams.h"
#include "evtinfonote.h"
#include "evtinfonoteinterface.h"
#include "evtinfonoteconsts.hrh"
#include "evtinfonoteimpl.h"

#include "evtdebug.h"

// Constant Definitions
_LIT(KResourceFileName, "evtinfonote.rsc");
static const TInt KLengthOfNum = 64;

//
// ------------------------- Member Function definition ----------------------

// ---------------------------------------------------------------------------
// CEvtInfoNote::CEvtInfoNote()
// ---------------------------------------------------------------------------
//
CEvtInfoNote::CEvtInfoNote()
    : CActive( EPriorityStandard )
    {
    }

// ---------------------------------------------------------------------------
// CEvtInfoNote::~CEvtInfoNote()
// ---------------------------------------------------------------------------
//
CEvtInfoNote::~CEvtInfoNote()
    {    
    // If there is an existing message outstanding then
    // terminate the message
    CompleteMessage( KErrServerTerminated );
        
    Cancel();
    
    delete iInputBuffer;
    
    // Free the resources
    iEikEnv->DeleteResourceFile( iResourceFileFlag );
        
    if ( iAppSwitchSupress && iAppUi )
        {
        iAppUi->SuppressAppSwitching( EFalse ); 
        iAppSwitchSupress = EFalse;                        
        }            
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNote::ConstructL()
// ---------------------------------------------------------------------------
//    
void CEvtInfoNote::ConstructL()
    {
    EVTUIDEBUG( "+ CEvtInfoNote::ConstructL" );
    
    // Store a Local pointer for Eikon Env
    iEikEnv = CEikonEnv::Static();
    iAppUi = static_cast< CAknNotifierAppServerAppUi* >( iEikEnv->EikAppUi());
    
    // Open the localized resource file
    TFileName dllDrive;;
    Dll::FileName( dllDrive );

    // File name without the drive letter.
    TFileName filename;
    filename += KDC_RESOURCE_FILES_DIR; 
    filename += KResourceFileName;        
        
    // Append the drive letter to the file name.    
    TParse parse;
	User::LeaveIfError( parse.Set( dllDrive, NULL, NULL ) );
    User::LeaveIfError( parse.Set( parse.Drive(), &filename, NULL ) );
	filename = parse.FullName();	                        
    
    BaflUtils::NearestLanguageFile(iEikEnv->FsSession(),filename);
    
    EVTUIDEBUG( "Before Resource File" );
    
    iResourceFileFlag=iEikEnv->AddResourceFileL(filename);
    
    EVTUIDEBUG( "After Resource File" );
    // Add the object to the Active scheduler
    CActiveScheduler::Add( this );
    
    EVTUIDEBUG( "- CEvtInfoNote::ConstructL" );
    }

// ---------------------------------------------------------------------------
// CEvtInfoNote* CEvtInfoNote::NewL()
// ---------------------------------------------------------------------------
//
CEvtInfoNote* CEvtInfoNote::NewL()
    {
    CEvtInfoNote* self = new ( ELeave ) CEvtInfoNote;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNote::Release()
// ---------------------------------------------------------------------------
//    
void CEvtInfoNote::Release()  
    {
    EVTUIDEBUG( "+ CEvtInfoNote::Release" );
    // This method is called to free all the resources
    // associated with the Notifier. Here it would mean deleting
    // the notifier handle
    delete this;
    EVTUIDEBUG( "- CEvtInfoNote::Release" );        
    }
    
// ---------------------------------------------------------------------------
// MEikSrvNotifierBase2::TNotifierInfo CEvtInfoNote::Info() const
// ---------------------------------------------------------------------------
//    
MEikSrvNotifierBase2::TNotifierInfo CEvtInfoNote::Info() const
    {
    EVTUIDEBUG( "+ CEvtInfoNote::Info" );
    // Return the notifier information
    return iInfo;
    }
    
// ---------------------------------------------------------------------------
// TPtrC8 CEvtInfoNote::StartL
// ---------------------------------------------------------------------------
//    
TPtrC8 CEvtInfoNote::StartL( const TDesC8&        /* aBuffer */ )
    {    
    TPtrC8 ret( KNullDesC8 );
    return ( ret );    
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNote::StartL
// ---------------------------------------------------------------------------
//    
void CEvtInfoNote::StartL( const TDesC8&          aBuffer,
                                  TInt            aReplySlot,
                            const RMessagePtr2&   aMessage )          
    {
    EVTUIDEBUG( "+ CEvtInfoNote::StartL" );
    // The notifier will handle only one request at a time.
    if ( IsActive())
        {
        User::Leave ( KErrInUse );
        }
    
    iTriggerEvent = EDisplayNotifier;      
    iMessage = aMessage;    
    
    // Delete the old buffer and copy the contents of the argument into this
    // buffer
    // This buffer needs to be stored because, the user is going to handle
    // the request asynchrnously.     
    delete iInputBuffer;  
    iInputBuffer = NULL;
    iInputBuffer = aBuffer.AllocL();
    
    // Copy the reply Slot
    iReplySlot = aReplySlot;
    
    TRequestStatus* status = &iStatus;
    SetActive();    
    User::RequestComplete( status, KErrNone );    
    EVTUIDEBUG( "- CEvtInfoNote::StartL" );
    }
    
// ---------------------------------------------------------------------------
// void CEvtInfoNote::Cancel
// ---------------------------------------------------------------------------
//    
void CEvtInfoNote::Cancel()
    {
    EVTUIDEBUG( "+ CEvtInfoNote::Cancel" );
    // Delete the running Information note
    delete iInfoNote;
    iInfoNote = NULL;
    
    CompleteMessage( KErrCancel );
    
    delete iInputBuffer;
    iInputBuffer = NULL;
        	        
    iTriggerEvent = ENone;   	         
    CActive::Cancel();
    EVTUIDEBUG( "- CEvtInfoNote::Cancel" );
    }

// ---------------------------------------------------------------------------
// TPtrC8 CEvtInfoNote::UpdateL
// ---------------------------------------------------------------------------
//    
TPtrC8 CEvtInfoNote::UpdateL( const TDesC8&   /* aBuffer */ )
    {
    TPtrC8 ret( KNullDesC8 );
    return ( ret );    
    }

// ---------------------------------------------------------------------------
// TPtrC8 CEvtInfoNote::UpdateL
// ---------------------------------------------------------------------------
//
void CEvtInfoNote::UpdateL( const TDesC8&         /* aBuffer */, 
                                   TInt            /* aReplySlot */, 
                             const RMessagePtr2&   aMessage)
    {
    aMessage.Complete( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CEvtInfoNote::TNotifierInfo CEvtInfoNote::RegisterL()
// ---------------------------------------------------------------------------
//
CEvtInfoNote::TNotifierInfo CEvtInfoNote::RegisterL()
    {
    iInfo.iUid          = KEvtInfoNote;
    iInfo.iChannel      = KEvtInfoNoteChannel;
    iInfo.iPriority     = ENotifierPriorityHigh; 
    return iInfo;    
    }
    

// ---------------------------------------------------------------------------
// void CEvtInfoNote::CompleteMessage()
// ---------------------------------------------------------------------------
// 
void CEvtInfoNote::CompleteMessage( TInt        aError,
                                    TInt        aKeyPressEvent )
	{
	EVTUIDEBUG( "+ CEvtInfoNote::CompleteMessage" );
	// Check if the message is not NULL
	if ( !iMessage.IsNull() )
		{
		
		// Obtain the Reply slot and communicate the response
		// to the user
		TInt desLength = iMessage.GetDesLength( iReplySlot );
		if (  desLength >= KLengthOfNum )
		    {
		    TBuf8< KLengthOfNum > returnBuf;
		    returnBuf.AppendNum( aKeyPressEvent );
		    
		    TInt length = returnBuf.Length();
		    length = length + 1;
		    TInt error = iMessage.Write( iReplySlot, returnBuf );
		    if ( !error )
		        {
		        EVTUIDEBUG1( "CEvtInfoNote::CompleteMessage - Write Error : %d", error );
		        }
		    }
		    
		iMessage.Complete( aError );	
		}
    
    if ( iAppSwitchSupress && iAppUi )
        {
        iAppUi->SuppressAppSwitching( EFalse ); 
        iAppSwitchSupress = EFalse;                        
        }
        
    iTriggerEvent = ENone;
    EVTUIDEBUG( "- CEvtInfoNote::CompleteMessage" );		
	}
	
// ---------------------------------------------------------------------------
// void CEvtInfoNote::RunL()
// ---------------------------------------------------------------------------
//                             
void CEvtInfoNote::RunL()
    {
    EVTUIDEBUG( "+ CEvtInfoNote::RunL" );  
    // Do this only on KErrNone
    if ( !iStatus.Int())
    	{
    	switch( iTriggerEvent )
    		{
    		case EDisplayNotifier:
    			{
    			// Fetch the input parameters 
    			CEvtInfoNoteInputParam* param = GetInputParametersLC();
    			CleanupStack::Pop( param );
    			
    			// Create a new dialog instance
    			CEvtInfoNoteImpl* queryDialog = CEvtInfoNoteImpl::NewLC( param );
    			
	            // Display the dialog in cover UI
	            // queryDialog->PublishDialogL( SuplQueryId(), KCatUidSuplQuery );
	                	         			    
			    // Store the value for the Query dialog. this would be useful
			    // for cancelling it.			    			    
			    iInfoNote = queryDialog;
			        	
			    // Supress the App switching
			    if ( !iAppSwitchSupress )
			        {
                    iAppUi->SuppressAppSwitching( ETrue );    	         
    	            iAppSwitchSupress = ETrue;			        
			        }
			    TEvtInfoNoteCmds flag;
			    TInt keypress = KErrNone;
			        
		    	keypress = queryDialog->RunLD( &flag );
		    	iInfoNote = NULL;
		    	
		    	EVTUIDEBUG1( "Key Press : %d", keypress );
				EVTUIDEBUG1( "Return Code : %d", flag );
				
		    	if ( keypress )
			    	{
				    switch( flag )
				    	{
						case EEvtCmdOpenEvent:
							{
							CompleteMessage( KErrNone, EOpenTrigger );
							break;				
							}		
						case EEvtCmdSnoozeEvent:
							{
							CompleteMessage( KErrNone, ESnoozeTrigger );
							break;				
							}		
						case EEvtCmdShowDetails:
							{
							CompleteMessage( KErrNone, EShowDetails );
							break;				
							}	
						case ECloseInfoNote:
							{
							CompleteMessage( KErrNone, ECloseInfoNote );
							break;				
							}
						default:
							{
							break;				
							}    			
				    	}	
			    	}
			    else
			    	{
			    	CompleteMessage( KErrNone, ECloseInfoNote );
			    	}
			    	
		    	// Reset the trigger Event
		    	iTriggerEvent = ENone;
		    	    
		    	break;
    			}   			
    		default:
    			{
    			User::Leave( KErrNotSupported );
    			break;
    			}
    		}
    	}
	else
		{
		EVTUIDEBUG1( "- CEvtInfoNote::Error %d ", iStatus.Int() );
		// Error Handling for non-KErrNone status
		CompleteMessage( iStatus.Int());
		}				
    EVTUIDEBUG( "- CEvtInfoNote::RunL" );
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNote::DoCancel()
// ---------------------------------------------------------------------------
// 
void CEvtInfoNote::DoCancel()
    {    
    // Reset the state machine
    iTriggerEvent = ENone;    
    }

// ---------------------------------------------------------------------------
// TInt CEvtInfoNote::RunError()
// ---------------------------------------------------------------------------
//    
TInt CEvtInfoNote::RunError( TInt aError )
	{
	EVTUIDEBUG( "+ CEvtInfoNote::RunError" );
	// If the dialog value is set then the dialog must be
	// set to NULL because the dialog would have been destroyed
	// already since its present on the cleanup stack
    iInfoNote= NULL;
	
	// Complete the outstanding request with the error code
	CompleteMessage( aError );
    
    // Reset the state machine.
    iTriggerEvent = ENone;  
    
    EVTUIDEBUG( "- CEvtInfoNote::RunError" );	
	return CActive::RunError( aError );	
	}
	
// ---------------------------------------------------------------------------
// TInt CEvtInfoNoteInputParam::GetInputParametersLC()
// ---------------------------------------------------------------------------
//	
CEvtInfoNoteInputParam* CEvtInfoNote::GetInputParametersLC()
    {
    EVTUIDEBUG( "+ CEvtInfoNote::GetInputParametersLC" );
    // Create a new input paramter object. The ownership of 
    // this object will be transferred back to the calling
    // function.
	CEvtInfoNoteInputParam* input = CEvtInfoNoteInputParam::NewL();
	CleanupStack::PushL( input );
				
	// Obtain the parameters from the Input Buffer
	CBufFlat* inputParams = 
	            CBufFlat::NewL( iInputBuffer->Des().Length() );
	CleanupStack::PushL( inputParams );
	inputParams->ResizeL( iInputBuffer->Des().Length());
	
    inputParams->Ptr( 0 ).Copy( *iInputBuffer );
    
    // Set the Read buffer to read the values from input buffer
    RBufReadStream  readStream( *inputParams, 0 );
    CleanupClosePushL( readStream );

    // Internalize the input buffer string to the current object.
    input->InternalizeL( readStream );
    
    CleanupStack::PopAndDestroy( 2, inputParams );
    
    EVTUIDEBUG( "- CEvtInfoNote::GetInputParametersLC" );
    
    return input;  
    }
