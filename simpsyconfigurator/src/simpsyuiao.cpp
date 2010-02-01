/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object interface for obtaining Location updates
*
*/


// System Includes
#include <StringLoader.h> // StringLoader
#include <simpsyui.rsg>
#include <aknnotedialog.h>
#include <AknQueryDialog.h>
    
// User Includes
#include "simpsyuiao.h"

// Constant Defintions
const TInt KFormatStringLength		= 200;
const TInt KFormatCharacterWidth	= 10;
const TInt KPositionDataGranularity = 6;
const TInt KFormatDecimalNumbers	= 4;

_LIT( KTabulator, "\t" );
_LIT( KSimPsyConfiguratorName, "SimPsyConfigurator");

// ---------------------------------------------------------------------------
// Private Constructor
// ---------------------------------------------------------------------------
//
CSimPsyUiAO::CSimPsyUiAO( MSimPsyAOObserver&    aObserver )
	:CActive( EPriorityStandard ),
	iObserver( aObserver )
    {
    }
 
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CSimPsyUiAO::~CSimPsyUiAO()
    {
    Cancel();
    } 

// ---------------------------------------------------------------------------
// CSimPsyUiAO* CSimPsyUiAO::NewL
// Static Two phase contructor that instantiates the CSimPsyUiAO
// 
// @param aObserver				    Observer to the Active object
// @return CSimPsyUiAO*	Reference to the object created
// ---------------------------------------------------------------------------
//    
CSimPsyUiAO* CSimPsyUiAO::NewL( MSimPsyAOObserver&    aObserver )
    {
    CSimPsyUiAO* self = new ( ELeave ) CSimPsyUiAO( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// void CSimPsyUiAO::ConstructL
// Second phase of the two phase constructor
// ---------------------------------------------------------------------------
// 
void CSimPsyUiAO::ConstructL()
    {    
    CActiveScheduler::Add( this );
    }           

// ---------------------------------------------------------------------------
// void CSimPsyUiAO::Initialize
// Obtains the current Location information using the Location
// Acquisition API. The function call is asynchronous and the
// notification regarding the completion of the operation is 
// provided through the MSimPsyAOObserver object
//
// ---------------------------------------------------------------------------
//
void CSimPsyUiAO::GetCurrentLocationL()
    {
    
    // Leave if there is an outstanding Location request
    if( IsActive())
        {
        User::Leave( KErrInUse );
        }
    
    TPositionModuleId KUidSimulationPsy = { 0x101f7a81 };
    TBuf<KFormatStringLength> buf;
            
    // Initialize the Location Server session
    TInt error = iPositionServer.Connect();
	if ( KErrNone != error )
		{
		// Show error to the user and break
    	HBufC* formatString = StringLoader::LoadL( R_SIMPSYUI_SERV_CONNECT_ERR );
    	StringLoader::Format( buf, *formatString,0, error );
		delete formatString;
    	ShowErrorL(buf);
		User::Leave( KErrCouldNotConnect );
		}

    // Get current Location using the SimulationPSY module
    error = iPositioner.Open( iPositionServer, KUidSimulationPsy );
    
    // The opening of a subsession failed
    if ( KErrNone != error )
        {
        // Show error to the user and return
        iPositionServer.Close();
    	HBufC* formatString = StringLoader::LoadL( R_SIMPSYUI_PSY_OPEN_ERR );
    	StringLoader::Format( buf, *formatString,0, error );
		delete formatString;
    	ShowErrorL(buf);
    	User::Leave( KErrAbort );
        }

    // Set Requestor information
    error = iPositioner.SetRequestor( CRequestor::ERequestorService, 
    							      CRequestor::EFormatApplication, 
    							      KSimPsyConfiguratorName );
    // The requestor could not be set
    if ( KErrNone != error )
        {
        // Show error to the user and return
        iPositioner.Close();
        iPositionServer.Close();
    	HBufC* formatString = StringLoader::LoadL( R_SIMPSYUI_SETTING_REQUESTOR_ERR );
    	StringLoader::Format( buf, *formatString,0, error );
		delete formatString;
    	ShowErrorL(buf);
        User::Leave( KErrAbort );
        }
  
    iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );
    SetActive();                                            
    }

// ---------------------------------------------------------------------------
// void CSimPsyUiAO::CancelLocationRequest
// Cancels any outstanding Location request
// ---------------------------------------------------------------------------
//
void CSimPsyUiAO::CancelLocationRequest()
    {
    Cancel();
    }
   
// ---------------------------------------------------------------------------
// void CSimPsyUiAO::DisplayCurrentLocation
// Displays the current Location
// ---------------------------------------------------------------------------
//
void CSimPsyUiAO::DisplayCurrentLocationL()
    {
    TPosition pos;
    iPositionInfo.GetPosition( pos );    
    TInt dummy = 0;
    HBufC* formatString = NULL;
    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &dummy );
    dlg->PrepareLC( R_SIMPSYUI_LOCATION_DIALOG );

    TReal64 latitude = pos.Latitude();
    TReal64 longitude = pos.Longitude();
    TReal32 altitude = pos.Altitude();
    TReal32 hAccuracy = pos.HorizontalAccuracy();
    TReal32 vAccuracy = pos.VerticalAccuracy();

    TRealFormat formatter( KFormatCharacterWidth,KFormatDecimalNumbers );
    TBuf<KFormatStringLength> buf;
    CDesCArrayFlat* locData =
        new( ELeave ) CDesCArrayFlat( KPositionDataGranularity );
    CleanupStack::PushL( locData );

    buf.Zero();
    formatString = StringLoader::LoadL( R_SIMPSYUI_LOCATION_INFO_LATITUDE );
    buf.Copy(*formatString);
    delete formatString;
    buf.Append( KTabulator );
    buf.AppendNum( latitude, formatter );
    locData->AppendL( buf );

    buf.Zero();
    formatString = StringLoader::LoadL( R_SIMPSYUI_LOCATION_INFO_LONGITUDE );
    buf.Copy(*formatString);
    delete formatString;
    buf.Append( KTabulator );
    buf.AppendNum( longitude, formatter );
    locData->AppendL( buf );

    buf.Zero();
    formatString = StringLoader::LoadL( R_SIMPSYUI_LOCATION_INFO_ALTITUDE );
    buf.Copy(*formatString);
    delete formatString;
    buf.Append( KTabulator );
    buf.AppendNum( altitude, formatter );
    locData->AppendL( buf );

    buf.Zero();
    formatString = StringLoader::LoadL( R_SIMPSYUI_LOCATION_INFO_HORZ_ACCURACY );
    buf.Copy(*formatString);
    delete formatString;
    buf.Append( KTabulator );
    buf.AppendNum( hAccuracy, formatter );
    locData->AppendL( buf );

    buf.Zero();
    formatString = StringLoader::LoadL( R_SIMPSYUI_LOCATION_INFO_VERT_ACCURACY );
    buf.Copy(*formatString);
    delete formatString;
    buf.Append( KTabulator );
    buf.AppendNum( vAccuracy, formatter );
    locData->AppendL( buf );
    
    CleanupStack::Pop( locData );
    dlg->SetItemTextArray( locData );
    dlg->SetOwnershipType( ELbmOwnsItemArray );

    dlg->RunLD();
    }
    
        
