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
 * Description:    LandmarksUi Content File - Landmarks icon selector map
 *
 */

// INCLUDE FILES
#include <eikenv.h>
#include <eiksbfrm.h>
#include <eikscrlb.h>
#include <bidivisual.h>
#include <avkon.rsg>
#include <AknPanic.h>
#include <avkon.hrh>
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknLayout.lag>

#include <aknappui.h>
#include <aknconsts.h>
#include <AknDef.h>
#include <PUAcodes.hrh>
#include <s32file.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK
// For the TEikScrollBarModelType
#include <eikscrlb.h>

#include <AknsDrawUtils.h>
#include <featmgr.h>

#include <AknsFrameBackgroundControlContext.h>

#include <lmkui.mbg>

#include "lmkiconmap.h"
#include "CLmkUiUtils.h"
#include <lmkerrors.h>

// The offset because of CEikDialogPage
const TInt KHorizontalDialogMargin = 0;
const TInt KVerticalDialogMargin = 0;
const TInt KAknSctCBaButtonDirections = 3; // bottom, right and left


/// Unnamed namespace for local definitions
namespace
    {
    const TInt KNumOfIcons(16);
    _LIT( KPanicMsg, "CLmkIconMap" );

    void Panic(TPanicCode aReason)
        {
        User::Panic(KPanicMsg, aReason);
        }
    } // namespace

// ============================ EXTENSION CLASS ===============================
//Class Declaration
NONSHARABLE_CLASS(CLmkIconMapExtension) : public CBase,
public MObjectProvider
    {
public:
    CLmkIconMapExtension();
    ~CLmkIconMapExtension();

protected:
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    MObjectProvider* MopNext();

public:
    // data
    MCoeControlObserver *iObserver;

    TUint iFlags;
    MObjectProvider* iIconMap;
    TInt iMaxVisibleRows;
    CAknsFrameBackgroundControlContext* iBgContext;
    };

// ============================ MEMBER FUNCTIONS OF CLmkIconMapExtension ===============================

