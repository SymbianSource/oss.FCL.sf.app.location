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
*       Implements the CLandmarksCategoriesView class
*
*/



#include <aknViewAppUi.h>
#include <eikmenup.h>
#include <AknQueryDialog.h>

#include <EPos_CPosLandmarkCategory.h>

#include "LmRefApp.hrh"
#include <lmrefapp.rsg>
#include "LandmarksCategoriesView.h"
#include "LandmarksCategoriesContainer.h"
#include "LandmarksApplicationEngine.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesView::CLandmarksCategoriesView(
    CLandmarksApplicationEngine* aEngine)
:   iEngine(aEngine)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesView::ConstructL()
    {
    BaseConstructL(R_LMREFAPP_CATEGORIES_VIEW);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesView* CLandmarksCategoriesView::NewLC(
    CLandmarksApplicationEngine* aEngine)
    {
    CLandmarksCategoriesView* self = 
        new (ELeave) CLandmarksCategoriesView(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesView::~CLandmarksCategoriesView()
    {
    delete iContainer;

    // This object is responsible for deleting the application engine instance.
    delete iEngine;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TUid CLandmarksCategoriesView::Id() const
    {
    return TUid::Uid(ELandmarksCategoriesViewId);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesView::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case ELandmarksDelete:
            iContainer->DeleteCategoryL();
            break;

        case ELandmarksRename:
            RenameCategoryL();
            break;

        case ELandmarksAddCategory:
            AddCategoryL();
            break;

        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/)
    {
    if (!iContainer)
        {
        iContainer = new (ELeave) CLandmarksCategoriesContainer(
            *this, *iEngine, *(MenuBar()));
        iContainer->SetMopParent(this);
	    iContainer->ConstructL(ClientRect());
        }

    // Enable receiving of keyboard events.
    AppUi()->AddToStackL(*this, iContainer);

    // Make view visible.
    iContainer->MakeVisible(ETrue);

    // Notify that this view is active.
    TBool isActive = ETrue;
    iEngine->NotifyViewActivated(Id(), isActive);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesView::DoDeactivate()
    {
    // Hide view.
    iContainer->MakeVisible(EFalse);

    // Notify that this view is inactive.
    TBool isActive = EFalse;
    iEngine->NotifyViewActivated(Id(), isActive);

    // Disable receiving keyboard events.
    AppUi()->RemoveFromStack(iContainer);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesView::HandleResourceChange( TInt aType )
    {
    if ( iContainer )
        {
        iContainer->HandleResourceChange( aType );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesView::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane)
    {
    CAknView::DynInitMenuPaneL(aResourceId, aMenuPane);
    if (aResourceId == R_LMREFAPP_CATEGORIES_MENU)
        {
        // If none or a global cat. is selected we need to hide some commands.
        TBool dimmed = 
            !iContainer->IsItemSelected() || iContainer->IsGlobalL();
        aMenuPane->SetItemDimmed(ELandmarksDelete, dimmed);
        aMenuPane->SetItemDimmed(ELandmarksRename, dimmed);
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesView::AddCategoryL()
    {
    // Create and initialize dialog
    TBuf<KPosLmMaxCategoryNameLength> categoryName;
    CAknTextQueryDialog* queryDialog = CAknTextQueryDialog::NewL(categoryName);
    queryDialog->SetMaxLength(KPosLmMaxCategoryNameLength);
    CleanupStack::PushL(queryDialog);

    // Create and set title
    HBufC* title = 
        iCoeEnv->AllocReadResourceAsDes16LC(R_LMREFAPP_NEW_CATEGORY);
    queryDialog->SetPromptL(*title);
    CleanupStack::PopAndDestroy(title);

    // Launch dialog
    TBool dialogDismissed = queryDialog->ExecuteLD(R_LMREFAPP_RENAME_QUERY);
    CleanupStack::Pop(queryDialog);
    if (dialogDismissed)
        {
        // Create and add a new category with user defined name.
        CPosLandmarkCategory* newCategory = CPosLandmarkCategory::NewLC();
        newCategory->SetCategoryNameL(categoryName);
        iEngine->AddCategoryL(*newCategory);
        CleanupStack::PopAndDestroy(newCategory);
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesView::RenameCategoryL()
    {
    // Fetch existing category name
    CPosLandmarkCategory* category = 
        iEngine->CategoryLC(iContainer->CurrentCategoryId());
    TPtrC categoryName;
    category->GetCategoryName(categoryName);
    TBuf<KPosLmMaxCategoryNameLength> newCategoryName;
    newCategoryName.Insert(0, categoryName);

    // Create and initialize dialog
    CAknTextQueryDialog* queryDialog = 
        CAknTextQueryDialog::NewL(newCategoryName);
    queryDialog->SetMaxLength(KPosLmMaxCategoryNameLength);
    CleanupStack::PushL(queryDialog);

    // Create and set title
    HBufC* title = 
        iCoeEnv->AllocReadResourceAsDes16LC(R_LMREFAPP_CATEGORY_NAME);
    queryDialog->SetPromptL(*title);
    CleanupStack::PopAndDestroy(title);

    // Launch dialog
    TBool dialogAccepted = queryDialog->ExecuteLD(R_LMREFAPP_RENAME_QUERY);
    CleanupStack::Pop(queryDialog);
    if (dialogAccepted)
        {
        // Update category name
        category->SetCategoryNameL(newCategoryName);
        iEngine->UpdateCategoryL(*category);
        }

    CleanupStack::PopAndDestroy(category);
    }


