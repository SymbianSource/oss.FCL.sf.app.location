/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A factory class which can create instances of
*                MBlidSettings.
*
*/


// INCLUDE FILES
#include "BlidModelFactory.h"
#include "CBlidSettings.h"
#include "CBlidLocation.h"
#include "CBlidRouter.h"

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// BlidModelFactory::SettingsL
// ----------------------------------------------------------------------------
//
MBlidSettings* BlidModelFactory::SettingsL()
    {
    return CBlidSettings::NewL();
    }

// ----------------------------------------------------------------------------
// BlidModelFactory::LocationL
// ----------------------------------------------------------------------------
//
MBlidLocation* BlidModelFactory::LocationL( RPositionServer& aServer, CBlidEng& aEngine )
    {
    return CBlidLocation::NewL(aServer, aEngine);
    }

// ----------------------------------------------------------------------------
// BlidModelFactory::RouterL
// ----------------------------------------------------------------------------
//
MBlidRouter* BlidModelFactory::RouterL()
    {
    return CBlidRouter::NewL();
    }

//  End of File
