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
* Description:    LandmarksUi Content File -
*
*/









// INCLUDE FILES
#include <AknPopupHeadingPane.h>
#include "CLmkIconSelectorPopup.h"
#include "LmkConsts.h"
#include "CLmkUiUtils.h"
#include <akniconarray.h>
#include <gulicon.h>
#include <AknLists.h>
#include <Lmkui.rsg>
#include <aknselectionlist.h>
#include <AknIconUtils.h>
#include <AknUtils.h>
#include <AknLayoutScalable_Avkon.cdl.h>
#include <LayoutMetaData.cdl.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
    _LIT( KFormat, "%d\t" );
//    const TInt KColorIndex( 215 );
    const TInt KNumOfColumns( 9 );
    const TInt KNumOfRows( 8 );
    const TInt KWidth( 18 );
    const TInt KHeight( 20 );
    const TInt KTextLength( 50 );
    const TInt KNumOfIcons( 16 );

}  // namespace
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkIconSelectorPopup::CLmkIconSelectorPopup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkIconSelectorPopup::CLmkIconSelectorPopup() :
    iNumOfColumns( KNumOfColumns ),
    iNumOfRows( KNumOfRows ),
    iSizeOfItems( TSize( KWidth, KHeight ) )
    {
    }

// -----------------------------------------------------------------------------
// CLmkIconSelectorPopup::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkIconSelectorPopup* CLmkIconSelectorPopup::NewL( CAknGrid& aGrid )
    {
    CLmkIconSelectorPopup* self =
        new( ELeave ) CLmkIconSelectorPopup( );

    CleanupStack::PushL( self );
    self->ConstructL( aGrid );
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkIconSelectorPopup::ConstructL
// ----------------------------------------------------
//
void CLmkIconSelectorPopup::ConstructL( CAknGrid& aGrid )
	{
	iEnv = CEikonEnv::Static();
    iGrid = &aGrid;
	}

// ----------------------------------------------------
// CLmkIconSelectorPopup::~CLmkIconSelectorPopup
// ----------------------------------------------------
//
CLmkIconSelectorPopup::~CLmkIconSelectorPopup()
    {
    }

// ----------------------------------------------------
// CLmkIconSelectorPopup::ExecuteLD
// ----------------------------------------------------
//
TBool CLmkIconSelectorPopup::ExecuteLD ()
	{
    CAknPopupList* popupList= CAknPopupList::NewL(
        iGrid, R_AVKON_SOFTKEYS_SELECT_CANCEL, AknPopupLayouts::EMenuGraphicWindow );
    CleanupStack::PushL( popupList );

	iGrid->ConstructL( popupList, CEikListBox::EPopout);

	TRect clientRect;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, clientRect);

	TAknLayoutRect main_pane;
	main_pane.LayoutRect(
		clientRect, AknLayoutScalable_Avkon::main_pane(2));

	TAknLayoutRect popup_grid_graphic_window;
	popup_grid_graphic_window.LayoutRect(
		clientRect, AknLayoutScalable_Avkon::popup_grid_graphic_window(0));

	popupList->SetRect(main_pane.Rect());

	// Calculate the size relatively
	TRect relativeDialog(TPoint(0,0),popup_grid_graphic_window.Rect().Size());

	// Get the layout of the actual character grid with scrollbar
    TAknLayoutRect listscroll_popup_graphic_pane;

    listscroll_popup_graphic_pane.LayoutRect(relativeDialog,
    	AknLayoutScalable_Avkon::listscroll_popup_graphic_pane());

	TAknLayoutRect grid_graphic_popup_pane;

	grid_graphic_popup_pane.LayoutRect(
		listscroll_popup_graphic_pane.Rect(),
		AknLayoutScalable_Avkon::grid_graphic_popup_pane(1));

	TRect popupGridRect = grid_graphic_popup_pane.Rect();

    iGrid->SetRect(popupGridRect);

    iSizeOfItems.iWidth = popupGridRect.Width() / iNumOfColumns;
    iSizeOfItems.iHeight = popupGridRect.Height() / iNumOfRows;

	// adjust the size of cells to the number of columns.
	iGrid->SetLayoutL( EFalse, ETrue, ETrue, iNumOfColumns,
                       iNumOfRows, iSizeOfItems );

    iGrid->SetPrimaryScrollingType(
                CAknGridView::EScrollIncrementLineAndLoops );
    iGrid->SetSecondaryScrollingType(
                CAknGridView::EScrollIncrementLineAndLoops );

	LoadGraphicsL();
    // create data
    AddDataL( KNumOfIcons );

    iGrid->CreateScrollBarFrameL(EFalse);
    iGrid->ScrollBarFrame()->SetScrollBarVisibilityL(
                                CEikScrollBarFrame::EOff,
                                CEikScrollBarFrame::EOff);


	// add title
    HBufC* title = iEnv->AllocReadResourceL( R_LMK_ICON_SELECTOR_TITLE );
    CleanupStack::PushL( title );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title ); //title
    title = NULL;

    CleanupStack::Pop(); //popupList
	TInt popupOk = popupList->ExecuteLD();
    delete this;
    return popupOk;
	}

