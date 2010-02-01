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
#include "CLmkUiItemBase.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkAOOperation::CLmkAOOperation
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkUiItemBase::CLmkUiItemBase()
    : iExtExists( EFalse )
    {
    }

// ---------------------------------------------------------
// CLmkUiItemBase::~CLmkUiItemBase
// ---------------------------------------------------------
//
CLmkUiItemBase::~CLmkUiItemBase()
    {
    }

// ---------------------------------------------------------
// CLmkUiItemBase::ExtensionIntExists
// ---------------------------------------------------------
//
EXPORT_C TBool CLmkUiItemBase::ExtensionIntExists()
    {
    return iExtExists;
    }

// ---------------------------------------------------------
// CLmkUiItemBase::SetExtensionInt
// ---------------------------------------------------------
//
EXPORT_C void CLmkUiItemBase::SetExtensionInt( TInt aInt )
    {
    iExtInt = aInt;
    iExtExists = ETrue;
    }

// ---------------------------------------------------------
// CLmkUiItemBase::ExtensionInt
// ---------------------------------------------------------
//
EXPORT_C TInt CLmkUiItemBase::ExtensionInt()
    {
    return iExtInt;
    }

//  End of File
