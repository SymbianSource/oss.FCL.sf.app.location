/*
 * Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Icon Selector Dialog Class Implementation
 *
 */

// INCLUDE FILES
#include <uikon.hrh>
#include <e32std.h>
#include <eikcapc.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknborders.h>
#include <aknconsts.h>
#include <AknUtils.h>
#include <aknPopupHeadingPane.h>
#include <AknSettingCache.h>
#include <AknLayout.lag>
#include <e32property.h>

#include <lmkui.rsg>

#include "landmarks.hrh"
#include "lmkicondialog.h"
#include "lmkiconmap.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CLmkIconMapDialog class
// CLmkIconMapDialog() - constructor
// ----------------------------------------------------------------------------
//
CLmkIconMapDialog::CLmkIconMapDialog(TInt& aIconIndex) :
    iIconIndex(&aIconIndex)
    {
    CEikDialog::SetBorder(AknBorderId::EAknBorderNotePopup);
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::~CLmkIconMapDialog()
// -----------------------------------------------------------------------------
//
CLmkIconMapDialog::~CLmkIconMapDialog()
    {
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::HandleResourceChange()
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::HandleResourceChange(TInt aType)
    {
    CAknDialog::HandleResourceChange(aType);
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::OkToExitL()
// -----------------------------------------------------------------------------
//
TBool CLmkIconMapDialog::OkToExitL(TInt aButtonId)
    {
    CLmkIconMap* iconmapControl =
            STATIC_CAST(CLmkIconMap*, Control(ELmkIconMapContentId));
    // Selection key / select softkey adds a iconacter only if 5-key was not used
    // to select multiple icons before.
    if (aButtonId == EAknSoftkeyOk || aButtonId == EAknSoftkeySelect
            || aButtonId == EKeyEnter || aButtonId == EStdKeyEnter)
        {
        TKeyEvent key;
        key.iCode = EKeyOK;
        key.iModifiers = 0;
        iconmapControl->OfferKeyEventL(key, EEventKey);
        }
    return (ETrue);
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::PreLayoutDynInitL()
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::PreLayoutDynInitL()
    {
    CLmkIconMap* iconmapControl =
            STATIC_CAST(CLmkIconMap*, Control(ELmkIconMapContentId));
    CAknPopupHeadingPane
            * headingPane =
                    STATIC_CAST(CAknPopupHeadingPane*, Control(EAknSCTQueryHeadingId));

    iconmapControl->SetIndex(*iIconIndex);

    SetLineNonFocusing(EAknSCTQueryHeadingId);

    // add title

    HBufC* title = iEikonEnv->AllocReadResourceL(R_LMK_ICON_SELECTOR_TITLE);
    CleanupStack::PushL(title);
    headingPane->SetTextL(*title);
    CleanupStack::PopAndDestroy(title); //title
    title = NULL;
#ifdef RD_SCALABLE_UI_V2
    iconmapControl->SetObserver(this);
#endif //RD_SCALABLE_UI_V2
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::PostLayoutDynInitL()
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::PostLayoutDynInitL()
    {
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->SetCommandSetL(R_LMK_ICON_SOFTKEYS_SELECT_CANCEL_SELECT);
    cba->MakeCommandVisibleByPosition(
            CEikButtonGroupContainer::ELeftSoftkeyPosition, EFalse);
    cba->DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::SetSizeAndPosition()
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::SetSizeAndPosition(const TSize& aSize)
    {
    SetBorder(TGulBorder::ENone); // No Dialog borders in SCT.
    CAknDialog::SetSizeAndPosition(aSize);
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::OfferKeyEventL()
// -----------------------------------------------------------------------------
//
TKeyResponse CLmkIconMapDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aModifiers)
    {
    if (aModifiers == EEventKey)
        {
        TUint code = aKeyEvent.iCode;
        CLmkIconMap* iconmapControl =
                STATIC_CAST(CLmkIconMap*, Control(ELmkIconMapContentId));

        switch (code)
            {
            case EKeyLeftArrow:
            case EKeyRightArrow:
            case EKeyUpArrow:
            case EKeyDownArrow:
                {
                TKeyResponse res(iconmapControl->OfferKeyEventL(aKeyEvent,
                        aModifiers));
                return res;
                }
            case EKeyEnter:
                // change fro EKeyEnter to EKeyOK
                {
                TKeyEvent keyEvent;
                keyEvent.iCode = EKeyOK;
                keyEvent.iScanCode = aKeyEvent.iScanCode;
                keyEvent.iModifiers = aKeyEvent.iModifiers;
                keyEvent.iRepeats = aKeyEvent.iRepeats;
                return CEikDialog::OfferKeyEventL(keyEvent, aModifiers);
                }
            default:
                break;
            }
        }
    return CEikDialog::OfferKeyEventL(aKeyEvent, aModifiers);
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::ExecuteLD()
// -----------------------------------------------------------------------------
//
TInt CLmkIconMapDialog::ExecuteLD(TInt aResourceId)
    {
    // Prepare real icon map
    CEikDialog::PrepareLC(aResourceId);
    return (RunLD());
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::CEikDialog_Reserved_1()
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::CEikDialog_Reserved_1()
    {
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::CEikDialog_Reserved_2()
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::CEikDialog_Reserved_2()
    {
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::CLmkIconMapDialog_Reserved()
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::CAknIconMapDialog_Reserved()
    {
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::DoLayout()
// Doing all the Layout calculations
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::DoLayout()
    {
    /*
     TAknLayoutRect dialogLayRect;

     CLmkIconMap* iconmapControl =
     STATIC_CAST(CLmkIconMap*, Control(ELmkIconMapContentId));

     // Number of icon rows in the icon table
     TInt rows = iconmapControl->HeightInRows();

     // Main pane without softkeys
     TRect mainPaneRect;
     if(!AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect))
     {
     mainPaneRect = iAvkonAppUi->ClientRect();
     }

     // Layout the dialog size
     dialogLayRect.LayoutRect( mainPaneRect,AknLayoutScalable_Avkon::popup_grid_graphic_window(3));

     // Set the retangle for dialog and eventually to CLmkIconMap
     SetRect(dialogLayRect.Rect());

     // Need to update popup heading's skin IDs to zero to draw background

     // Offscreen background for picto table.
     iconmapControl->DrawOffscreenBackgroundIfRequired();
     */
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::CreateCustomControlL()
// Virtual function override from the base class to implement the custom control
// -----------------------------------------------------------------------------
//
SEikControlInfo CLmkIconMapDialog::CreateCustomControlL(TInt aControlType)
    {
    CLmkIconMap *control = NULL;
    if (aControlType == ELmkCtIconMap)
        {
        control = CLmkIconMap::NewL();
        }
    SEikControlInfo info;
    info.iControl = control;
    info.iFlags = 0;
    info.iTrailerTextId = 0;
    return info;
    }

#ifdef RD_SCALABLE_UI_V2
// -----------------------------------------------------------------------------
// CLmkIconMapDialog::HandleControlEventL
// Handles the event.
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::HandleControlEventL(CCoeControl* /*aControl*/,
        TCoeEvent aEventType)
    {
    if (aEventType == EEventStateChanged)
        {
        iConSelected = ETrue;
        TryExitL(EAknSoftkeyOk);
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMapDialog::HandleDialogPageEventL
// Handles the event.
// -----------------------------------------------------------------------------
//
void CLmkIconMapDialog::HandleDialogPageEventL(TInt aEventID)
    {
    if (AknLayoutUtils::PenEnabled() && (aEventID
            == MEikDialogPageObserver::EDialogPageTapped && iConSelected))
        {
        TryExitL(EAknSoftkeyOk);
        }
    }

//--------------------------------------------------------------------------
// void CLmkIconMapDialog::HandlePointerEventL()
//--------------------------------------------------------------------------
void CLmkIconMapDialog::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
    {
    if (AknLayoutUtils::PenEnabled())
        {
        if (aPointerEvent.iPosition.iY <= 0 && aPointerEvent.iType
                == TPointerEvent::EButton1Down)
            {
            TryExitL(EAknSoftkeyCancel);
            }
        else
            {
            CLmkIconMap* iconmapControl =
                    STATIC_CAST(CLmkIconMap*, Control(ELmkIconMapContentId));
            iconmapControl->HandlePointerEventL(aPointerEvent);
            }
        }
    }
#endif //RD_SCALABLE_UI_V2
//  End of File

