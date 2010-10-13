/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Provides LM main view class methods.
*
*/







// INCLUDE FILES
#include "CLmkBaseView.h"
#include "CLmkBaseContainer.h"
#include "landmarks.hrh"
#include <aknlists.h>
#include <aknViewAppUi.h>
#include <AknQueryDialog.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <eikmenub.h> // CEikMenuBar
#include <Landmarks.rsg>
#include "clmkmapnavigationinterface.h"
#include <locationvariationdomaincrkeys.h>
#include <centralrepository.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CLmkBaseView::ConstructL
// ---------------------------------------------------------
//
CLmkBaseView::CLmkBaseView( MLmkKeyProcessor& aKeyProcessor )
        : iKeyProcessor( aKeyProcessor )
    {
    }

// ---------------------------------------------------------
// CLmkBaseView::BaseConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CLmkBaseView::BaseConstructL( TInt aResId )
    {
    FeatureManager::InitializeLibL();
    CAknView::BaseConstructL( aResId );
	
    // Check if the Coordinates has to be hidden
	CheckHideCoordinateL();
    }

// ---------------------------------------------------------
// CLmkBaseView::~CLocBaseView()
// ---------------------------------------------------------
//
CLmkBaseView::~CLmkBaseView()
    {
    if ( iBaseContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iBaseContainer );
        }
    delete iBaseContainer;
    FeatureManager::UnInitializeLib();
    if(iMapNavInterface)
    	{
    	iMapNavInterface->Release();
    	}
    }

// ---------------------------------------------------------
// CLmkBaseView::ProcessKeyEventL
// ---------------------------------------------------------
//
TBool CLmkBaseView::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                      TEventCode aType )
    {
    return iKeyProcessor.ProcessKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CLmkBaseView::TabObserver()
// ---------------------------------------------------------
//
MAknTabObserver* CLmkBaseView::TabObserver()
    {
    return iKeyProcessor.TabObserver();
    }

// ---------------------------------------------------------
// CLmkBaseView::HandleHelpFeature
// ---------------------------------------------------------
//
void CLmkBaseView::HandleHelpFeature( CEikMenuPane* aMenuPane ) const
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) &&
            !iIsHideHelp )
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp, EFalse );
        }
    else
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
        }
    }

// ---------------------------------------------------------
// CLmkBaseView::CheckHideCoordinateL()
// ---------------------------------------------------------
//
void CLmkBaseView::CheckHideCoordinateL()
	{
    TInt coordinateSettingValue = ELocCoordinatesNotHidden;
    TInt helpSettingValue = ELocHelpNotHidden;
    CRepository* repository = CRepository::NewL( TUid::Uid( KCRUidLocLocalVariation) );
    CleanupStack::PushL( repository );
    User::LeaveIfError( repository->Get( KLocHideCoordinates,
                                              coordinateSettingValue ));  
    User::LeaveIfError( repository->Get( KLocHideHelp,
                                              helpSettingValue ));  
    CleanupStack::PopAndDestroy( repository );
	
	if( ELocCoordinatesHidden == coordinateSettingValue )
		iIsHideCoordinate = ETrue;
	else
		iIsHideCoordinate = EFalse;
	
	if( ELocHelpHidden == helpSettingValue )
		iIsHideHelp = ETrue;
	else
		iIsHideHelp = EFalse;
	
	}

// ---------------------------------------------------------
// CLmkBaseView::Update()
// ---------------------------------------------------------
//
void CLmkBaseView::Update()
	{
	//base class do not have any implementation logic
	}

// ---------------------------------------------------------
// CLmkBaseView::EnableMskMenuL()
// ---------------------------------------------------------
//
void CLmkBaseView::EnableMskMenuL(TBool /*aEnable*/)
	{
	//base class do not have any implementation logic
	}

// ---------------------------------------------------------
// CLmkBaseView::EnableMskLabelL()
// ---------------------------------------------------------
//
void CLmkBaseView::EnableMskLabelL(TBool /*aEnable*/)
	{
	//base class do not have any implementation logic
	}

// ---------------------------------------------------------
// CLmkBaseView::UpdateMskViewL()
// ---------------------------------------------------------
//
void CLmkBaseView::UpdateMskViewL(TLmkMskDispItem aMskDispItem)
	{
	switch(aMskDispItem)
		{
		case EContextMenu:
			{
			EnableMskMenuL(ETrue);
			break;
			}
		case ELabel:
			{
			EnableMskLabelL(ETrue);
			break;
			}
		case ENoMsk:
			{
			EnableMskMenuL(EFalse);
			EnableMskLabelL(EFalse);
			break;
			}
		default:
		    User::Leave(KErrGeneral);
			break;
		}
	}

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CLmkBaseView::HandleListBoxEventL()
// ---------------------------------------------------------
//
void CLmkBaseView::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent /*aEventType*/ )
	{
	//does nothing
	}
#endif//RD_SCALABLE_UI_V2
// End of File