// ---------------------------------------------------------------------------
// void CSimPsyUiAO::RunL
// Inherited from CActive
// ---------------------------------------------------------------------------
// 
void CSimPsyUiAO::RunL()
    {   
    switch( iStatus.Int())
        {
        case KErrNone:
            {
            // Close the server sessions
            iPositioner.Close();
            iPositionServer.Close();            
            iObserver.NotifyLocationRequestCompletedL();
            break;
            }
        case KErrCancel:
            {
            // There is no need of closing the sessions incase of Cancel since
            // the sessions are closed as a part of DoCancel
            break;
            }
        default:
            {
            // Close the server sessions
            iPositioner.Close();
            iPositionServer.Close();
        	TBuf<KFormatStringLength> buf;
        	// Display Error Code and Break
        	HBufC* formatString = StringLoader::LoadL( R_SIMPSYUI_GET_LOCATION_ERR );
        	StringLoader::Format( buf, *formatString,0, iStatus.Int() );
        	delete formatString;
        	ShowErrorL(buf);
        	break;				
    	    }
        }        
                    
    }

// ---------------------------------------------------------------------------
// void CSimPsyUiAO::DoCancel
// Inherited from CActive
// ---------------------------------------------------------------------------
//     
void CSimPsyUiAO::DoCancel()
    {
    iPositioner.CancelRequest( EPositionerNotifyPositionUpdate );
    // Close the server sessions
    iPositioner.Close();
    iPositionServer.Close();           
    }
    
// ---------------------------------------------------------------------------
// void CSimPsyUiAO::ShowErrorL
// Displays error note
//
// @param aErrorText Error text
// ---------------------------------------------------------------------------
//
void CSimPsyUiAO::ShowErrorL(const TDesC& aErrorText)
    {
	CAknNoteDialog* note = new( ELeave ) CAknNoteDialog( CAknNoteDialog::EErrorTone,
                CAknNoteDialog::ENoTimeout );
	            CleanupStack::PushL( note );
	note->SetTextWrapping( ETrue );
	note->PrepareLC( R_SIMPSYUI_ERROR_NOTE );
	note->SetTextL( aErrorText );
	CleanupStack::Pop( note );
	note->RunLD();
	return;    
    }