// -----------------------------------------------------------------------------
// CLmkIconMapExtension::CLmkIconMapExtension
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkIconMapExtension::CLmkIconMapExtension() :
    iMaxVisibleRows(0)
    {
    iObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CLmkIconMapExtension::MopSupplyObject()
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CLmkIconMapExtension::MopSupplyObject(TTypeUid aId)
    {
    return MAknsControlContext::SupplyMopObject(aId, iBgContext);
    }

// -----------------------------------------------------------------------------
// CLmkIconMapExtension::MopNext()
// -----------------------------------------------------------------------------
//
MObjectProvider* CLmkIconMapExtension::MopNext()
    {
    return iIconMap;
    }

// -----------------------------------------------------------------------------
// CLmkIconMapExtension::~CLmkIconMapExtension()
// -----------------------------------------------------------------------------
//
CLmkIconMapExtension::~CLmkIconMapExtension()
    {

    delete iBgContext;
    }

// ============================ MEMBER FUNCTIONS OF CLmkIconMap ===============================

// -----------------------------------------------------------------------------
// CLmkIconMap::CLmkIconMap
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkIconMap::CLmkIconMap() :
    iCursorPos(TPoint(0, 0)), iOldCursorPos(TPoint(0, 0)), iMaxColumns(-1),
            iCurrentPage(0), iNumPages(0)
    {
    iConsArray = (CArrayPtr<CGulIcon>*) NULL;
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkIconMap* CLmkIconMap::NewL()
    {
    CLmkIconMap* self = new (ELeave) CLmkIconMap();

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); //self
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkIconMap::ConstructL()
    {
    // Must be created here to get the member variables available
    iExtension = new (ELeave) CLmkIconMapExtension;
    iExtension->iIconMap = this;

    iConsArray = new (ELeave) CAknIconArray(KNumOfIcons);

    iExtension->iFlags = 0x00;

    DoLayout();
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::~CLmkIconMap()
// -----------------------------------------------------------------------------
//
CLmkIconMap::~CLmkIconMap()
    {
    delete iSBFrame;
    iSBFrame = (CEikScrollBarFrame*) NULL;

    delete iOffscreenBg;
    delete iBitmapDevice;
    delete iBitmapGc;

    delete iExtension;
    iExtension = (CLmkIconMapExtension*) NULL;

    if (iConsArray)
        {
        iConsArray->ResetAndDestroy();
        delete iConsArray;
        iConsArray = (CArrayPtr<CGulIcon>*) NULL;
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::DoLayout()
// -----------------------------------------------------------------------------
//
void CLmkIconMap::DoLayout()
    {
    TRAPD(err, LoadIconL());
    if (err)
        return;

    iIsMirrored = AknLayoutUtils::LayoutMirrored();
    iDrawnBefore = EFalse;
    CountMaxColumnsAndCellSizes();
    SizeChanged();
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::ConstructFromResourceL()
// -----------------------------------------------------------------------------
//
void CLmkIconMap::ConstructFromResourceL(TResourceReader& /*aReader*/)
    {
    LoadIconL();
    CreateScrollBarAndIconRowL();

    Extension()->iBgContext = CAknsFrameBackgroundControlContext::NewL(
            KAknsIIDQsnFrPopup, TRect(0, 0, 1, 1), TRect(0, 0, 1, 1), EFalse);

    if (DrawableWindow() && AknLayoutUtils::PenEnabled())
        {
        EnableDragEvents();
        SetGloballyCapturing( ETrue);
        SetPointerCapture(ETrue);
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::LoadIcons()
// This actually load the all icons to be shown on dialog from icon file
// -----------------------------------------------------------------------------
//
void CLmkIconMap::LoadIconL()
    {
    if (iConsArray)
        {
        iConsArray->ResetAndDestroy();
        }
    // Draw all the Icons.
    TFileName* iconFile = CLmkUiUtils::LmkUiIconFileLC();

    // Create icon bitmap and mask.
    for (TInt i(0); i < (KNumOfIcons * 2); i++)
        {
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* bitmapMask = NULL;
        AknIconUtils::CreateIconLC(bitmap, bitmapMask, *iconFile,
                EMbmLmkuiQgn_prop_lm_transport + i,
                EMbmLmkuiQgn_prop_lm_transport + i + 1);
        i++;
        AknIconUtils::SetSize(bitmap, TSize(iGridItemWidth, iGridItemHeight)); //fix
        AknIconUtils::SetSize(bitmapMask, TSize(iGridItemWidth,
                iGridItemHeight)); //fix
        CGulIcon* icon = CGulIcon::NewL(bitmap, bitmapMask);
        CleanupStack::PushL(icon);
        if (iConsArray)
            iConsArray->AppendL(icon);
        CleanupStack::Pop(icon); // icon
        CleanupStack::Pop(bitmapMask); // mask
        CleanupStack::Pop(bitmap); // bitmap
        }
    CleanupStack::PopAndDestroy();// iconFile
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::HeightInRows()
// This actually returns the no of rows to be shown on a page
// Depends upon the Layout size
// -----------------------------------------------------------------------------
//
TInt CLmkIconMap::HeightInRows()
    {
    return (iRows);
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::CreateScrollBarAndIconRowL()
// This actually creates the scroll bar sets the number of
// pages and rows on a page to be shown
// -----------------------------------------------------------------------------
//
void CLmkIconMap::CreateScrollBarAndIconRowL()
    {
    __ASSERT_DEBUG(iExtension, Panic(KLmkPanicInvalidResourceData));

    __ASSERT_ALWAYS(iConsArray, Panic(KLmkPanicInvalidResourceData));

    iRows = ((iConsArray->Count() - 1) / iMaxColumns) + 1;
    iFirstVisibleRow = 0;
    iAnimated = EFalse;
    iCursorPos = TPoint(0, 0);
    iNumPages = (iRows / iExtension->iMaxVisibleRows) + (iRows
            % iExtension->iMaxVisibleRows ? 1 : 0);
    iCurrentPage = 1;

    // Create and set the scb visible even though there is nothing to scroll
    delete iSBFrame;
    iSBFrame = NULL;

    if (AknLayoutUtils::PenEnabled())
        {
        iSBFrame = new (ELeave) CEikScrollBarFrame(this, this, ETrue);
        }
    else
        {
        iSBFrame = new (ELeave) CEikScrollBarFrame(this, NULL, ETrue);
        }
    // Decide which type of scrollbar is shown
    CAknAppUi* appUi = iAvkonAppUi;
    if (AknLayoutUtils::DefaultScrollBarType(appUi)
            == CEikScrollBarFrame::EDoubleSpan)
        {
        // For EDoubleSpan type scrollbar
        if (AknLayoutUtils::PenEnabled())
            {
            iSBFrame->CreateDoubleSpanScrollBarsL(ETrue, EFalse, ETrue,
                    EFalse); // window owning scrollbar
            }
        else
            {
            iSBFrame->CreateDoubleSpanScrollBarsL(EFalse, EFalse, ETrue,
                    EFalse); // non-window owning scrollbar
            }
        iSBFrame->SetTypeOfVScrollBar(CEikScrollBarFrame::EDoubleSpan);
        }
    else
        {
        // For EArrowHead type scrollbar
        iSBFrame->SetTypeOfVScrollBar(CEikScrollBarFrame::EArrowHead);
        }

    iSBFrame->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto);
    iSBFrame->VerticalScrollBar()->SetMopParent(iExtension);
    UpdateScrollIndicatorL();
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::SetIndex
// set the reference of the selected icon index from the table
// -----------------------------------------------------------------------------
//
void CLmkIconMap::SetIndex(TInt& aIconIndex)
    {
    iIconIndex = &aIconIndex;
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::MinimumSize
//
// -----------------------------------------------------------------------------
//
TSize CLmkIconMap::MinimumSize()
    {
    iIsMirrored = AknLayoutUtils::LayoutMirrored();
    CountMaxColumnsAndCellSizes();

    TRect rect;
    // Used the set rect, but resolution changes cannot be handled when it is used

    TAknLayoutRect dialogLayRect;

    // Main pane without softkeys
    TRect mainPaneRect;
    if (!AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,
            mainPaneRect))
        {
        mainPaneRect = iAvkonAppUi->ClientRect();
        }

    // Dialog layout, check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety =
            AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();

    // Calc the variety
    TInt maxVariety = iconMapDialogVariety.LastVariety();

    // Check the CBA, if the orientation is not landscape
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    TInt maxVarietyOffset = 0; // the offset for the certain cba location variety
    TInt varietyOffset = maxVariety + 1; // the number of varieties

    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the variety
    if (Layout_Meta_Data::IsLandscapeOrientation())
        {
        varietyOffset = (maxVariety + 1) / KAknSctCBaButtonDirections; // the offset for one variety
        }

    // for right and left cba buttons the max variety is not zero
    // the varities are ordered by the location of the cba and the descending order
    // e.g the biggest sized layout first, the smallest last
    if (location == AknLayoutUtils::EAknCbaLocationRight)
        {
        maxVarietyOffset = varietyOffset;
        }
    else if (location == AknLayoutUtils::EAknCbaLocationLeft)
        {
        maxVarietyOffset = varietyOffset + varietyOffset; // 2*
        }

    TInt varietyNumber = varietyOffset - iRows - 1;

    // if more lines than possible to show, use the default
    // (the biggest grid) variety
    if (varietyNumber < 0)
        varietyNumber = 0;
    // if zero rows, use the minimum
    else if (iRows <= 0)
        varietyNumber -= 1;

    //add the varietyoffset
    varietyNumber += maxVarietyOffset;

    if (Layout_Meta_Data::IsLandscapeOrientation() && (location
            == AknLayoutUtils::EAknCbaLocationRight))
        {
        varietyNumber = 10;
        }
    else
        {
        if (iRows == 0)
            {
            varietyNumber = 5;
            }
        else
            {
            varietyNumber = 3;
            }
        }
    // Layout the dialog size
    dialogLayRect.LayoutRect(mainPaneRect,
            AknLayoutScalable_Avkon::popup_grid_graphic_window(varietyNumber));

    // Layout the grid
    TAknLayoutRect gridWithScrollLayRect;
    gridWithScrollLayRect.LayoutRect(dialogLayRect.Rect(),
            AknLayoutScalable_Avkon::listscroll_popup_graphic_pane());

    return TSize(dialogLayRect.Rect().Width(),
            gridWithScrollLayRect.Rect().Height());
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::ActivateL()
// This method is needed to set correct initial value to scroll indicator.
// -----------------------------------------------------------------------------
//
void CLmkIconMap::ActivateL()
    {
    CCoeControl::ActivateL();
    if (iRows > Extension()->iMaxVisibleRows)
        {
        UpdateScrollIndicatorL();
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::OfferKeyEventL
// Handles all the Keypad events
// -----------------------------------------------------------------------------
//
TKeyResponse CLmkIconMap::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode /*aModifiers*/)
    {
    TUint code = aKeyEvent.iCode;

    switch (code)
        {
        case EKeyLeftArrow:
        case '4':
            MoveCursorL(-1, 0);
            break;
        case EKeyRightArrow:
        case '6':
            MoveCursorL(1, 0);
            break;
        case EKeyUpArrow:
        case '2':
            MoveCursorL(0, -1);
            break;
        case EKeyDownArrow:
        case '8':
            MoveCursorL(0, 1);
            break;
        case EKeyOK:
        case '5':
        case EKeySpace:
        case EKeyEnter:
            {
            if (iConsArray)
                {
                TInt ret = iMaxColumns * (iFirstVisibleRow + iCursorPos.iY)
                        + iCursorPos.iX;
                if (ret <= iConsArray->Count())
                    {
                    *iIconIndex = ret;
                    }
                else
                    {
                    *iIconIndex = -1;
                    }
                }

            }
            break;
        default:
            return EKeyWasNotConsumed;

        }
    return EKeyWasConsumed;
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::InputCapabilities()
//
// -----------------------------------------------------------------------------
//
TCoeInputCapabilities CLmkIconMap::InputCapabilities() const
    {
    return TCoeInputCapabilities(TCoeInputCapabilities::EAllText);
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::SizeChanged()
// Control position of this control is registered for skin library when necessary
// in CEikDialogPage::SetDataPosition, so we do not do that in this method.
// -----------------------------------------------------------------------------
//
void CLmkIconMap::SizeChanged()
    {
    // Get the layout

    // Main pane without softkeys
    TRect mainPaneRect;

    if (!AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,
            mainPaneRect))
        {
        mainPaneRect = iAvkonAppUi->ClientRect();
        }

    // Dialog layout, check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety =
            AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();

    TInt maxVariety = iconMapDialogVariety.LastVariety();

    // Check the CBA, if the orientation is not landscape
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    TInt maxVarietyOffset = 0; // the offset for the certain cba location variety
    TInt varietyOffset = maxVariety + 1;

    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the variety
    if (Layout_Meta_Data::IsLandscapeOrientation())
        {
        varietyOffset = (maxVariety + 1) / KAknSctCBaButtonDirections; // the offset for one variety
        }

    // for right and left cba buttons the max variety is not zero
    // the varities are ordered by the location of the cba and the descending order
    // e.g the biggest sized layout first, the smallest last
    if (location == AknLayoutUtils::EAknCbaLocationRight)
        {
        maxVarietyOffset = varietyOffset;
        }
    else if (location == AknLayoutUtils::EAknCbaLocationLeft)
        {
        maxVarietyOffset = varietyOffset + varietyOffset; // 2*
        }

    TInt varietyNumber = varietyOffset - iRows - 1;

    // if more lines than possible to show, use the default
    // (the biggest grid) variety
    if (varietyNumber < 0)
        varietyNumber = 0;
    // if zero rows, use the minimum
    else if (iRows <= 0)
        varietyNumber -= 1;

    //add the varietyoffset
    varietyNumber += maxVarietyOffset;

    if (Layout_Meta_Data::IsLandscapeOrientation() && (location
            == AknLayoutUtils::EAknCbaLocationRight))
        {
        varietyNumber = 10;
        }
    else
        {
        if (iRows == 0)
            {
            varietyNumber = 5;
            }
        else
            {
            varietyNumber = 3;
            }

        }

    TAknLayoutRect popupGridLayRect;
    popupGridLayRect.LayoutRect(mainPaneRect,
            AknLayoutScalable_Avkon::popup_grid_graphic_window(varietyNumber));

    // Calculate the size relatively
    TRect relativeDialog(TPoint(0, 0), popupGridLayRect.Rect().Size());

    // Get the layout of the actual icon grid with scrollbar
    TAknLayoutRect gridWithScrollLayRect;
    gridWithScrollLayRect.LayoutRect(relativeDialog,
            AknLayoutScalable_Avkon::listscroll_popup_graphic_pane());

    // Then the grid area without scrollbar
    // NOTE: The grid with scroll bar is used as reference
    TAknLayoutRect gridLayRect;
    gridLayRect.LayoutRect(gridWithScrollLayRect.Rect(),
            AknLayoutScalable_Avkon::grid_graphic_popup_pane(0));

    // Different parent if SCT inside editing menu.
    TRect contentRect = gridLayRect.Rect();

    // The x coordinate is 3 pixels to right and y coordinate 3 pixels up
    // so substract from x coordinate and add to y coordinate
    if (iIsMirrored)
        {
        iOffset = TPoint(contentRect.iBr.iX - KHorizontalDialogMargin
                - iGridItemWidth + 1, contentRect.iTl.iY
                - KVerticalDialogMargin + 1);
        iGridTopLeft.iX = contentRect.iBr.iX - KHorizontalDialogMargin
                - (iMaxColumns * iGridItemWidth);
        iGridTopLeft.iY = contentRect.iTl.iY - KVerticalDialogMargin;
        }
    else // not mirrored
        {
        iOffset = TPoint(contentRect.iTl.iX - KHorizontalDialogMargin + 1,
                contentRect.iTl.iY - KVerticalDialogMargin + 1);
        iGridTopLeft.iX = contentRect.iTl.iX - KHorizontalDialogMargin;
        iGridTopLeft.iY = contentRect.iTl.iY - KVerticalDialogMargin;
        }

    // The last, update background context
    if (Extension()->iBgContext)
        {
        TInt bgVariety = 0;
        if (Layout_Meta_Data::IsLandscapeOrientation())
            bgVariety = 1;

        TAknLayoutRect innerRect;
        innerRect.LayoutRect(relativeDialog,
                AknLayoutScalable_Avkon::bg_popup_window_pane_g1(bgVariety));

        Extension()->iBgContext->SetFrameRects(relativeDialog,
                innerRect.Rect());
        }
    TRAPD(err, UpdateScrollIndicatorL());
    if (err)
        return;
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::HandleResourceChange()
//
// -----------------------------------------------------------------------------
//
void CLmkIconMap::HandleResourceChange(TInt aType)
    {
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        // save the old info for the magnitudes of the SCT grid
        TInt oldMaxColumns = iMaxColumns;
        //TInt oldMaxRows = 0;
        //oldMaxRows = iRows;
        // calculate the new magnitudes
        DoLayout();

        // then calculate the index position of the cursor in the icon table
        // and update the x and y positions for the new grid with it

        TInt oldCursorPosition = (iFirstVisibleRow + iOldCursorPos.iY)
                * oldMaxColumns + iOldCursorPos.iX;

        TInt currentCursorPosition = (iFirstVisibleRow + iCursorPos.iY)
                * oldMaxColumns + iCursorPos.iX;

        // the new first row is the top row on the page where the new focus is
        iFirstVisibleRow = Extension()->iMaxVisibleRows
                * (currentCursorPosition / (iMaxColumns
                        * Extension()->iMaxVisibleRows));

        // the cursor positions are relative to current page
        iCursorPos.iY = (currentCursorPosition - (iMaxColumns
                * iFirstVisibleRow)) / iMaxColumns;
        iCursorPos.iX = currentCursorPosition - (iMaxColumns
                * iFirstVisibleRow) - (iMaxColumns * iCursorPos.iY);

        iOldCursorPos.iY = (oldCursorPosition - (iMaxColumns
                * iFirstVisibleRow)) / iMaxColumns;
        iOldCursorPos.iX = oldCursorPosition - (iMaxColumns
                * iFirstVisibleRow) - (iMaxColumns * iOldCursorPos.iY);
        }

    if (aType == KAknsMessageSkinChange)
        {
        iOffscreenBgDrawn = EFalse;
        }
    CCoeControl::HandleResourceChange(aType);

    }

// -----------------------------------------------------------------------------
// CLmkIconMap::Draw()
//
// -----------------------------------------------------------------------------
//
void CLmkIconMap::Draw(const TRect& /*aRect*/) const
    {

    TInt cursorPos = 0;
    CWindowGc& gc = SystemGc();

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext(this);

    TRect rect = Rect();

    // Main pane without softkeys
    TRect mainPaneRect;
    if (!AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,
            mainPaneRect))
        {
        mainPaneRect = iAvkonAppUi->ClientRect();
        }

    // Dialog layout, check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety =
            AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();

    // The variety starts from 0 so add +1
    TInt maxVariety = iconMapDialogVariety.LastVariety();

    // Check the CBA, if the orientation is not landscape
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    TInt maxVarietyOffset = 0; // the offset for the certain cba location variety
    TInt varietyOffset = maxVariety + 1;

    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the variety
    if (Layout_Meta_Data::IsLandscapeOrientation())
        {
        varietyOffset = (maxVariety + 1) / KAknSctCBaButtonDirections; // the offset for one variety
        }

    // for right and left cba buttons the max variety is not zero
    // the varities are ordered by the location of the cba and the descending order
    // e.g the biggest sized layout first, the smallest last
    if (location == AknLayoutUtils::EAknCbaLocationRight)
        {
        maxVarietyOffset = varietyOffset;
        }
    else if (location == AknLayoutUtils::EAknCbaLocationLeft)
        {
        maxVarietyOffset = varietyOffset + varietyOffset; // 2*
        }

    TInt varietyNumber = varietyOffset - iRows - 1;

    // if more lines than possible to show, use the default
    // (the biggest grid) variety
    if (varietyNumber < 0)
        varietyNumber = 0;
    // if zero rows, use the minimum
    else if (iRows <= 0)
        varietyNumber -= 1;

    //add the varietyoffset
    varietyNumber += maxVarietyOffset;

    if (Layout_Meta_Data::IsLandscapeOrientation() && (location
            == AknLayoutUtils::EAknCbaLocationRight))
        {
        varietyNumber = 10;
        }
    else
        {
        varietyNumber = 3;
        }
    // Layout the dialog size
    TAknLayoutRect dialogLayRect;
    dialogLayRect.LayoutRect(mainPaneRect,
            AknLayoutScalable_Avkon::popup_grid_graphic_window(varietyNumber));

    // Get the missing height for the background
    TInt backgroundHeightOffset = dialogLayRect.Rect().Height() - rect.iBr.iY;

    rect.iBr.iY += backgroundHeightOffset;

    // Check if we got an offscreen bitmap allocated for skin background and
    // there is bitmap background in the current skin.
    if (iOffscreenBg && iHasBitmapBackground)
        {
        DrawOffscreenBackgroundIfRequired();
        gc.BitBlt(rect.iTl, iOffscreenBg);
        }
    else
        {
        AknsDrawUtils::Background(skin, cc, this, gc, rect);
        }

    TInt numberOfIconsToBeDrawn = iConsArray->Count();
    numberOfIconsToBeDrawn -= (iFirstVisibleRow * iMaxColumns);
    if (numberOfIconsToBeDrawn > 0)
        {
        if (numberOfIconsToBeDrawn > (Extension()->iMaxVisibleRows
                * iMaxColumns))
            {
            numberOfIconsToBeDrawn = Extension()->iMaxVisibleRows
                    * iMaxColumns;
            }
        __ASSERT_DEBUG( numberOfIconsToBeDrawn >= 1, Panic( KLmkPanicOutOfRange ) );

        gc.SetPenStyle(CGraphicsContext::ESolidPen);
        gc.SetBrushStyle(CGraphicsContext::ENullBrush);
        gc.SetPenSize(TSize(1, 1));

        // 2) Draw the grid
        const TSize gridItemRectSize(iGridItemWidth + 1, iGridItemHeight + 1);

        TInt numberOfGridCellsToBeDrawn = numberOfIconsToBeDrawn;

        TRgb colorLine = AKN_LAF_COLOR(219);
        AknsUtils::GetCachedColor(skin, colorLine, KAknsIIDQsnLineColors,
                EAknsCIQsnLineColorsCG5);
        TRgb colorRecentLine = AKN_LAF_COLOR(215);
        AknsUtils::GetCachedColor(skin, colorRecentLine,
                KAknsIIDQsnLineColors, EAknsCIQsnLineColorsCG7);

        // default pen color
        gc.SetPenColor(colorLine);

        TInt fullRows = numberOfGridCellsToBeDrawn / iMaxColumns;

        // how many left after the full rows
        numberOfGridCellsToBeDrawn -= fullRows * iMaxColumns;

        TPoint pos = iGridTopLeft;

        TInt endX = pos.iX + iGridItemWidth * iMaxColumns + 1;
        TInt endY = pos.iY + iGridItemHeight * fullRows;

        TInt ii = 0;

        if (fullRows)
            {
            // Draw full vertical lines
            for (ii = 0; ii <= iMaxColumns; ii++)
                {
                gc.SetPenColor(colorLine);
                gc.SetPenSize(TSize(1, 1));
                gc.DrawLine(pos, TPoint(pos.iX, endY));
                pos.iX += iGridItemWidth;
                }

            pos = iGridTopLeft;

            // Draw full horizontal lines
            for (ii = 0; ii <= fullRows; ii++)
                {
                gc.SetPenSize(TSize(1, 1));
                gc.SetPenColor(colorLine);
                gc.DrawLine(pos, TPoint(endX, pos.iY));
                pos.iY += iGridItemHeight;
                }
            gc.SetPenColor(colorLine);
            gc.SetPenSize(TSize(1, 1));
            }

        if (numberOfGridCellsToBeDrawn)
            {
            // Remaining cells in the last, non-full row
            pos = iOffset;
            pos.iX--; // iOffset is cell area topLeft, grid is not included in it
            pos.iY--;

            pos.iY += iGridItemHeight * fullRows;

            for (ii = 0; ii < numberOfGridCellsToBeDrawn; ii++)
                {
                gc.DrawRect(TRect(pos, gridItemRectSize));

                if (iIsMirrored)
                    pos.iX -= iGridItemWidth;
                else
                    // not mirrored
                    pos.iX += iGridItemWidth;
                }
            }

        TInt iconIndex = (iCurrentPage - 1) * (iMaxColumns
                * Extension()->iMaxVisibleRows);
        TInt lCnt = iConsArray->Count();
        cursorPos = iCursorPos.iX + iCursorPos.iY * iMaxColumns;
        if (lCnt > 0)
            {
            TRect cellRect(TPoint(0, 0), TSize(iGridItemWidth - 1,
                    iGridItemHeight - 1));
            for (TInt j = iconIndex, i = 0; j < lCnt && (i
                    < numberOfIconsToBeDrawn); j++, i++)
                {
                DrawItem(gc, CursorRect(i), j, (cursorPos == i), EFalse);
                }
            }

        }
    iDrawnBefore = ETrue;
    gc.DiscardFont();
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::DrawItem()
//
// -----------------------------------------------------------------------------
//
void CLmkIconMap::DrawItem(CWindowGc& aGc, const TRect& aSctPosition,
        TInt aIconIndex, TBool aHighlighted, TBool aDrawBackground) const
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext(this);

    TBool skins = AknsDrawUtils::Background(skin, cc, aGc, aSctPosition);
    TRgb color;
    if (!skins)
        aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    if (aHighlighted)
        {
        TRgb colorHightLightRect = AKN_LAF_COLOR(215);
        AknsUtils::GetCachedColor(skin, colorHightLightRect,
                KAknsIIDQsnLineColors, EAknsCIQsnLineColorsCG7);
        aGc.SetPenColor(colorHightLightRect);
        aGc.DrawRect(aSctPosition);

        // Shrink by one pixel in all directions.
        TRect innerRect = aSctPosition;
        innerRect.Shrink(1, 1);

        color = AKN_LAF_COLOR(210);
        AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnHighlightColors,
                EAknsCIQsnHighlightColorsCG1);
        aGc.SetBrushColor(color);
        aGc.Clear(innerRect);
        }
    else if (aDrawBackground)
        {
        TRect innerRect = aSctPosition;
        aGc.SetBrushColor(AKN_LAF_COLOR(0));
        if (!skins)
            aGc.Clear(innerRect);
        else
            AknsDrawUtils::Background(skin, cc, this, aGc, innerRect);
        }

    if (iConsArray)
        {
        TInt lCnt = iConsArray->Count();
        if (lCnt > 0 && aIconIndex < lCnt && aIconIndex >= 0)
            {
            TRect cellRect(TPoint(0, 0), TSize(iGridItemWidth - 1,
                    iGridItemHeight - 1));

            CGulIcon* bitmap = NULL;
            bitmap = iConsArray->At(aIconIndex);
            TInt numIconsInaPage = Extension()->iMaxVisibleRows * iMaxColumns;
            TInt cellIndex = aIconIndex;
            if (aIconIndex >= numIconsInaPage)
                {
                cellIndex = aIconIndex % numIconsInaPage;
                }
            aGc.BitBltMasked(CursorPoint(cellIndex), bitmap->Bitmap(),
                    cellRect, bitmap->Mask(), EFalse);
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::DrawCursor()
// Optimizes drawing. Only cursor is drawn.
// -----------------------------------------------------------------------------
void CLmkIconMap::DrawCursor() const
    {
    // Whole Icon Map has to be drawn at least once.
    // If the user presses arrow key before Icon Map has been drawn,
    // only cursor position is drawn without this check.
    if (!iDrawnBefore)
        {
        DrawNow();
        }
    // Only redraw old and new cursor position cells
    ActivateGc();
    CWindowGc& gc = SystemGc();

    TInt cursorPos = iOldCursorPos.iX + iOldCursorPos.iY * iMaxColumns;
    if (cursorPos >= 0)
        {
        DrawCell(cursorPos, EFalse);

        cursorPos = iCursorPos.iX + iCursorPos.iY * iMaxColumns;
        DrawCell(cursorPos, ETrue);
        }

    gc.DiscardFont();
    DeactivateGc();
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::DrawCell
// -----------------------------------------------------------------------------
//
void CLmkIconMap::DrawCell(TInt aCursorPos, TBool aHighLighted) const
    {
    // calculate icon index
    TInt iconIndex = aCursorPos + iFirstVisibleRow * iMaxColumns;

    // If we are only redrawing for animations, no need to draw non-animated items.
    TRect rect = CursorRect(aCursorPos);

    Window().Invalidate(rect);
    Window().BeginRedraw(rect);
    DrawItem(SystemGc(), rect, iconIndex, aHighLighted, ETrue);
    Window().EndRedraw();
    SystemGc().DiscardFont();
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::CursorRect
// -----------------------------------------------------------------------------
//
TRect CLmkIconMap::CursorRect(TInt aCursorPos) const
    {
    TPoint pos = iOffset;

    if (iIsMirrored)
        {
        pos.iX -= (aCursorPos % iMaxColumns) * iGridItemWidth;
        }
    else // Not mirrored
        {
        pos.iX += (aCursorPos % iMaxColumns) * iGridItemWidth;
        }

    pos.iY += (aCursorPos / iMaxColumns) * iGridItemHeight;
    return TRect(pos, TSize(iGridItemWidth - 1, iGridItemHeight - 1));
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::CursorPoint
// -----------------------------------------------------------------------------
//
TPoint CLmkIconMap::CursorPoint(TInt aCursorPos) const
    {
    TPoint pos = iOffset;

    if (iIsMirrored)
        {
        pos.iX -= (aCursorPos % iMaxColumns) * iGridItemWidth;
        }
    else // Not mirrored
        {
        pos.iX += (aCursorPos % iMaxColumns) * iGridItemWidth;
        }

    pos.iY += (aCursorPos / iMaxColumns) * iGridItemHeight;
    return pos;
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::MoveCursorL
// -----------------------------------------------------------------------------
//
void CLmkIconMap::MoveCursorL(TInt aDeltaX, TInt aDeltaY)
    {
    __ASSERT_DEBUG((aDeltaX <= 1) && (aDeltaX >= -1) && (aDeltaY <= 1)
            && (aDeltaY >= -1) && ((aDeltaX * aDeltaY) == 0), Panic(
            KLmkPanicOutOfRange));

    if (iIsMirrored)
        aDeltaX = -aDeltaX;

    if ((iConsArray->Count() < 8))
        return;

    iOldCursorPos = iCursorPos;
    TInt oldFirstVisibleRow = iFirstVisibleRow;

    TInt globalYPos = iCursorPos.iY + iFirstVisibleRow;
    TInt lastColumnOnLastRow = ((iConsArray->Count() - 1) % iMaxColumns);

    TInt skipicon = aDeltaX != 0 ? 1 : 0;

    if (aDeltaX < 0)
        {
        // Cursor was moved to left.
        if (iCursorPos.iX > skipicon - 1)
            {
            iCursorPos.iX -= skipicon;
            }
        else
            {
            if (skipicon > iMaxColumns)
                {
                globalYPos--;
                iCursorPos.iX = iMaxColumns;
                }
            // Go to previous line
            globalYPos--;
            if (globalYPos < 0)
                {
                // Cursor was on the first line - go to last line.
                globalYPos = iRows - 1;
                // x - position to the last item on the last row.
                iCursorPos.iX = lastColumnOnLastRow;
                }
            else
                {
                // x - position to last column.
                iCursorPos.iX = iMaxColumns - skipicon;
                }
            }
        }

    if (aDeltaX > 0)
        {
        // Cursor was moved to right.
        if (globalYPos < iRows - 1)
            {
            // Not in the last row.
            if (iCursorPos.iX < iMaxColumns - skipicon)
                {
                // If not on the last columns, move cursor to next column.
                iCursorPos.iX += skipicon;
                }
            else
                {
                // Cursor was on last column,
                // move to first column of the next line.
                iCursorPos.iX = 0;
                globalYPos++;
                }
            }
        else
            {
            // Currently on the last row.
            if (iCursorPos.iX < lastColumnOnLastRow)
                {
                // If there are more items on this row, move cursor to next item.
                iCursorPos.iX++;
                }
            else
                {
                // No more item on the current row.
                // Move to first item on the first row.
                iCursorPos.iX = 0;
                globalYPos = 0;
                }
            }
        }

    if (aDeltaY < 0)
        {
        iCursorPos.iX -= skipicon;
        if (iCursorPos.iX < 0)
            {
            iCursorPos.iX += (iMaxColumns - 1);
            globalYPos--;
            }
        // Cursor was moved to up.
        if (globalYPos > 0)
            {
            // Cursor was not on the first line - move it to previous row.
            globalYPos--;
            }
        else
            {
            // Move cursot to last to row.
            globalYPos = iRows - 1;
            if (iCursorPos.iX > lastColumnOnLastRow)
                {
                // No items in the current column on the last row -
                // move cursor to last item on the row.
                iCursorPos.iX = lastColumnOnLastRow;
                }
            }
        }

    if (aDeltaY > 0)
        {
        iCursorPos.iX = (iCursorPos.iX + skipicon) % iMaxColumns;
        globalYPos += (iCursorPos.iX + skipicon) / iMaxColumns;
        // Cursor was moved to down.
        if (globalYPos < iRows - 1)
            {
            // Cursor is not on the last row. Move cursor to next row.
            globalYPos++;
            if (globalYPos == iRows - 1 && iCursorPos.iX
                    > lastColumnOnLastRow)
                {
                // No items in the current column on the last row -
                // move cursor to last item on the row.
                iCursorPos.iX = lastColumnOnLastRow;
                }
            }
        else
            {
            // Cursor was at the last row - move it to the first row.
            globalYPos = 0;
            }
        }
    iCursorPos.iY = globalYPos - iFirstVisibleRow;

    if (globalYPos < iFirstVisibleRow)
        {
        // Cursor was moved from the top row.
        if (globalYPos <= 0)
            {
            iFirstVisibleRow = 0;
            iCursorPos = TPoint(iCursorPos.iX, 0);
            }
        else
            {
            // If cursor was moved up out of the visible area - show it again.
            iFirstVisibleRow -= Extension()->iMaxVisibleRows;
            iCursorPos = TPoint(iCursorPos.iX, Extension()->iMaxVisibleRows
                    - 1);
            }
        }

    if (globalYPos > iFirstVisibleRow + Extension()->iMaxVisibleRows - 1)
        {
        if (globalYPos == iRows - 1)
            {
            // When cursor has moved from the top line,
            // it is adjusted to a page boundary.
            iCursorPos = TPoint(iCursorPos.iX, (iRows - 1)
                    % Extension()->iMaxVisibleRows);
            iFirstVisibleRow = ((iRows - 1) / Extension()->iMaxVisibleRows)
                    * Extension()->iMaxVisibleRows;
            }
        else
            {
            // If cursor was moved down out of the visible area - show it again.
            iFirstVisibleRow += Extension()->iMaxVisibleRows;
            iCursorPos = TPoint(iCursorPos.iX, 0);
            }
        }

    //TInt increment(1);
    //if (aDeltaY < 0 || aDeltaX < 0)
    //    {
    ////    increment = -1;
    //    }
    if ((iRows > Extension()->iMaxVisibleRows) && (iOldCursorPos.iY
            + oldFirstVisibleRow != iCursorPos.iY + iFirstVisibleRow))
        {
        UpdateScrollIndicatorL();
        }

    if (oldFirstVisibleRow == iFirstVisibleRow)
        {
        // Draw only cursor if the view to the content was not scrolled.
        DrawCursor();
        }
    else
        {
        DrawNow();
        }

    }

// -----------------------------------------------------------------------------
// CLmkIconMap::UpdateScrollIndicatorL()
//
// -----------------------------------------------------------------------------
//
void CLmkIconMap::UpdateScrollIndicatorL()
    {
    if (!iSBFrame)
        {
        return;
        }
    TEikScrollBarModel hSbarModel;
    TEikScrollBarModel vSbarModel;

    TEikScrollBarFrameLayout layout;

    // Main pane without softkeys
    TRect mainPaneRect;
    if (!AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,
            mainPaneRect))
        {
        mainPaneRect = iAvkonAppUi->ClientRect();
        }

    // Dialog layout, check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety =
            AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();

    TInt maxVariety = iconMapDialogVariety.LastVariety();

    // Check the CBA, if the orientation is not landscape
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    TInt maxVarietyOffset = 0; // the offset for the certain cba location variety
    TInt varietyOffset = maxVariety + 1;

    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the variety
    if (Layout_Meta_Data::IsLandscapeOrientation())
        {
        varietyOffset = (maxVariety + 1) / KAknSctCBaButtonDirections; // the offset for one variety
        }

    // for right and left cba buttons the max variety is not zero
    // the varities are ordered by the location of the cba and the descending order
    // e.g the biggest sized layout first, the smallest last
    if (location == AknLayoutUtils::EAknCbaLocationRight)
        {
        maxVarietyOffset = varietyOffset;
        }
    else if (location == AknLayoutUtils::EAknCbaLocationLeft)
        {
        maxVarietyOffset = varietyOffset + varietyOffset; // 2*
        }

    TInt varietyNumber = varietyOffset - iRows - 1;

    // if more lines than possible to show, use the default
    // (the biggest grid) variety
    if (varietyNumber < 0)
        varietyNumber = 0;
    // if zero rows, use the minimum
    else if (iRows <= 0)
        varietyNumber -= 1;

    //add the varietyoffset
    varietyNumber += maxVarietyOffset;

    if (Layout_Meta_Data::IsLandscapeOrientation() && (location
            == AknLayoutUtils::EAknCbaLocationRight))
        {
        varietyNumber = 10;
        }
    else
        {
        varietyNumber = 3;
        }
    // Layout the dialog size
    TAknLayoutRect dialogLayRect;
    dialogLayRect.LayoutRect(mainPaneRect,
            AknLayoutScalable_Avkon::popup_grid_graphic_window(varietyNumber));

    TRect dialogRect = dialogLayRect.Rect();

    // Get the layout of the actual icon grid with scrollbar
    TAknLayoutRect gridWithScrollLayRect;

    gridWithScrollLayRect.LayoutRect(TRect(TPoint(0, 0), TSize(
            dialogRect.Size())),
            AknLayoutScalable_Avkon::listscroll_popup_graphic_pane());

    // Calculate the relative rect for the grid
    TRect parent = gridWithScrollLayRect.Rect();

    TAknWindowComponentLayout scrollbarLayout =
            AknLayoutScalable_Avkon::scroll_pane_cp5();

    iCurrentPage = (iFirstVisibleRow / Extension()->iMaxVisibleRows) + 1;

    vSbarModel.iScrollSpan = iNumPages * Extension()->iMaxVisibleRows;
    vSbarModel.iThumbSpan = Extension()->iMaxVisibleRows;

    if (iSBFrame && iSBFrame->TypeOfVScrollBar()
            == CEikScrollBarFrame::EDoubleSpan)
        {
        // For EDoubleSpan type scrollbar
        vSbarModel.iThumbPosition = (iCurrentPage - 1)
                * Extension()->iMaxVisibleRows;
        TAknDoubleSpanScrollBarModel hDsSbarModel(hSbarModel);
        TAknDoubleSpanScrollBarModel vDsSbarModel(vSbarModel);

        // The y coordinate must be sifted 3 pixels up and x 3 to left
        parent.iTl.iY -= KVerticalDialogMargin;
        parent.iBr.iY -= KVerticalDialogMargin;
        parent.iTl.iX -= KHorizontalDialogMargin;
        parent.iBr.iX -= KHorizontalDialogMargin;

        layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;
        iSBFrame->Tile(&vDsSbarModel);
        AknLayoutUtils::LayoutVerticalScrollBar(iSBFrame, parent,
                scrollbarLayout);
        iSBFrame->SetVFocusPosToThumbPos(vDsSbarModel.FocusPosition());
        }
    else
        {
        // For EArrowHead type scrollbar
        vSbarModel.iThumbPosition = iCursorPos.iY + iFirstVisibleRow;
        iSBFrame->TileL(&hSbarModel, &vSbarModel, parent, parent, layout);
        iSBFrame->SetVFocusPosToThumbPos(vSbarModel.iThumbPosition);
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::Reserved_1()
//
// -----------------------------------------------------------------------------
//
void CLmkIconMap::Reserved_1()
    {
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::Reserved_2()
//
// -----------------------------------------------------------------------------
//
void CLmkIconMap::Reserved_2()
    {
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::DrawOffscreenBackgroundIfRequired
//
// -----------------------------------------------------------------------------
//
void CLmkIconMap::DrawOffscreenBackgroundIfRequired() const
    {
    if (iOffscreenBg && iHasBitmapBackground)
        {
        if (!iOffscreenBgDrawn)
            {
            TRect mainPaneRect;
            if (!AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,
                    mainPaneRect))
                {
                mainPaneRect = iAvkonAppUi->ClientRect();
                }

            // Dialog layout, check variety first
            TAknLayoutScalableParameterLimits
                    iconMapDialogVariety =
                            AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();

            TInt maxVariety = iconMapDialogVariety.LastVariety();

            // Check the CBA, if the orientation is not landscape
            // there is not so much varieties
            AknLayoutUtils::TAknCbaLocation location =
                    AknLayoutUtils::CbaLocation();
            TInt maxVarietyOffset = 0; // the offset for the certain cba location variety
            TInt varietyOffset = maxVariety + 1;

            // landscape variety number must be calculated offset == number of varieties
            // same applies to the variety number for the biggest sized layout for the variety
            if (Layout_Meta_Data::IsLandscapeOrientation())
                {
                varietyOffset = (maxVariety + 1) / KAknSctCBaButtonDirections; // the offset for one variety
                }

            // for right and left cba buttons the max variety is not zero
            // the varities are ordered by the location of the cba and the descending order
            // e.g the biggest sized layout first, the smallest last
            if (location == AknLayoutUtils::EAknCbaLocationRight)
                {
                maxVarietyOffset = varietyOffset;
                }
            else if (location == AknLayoutUtils::EAknCbaLocationLeft)
                {
                maxVarietyOffset = varietyOffset + varietyOffset; // 2*
                }
            TInt varietyNumber = varietyOffset - iRows - 1;

            // if more lines than possible to show, use the default
            // (the biggest grid) variety
            if (varietyNumber < 0)
                varietyNumber = 0;
            // if zero rows, use the minimum
            else if (iRows <= 0)
                varietyNumber -= 1;

            //add the varietyoffset
            varietyNumber += maxVarietyOffset;

            TAknLayoutRect popupGridLayRect;
            popupGridLayRect.LayoutRect(mainPaneRect,
                    AknLayoutScalable_Avkon::popup_grid_graphic_window(5));

            TRect popupGridRect = popupGridLayRect.Rect();

            // set the top left height as the control starting point
            popupGridRect.iTl.iY = Rect().iTl.iY;

            //if(popupGridRect.iBr.iY < mainPaneRect.iBr.iY)
            //	popupGridRect.iBr.iY = mainPaneRect.iBr.iY
            MAknsSkinInstance* skin = AknsUtils::SkinInstance();
            MAknsControlContext* cc = AknsDrawUtils::ControlContext(this);

            // draw to upper left corner, and normalize the retangle to
            // fact that the dialog starts from coordinates (0,0),
            // so the y-coordinate is correct (heading pane)
            // but x must be set to zero
            TPoint point = TPoint(0, 0);
            popupGridRect.Move(-popupGridRect.iTl.iX, 0);

            AknsDrawUtils::DrawBackground(skin, cc, this, *iBitmapGc, point,
                    popupGridRect, KAknsDrawParamDefault);

            iOffscreenBgDrawn = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::ComponentControl(TInt aIndex) const
// Return the controll pointer
// -----------------------------------------------------------------------------
//
CCoeControl* CLmkIconMap::ComponentControl(TInt aIndex) const
    {
    if (aIndex == 0 && iSBFrame && iSBFrame->TypeOfVScrollBar()
            == CEikScrollBarFrame::EDoubleSpan)
        {
        return iSBFrame->VerticalScrollBar();
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::CountComponentControls()
// Return no of controll to be placed on the container control
// -----------------------------------------------------------------------------
//
TInt CLmkIconMap::CountComponentControls() const
    {
    if (iSBFrame && iSBFrame->TypeOfVScrollBar()
            == CEikScrollBarFrame::EDoubleSpan)
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::CountMaxColumnsAndCellSizes
// Counts no of columns and the cell size will be displayed in the icon table
// -----------------------------------------------------------------------------
//
void CLmkIconMap::CountMaxColumnsAndCellSizes()
    {

    TRect cellRect; // retangle of one item in grid
    TRect gridRect; // retangle of the grid contaning the items

    // 1. Get the layout

    // Get the parent rect
    TRect mainPaneRect;
    if (!AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,
            mainPaneRect))
        {
        mainPaneRect = iAvkonAppUi->ClientRect();
        }

    // Calculate the layout of the whole popup with the biggest possible -> 0
    // Dialog layout, check variety first

    // Get the layout rect of the dialog

    // Check variety first
    TAknLayoutScalableParameterLimits iconMapDialogVariety =
            AknLayoutScalable_Avkon::popup_grid_graphic_window_ParamLimits();

    TInt maxVariety = iconMapDialogVariety.LastVariety();

    // Check the CBA, if the orientation is not landscape
    // there is not so much varieties
    AknLayoutUtils::TAknCbaLocation location = AknLayoutUtils::CbaLocation();
    TInt maxVarietyOffset = 0; // the offset for the certain cba location variety
    TInt varietyOffset = maxVariety + 1; // the number of varieties

    // landscape variety number must be calculated offset == number of varieties
    // same applies to the variety number for the biggest sized layout for the variety
    if (Layout_Meta_Data::IsLandscapeOrientation())
        {
        varietyOffset = (maxVariety + 1) / KAknSctCBaButtonDirections; // the offset for one variety
        }

    if (location == AknLayoutUtils::EAknCbaLocationRight)
        {
        maxVarietyOffset = varietyOffset;
        }
    else if (location == AknLayoutUtils::EAknCbaLocationLeft)
        {
        maxVarietyOffset = varietyOffset + varietyOffset; // 2*
        }

    TAknLayoutRect popupGridLayRect;
    popupGridLayRect.LayoutRect(mainPaneRect,
            AknLayoutScalable_Avkon::popup_grid_graphic_window(
                    maxVarietyOffset));

    // Get the layout of the actual icon grid with scrollbar
    TAknLayoutRect gridWithScrollLayRect;
    gridWithScrollLayRect.LayoutRect(popupGridLayRect.Rect(),
            AknLayoutScalable_Avkon::listscroll_popup_graphic_pane());

    // Then the grid area without scrollbar
    // NOTE: The grid with scroll bar is used as reference
    TAknLayoutRect gridLayRect;
    gridLayRect.LayoutRect(gridWithScrollLayRect.Rect(),
            AknLayoutScalable_Avkon::grid_graphic_popup_pane(0));

    // Different parent if SCT inside editing menu.
    TRect rect = Rect();
    gridRect = gridLayRect.Rect();
    // cell size, AGAIN 7 item
    TAknLayoutRect cellLayRect;
    cellLayRect.LayoutRect(gridRect,
            AknLayoutScalable_Avkon::cell_graphic_popup_pane(0, 0, 0));

    cellRect = cellLayRect.Rect();

    // 2. Calculate width related
    // - item width
    // - max number of columns

    // Width of the items area
    TInt gridWidth = gridRect.Width();

    // Width of one item
    TInt cellWidth = cellRect.Width();

    // ensure the item width and store it
    TAknLayoutRect secondCellLayRect;
    secondCellLayRect.LayoutRect(gridRect,
            AknLayoutScalable_Avkon::cell_graphic_popup_pane(0, 1, 0));

    iGridItemWidth = secondCellLayRect.Rect().iTl.iX
            - cellLayRect.Rect().iTl.iX;

    if (iGridItemWidth < 0)
        {
        // The result is negative with lay file data if the layout is mirrored.
        iGridItemWidth = -iGridItemWidth;
        }

    // Calculate the amount of columns
    iMaxColumns = gridWidth / cellWidth;

    // 2. Calculate height related
    // - item height
    // - max number of rows

    // Get the grid height
    // NOTE: in landscape the LAF is wrong so use the main panes height
    TInt gridHeight = gridRect.Height();

    // get the item height
    TInt cellHeight = cellRect.Height();

    // calculate the number of items fitting to grid
    Extension()->iMaxVisibleRows = 3;//gridHeight / cellHeight;


    // Store the item height
    TAknLayoutRect secondRowLayRect;
    secondRowLayRect.LayoutRect(gridLayRect.Rect(),
            AknLayoutScalable_Avkon::cell_graphic_popup_pane(0, 0, 1));

    iGridItemHeight = secondRowLayRect.Rect().iTl.iY
            - cellLayRect.Rect().iTl.iY;
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::HandleScrollEventL
// Handles the different scroll events so that the map reacts accordingly.
// -----------------------------------------------------------------------------
//
void CLmkIconMap::HandleScrollEventL(CEikScrollBar* aScrollBar,
        TEikScrollEvent aEventType)
    {
    TBool update = EFalse;

    switch (aEventType)
        {
        case EEikScrollUp:
        case EEikScrollPageUp:
            {
            // nothing done if we are already on the first page.
            if (iFirstVisibleRow != 0)
                {
                iFirstVisibleRow -= Extension()->iMaxVisibleRows;
                update = ETrue;
                }
            UpdateScrollIndicatorL();
            }
            break;
        case EEikScrollDown:
        case EEikScrollPageDown:
            {
            // nothing done if we are already on the last page.
            if (iFirstVisibleRow != (iRows / Extension()->iMaxVisibleRows)
                    * Extension()->iMaxVisibleRows)
                {
                iFirstVisibleRow += Extension()->iMaxVisibleRows;
                update = ETrue;
                }
            UpdateScrollIndicatorL();
            }
            break;
        case EEikScrollThumbDragVert:
            {
            TInt thumbPosition;
            TInt halfPage = Extension()->iMaxVisibleRows / 2;
            // Ask which type of scrollbar is shown
            //CAknAppUi* appUi = iAvkonAppUi;
            TBool isDoubleSpan = CEikScrollBarFrame::EDoubleSpan
                    == iSBFrame->TypeOfVScrollBar();
            if (isDoubleSpan)
                {
                thumbPosition
                        = static_cast<const TAknDoubleSpanScrollBarModel*> (aScrollBar->Model())->FocusPosition();
                }
            else
                {
                thumbPosition = aScrollBar->Model()->iThumbPosition;
                }

            // If the slider is in the range of less then a half page from a possible correct thumb position.
            // thus 0 <= iFirstVisibleRow - thumbPosition < halfPage. Or in the other direction:
            // 0 <= thumbPosition - iFirstVisibleRow < halfPage
            if (!((0 <= iFirstVisibleRow - thumbPosition && iFirstVisibleRow
                    - thumbPosition < halfPage) || (0 <= thumbPosition
                    - iFirstVisibleRow && thumbPosition - iFirstVisibleRow
                    < halfPage)))
                {
                TReal toRound = thumbPosition
                        / (TReal) Extension()->iMaxVisibleRows;
                if (toRound * 2 > (TInt) toRound * 2 + 1)
                    {
                    toRound++;
                    }
                iFirstVisibleRow = (TInt) toRound
                        * Extension()->iMaxVisibleRows;
                iCurrentPage = (iFirstVisibleRow
                        / Extension()->iMaxVisibleRows) + 1;
                update = ETrue;
                }
            }
            break;
        case EEikScrollThumbReleaseVert:
            {
            UpdateScrollIndicatorL();
            }
            break;
        case EEikScrollLeft: // flow through
        case EEikScrollRight: // flow through
        case EEikScrollPageLeft: // flow through
        case EEikScrollPageRight: // flow through
        case EEikScrollThumbDragHoriz: // flow through
        case EEikScrollThumbReleaseHoriz: // flow through
            // do nothing
            break;
        default:
            // do nothing
            break;
        }

    // If we have moved down to the last page we check that the cursor is in a place where it can be
    // drawn.
    if (iFirstVisibleRow == (iRows / Extension()->iMaxVisibleRows)
            * Extension()->iMaxVisibleRows)
        {
        // the old cursor is set to a "safe" position where it at least can be.
        iOldCursorPos.iX = 0;
        iOldCursorPos.iY = 0;
        // if the last page has only one line which isn't filled complitely.
        if (iConsArray->Count() % iMaxColumns - 1 < iCursorPos.iX && iRows
                % Extension()->iMaxVisibleRows == 1)
            {
            TInt xVal = iConsArray->Count() % iMaxColumns - 1;
            if (xVal >= 0)
                iCursorPos.iX = xVal;
            }
        // If the cursor is in a position where it would go unto a spot with out
        // a icon when scrolled.
        if (iCursorPos.iY + iFirstVisibleRow >= iRows)
            {
            if (iConsArray->Count() % iMaxColumns > iCursorPos.iX)
                {
                TInt yVal = iRows - 1 - iFirstVisibleRow;
                if (yVal >= 0)
                    iCursorPos.iY = yVal;
                }
            else
                {
                TInt yVal = iRows - 2 - iFirstVisibleRow;
                if (yVal >= 0)
                    iCursorPos.iY = yVal;
                }
            }
        // If the cursor is actually on the last row, but is still in the
        // area where there is now icons. (the rest of the last row)
        if (iConsArray->Count() <= (iFirstVisibleRow + iCursorPos.iY)
                * iMaxColumns + iCursorPos.iX && iCursorPos.iY
                + iFirstVisibleRow + 1 == iRows)
            {
            iCursorPos.iY--;
            }
        // if the corrections did not help and the cursor is in the area
        // where there is a valid row, but no icons anymore
        }

    // to avoid flicker we draw only if there really was something new to draw.
    if (update)
        {
        if (Extension()->iObserver)
            {
            Extension()->iObserver->HandleControlEventL(this,
                    MCoeControlObserver::EEventRequestFocus);
            }
        DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::Extension
// Asserts that extension object has been created.
// -----------------------------------------------------------------------------
//
CLmkIconMapExtension* CLmkIconMap::Extension() const
    {
    __ASSERT_ALWAYS(iExtension, Panic(KLmkPanicNullPointer));
    return iExtension;
    }

#ifdef RD_SCALABLE_UI_V2
//--------------------------------------------------------------------------
// void CLmkIconMap::HandlePointerEventL()
//--------------------------------------------------------------------------
void CLmkIconMap::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    if (AknLayoutUtils::PenEnabled())
        {
        TInt newGridX; // For the whole
        TInt newGridY; // For the whole grid.
        TInt yInPixels = aPointerEvent.iPosition.iY - iGridTopLeft.iY;
        newGridY = yInPixels / iGridItemHeight;
        if ((aPointerEvent.iPosition.iX - iGridTopLeft.iX) < 0)
            {
            newGridX = -1;
            }
        else
            {
            newGridX = (aPointerEvent.iPosition.iX - iGridTopLeft.iX)
                    / iGridItemWidth;
            }
        TInt previousRows = (iRows / iExtension->iMaxVisibleRows)
                * iExtension->iMaxVisibleRows;
        TUint lastRowsLength = iConsArray->Count() % iMaxColumns;

        // The pointer has been set down or dragged into the area of the grid. (it might be in
        // the "white space" at the end of the grid)
        if ((yInPixels >= 0 && yInPixels < iGridItemHeight
                * iExtension->iMaxVisibleRows) &&
        // When the pointer is in rows except the recent icon row
                ((((newGridY + iFirstVisibleRow) != 0) && newGridX
                        < iMaxColumns && newGridX >= 0) ||
                // When the pointer is in the recent icon row
                        ((newGridY + iFirstVisibleRow == 0) && (newGridX
                                < iMaxColumns))))
            {
            // For any action to be taken, the pointer event must either be a Button1Down or a drag event
            // which has originated from a Button1Down in to the grid.
            if (aPointerEvent.iType == TPointerEvent::EButton1Down)
                {
                TUint globalY = newGridY + iFirstVisibleRow;
                // The user tapps a cell which has no icon. it is ignored.
                if (iConsArray->Count() > globalY * iMaxColumns + newGridX)
                    {
                    //If icon is already selected then on single tap it should change the icon.
                    if (iCursorPos.iY == newGridY && iCursorPos.iX
                            == newGridX)
                        {
#ifdef RD_TACTILE_FEEDBACK
                        // The user tapps a cell which has icon, selection has been accepted
                        MTouchFeedback* feedback = MTouchFeedback::Instance();
                        if (feedback)
                            {
                            feedback->InstantFeedback(this,
                                    ETouchFeedbackBasic);
                            }
#endif //RD_TACTILE_FEEDBACK
                        //iIsChangeIcon = ETrue;
                        }
                    else if (iCursorPos.iX + (iCursorPos.iY
                                    + iFirstVisibleRow) * iMaxColumns
                            < iConsArray->Count())
                        {
#ifdef RD_TACTILE_FEEDBACK
                        // The user tapps a cell which has icon, send feedback if position has changed
                        MTouchFeedback* feedback = MTouchFeedback::Instance();
                        const TBool feedbackNeeded = (iCursorPos.iY
                                != newGridY) || (iCursorPos.iX != newGridX);
                        if (feedback && feedbackNeeded)
                            {
                            feedback->InstantFeedback(this,
                                    ETouchFeedbackBasic);
                            }
#endif //RD_TACTILE_FEEDBACK
                        iOldCursorPos = iCursorPos;
                        iCursorPos.iY = newGridY;
                        iCursorPos.iX = newGridX;
                        DrawCursor();
                        }
                    else
                        {
                        iOldCursorPos = iCursorPos;
                        iCursorPos.iY = newGridY;
                        iCursorPos.iX = newGridX;
                        DrawCursor();
                        }
                    }
                }
            else if (aPointerEvent.iType == TPointerEvent::EDrag)
                {
                //Handle drag event only if the icon is not already selected, if already selected then do nothing for EDrag event
                if (iCursorPos.iY != newGridY || iCursorPos.iX != newGridX)
                    {
                    //iIsChangeIcon = EFalse;
                    TUint globalY = newGridY + iFirstVisibleRow;
                    // The user tapps a cell which has no icon. it is ignored.
                    if (iConsArray->Count() > globalY * iMaxColumns
                            + newGridX)
                        {
                        if (iCursorPos.iX
                                + (iCursorPos.iY + iFirstVisibleRow)
                                * iMaxColumns < iConsArray->Count())
                            {
#ifdef RD_TACTILE_FEEDBACK
                            // While dragging cell is changed, give sensitive feedback
                            MTouchFeedback* feedback =
                            MTouchFeedback::Instance();
                            const TBool feedbackNeeded = (iCursorPos.iY
                                    != newGridY) || (iCursorPos.iX
                                    != newGridX);
                            if (feedback && feedbackNeeded)
                                {
                                feedback->InstantFeedback(this,
                                        ETouchFeedbackSensitive);
                                }
#endif //RD_TACTILE_FEEDBACK
                            iOldCursorPos = iCursorPos;
                            iCursorPos.iY = newGridY;
                            iCursorPos.iX = newGridX;
                            DrawCursor();
                            }
                        else
                            {
                            iOldCursorPos.iX = 0;
                            iOldCursorPos.iY = 0;
                            }
                        }
                    }

                }
            else if (aPointerEvent.iType == TPointerEvent::EButton1Up)
                {
                //if (iIsChangeIcon)

                    {
                    iExtension->iObserver->HandleControlEventL(this,
                            MCoeControlObserver::EEventStateChanged);
                    //iIsChangeIcon = EFalse;
                    }
                }
            }
        // Events: Drag and repeat pointer events which are not on the scrollbar are handled here.

        else if (iConsArray->Count() > iMaxColumns
                * iExtension->iMaxVisibleRows && newGridX < iMaxColumns
                && newGridX >= 0 && newGridY < iExtension->iMaxVisibleRows
                && newGridY >= 0)
            {
            TRect ignoreUpRect(TPoint(KMinTInt, KMinTInt), TPoint(KMaxTInt,
                            iGridTopLeft.iY));
            TRect ignoreDownRect(TPoint(KMinTInt, (iGridTopLeft.iY
                                    + iGridItemHeight * iExtension->iMaxVisibleRows)),
                    TPoint(KMaxTInt, KMaxTInt));
            // Drag events
            if (aPointerEvent.iType == TPointerEvent::EDrag
                    || aPointerEvent.iType == TPointerEvent::EButtonRepeat)
                {
                // The pointer is dragged upwards from map
                if (aPointerEvent.iPosition.iY < iGridTopLeft.iY
                        && aPointerEvent.iPosition.iX < iGridTopLeft.iX) //scroll up
                    {
                    // focus on first page
                    if (iFirstVisibleRow == 0)
                        {
                        iFirstVisibleRow = previousRows;
                        // if the last page contains only one partial row.
                        if (lastRowsLength - 1 < iCursorPos.iX && iRows
                                % iExtension->iMaxVisibleRows == 1)
                            {
                            iCursorPos.iX = lastRowsLength - 1;
                            }
                        }
                    // focus on some other page than first
                    else
                        {
                        iFirstVisibleRow -= iExtension->iMaxVisibleRows;
                        }
                    // For odd reason the user has been able to induce upward dragging with out moving
                    // up through the grid.
                    if (iCursorPos.iY + iExtension->iMaxVisibleRows
                            != iFirstVisibleRow)
                        {
                        iCursorPos.iY = 0;
                        }
                    UpdateScrollIndicatorL();
                    if (iExtension->iObserver)
                        {
                        iExtension->iObserver->HandleControlEventL(this,
                                MCoeControlObserver::EEventRequestFocus);
                        }
                    DrawDeferred();
                    }
                // the pointer is dragged downwards from the map.
                else if (yInPixels >= iGridItemHeight
                        * iExtension->iMaxVisibleRows) //scroll down
                    {
                    // The focus is on the last page
                    if (iFirstVisibleRow == previousRows)
                        {
                        iFirstVisibleRow = 0;
                        iCursorPos.iY = iExtension->iMaxVisibleRows - 1;
                        }
                    // The focus is on some other page than the last one.
                    else
                        {
                        iFirstVisibleRow += iExtension->iMaxVisibleRows;
                        // if the next page is the last page
                        if (iFirstVisibleRow == previousRows)
                            {
                            // the old cursor is set to a "safe" position where it at least can be.
                            iOldCursorPos.iX = 0;
                            iOldCursorPos.iY = 0;

                            // if the last page has only one line which isn't filled complitely.
                            if (lastRowsLength - 1 < iCursorPos.iX && iRows
                                    % iExtension->iMaxVisibleRows == 1)
                                {
                                iCursorPos.iX = lastRowsLength - 1;
                                }
                            // If the cursor is in a position where it would go unto a spot with out
                            // a icon when scrolled.
                            if (lastRowsLength > iCursorPos.iX)
                                {
                                //
                                iCursorPos.iY = iRows - 1 - iFirstVisibleRow;
                                }
                            else
                                {
                                //
                                iCursorPos.iY = iRows - 2 - iFirstVisibleRow;
                                }
                            }
                        }
                    UpdateScrollIndicatorL();
                    if (iExtension->iObserver)
                        {
                        iExtension->iObserver->HandleControlEventL(this,
                                MCoeControlObserver::EEventRequestFocus);
                        }
                    DrawDeferred();
                    }
                }
            }

        else // For a non window owning scrollbar.
            {
            CCoeControl::HandlePointerEventL(aPointerEvent);
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconMap::SetObserver
// Sets the observer.
// -----------------------------------------------------------------------------
//
void CLmkIconMap::SetObserver( MCoeControlObserver* aObserver )
    {
    if (iExtension)
        {
        iExtension->iObserver = aObserver;
        }
    }

#endif //RD_SCALABLE_UI_V2
//  End of File
