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
* Description:  Implements the CLandmarksAppUi class
*
*/



#include <barsread2.h>

#include <eikprogi.h>
#include <AknProgressDialog.h>
#include <akntabgrp.h>
#include <aknnavide.h>

#include "LmRefApp.hrh"
#include <lmrefapp.rsg>
#include "LandmarksAppUi.h"
#include "LandmarksView.h"
#include "LandmarksCategoriesView.h"
#include "LandmarksInfoView.h"
#include "LandmarksApplicationEngine.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksAppUi::CLandmarksAppUi()
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin);

    // Fetch pointer to the status pane.
    CEikStatusPane* statusPane = StatusPane();

    // Fetch pointer to the default navi pane control.
    CAknNavigationControlContainer* naviPane = 
        (CAknNavigationControlContainer*) statusPane->ControlL(
            TUid::Uid(EEikStatusPaneUidNavi));

    // Initialise the nave decorator that visualizes the tabs and navi arrows.
    iNaviDecorator = naviPane->ResourceDecorator();
    CAknTabGroup* tabGroup =
        static_cast<CAknTabGroup*>(iNaviDecorator->DecoratedControl());
    tabGroup->SetObserver(this);

    // Create and init the application engine.
    iEngine = CLandmarksApplicationEngine::NewL();

    // Initialize the default landmark database if necessary.
    if (iEngine->StartInitializeDbIfNecessaryL(this))
        {
        LaunchProgressDialogL();
        }
    else
        {
        // Db is already initialized - OK to construct views.
        PostConstructL();
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksAppUi::~CLandmarksAppUi()
    {
    delete iNaviDecorator;

    // Cannot delete iEngine here since it is needed when deactivating views.
    iEngine = NULL;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            if (iProgressDialog)
                {
                // The progress dialog has been dismissed but not yet deleted.
                // iProgressDialog->ProcessFinishedL() deletes the progress 
                // dialog asynchronously. If the app is exited before the 
                // dialog is deleted, a Cone 8 panic will occur.
                // Thus, iProgressDialog must be deleted explicitely here.
                delete iProgressDialog;
                iProgressDialog = NULL;
                }
            Exit();
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TKeyResponse CLandmarksAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    // Offer arrow keys to CAknTabGroup, it then calculates new active tab
    // and later calls our TabChangedL() -method with correct new tab index.

    CAknTabGroup* tabGroup = 
        static_cast <CAknTabGroup*> (iNaviDecorator->DecoratedControl());

    if (tabGroup &&
        (aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyRightArrow))
        {
        return tabGroup->OfferKeyEventL(aKeyEvent, aType);
        }

    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CLandmarksAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    
    CLandmarksView* lmView = ( CLandmarksView* ) View( TUid::Uid( ELandmarksViewId ) );
    if ( lmView )
        {
        lmView->HandleResourceChange( aType );
        }

    CLandmarksCategoriesView* catView = ( CLandmarksCategoriesView* ) View( TUid::Uid( ELandmarksCategoriesViewId ) );
    if ( catView )
        {
        catView->HandleResourceChange( aType );
        }

    CLandmarksInfoView* infoView = ( CLandmarksInfoView* ) View( TUid::Uid( ELandmarksInfoViewId ) );
    if ( infoView )
        {
        infoView->HandleResourceChange( aType );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksAppUi::TabChangedL(TInt aIndex)
    {
    switch (aIndex)
        {
        case 0:
            ActivateLocalViewL(TUid::Uid(ELandmarksViewId));
            break;
        case 1:
            ActivateLocalViewL(TUid::Uid(ELandmarksCategoriesViewId));
            break;
        default:
            break;    
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksAppUi::NotifyOperationProgressL(
    TOperation aOperation, 
    TInt aProgress, 
    TInt aErrorCode)
    {
    if (aOperation == EInitializeDb)
        {
        // Update Progressbar
        CEikProgressInfo* progressBar = iProgressDialog->GetProgressInfoL();
        progressBar->SetAndDraw(aProgress);

        if (aErrorCode == KErrNone) // Db initialization ready.
            {
            // Dismiss dialog. 
            iProgressDialog->ProcessFinishedL();
            
            // Continue with construction
            PostConstructL();
            }
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksAppUi::PostConstructL()
    {
    // Create landmarks view
    CLandmarksView* landmarksView = 
        CLandmarksView::NewLC(iNaviDecorator, *iEngine);
    AddViewL(landmarksView); // Transfer ownership to base class 
    CleanupStack::Pop(landmarksView);

    // Create categories view
    CLandmarksCategoriesView* categoriesView = 
        CLandmarksCategoriesView::NewLC(iEngine); // ownership is transferred
    AddViewL(categoriesView); // Transfer ownership to base class 
    CleanupStack::Pop(categoriesView);

    // Create landmark info view
    CLandmarksInfoView* infoView = 
        CLandmarksInfoView::NewLC(iNaviDecorator, *iEngine);
    AddViewL(infoView); // Transfer ownership to base class 
    CleanupStack::Pop(infoView);

    // Activate landmarks view
    landmarksView->ActivateViewL(landmarksView->ViewId());
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksAppUi::DialogDismissedL(TInt /*aButtonId*/)
    {
    iProgressDialog = NULL;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksAppUi::LaunchProgressDialogL()
    {
    // Initialize dialog
    TBool showImmediately(ETrue);
    iProgressDialog = new (ELeave) CAknProgressDialog(
        reinterpret_cast <CEikDialog**> (&iProgressDialog), showImmediately);
    iProgressDialog->PrepareLC(R_LMREFAPP_INIT_DB_PROGRESS_DIALOG);
    iProgressDialog->SetCallback(this);

    // Initialize progress bar.
    CEikProgressInfo* progressBar = iProgressDialog->GetProgressInfoL();
    const TInt KFinalProgressValue = 100;  // 100% done when ready
    progressBar->SetFinalValue(KFinalProgressValue);

    // Launch the prepared dialog.
    iProgressDialog->RunLD();

    // Draw the progress bar
    progressBar->IncrementAndDraw(0);
    }

