/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CLandmarksInfoContainer class
*
*/



#include <aknsfld.h>
#include <aknnavi.h>
#include <aknlists.h> 
#include <eikclbd.h>
#include <akntitle.h>
#include <aknview.h>
#include <aknnavide.h>

#include <lbspositioninfo.h>

#include <lmrefapp.rsg>
#include "LmRefApp.hrh"
#include "LandmarksCommonData.h"
#include "LandmarksInfoContainer.h"
#include "LandmarksInfoModel.h"
#include "LandmarksEditDialog.h"
#include "LandmarksApplicationEngine.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksInfoContainer::CLandmarksInfoContainer(
    CAknView& aView,
    CLandmarksApplicationEngine& aEngine,
    CAknNavigationDecorator* aNaviDecorator)
:   iView(aView),
    iEngine(aEngine), 
    iNaviDecorator(aNaviDecorator)
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    CreateListBoxL();

    // Create data model.
    iModel = new (ELeave) CLandmarksInfoModel(iEngine);

    SetRect(aRect);
    ActivateL();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksInfoContainer::~CLandmarksInfoContainer()
    {
    iFields.Close();
    delete iListBox;
	delete iLandmark;
    delete iModel;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TInt CLandmarksInfoContainer::CountComponentControls() const
    {
    TInt count = 0;
    if (iListBox)
        {
        count++;
        }

    return count; // Return the number of controls inside this container.
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CCoeControl* CLandmarksInfoContainer::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            return iListBox; // Returns the pointer to listbox object.
        default:
            return NULL; // Exception : Returns NULL.
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TKeyResponse CLandmarksInfoContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
	TKeyResponse response = EKeyWasNotConsumed;

    switch (aKeyEvent.iCode)
        {
        case EKeyUpArrow:
        case EKeyDownArrow:
            // Scrolling the listbox. Forward to iListBox.
            response = iListBox->OfferKeyEventL(aKeyEvent, aType);
            break;

        case EKeyLeftArrow:
        case EKeyRightArrow:
            // Eat these to disable switching views
            response = EKeyWasConsumed;
            break;

        case EKeyDevice3:
            EditLandmarkL();
            response = EKeyWasConsumed;
            break;
            
        default:
            // menu keys should be handled
            break;
        }
        
	return response;
    }
    
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::StoreTitlePaneL()
    {
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    iTitlePane = (CAknTitlePane*) statusPane->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle));

    iOriginalTitle = iTitlePane->Text()->AllocL();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::RestoreTitlePane()
    {
    if (iTitlePane && iOriginalTitle)
        {
        iTitlePane->SetText(iOriginalTitle); // ownership is transferred
        }
    else
        {
        delete iOriginalTitle;
        }
    iOriginalTitle = NULL;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::CreateListBoxL()
    {
    // Create a control to display a list of descriptors
    iListBox = new (ELeave) CAknSingleHeadingStyleListBox();
    iListBox->SetContainerWindowL(*this);
    iListBox->ConstructL(this, 0);

    // Create scrollbars
    CEikScrollBarFrame* scrollBar = iListBox->CreateScrollBarFrameL();
    scrollBar->SetScrollBarVisibilityL(
		CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::SizeChanged()
    {
	// Set list box size.
	iListBox->SetRect(Rect());
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect(iView.ClientRect());
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::LoadLandmarkL(TPosLmItemId aLandmarkId)
    {
	delete iLandmark;
	iLandmark = NULL;

    iLandmark = iEngine.LandmarkLC(aLandmarkId);
    CleanupStack::Pop(iLandmark);

    UpdateL();
    iListBox->View()->SetCurrentItemIndex(0);
    }
    
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::EditLandmarkL()
    {
    // Create editor
    TBool isEdited = EFalse;

    TInt selectedLine = iListBox->View()->CurrentItemIndex();
    TInt activeField = 0;
    if (selectedLine >= 0 && selectedLine < iFields.Count())
        {
        activeField = iFields[selectedLine];
        }
    
    CLandmarksEditDialog* editDialog = CLandmarksEditDialog::NewLC(
        *iLandmark, EFalse, isEdited, iEngine, activeField);

    TInt err = editDialog->ExecuteLD(R_LMREFAPP_EDIT_DIALOG);
    CleanupStack::Pop(editDialog);
    User::LeaveIfError(err);

    UpdateL();

    // Commit if edited
    if (isEdited)
        {
        iEngine.CommitLandmarkL(*iLandmark);
    
        // move to first field
        iListBox->View()->SetCurrentItemIndex(0);
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::UpdateL()
    {
    CDesCArray* infoArray = iModel->GetLandmarkInfoL(*iLandmark, iFields);

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(infoArray);
    model->SetOwnershipType(ELbmOwnsItemArray);
    iListBox->HandleItemAdditionL();

    UpdateTitleBarL();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksInfoContainer::UpdateTitleBarL()
    {
    TPtrC ptr;
    if (iLandmark)
        {
        iLandmark->GetLandmarkName(ptr);
        iTitlePane->SetTextL(ptr);
        }
    else 
        {
        iTitlePane->SetTextL(*iOriginalTitle);
        }
    }


