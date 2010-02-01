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
* Description:  Applications interface class to map and navigation use cases
*
*/


// INCLUDE FILES
#include <epos_poslandmarkserialization.h>
#include <EPos_CPosLandmark.h>
#include <AiwGenericParam.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <evtmgmtui.rsg>
#include <AiwServiceHandler.h>
#include <AiwCommon.h> 
#include <mnaiwservices.h>
#include <mnmapview.h>
#include <mnprovider.h>
#include <mnproviderfinder.h>

#include "evtdebug.h"
#include "evtmgmtuimapnavigationadapter.h"

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
	}

// -----------------------------------------------------------------------------
// CEvtMgmtUiMapNavigationAdapter::ShowOnMapL
// It will show location on map
// -----------------------------------------------------------------------------
void CEvtMgmtUiMapNavigationAdapter::ShowOnMapL( const TCoordinate& aCoordinate )
    {
    CPosLandmark* lmk = CPosLandmark::NewLC();
    TLocality locality;
    HBufC8* lmBuf;
    
    //set location
    locality.SetCoordinate( aCoordinate.Latitude(), aCoordinate.Longitude(), aCoordinate.Altitude() );
 
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
void CEvtMgmtUiMapNavigationAdapter::NavigateToPlaceL(  const TCoordinate& aCoordinate )
    {
    CPosLandmark* lmk = CPosLandmark::NewLC();
    TLocality locality;
    HBufC8* lmBuf;
    
    //set location
    locality.SetCoordinate( aCoordinate.Latitude(), aCoordinate.Longitude(), aCoordinate.Altitude() );
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
    return KErrNone;
    }
// End of file
