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
* Description:   Applications interface class to map and navigation use cases
*
*/

// INCLUDE FILES
#include <AiwGenericParam.h>
#include <epos_poslandmarkserialization.h>
#include <lbsposition.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <evtmgmteditorui.rsg>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmOperation.h>
#include <EPos_CPosLandmark.h>
#include <EPos_Landmarks.h>
#include <e32math.h>
#include <TLmkItemIdDbCombiInfo.h> 
#include <AiwServiceHandler.h>
#include <AiwCommon.h> 
#include <mnaiwservices.h>
#include <mnmapview.h>
#include <mnprovider.h>
#include <mnproviderfinder.h>

#include "evtdebug.h"
#include "evtmgmtuimapnavigationadapter.h"
#include "evteditorconsts.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::CLmkMapNavigationInterface
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CEvtMgmtUiMapNavigationAdapter::CEvtMgmtUiMapNavigationAdapter()
	{	
	}

// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CEvtMgmtUiMapNavigationAdapter::ConstructL()
	{
	iAiwServiceHandler = CAiwServiceHandler::NewL();
	iInList = CAiwGenericParamList::NewL();
    iOutList = CAiwGenericParamList::NewL();  
	}

// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::NewL
// Symbian 1st phase constructor can leave.
// -----------------------------------------------------------------------------
CEvtMgmtUiMapNavigationAdapter* CEvtMgmtUiMapNavigationAdapter::NewL()
	{	
	CEvtMgmtUiMapNavigationAdapter* self = new (ELeave) CEvtMgmtUiMapNavigationAdapter();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
	}
	
// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::NewLC
// Symbian 1st phase constructor can leave.
// -----------------------------------------------------------------------------
CEvtMgmtUiMapNavigationAdapter* CEvtMgmtUiMapNavigationAdapter::NewLC()
    {   
    CEvtMgmtUiMapNavigationAdapter* self = new (ELeave) CEvtMgmtUiMapNavigationAdapter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();//self
    return self;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::~CEvtMgmtUiMapNavigationAdapter
// C++ Destructor
// -----------------------------------------------------------------------------
 CEvtMgmtUiMapNavigationAdapter::~CEvtMgmtUiMapNavigationAdapter()
	{	
	delete iAiwServiceHandler;    
	iAiwServiceHandler = NULL;
    delete iInList;
    iInList = NULL;
    delete iOutList;   
    iOutList = NULL;
    if(iPlace)
        {
        delete iPlace;
        iPlace = NULL;
        }
	}
 
 // -----------------------------------------------------------------------------
 // CEvtMgmtUiMapNavigationAdapter::SetObserver
 // It will set observer to notify completion of map operation
 // -----------------------------------------------------------------------------
 void CEvtMgmtUiMapNavigationAdapter::SetObserver( MEvtMgmtUiMapAndNavigationObserver* aObserver )
     {
     iObserver = aObserver;
     } 
 
 // -----------------------------------------------------------------------------
 // CEvtMgmtUiMapNavigationAdapter::SetPlaceL
 // It will set place.
 // @param [in] aPlace - place of existing event
 //                    - empty for newly created event
 // ----------------------------------------------------------------------------- 
 void CEvtMgmtUiMapNavigationAdapter::SetPlaceL( const TPtrC& aPlace )
     {
     iPlace = aPlace.AllocL();
     }
 
 // -----------------------------------------------------------------------------
 // CEvtMgmtUiMapNavigationAdapter::SetLocation
 // It will set location.
 // param[in] aTriggerLocation - location of trigger of existing event
 //                            - NaN for newly created event
 // -----------------------------------------------------------------------------
 void CEvtMgmtUiMapNavigationAdapter::SetLocation( TCoordinate  aTriggerLocation )
     {
     iTriggerLocation = aTriggerLocation;
     }

 // -----------------------------------------------------------------------------
 // CEvtMgmtUiMapNavigationAdapter::Place()
 // It will return place value recieved from map.
 // @ret selected place
 // -----------------------------------------------------------------------------
 TPtrC CEvtMgmtUiMapNavigationAdapter::Place() const
     {
     //if place, it is set otherwise empty string
     if(iPlace)
         return *iPlace;
     else
         return KNullDesC();
     }

// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::PlaceFromMapL
// This method is called for the get landmarks from map use case.
// -----------------------------------------------------------------------------
    void CEvtMgmtUiMapNavigationAdapter::PlaceFromMapL()
	{
	// This string will be sent to the provider to display it in the map view
    HBufC* buf = StringLoader::LoadLC( R_EVTUI_CREATENEW_FORM_PLACEPOPUP_TITLE , CEikonEnv::Static() );    
	TAiwGenericParam param( EGenericParamRequestText, TAiwVariant( *buf ) );
    iInList->Reset();
    iInList->AppendL( param );
    iOutList->Reset(); 
    iAiwServiceHandler->AttachL( R_EVTMGMTUI_AIW_INTEREST_SELECTFROMMAP );
	iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdMnSelectFromMap, *iInList, *iOutList, 0, this );	
	CleanupStack::PopAndDestroy( buf ); 
	}

// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::ShowOnMapL
// It will show location on map
// -----------------------------------------------------------------------------
void CEvtMgmtUiMapNavigationAdapter::ShowOnMapL()
    {
    CPosLandmark* lmk = CPosLandmark::NewLC();
    TLocality locality;
    HBufC8* lmBuf;
    
    //set location
    locality.SetCoordinate( iTriggerLocation.Latitude(), iTriggerLocation.Longitude(), iTriggerLocation.Altitude() );
 
    lmk->SetPositionL(locality);
    
    //set  in parameters
    iInList->Reset();
    lmBuf =PosLandmarkSerialization::PackL( *lmk );
    CleanupStack::PushL( lmBuf );
    TAiwGenericParam param( EGenericParamLandmark, TAiwVariant( *lmBuf ) );     
    iInList->AppendL( param );       
    TMnAiwCommonOptionsParam options;
    options.iRunChained = EFalse;
    TPckg<TMnAiwCommonOptionsParam> optionsPack( options );
    TAiwGenericParam optParam( EGenericParamMnCommonOptions, TAiwVariant( optionsPack ) );
    iInList->AppendL( optParam );
            
    //show location on map
    iAiwServiceHandler->AttachL( R_EVTMGMTUI_AIW_INTEREST_SHOWONMAP );
    iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdMnShowMap, *iInList, *iOutList, 0, this ); 
    
    CleanupStack::PopAndDestroy( 2 ); //lmbuf, lmk
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::NavigateToPlaceL
// It will navigate to location
// -----------------------------------------------------------------------------
void CEvtMgmtUiMapNavigationAdapter::NavigateToPlaceL()
    {
    CPosLandmark* lmk = CPosLandmark::NewLC();
    TLocality locality;
    HBufC8* lmBuf;
    
    //set location
    locality.SetCoordinate( iTriggerLocation.Latitude(), iTriggerLocation.Longitude(), iTriggerLocation.Altitude() );
    lmk->SetPositionL(locality);
    
    //set in parameters
    iInList->Reset();
    lmBuf =PosLandmarkSerialization::PackL( *lmk );
    CleanupStack::PushL( lmBuf );
    TAiwGenericParam param( EGenericParamLandmark, TAiwVariant( *lmBuf ) );     
    iInList->AppendL( param );       
    TMnAiwCommonOptionsParam options;
    options.iRunChained = EFalse;
    TPckg<TMnAiwCommonOptionsParam> optionsPack( options );
    TAiwGenericParam optParam( EGenericParamMnCommonOptions, TAiwVariant( optionsPack ) );
    iInList->AppendL( optParam );
        
    //navigate to location
    iAiwServiceHandler->AttachL( R_EVTMGMTUI_AIW_INTEREST_NAVIGATETO );
    iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdMnNavigateTo, *iInList, *iOutList, 0, this ); 
    
    CleanupStack::PopAndDestroy( 2 ); //lmbuf, lmk
    }
        
        
// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::MapServiceProvidersL
// It will return total map service providers.
// @ret total number of map service providers
// -----------------------------------------------------------------------------
//
TInt CEvtMgmtUiMapNavigationAdapter::MapServiceProvidersL()
    {
    RPointerArray<CMnProvider> providers;
    // get all providers
    MnProviderFinder::FindProvidersL( providers );
    TInt total = providers.Count();
    providers.ResetAndDestroy();
    providers.Close();
    Debug(_L("Total map service providers = %d"), total);
    return total;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::HandleNotifyL
// Derived from MAiwNotifyCallback
// -----------------------------------------------------------------------------
 TInt CEvtMgmtUiMapNavigationAdapter::HandleNotifyL( TInt /*aCmdId*/, 
                                                        TInt /*aEventId*/, 
                                                        CAiwGenericParamList& /*aEventParamList*/,
                                                        const CAiwGenericParamList& /*aInParamList*/ )
    {
    if ( iOutList->Count() ) //if location is selected on map
        {
        const TAiwGenericParam& param = ( *iOutList )[0];
        if ( EGenericParamLandmark == param.SemanticId() )
            {
            TPtrC8 landmarkData( param.Value().AsData() );
            CPosLandmark* lm = PosLandmarkSerialization::UnpackL( landmarkData );
            CleanupStack::PushL( lm );           
            
            SetPlaceFromLandmarkL( *lm );
            CleanupStack::PopAndDestroy( lm );
            iOutList->Reset(); 
            iObserver->HandleMapAndNavigationResponseL( KErrNone, *iPlace, iTriggerLocation );
            }
        else
            {
            iOutList->Reset(); 
            iObserver->HandleMapAndNavigationResponseL( KErrCancel, *iPlace, iTriggerLocation );             
            }         
        }
    else //if map application is cancelled
        iObserver->HandleMapAndNavigationResponseL( KErrCancel, *iPlace, iTriggerLocation );
    return KErrNone;
    }
 
 // -----------------------------------------------------------------------------
 // CEvtMgmtUiMapNavigationAdapter::PlaceFromLandmarkL
 // It will set place details from landmark recieved from map
 // @param[in] aLandmark - landmark selected on map 
 // -----------------------------------------------------------------------------
 void CEvtMgmtUiMapNavigationAdapter::SetPlaceFromLandmarkL( CPosLandmark& aLandmark )
     {            
    _LIT(KCommaSeparator,", ");
    
    //retrieve lat long info
    TLocality locality;
    aLandmark.GetPosition( locality );
    iTriggerLocation = locality;
    
    //retrive position description
    delete iPlace;
    iPlace = NULL;
    iPlace = HBufC::NewL(KPlaceMaxLength);       
    TPtr placePtr = iPlace->Des();
    
    TPtrC ptr(KNullDesC);
    
    //Landmark Name
    if( aLandmark.GetLandmarkName(ptr) == KErrNone && ptr.Compare(KNullDesC)!=0 )
        {                      
        placePtr.Copy(ptr);           
        placePtr.Append(_L(" "));            
        }
             
    //retrieve street
    if(aLandmark.GetPositionField(EPositionFieldStreet,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
       {           
       placePtr.Append(ptr);
       placePtr.Append(KCommaSeparator);
       }
    //retrieve city
    if(aLandmark.GetPositionField(EPositionFieldCity,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
      { 
      placePtr.Append(ptr);
      placePtr.Append(KCommaSeparator);
      }  
    //retrieve state
    if(aLandmark.GetPositionField(EPositionFieldState,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
      {           
      placePtr.Append(ptr);
      placePtr.Append(KCommaSeparator);
      }
    //retrieve county
    if(aLandmark.GetPositionField(EPositionFieldCounty,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
      { 
      placePtr.Append(KCommaSeparator);
      placePtr.Append(ptr);
      }
    //retrieve country
    if(aLandmark.GetPositionField(EPositionFieldCountry,ptr)!=KErrNotFound && ptr.Compare(KNullDesC)!=0)
      {           
      placePtr.Append(ptr);
      placePtr.Append(KCommaSeparator);
      }      
       
    // if address is not present, set place to timestamp
    if( placePtr.Compare( KNullDesC ) != 0 )
        {
        TInt commaDesLen = KCommaSeparator().Length();
        if(placePtr.Mid(placePtr.Length()-commaDesLen,commaDesLen).Compare(KCommaSeparator)==0)
            placePtr.Delete(placePtr.Length()-commaDesLen, commaDesLen);
        else
            placePtr.Delete(placePtr.Length()-1,1); 
        }
    }
// End of file
