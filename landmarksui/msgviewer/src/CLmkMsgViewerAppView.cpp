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
* Description:    LandmarksUi Content File -    Application view control.
*
*/








// INCLUDE FILES
#include <coemain.h>
#include <aknlists.h>
#include <barsread.h>

#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>

#include "CLmkMsgViewerAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkMsgViewerAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkMsgViewerAppView* CLmkMsgViewerAppView::NewL( const TRect& aRect )
    {
    CLmkMsgViewerAppView* self = CLmkMsgViewerAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkMsgViewerAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkMsgViewerAppView* CLmkMsgViewerAppView::NewLC( const TRect& aRect )
    {
    CLmkMsgViewerAppView* self = new ( ELeave ) CLmkMsgViewerAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkMsgViewerAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkMsgViewerAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect( aRect );

    // Get the skin instance
    iSkinInstance = AknsUtils::SkinInstance();

    // Create background control context for skinning the background
    iBackgroundSkinContext = CAknsBasicBackgroundControlContext::NewL(
                    KAknsIIDQsnBgAreaMain, aRect, ETrue );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CLmkMsgViewerAppView::CLmkMsgViewerAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CLmkMsgViewerAppView::CLmkMsgViewerAppView()
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CLmkMsgViewerAppView::~CLmkMsgViewerAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CLmkMsgViewerAppView::~CLmkMsgViewerAppView()
    {
    delete iBackgroundSkinContext;
    CloseWindow();
    }

// -----------------------------------------------------------------------------
// CLmkMsgViewerAppView::CountComponentControls()
// -----------------------------------------------------------------------------
//
TInt CLmkMsgViewerAppView::CountComponentControls() const
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CLmkMsgViewerAppView::ComponentControl()
// -----------------------------------------------------------------------------
//
CCoeControl* CLmkMsgViewerAppView::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CLmkMsgViewerAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CLmkMsgViewerAppView::Draw( const TRect& /*aRect*/ ) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    AknsDrawUtils::Background(
        iSkinInstance, iBackgroundSkinContext, this, gc, Rect());
    }

// End of File
