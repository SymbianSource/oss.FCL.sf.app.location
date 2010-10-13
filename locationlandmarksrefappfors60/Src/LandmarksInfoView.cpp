/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*       Implements the CLandmarksInfoView class
*
*/


#include <eikmenup.h>
#include <aknViewAppUi.h>
#include <aknnavide.h>

#include <lbspositioninfo.h>
#include <EPos_Landmarks.h>

#include <LmRefApp.rsg>
#include "LmRefApp.hrh"

#include "LandmarksAppUi.h"
#include "LandmarksUtils.h"
#include "LandmarksInfoView.h"
#include "LandmarksApplicationEngine.h"
#include "LandmarksInfoContainer.h"
#include "LandmarksEditDialog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksInfoView::CLandmarksInfoView(
    CAknNavigationDecorator* aNaviDecorator,
    CLandmarksApplicationEngine& aEngine)
:   iEngine(aEngine), 
    iNaviDecorator(aNaviDecorator)
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoView::ConstructL()
    {
    BaseConstructL(R_LMREFAPP_INFO_VIEW);
    iAppName = iCoeEnv->AllocReadResourceAsDes16L(R_LMREFAPP_APP_TITLE);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksInfoView* CLandmarksInfoView::NewLC(
    CAknNavigationDecorator* aNaviDecorator,
    CLandmarksApplicationEngine& aEngine)
    {
    CLandmarksInfoView* self = new (ELeave) CLandmarksInfoView(
        aNaviDecorator, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksInfoView::~CLandmarksInfoView()
    {
    delete iContainer;
    delete iAppName;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TUid CLandmarksInfoView::Id() const
    {
    return TUid::Uid(ELandmarksInfoViewId);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoView::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EAknSoftkeyEdit:
            iContainer->EditLandmarkL();
            break;

        case EAknSoftkeyBack:
            AppUi()->ActivateLocalViewL(TUid::Uid(ELandmarksViewId));
            break;

        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage)
    {
    __ASSERT_ALWAYS(aCustomMessageId.iUid == ELandmarksViewInfoById,
                    LandmarksUtils::Panic(KErrGeneral));
    
    if (!iContainer)
        {
        iContainer = new (ELeave) CLandmarksInfoContainer(
            *this, iEngine, iNaviDecorator);
        iContainer->SetMopParent(this);
	    iContainer->ConstructL(ClientRect());
        }

    iContainer->StoreTitlePaneL();

    // Enable receiving of keyboard events.
    AppUi()->AddToStackL(*this, iContainer);

    // custom message contains landmark id
    TPckgBuf<TPosLmItemId> lmid;
    lmid.Copy(aCustomMessage);
    iContainer->LoadLandmarkL(lmid());
    
    // Make view visible.
    iNaviDecorator->MakeVisible(EFalse);
    iContainer->MakeVisible(ETrue);

    // Notify that this view is active
    iEngine.NotifyViewActivated(Id(), ETrue);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoView::DoDeactivate()
    {
    // Hide view.
    iContainer->MakeVisible(EFalse);
    iNaviDecorator->MakeVisible(ETrue);
    iContainer->RestoreTitlePane();

    // Notify that this view is inactive
    TBool isActive = EFalse;
    iEngine.NotifyViewActivated(Id(), isActive);

    // Disable receiving keyboard events.
    AppUi()->RemoveFromStack(iContainer);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoView::HandleResourceChange( TInt aType )
    {
    if ( iContainer )
        {
        iContainer->HandleResourceChange( aType );
        }
    }


