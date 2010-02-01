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
* Description:    LandmarksUi Content File -    Empty Dialog.....
*
*/







// Class include
#include "CLmkEmptyDialog.h"
#include <eikspane.h>
#include <avkon.hrh>
#include <akntitle.h>
#include <aknViewAppUi.h>

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CLmkEmptyDialog::CLmkEmptyDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkEmptyDialog::CLmkEmptyDialog()
{
}

// ----------------------------------------------------------------------------
// CLmkEmptyDialog::~CLmkEmptyDialog()
// ----------------------------------------------------------------------------
//
CLmkEmptyDialog::~CLmkEmptyDialog()
{
}

// ----------------------------------------------------------------------------
// CLmkEmptyDialog::PostLayoutDynInitL()
// ----------------------------------------------------------------------------
//
void CLmkEmptyDialog::PostLayoutDynInitL()
	{
	CAknDialog::PostLayoutDynInitL();
	}


// ----------------------------------------------------------------------------
// CLmkEmptyDialog::PreLayoutDynInitL()
// ----------------------------------------------------------------------------
//
void CLmkEmptyDialog::PreLayoutDynInitL()
    {
    CAknDialog::PreLayoutDynInitL();
    }

// ----------------------------------------------------------------------------
// CLmkEmptyDialog::SetSizeAndPosition()
// ----------------------------------------------------------------------------
//
void CLmkEmptyDialog::SetSizeAndPositionL(const TSize& /*aSize*/)
    {
    CEikStatusPane * statusPane=iEikonEnv->AppUiFactory()->StatusPane();
    TRect lStatusRect = statusPane->PaneRectL(TUid::Uid(EEikStatusPaneUidNavi));

    TRect lTitleRect = statusPane->PaneRectL(TUid::Uid(EEikStatusPaneUidTitle));

    CWindowGc& gc=SystemGc();
    CGraphicsDevice* device = gc.Device();
    TSize lScreenHight = device->SizeInPixels();

    TInt lHeight =  lScreenHight.iHeight - lTitleRect.Height() - lStatusRect.Height();
    TInt lStartY =  lTitleRect.Height() + lStatusRect.Height();

    TSize size(lScreenHight.iWidth,lHeight);
    TPoint point(0,lStartY);

    CEikDialog::SetSizeAndPosition(size);
    SetPosition(point);
    }
// ----------------------------------------------------------------------------
// CLmkEmptyDialog::OkToExitL()
// ----------------------------------------------------------------------------
//
TBool CLmkEmptyDialog::OkToExitL( TInt /*aButtonId*/)
    {
    return (ETrue);
    }

// ----------------------------------------------------------------------------
// CLmkEmptyDialog::EmptyDialogExitL()
// ----------------------------------------------------------------------------
//
void CLmkEmptyDialog::EmptyDialogExitL()
    {
    TryExitL(-1);
    }

