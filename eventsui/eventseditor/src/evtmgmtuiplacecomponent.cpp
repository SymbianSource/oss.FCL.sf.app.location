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
* Description:   'Set Place' component for retrieving place.
*
*/

#include <evtmgmteditorui.rsg>
#include <aknlistquerydialog.h>
#include <StringLoader.h>
#include <CLmkLandmarkSelectorDlg.h> 
#include <TLmkItemIdDbCombiInfo.h> 
#include <lbsposition.h>
#include <aknnotewrappers.h>
#include <aknmessagequerydialog.h>
#include <AknIconArray.h>       // Icon Array
#include <evteditor.mbg>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <data_caging_path_literals.hrh>
#include <gulicon.h>            // Gul Icon
#include <eikapp.h>
#include <aknconsts.h>
#include <textresolver.h>

#include "evteditor.h"
#include "evtmgmteditorui.hrh"
#include "evtmgmtuiplacecomponent.h"
#include "evtdebug.h"
#include "evtmgmtuilocsettinglauncher.h"
#include "evtmgmtuilocsettinglauncherobserver.h"
#include "evteditorconsts.h"
#include "evtmgmtuilbtadapter.h"

// CONSTANTS
const TInt KSetPlaceGranularity = 3;
const TInt KMToKMConversion = 1000;
_LIT( KFloatAccuracyFormat, "%.2f" );

static TInt LinkClickedL(TAny* aPtr);