// ----------------------------------------------------
// CLmkIconSelectorPopup::LoadGraphicsL
// ----------------------------------------------------
//
void CLmkIconSelectorPopup::LoadGraphicsL()
    {
    TFileName iconFile( CLmkUiUtils::LmkUiIconFileL() );

    CArrayPtr< CGulIcon >* icons = new( ELeave ) CAknIconArray(KNumOfIcons);
    CleanupStack::PushL( icons );

    // Create icon bitmap and mask.
    for ( TInt i(0); i < ( KNumOfIcons*2 ); i++ )
        {

/*******************************************************************/
			CFbsBitmap* bitmap = NULL;
			CFbsBitmap* bitmapMask = NULL;

			AknIconUtils::CreateIconLC(bitmap, bitmapMask, iconFile,
                             EMbmLmkuiQgn_prop_lm_transport + i, EMbmLmkuiQgn_prop_lm_transport + i + 1);
			i++;
			AknIconUtils::SetSize(bitmap, iSizeOfItems);  	//fix
			AknIconUtils::SetSize(bitmapMask, iSizeOfItems); //fix
			CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );
	        CleanupStack::PushL( icon );
		    icons->AppendL( icon );
			CleanupStack::Pop(3); // icon, mask, bitmap
/*******************************************************************/
        }

    // Set icon array.
    iGrid->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    CleanupStack::Pop(); // icons array
    }

// ----------------------------------------------------
// CLmkIconSelectorPopup::AddDataL
// ----------------------------------------------------
//
void CLmkIconSelectorPopup::AddDataL( TInt aNum )
    {
    MDesCArray* array = iGrid->Model()->ItemTextArray();
    CDesCArray* cArray = ( CDesCArray* )array;

    // Makes text which is added to.
    TBuf< KTextLength > number;
    TBuf< KTextLength > format( KFormat );

    // The number of Grids.
    TInt numberOfData = iGrid->GridModel()->NumberOfData() ;

    // Add the data to the last
    for ( TInt i = numberOfData; i < aNum + numberOfData; i++ )
        {
        number.Format( format, i );
        cArray->AppendL( number );
        }

    // Inform list box that data added
    iGrid->HandleItemAdditionL();

    // If item size changes, need to alter the item style
    ApplySelGridGraphicStyleL();

    }

// ----------------------------------------------------
// CLmkIconSelectorPopup::ApplySelGridGraphicStyleL
// ----------------------------------------------------
//
void CLmkIconSelectorPopup::ApplySelGridGraphicStyleL()
    {

    if ( !iGrid )
        {
        return;
        }

    AknListBoxLayouts::SetupStandardGrid( *iGrid );

    TInt grWidth( iGrid->ColumnWidth() );
    TInt grHeight( iGrid->ItemHeight() );
    // layout of the graphic
    AknListBoxLayouts::SetupGridFormGfxCell( *iGrid,
            iGrid->ItemDrawer(),
            0, 0, 0, 0, 0,
            grWidth,
            grHeight,
            TPoint( 0, 0 ),
            TPoint( grWidth, grHeight ) );
    }
void CLmkIconSelectorPopup::HandleScreenSizeChange()
	{
	TRect clientRect;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, clientRect);
	TAknLayoutRect popup_grid_graphic_window;
	popup_grid_graphic_window.LayoutRect(
		clientRect, AknLayoutScalable_Avkon::popup_grid_graphic_window(0));

	// Calculate the size relatively
	TRect relativeDialog(TPoint(0,0),popup_grid_graphic_window.Rect().Size());

	// Get the layout of the actual character grid with scrollbar
    TAknLayoutRect listscroll_popup_graphic_pane;

    listscroll_popup_graphic_pane.LayoutRect(relativeDialog,
    	AknLayoutScalable_Avkon::listscroll_popup_graphic_pane());

	TAknLayoutRect grid_graphic_popup_pane;

	grid_graphic_popup_pane.LayoutRect(
		listscroll_popup_graphic_pane.Rect(),
		AknLayoutScalable_Avkon::grid_graphic_popup_pane(1));

	TRect popupGridRect = grid_graphic_popup_pane.Rect();

	TSize sizeOfItems(popupGridRect.Width() / KNumOfColumns,
					  popupGridRect.Height() / KNumOfRows);

	// adjust the size of cells to the number of columns.
	TRAP_IGNORE(iGrid->SetLayoutL( EFalse, ETrue, ETrue, KNumOfColumns,
                       KNumOfRows, sizeOfItems ));

    CArrayPtr< CGulIcon >* icons =
    	iGrid->ItemDrawer()->FormattedCellData()->IconArray();

    for ( TInt i(0); i < icons->Count(); i++ )
        {
        	CGulIcon *icon = icons->At(i);
			CFbsBitmap* bitmap = icon->Bitmap();
			CFbsBitmap* bitmapMask = icon->Mask();
			AknIconUtils::SetSize(bitmap, sizeOfItems);  	//fix
			AknIconUtils::SetSize(bitmapMask, sizeOfItems); //fix
        }
	TRAP_IGNORE(ApplySelGridGraphicStyleL());
	}
//  End of File