static TInt LinkClickedL(TAny* aPtr)
    {
    CEvtMgmtUiPlaceComponent* self = static_cast<CEvtMgmtUiPlaceComponent*>(aPtr);
    self->LaunchPositioningSettingsL();
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent ::NewLC()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiPlaceComponent* CEvtMgmtUiPlaceComponent::NewLC(const TPtrC aPrevPlace,  TCoordinate& aTriggerLocation )
    {
    CEvtMgmtUiPlaceComponent * self = new (ELeave) CEvtMgmtUiPlaceComponent( aTriggerLocation);
    CleanupStack::PushL(self);
    self->ConstructL(aPrevPlace);
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent ::NewL()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiPlaceComponent* CEvtMgmtUiPlaceComponent::NewL(const TPtrC aPrevPlace,  TCoordinate& aTriggerLocation )
    {
    CEvtMgmtUiPlaceComponent * self = CEvtMgmtUiPlaceComponent::NewLC( aPrevPlace, aTriggerLocation );
    CleanupStack::Pop(self);    
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent ::CEvtMgmtUiPlaceComponent()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiPlaceComponent::CEvtMgmtUiPlaceComponent(  TCoordinate& aTriggerLocation ):iTriggerLocation(aTriggerLocation)
    {   
    iIsDlgOutstanding = EFalse;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent ::~CEvtMgmtUiPlaceComponent()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiPlaceComponent::~CEvtMgmtUiPlaceComponent()
    {    
    delete iPlace;
    iPlace = NULL;
    
    if( iMapAdapter )
        {
        delete iMapAdapter;
        iMapAdapter = NULL;
        }    
    
    if( iLocationServiceAdapter )
        {
        delete iLocationServiceAdapter;
        iLocationServiceAdapter = NULL;
        }
    
    if( iWaitDialogLauncher )
        {
        delete iWaitDialogLauncher;
        iWaitDialogLauncher = NULL;
        }
    
    if(iPosSetLauncher)
        {
        delete iPosSetLauncher;
        iPosSetLauncher = NULL;
        }
    
    if( iIsDlgOutstanding )
        {
        TKeyEvent key;
        key.iCode = EKeyEscape;
        key.iModifiers = 0;
        TRAP_IGNORE( CEikonEnv::Static()->SimulateKeyEventL(key, EEventKey)) ;
        }
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent ::ConstructL()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent ::ConstructL(const TPtrC aPrevPlace)
    {
    iPlace = aPrevPlace.AllocL();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::DisplayPlacePopupDlg()
// It is used as a wrapper class to trap leaving function DisplayPlacePopupDlgL()
// @param[in] aStatus - status which will contain success/failure state
//                       based on retrieval of location is successful or failed
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::DisplayPlacePopupDlg( TRequestStatus& aStatus )
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::DisplayPlacePopupDlg()" );
    iStatus = &aStatus;
    TInt response(0);
    
    // show 'set place' popup dialog and retrieve location information
    TRAPD( err, response = DisplayPlacePopupDlgL() );
    
    if( err != KErrNone )// if the request is failed
        {
        iIsDlgOutstanding = EFalse;
		//TRAP_IGNORE( ResolveErrorL( err ) );
        User::RequestComplete( iStatus, err );
        }
    else 
        {
        //if from landmark, complete the request otherwise wait till asychronous 
        //operation is completed
        if( response == EEvtMgmtUiSetPlaceLandmark ) 
            {
            if( iStatus )
                User::RequestComplete( iStatus, KErrNone );
            }
        
        //if dialog is cancelled
        if( response == KErrCancel ) 
            {
            if( iStatus )
                User::RequestComplete( iStatus, KErrCancel );
            }
        }
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::DisplayPlacePopupDlg()" );
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::Place()
// returns place value
// -----------------------------------------------------------------------------

TPtrC CEvtMgmtUiPlaceComponent::Place() const
    {
    if(iPlace)
        return *iPlace;
    else
        return KNullDesC();
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::TriggerLocation()
// returns location recentrly retrieved
// -----------------------------------------------------------------------------

TCoordinate& CEvtMgmtUiPlaceComponent::TriggerLocation() const
    {
    return iTriggerLocation;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::HandleMapAndNavigationResonseL()
// Derived from MEvtMgmtUiMapAndNavigationObserver
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::HandleMapAndNavigationResponseL( TInt aErrorCode, const TPtrC aPlace, TCoordinate aTriggerLocation )
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::HandleMapAndNavigationResponseL()" );
    if( aErrorCode == KErrNone )
        {
        //if successful , set place and location to those retrieved from map
        if(aPlace.Length())
            {
            delete iPlace;
            iPlace = NULL;
            iPlace = aPlace.AllocL();     
            }
        
        // Get Place
        aErrorCode = GetPlaceL();
        
        iTriggerLocation = aTriggerLocation;
        }
   
    // complete 'set place' operation selected from map
    if(iStatus)
        User::RequestComplete( iStatus, aErrorCode );
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::HandleMapAndNavigationResponseL()" );
    }


//---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::HandleLocationServiceResponseL()
// Derived from MEvtMgmtUiLocationServiceObserver
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::HandleLocationServiceResponseL( const TInt aStatus )
    {    
    EVTUIDEBUG1("+ CEvtMgmtUiPlaceComponent::HandleLocationServiceResponseL() - %d", aStatus );
    TInt err = aStatus;
    // Stop the running wait dialog
    TInt buttonId = iWaitDialogLauncher->StopWaitDialogL();
    
    if( EEikBidCancel != buttonId ) // if busy dialog is not cancelled
        {
        switch( aStatus )
            {
            case KErrNone:
            case KPositionPartialUpdate:
                {    
                if(iLocationServiceAdapter)    
                    {
                    EVTUIDEBUG("========== Retrieving current location ========" );
                    
                    //Set location
                    iTriggerLocation = iLocationServiceAdapter->CurrentPosition();    
                    TPosition pos = iLocationServiceAdapter->CurrentPosition();

                    if ( !Math::IsNaN( pos.HorizontalAccuracy() ) )
                        DisplayAccuracyNoteL(pos);
                    else
                        DisplayAccuracyNoteL();
                    
                    // Get Place
                    err = GetPlaceL();
                    }         
                EVTUIDEBUG("========== Retrieved current location ========" );
                break;
                }
            default:
                {
                break;
                }
            }
        // complete 'set place' operation selected from current location
        if( iStatus  )
            User::RequestComplete( iStatus, err );
        }
    else
        {
        TRAP( err, DisplayErrorNoteL( R_EVTUI_CANCEL_REQUESTING_LOCATION ) );       
        EVTUIDEBUG1("Erro while displaying error note =%d", err );
        
        // complete 'set place' operation selected from current location
        if( iStatus  )
            User::RequestComplete( iStatus, KErrCancel );
        }
    
    //Delete LocAcq object
    if( iLocationServiceAdapter )
         {
         delete iLocationServiceAdapter;
         iLocationServiceAdapter = NULL;
         }     
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::HandleLocationServiceResponseL()");
    }

//---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::HandleLocationServiceErrorL()
// Derived from MEvtMgmtUiLocationServiceObserver
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::HandleLocationServiceErrorL( TInt aErrorCode )
    {
    EVTUIDEBUG1("+ CEvtMgmtUiPlaceComponent::HandleLocationServiceErrorL - %d", aErrorCode );
    
    //Error handling   
    switch( aErrorCode )
        {
        case KErrNotFound: //if no psy selected
            {
		    //Delete wait dialog
		    if( iWaitDialogLauncher )
		        iWaitDialogLauncher->StopWaitDialogL();
		        
            if( DisplayNoPositioningMethodInfoNoteL( R_EVTUI_MESSAGE_QUERY ) ) //if Open is pressed
                {
                return;
                }            
            break;
            }
        case KErrCancel:
            {                        
            //Delete wait dialog
            if( iWaitDialogLauncher )
                iWaitDialogLauncher->StopWaitDialogL();
            
            TRAPD( err, DisplayErrorNoteL( R_EVTUI_CANCEL_REQUESTING_LOCATION ) );       
            EVTUIDEBUG1("Erro while displaying error note =%d", err );
            
            break;
            }
        case KErrTimedOut:
        default:
            {
		    //Delete wait dialog
		    if( iWaitDialogLauncher )
		        iWaitDialogLauncher->StopWaitDialogL();
		        
            //disply retry message
            CAknErrorNote* errNote = new(ELeave) CAknErrorNote( ETrue );
            TBuf<128> msg;
            CEikonEnv::Static()->ReadResourceAsDes16L(msg, R_EVTUI_RETRY_REQUESTING_LOCATION );        
            errNote->ExecuteLD( msg );
            
            break;
            }
        }
    
    //delete LocAcq object
    if( iLocationServiceAdapter )
        {
        delete iLocationServiceAdapter;
        iLocationServiceAdapter = NULL;
        }
    
    // Complete 'set place' operation selected from current location with failure
    User::RequestComplete( iStatus, KErrCancel );
    
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::HandleLocationServiceErrorL()");
    }

//---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::HandleDialogDismissed()
// Derived from MEvtMgmtUiWaitDialogLauncherObserver
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::HandleDialogDismissed( TInt aButtonId )
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::HandleDialogDismissed()");
    if( EEikBidCancel == aButtonId && iLocationServiceAdapter &&
            iLocationServiceAdapter->IsActive() )
        {
        EVTUIDEBUG("wait dialog is dismissed manually" );
        
        //Cancel Outstanding Request
        iLocationServiceAdapter->CancelRequest();
        }
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::HandleDialogDismissed()");
    }

//---------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::NotifyLocSettingLaucherChangeL()
// Derived from MEvtMgmtUiLocSettingsLauncherObserver
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::NotifyLocSettingLaucherChangeL( TInt /*aErrorCode*/ )
    {
    // Nothing to do.
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiPlaceComponent::DisplayErrorNoteL()
// It is used to display error note.
// @param[in] aResourceId - resource ID of the text to be displayed on error note.
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::DisplayErrorNoteL(TInt aResourceId )
    { 
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::DisplayErrorNoteL" );
    CAknErrorNote* errNote = new(ELeave) CAknErrorNote( ETrue );
    TBuf<32> msg;
    CEikonEnv::Static()->ReadResourceAsDes16L(msg, aResourceId );
    iIsDlgOutstanding = ETrue;
    errNote->ExecuteLD( msg );
    iIsDlgOutstanding = EFalse;
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::DisplayErrorNoteL" );
    }

// ---------------------------------------------------------------------------
// TInt CEvtMgmtUiPlaceComponent::GetPlaceL()
// Displays the Query dialog for getting place.
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiPlaceComponent::GetPlaceL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::GetPlaceL()");
    
    HBufC* place = HBufC::NewLC( KPlaceMaxLength );
    place->Des().Copy( *iPlace );
    TPtr placePtr(place->Des());
    TInt ret = KErrCancel;

    HBufC* prompt = StringLoader::LoadLC(R_LOCEV_NOTE_PROMPT_TEXT);
    
    CAknTextQueryDialog* dlg = new(ELeave) CAknTextQueryDialog( placePtr ,*prompt, CAknQueryDialog::ENoTone );
    if( EEikBidOk == dlg->ExecuteLD( R_EVTUI_PLACE_DATA_QUERY ))
        {
        delete iPlace;
        iPlace = NULL;
        iPlace = place->AllocL();
        ret = KErrNone;
        }
    
    CleanupStack::PopAndDestroy(2); // prompt, place
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::GetPlaceL()");
    return ret;
    }

// ---------------------------------------------------------------------------
// TInt CEvtMgmtUiPlaceComponent::LaunchPositioningSettingsL()
// It is used to launch position settings
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::LaunchPositioningSettingsL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::LaunchPositioningSettingsL()");
    if(!iPosSetLauncher)
          {
          iPosSetLauncher = CEvtMgmtUiLocSettingsLauncher::NewL();                
          }
    iPosSetLauncher->SetObserver(this);
    iPosSetLauncher->LaunchL();
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::LaunchPositioningSettingsL()");
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::DisplayPlacePopupDlgL()
// It is used to display 'set place' popup dialog and retrieve place 
// using selected method.
// @ret it returns the option selected.
//      it leaves with KErrCancel, if popup dialog is cancelled.
//      it may leave with other errors, if retrieval of place fails.     
// -----------------------------------------------------------------------------
//
TInt CEvtMgmtUiPlaceComponent::DisplayPlacePopupDlgL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::DisplayPlacePopupDlgL()");
    TInt index;
    const TInt KCpIconIndex = 0;
    const TInt KLmkIconIndex = 1;
    const TInt KMapIconIndex = 2;
    const TInt KIconTextLength = 1;
    _LIT(KTab, "\t");  
    HBufC* dummy = NULL;
    
    // Retrieve number of landmarks stored in landmark database.
    TInt lmks = LandmarksL();
   
    //Retrive strings for popup dialog.
   
    //Current Location
    dummy = StringLoader::LoadLC(R_EVTUI_CREATENEW_FORM_PLACEPOPUPOPTION_CURRENTPLACE);
    HBufC* cp = HBufC::NewL( KIconTextLength + KTab().Length() + 
                        dummy->Length() + KTab().Length( )+ KTab().Length() );    
    TPtr ptrDummy = cp->Des();    
    ptrDummy.Num(KCpIconIndex);
    ptrDummy.Append(KTab);
    ptrDummy.Append(*dummy);
    ptrDummy.Append(KTab);
    ptrDummy.Append(KTab);
    CleanupStack::PopAndDestroy(dummy);
    CleanupStack::PushL(cp);
    
    //Landmark
    dummy   = StringLoader::LoadLC(R_EVTUI_CREATENEW_FORM_PLACEPOPUPOPTION_LANDMARK);
    HBufC* lmk = HBufC::NewL( KIconTextLength + KTab().Length() + 
                           dummy->Length() + KTab().Length( )+ KTab().Length() );
    ptrDummy.Set(lmk->Des());
    ptrDummy.Num(KLmkIconIndex);
    ptrDummy.Append(KTab);
    ptrDummy.Append(*dummy);
    ptrDummy.Append(KTab);
    ptrDummy.Append(KTab);
    CleanupStack::PopAndDestroy(dummy);
    CleanupStack::PushL(lmk);
   
    //Map
    dummy   = StringLoader::LoadLC(R_EVTUI_CREATENEW_FORM_PLACEPOPUPOPTION_FROMMAP);
    HBufC* map = HBufC::NewL( KIconTextLength + KTab().Length() + 
                           dummy->Length() + KTab().Length( )+ KTab().Length() );    
    ptrDummy.Set(map->Des());
    ptrDummy.Num(KMapIconIndex);
    ptrDummy.Append(KTab);
    ptrDummy.Append(*dummy);
    ptrDummy.Append(KTab);
    ptrDummy.Append(KTab);
    CleanupStack::PopAndDestroy(dummy);
    CleanupStack::PushL(map);
               
    HBufC* title = StringLoader::LoadLC(R_EVTUI_CREATENEW_FORM_PLACEPOPUP_TITLE);
               
    //Create array 
    CDesCArrayFlat* array = new(ELeave)CDesCArrayFlat( KSetPlaceGranularity );
    CleanupStack::PushL(array);
    
    //Add current location
    array->AppendL(*cp);
    
    //if landmarks are present, then add 'Landmark'
    if( lmks )
        array->AppendL(*lmk);
   
    //Create map object to retrieve place from maps
    if( iMapAdapter )
        {
        delete iMapAdapter;
        iMapAdapter = NULL;
        }
    iMapAdapter = CEvtMgmtUiMapNavigationAdapter::NewL();
    iMapAdapter->SetPlaceL( *iPlace );
    iMapAdapter->SetLocation( iTriggerLocation );
    iMapAdapter->SetObserver( this );
    
    //if map service providers are present, then add 'From map'     
    if(iMapAdapter->MapServiceProvidersL())          
        array->AppendL(*map);
    else //if map service provider is not present, delete map object
        {
        delete iMapAdapter;
        iMapAdapter = NULL;
        }
     
    //popup dialog
   CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );
   dlg->PrepareLC(R_EVTMGMTUI_LIST_QUERY);
   UpdateIconsL( *dlg );
   dlg->SetItemTextArray(array );
   dlg->SetOwnershipType(ELbmOwnsItemArray);  
   dlg->QueryHeading()->SetTextL(*title);    
      
   iIsDlgOutstanding = ETrue;
    if ( dlg->RunLD() ) //if item is selected
        {  
        iIsDlgOutstanding = EFalse;
       CleanupStack::Pop();// pop up array since ownership is handed to dlg
       CleanupStack::PopAndDestroy(4) ;//title, map, lmk, cp   
       switch(index)
           {
           case EEvtMgmtUiSetPlaceCurrentPlace: //if current place is selected
               {  
    			EVTUIDEBUG("= Set Current place");              
               PlaceFromCurrentLocationL();                
               break;
               }
           case EEvtMgmtUiSetPlaceLandmark:
               {
               if(lmks) //if landmarks are present, 'Landmark' is pressed
                   {
    				EVTUIDEBUG("= Set Landmark place");   
                   PlaceFromLandmarkL();
                   break;
                   }
               //else 'From map' is pressed, hence request place from map
               //break; ....intentionally commented
               }            
           case EEvtMgmtUiSetPlaceFromMap:
               {
    			EVTUIDEBUG("= Set Map place");   
               index = EEvtMgmtUiSetPlaceFromMap;
               iMapAdapter->PlaceFromMapL();          
               break;
               }
           default:
               {
               break;
               }
           }
       return index;
       }  
    else
        {
        iIsDlgOutstanding = EFalse;
        CleanupStack::Pop();// pop up array since ownership is handed to dlg
        CleanupStack::PopAndDestroy(4) ;//title, map, lmk, cp
        return KErrCancel;
        }    
     }

// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::PlaceFromLandmarkL()
// It retrieves place and location details from landmark     
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::PlaceFromLandmarkL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::PlaceFromLandmarkL()");
    _LIT(KCommaSeparator,", ");
    TLmkItemIdDbCombiInfo lmkInfo;
    CLmkLandmarkSelectorDlg* dlg =  CLmkLandmarkSelectorDlg::NewL();
    iIsDlgOutstanding = ETrue;
    if(dlg->ExecuteLD(lmkInfo))
        {
        iIsDlgOutstanding = EFalse;
        CPosLandmarkDatabase* lmkDb = lmkInfo.GetLmDb();
        CleanupStack::PushL(lmkDb);
        CPosLandmark* lmk = lmkDb->ReadLandmarkLC( lmkInfo.GetItemId() );
        
        //retrieve lat long info
        TLocality locality;
        lmk->GetPosition( locality );
        if(Math::IsNaN( locality.Latitude() ) || Math::IsNaN( locality.Longitude() ) )
            {
            CAknErrorNote* errNote = new(ELeave) CAknErrorNote( ETrue );
            TBuf<128> msg; 
            CEikonEnv::Static()->ReadResourceAsDes16L(msg, R_EVTUI_LANDMARK_LOCMISS_ERROR );        
            errNote->ExecuteLD( msg );
            CleanupStack::PopAndDestroy(2);//lmk,lmkDb
            iIsDlgOutstanding = EFalse;          
            User::Leave(KErrCancel);
            }
        iTriggerLocation = locality;
        
        //retrive position description
        delete iPlace;
        iPlace = NULL;
        iPlace = HBufC::NewL(KPlaceMaxLength);       
        TPtr placePtr = iPlace->Des();
        
        TPtrC ptr(KNullDesC);
        
        if(lmk->GetLandmarkName(ptr)!=KErrNotFound)
            {                      
            placePtr.Copy(ptr);           
            placePtr.Append(_L(" "));            
            }
        if(lmk->GetPositionField(EPositionFieldStreet,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
            {           
            placePtr.Append(ptr);
            placePtr.Append(KCommaSeparator);
            }
        if(lmk->GetPositionField(EPositionFieldCity,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
           { 
           placePtr.Append(ptr);
           placePtr.Append(KCommaSeparator);
           }    
        if(lmk->GetPositionField(EPositionFieldState,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
           {           
           placePtr.Append(ptr);
           placePtr.Append(KCommaSeparator);
           }
        if(lmk->GetPositionField(EPositionFieldCounty,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
           { 
           placePtr.Append(KCommaSeparator);
           placePtr.Append(ptr);
           }
        if(lmk->GetPositionField(EPositionFieldCountry,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
           {           
           placePtr.Append(ptr);
           placePtr.Append(KCommaSeparator);
           }
        TInt commaDesLen = KCommaSeparator().Length();
        if(placePtr.Mid(placePtr.Length()-commaDesLen,commaDesLen).Compare(KCommaSeparator)==0)
            placePtr.Delete(placePtr.Length()-commaDesLen, commaDesLen);
        else
            placePtr.Delete(placePtr.Length()-1,1);
        
        //iEvent.SetPlaceL(*place);
        CleanupStack::PopAndDestroy(2);//lmk,lmkDb
        }
    else
        {
        iIsDlgOutstanding = EFalse;
        if( iStatus )
           User::RequestComplete( iStatus, KErrCancel );
        }
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::PlaceFromLandmarkL()");
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::LandmarksL()
// It returns total number of landmarks stored.
// If there is any error in retrieving details, it will return zero landmark.
// -----------------------------------------------------------------------------
//
TInt CEvtMgmtUiPlaceComponent::LandmarksL()
    {  
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::LandmarksL()" );  
    TInt lmks = 0;
    
    CPosLandmarkDatabase* lmkDb;
	lmkDb = CPosLandmarkDatabase::OpenL();			
	CleanupStack::PushL(lmkDb);
	// Initialize Landmarks data base if required			
	CPosLmOperation* operation = lmkDb->InitializeL();
	CleanupStack::PushL(operation);			
	operation->ExecuteL();
	CPosLmItemIterator* iterator = lmkDb->LandmarkIteratorL();
	CleanupStack::PushL(iterator);
	lmks = iterator->NumOfItemsL();
    CleanupStack::PopAndDestroy(3); //lmkDb, iterator and operation    

    EVTUIDEBUG1("Number of Landmarks = %d", lmks );
    
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::LandmarksL()" );
    return lmks;   
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::PlaceFromCurrentLocationL()
// It initiates set place request from current location.
// If there is no positioning method enabled, it will leave with KErrCancel.
// Otherwise it will request current location and will launch busy dialog
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::PlaceFromCurrentLocationL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::PlaceFromCurrentLocationL()" );
    // Delete any previously created LocAcq object.
    if( !iLocationServiceAdapter )
     {     
     //Create new LocAcq object
     iLocationServiceAdapter = CEvtMgmtUiLocationServiceAdapter::NewL( *this );
     }
   
    EVTUIDEBUG("Before iLocationServiceAdapter->LocationRequestL" );
    iLocationServiceAdapter->LocationRequestL();
    EVTUIDEBUG("After iLocationServiceAdapter->LocationRequestL" );
    
    if( iWaitDialogLauncher )
        {
        EVTUIDEBUG("Deleting iWaitDialogLauncher..." );
        delete iWaitDialogLauncher;
        iWaitDialogLauncher = NULL;
        }
    
    // Create the wait dialog launcher
    iWaitDialogLauncher = CEvtMgmtUiWaitDialogLauncher::NewL(); 
    iWaitDialogLauncher->SetObserver(this);
    iWaitDialogLauncher->SetTextL( R_EVTUI_WAIT_REQUESTING_LOCATION );
    
    EVTUIDEBUG("Before iWaitDialogLauncher->StartWaitDialogL" );
    //Start busy wait dialog while requesting current location
    TRAPD( error, iWaitDialogLauncher->StartWaitDialogL());
    EVTUIDEBUG1("After iWaitDialogLauncher->StartWaitDialogL - %d", error );
    
    //if error while displaying dialog, delete LocAcq object and leave
    if( error )
        {
        delete iLocationServiceAdapter;
        iLocationServiceAdapter = NULL;
        
        User::Leave( error );
        } 
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::PlaceFromCurrentLocationL()" );
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::DisplayNoPositioningMethodInfoNoteL()
// It displays message query if there is no positioning method enabled.
// @param aResourceId - resource id of the wait dialog to be displayed.
// @ret If pressed Open, it will return true.
//      If pressed Close, it will return false.
// -----------------------------------------------------------------------------
//
TBool CEvtMgmtUiPlaceComponent::DisplayNoPositioningMethodInfoNoteL( TInt aResourceId )
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::DisplayNoPositioningMethodInfoNoteL()");
    TBool response = EFalse;
    
    //all strings 
     HBufC* errMsg = StringLoader::LoadLC( R_EVTUI_POSITIONINGERROR, CCoeEnv::Static() );
     HBufC* errMsgLink = StringLoader::LoadLC( R_EVTUI_POSITIONINGERROR_LINK, CCoeEnv::Static() );
     HBufC* errMsgTitle = StringLoader::LoadLC( R_EVTUI_POSITIONINGERROR_HEADER, CCoeEnv::Static() );
     HBufC* errCompleteMsg = HBufC::NewLC( errMsg->Length() + KNewLine().Length() + errMsgLink->Length() );
     errCompleteMsg->Des().Copy(*errMsg);
     errCompleteMsg->Des().Append(KNewLine);
     errCompleteMsg->Des().Append(*errMsgLink);
     
     //Launch dialog     
     CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*errCompleteMsg);
     dlg->PrepareLC( aResourceId );
     dlg->SetLinkTextL(*errMsgLink);
     TCallBack callBack = TCallBack(LinkClickedL, this);
     dlg->SetLink( callBack );
     dlg->QueryHeading()->SetTextL( *errMsgTitle );
     if( dlg->RunLD() ==  EAknSoftkeyOk )
         {
         CleanupStack::PopAndDestroy(4); //errMsgTitle, errMsg, errMsgLink, errCompleteMsg
         
         if( ! iLocationServiceAdapter->AllPositionMethodsDisabledL() ) //if atleast one psy is enabled
             {
             response = ETrue;
             //request current location
             PlaceFromCurrentLocationL();
             }
         else
             response = EFalse;
         }
     else
         {
         response = EFalse;
         CleanupStack::PopAndDestroy(4); //errMsgTitle, errMsg, errMsgLink, errCompleteMsg
         }
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::DisplayNoPositioningMethodInfoNoteL()");
     return response;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::UpdateIconsL
// It update icons
// -----------------------------------------------------------------------------
void CEvtMgmtUiPlaceComponent::UpdateIconsL( CAknListQueryDialog& aDialog )
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::UpdateIconsL()" );
    
    CAknIconArray* icons = new( ELeave ) CAknIconArray( KSetPlaceGranularity );
    CleanupStack::PushL( icons );

    TFileName *iconFile = IconFileNameLC();

    AddIconL( *icons, 
              *iconFile,
              KAknsIIDQgnPropLocevCurrentLocation, 
              EMbmEvteditorQgn_prop_locev_current_location, EMbmEvteditorQgn_prop_locev_current_location_mask);
             
    AddIconL( *icons, 
              *iconFile,
              KAknsIIDQgnPropLocevLandmark, 
              EMbmEvteditorQgn_prop_locev_landmark, EMbmEvteditorQgn_prop_locev_landmark_mask);
       
    AddIconL( *icons, 
              *iconFile,
              KAknsIIDQgnPropLocevMap, 
              EMbmEvteditorQgn_prop_locev_map, EMbmEvteditorQgn_prop_locev_map_mask);

    CleanupStack::PopAndDestroy(iconFile);

    //set new icons to array
    aDialog.SetIconArrayL( icons );  
    CleanupStack::Pop(icons);
    
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::UpdateIconsL()" );
        
    }
    
// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::AddIconL
// Append icon to icon array. CAknIconArray owns icons so it is 
// easier to it leave safely.
// @param aIcons icon array
// @param aIconFileWithPath icon file name with path
// @param aSkinID skin id of the icon
// @param aIconGraphicsIndex picture index
// @param aIconGraphicsMaskIndex mask index
// -----------------------------------------------------------------------------
void CEvtMgmtUiPlaceComponent::AddIconL( CAknIconArray&  aIcons,
                                           const TDesC&    aIconFileWithPath,
                                           TAknsItemID     aSkinID,
                                           TInt            aIconGraphicsIndex,
                                           TInt            aIconGraphicsMaskIndex )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    EVTUIDEBUG( "AknsUtils::SkinInstance" );
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
    TRAPD( err, AknsUtils::CreateIconL( skin,
                            aSkinID,
                            bitmap,
                            bitmapMask,
                            aIconFileWithPath,
                            aIconGraphicsIndex,
                            aIconGraphicsMaskIndex ) );

	
    EVTUIDEBUG1( "AknsUtils::CreateIconL - %d", err );
    if(err != KErrNone)
	    {
	    TRAP( err, AknIconUtils::CreateIconL( bitmap,
	                            bitmapMask,
	                            aIconFileWithPath,
	                            aIconGraphicsIndex,
	                            aIconGraphicsMaskIndex ) );
    	EVTUIDEBUG1( "AknIconUtils::CreateIconL - %d", err );
	    }
    User::LeaveIfError( err );
    
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( bitmapMask );
    
    CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );
    
    // Remove the Bitmap and the Bitmap Icon from the Clean up stack since 
    // the ownership is taken over by the icon
    CleanupStack::Pop( bitmapMask );
    CleanupStack::Pop( bitmap ); 
    
    CleanupStack::PushL( icon );
    aIcons.AppendL( icon );
    CleanupStack::Pop( icon ); // icon
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiPlaceComponent::IconFileNameLC
// Gets Events Management UI icon file path and name. Ownership
// is transferred, item is left in the cleanup stack.
// @return TFileName* The full name of the Events UI icon file.
// -----------------------------------------------------------------------------
TFileName* CEvtMgmtUiPlaceComponent::IconFileNameLC() const
    {
    TFileName* fileName = new(ELeave) TFileName;
    CleanupStack::PushL( fileName );
    
    TFileName* tmpName = new(ELeave) TFileName;
    CleanupStack::PushL( tmpName );
    
    tmpName->Append( KDC_BITMAP_DIR );
    tmpName->Append( KEvtMgmtUiIconFileName );

    CEikAppUi *appUi = ( CEikAppUi *)( CEikonEnv::Static()->AppUi());
    TFileName appDrive = appUi->Application()->AppFullName();
    
    TParse parse;
    User::LeaveIfError( parse.Set( appDrive, NULL, NULL ) );
    User::LeaveIfError( parse.Set( parse.Drive(), tmpName, NULL ) );
    
    fileName->Copy(parse.FullName());

    CleanupStack::PopAndDestroy( tmpName );

    return fileName;
    }    

// ---------------------------------------------------------
// CEvtMgmtUiPlaceComponent::DisplayAccuracyNoteL
// It is used to display accuracy note
// ---------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::DisplayAccuracyNoteL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::DisplayAccuracyNoteL() - Unknown" );
    HBufC* infoMsg = StringLoader::LoadLC(R_LOCEV_NOTE_LIST_ACC_UNKNOWN, CEikonEnv::Static());
    CAknInformationNote* infoNote = new(ELeave) CAknInformationNote( ETrue );
    infoNote->ExecuteLD( *infoMsg  );
    CleanupStack::PopAndDestroy(); //infoMsg  
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::DisplayAccuracyNoteL()" );
    }

// ---------------------------------------------------------
// CEvtMgmtUiPlaceComponent::DisplayAccuracyNoteL
// It is used to display accuracy note
// ---------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::DisplayAccuracyNoteL(TPosition& aPos)
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::DisplayAccuracyNoteL() - TPosition" );
    _LIT(KNewLine, "\n");
    HBufC* acclevel = NULL;
    HBufC* accString = NULL;
    
    // Calculate the Accuracy Level
    TReal accuracy = aPos.HorizontalAccuracy();
    EVTUIDEBUG1("Horizontal Accuracy = %f", accuracy );
    
    if(accuracy < KLowAccuracyMaxLimit )
        acclevel = StringLoader::LoadLC(R_EVTUI_ACCURACYLEVEL_HIGH );
    if(accuracy >=KLowAccuracyMaxLimit && accuracy <= KMediumAccuracyMaxLimit )
        acclevel = StringLoader::LoadLC(R_EVTUI_ACCURACYLEVEL_MEDIUM );
    if(accuracy > KMediumAccuracyMaxLimit )
        acclevel = StringLoader::LoadLC( R_EVTUI_ACCURACYLEVEL_LOW );
               
    // Get the Accuracy String   
    GetAccuracyStringL( accuracy, accString );
    CleanupStack::PushL( accString );
    
    HBufC* infoMsg = HBufC::NewLC( acclevel->Length() + KNewLine().Length() + accString->Length() );
    TPtr ptrMsg = infoMsg->Des();
    ptrMsg.Copy( *acclevel );
    ptrMsg.Append( KNewLine );
    ptrMsg.Append( *accString );
    CAknInformationNote* infoNote = new(ELeave) CAknInformationNote( ETrue );
    infoNote->ExecuteLD( *infoMsg  );
    CleanupStack::PopAndDestroy(3); //infoMsg, accString, acclevel  
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::DisplayAccuracyNoteL()" );
    }


// ---------------------------------------------------------
// CEvtMgmtUiPlaceComponent::ResolveErrorL
// Resolves the Error code
// ---------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::ResolveErrorL( TInt aErrorCode )
    {
    TPtrC buf;
    CTextResolver* textResolver = CTextResolver::NewLC( *(CEikonEnv::Static()) );
    buf.Set(textResolver->ResolveErrorString(aErrorCode));
    	
    CAknErrorNote* dialog = 
	        new(ELeave)CAknErrorNote(ETrue);    
    dialog->SetTone(CAknNoteDialog::EErrorTone);
    dialog->ExecuteLD( buf ); 
    	
    CleanupStack::PopAndDestroy( textResolver );
    }

// ---------------------------------------------------------
// CEvtMgmtUiPlaceComponent::DisplayAccuracyNoteL
// It is used to provide Accuracy String based on current 
// System of Measurement Settings
// ---------------------------------------------------------
//
void CEvtMgmtUiPlaceComponent::GetAccuracyStringL( TReal& aAccuracy, HBufC*& aAccuracyString )
    {
    EVTUIDEBUG("+ CEvtMgmtUiPlaceComponent::GetAccuracyStringL()" );
    TInt settingsValue = 0;
    CRepository* repository = CRepository::NewL( TUid::Uid( KCRUidLocNotationPref) );
    CleanupStack::PushL( repository );
    User::LeaveIfError( repository->Get( KLocSystemofMeasurements,
                                              settingsValue ));  
    CleanupStack::PopAndDestroy( repository );
    
    aAccuracy = aAccuracy/KMToKMConversion;
    
    switch( static_cast<TLocSystemofMeasurementValues>( settingsValue ) )
       {
       case ELocImperial: // mile              
           {                   
           aAccuracy *= KUnitConversion;
           TReal approxAccuracy;
           TBuf<8> accuracyString;
           Math::Round( approxAccuracy, aAccuracy, 2 );
           accuracyString.Format(KFloatAccuracyFormat, approxAccuracy);
           aAccuracyString = StringLoader::LoadL( R_LOCEV_NOTE_TXT_ACC_MILES, accuracyString, CEikonEnv::Static() );
           break;
           }
       case ELocMetric: // metric
           {
           TReal approxAccuracy;
           TBuf<8> accuracyString;
           Math::Round( approxAccuracy, aAccuracy, 2 );
           accuracyString.Format(KFloatAccuracyFormat, approxAccuracy);
           aAccuracyString = StringLoader::LoadL( R_LOCEV_NOTE_TXT_ACC_KM, accuracyString, CEikonEnv::Static() );
           }
           break;           
       default: 
           break;
       }
    EVTUIDEBUG("- CEvtMgmtUiPlaceComponent::GetAccuracyStringL()" );
    }
//end of file
