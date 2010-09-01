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
#include "LmkListProviderFactory.h"
#include "CLmkLandmarkListProvider.h"
#include "CLmkCategoryListProvider.h"
#include "CLmkSearchedLmListProvider.h"
#include "CLmkParsedLmListProvider.h"
#include "CLmkParser.h"
#include <EPos_CPosLandmarkDatabase.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// LmkListProviderFactory::CreateProviderL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkListProviderBase* LmkListProviderFactory::CreateProviderL(
    CPosLandmarkDatabase& aDb,
    TLmkSelectorType aType,
    TLmkProviderType aShowType )
    {
    CLmkListProviderBase* provider = NULL;
    switch ( aType )
        {
        case ELandmarkSelector:
            {
            provider = CLmkLandmarkListProvider::NewL( aDb );
            break;
            }
        case ECategorySelector:
            {
            provider =
                CLmkCategoryListProvider::NewL( aDb, aShowType );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
    return provider;
    }

// ---------------------------------------------------------
// LmkListProviderFactory::CreateProviderL
// ---------------------------------------------------------
//
EXPORT_C CLmkListProviderBase* LmkListProviderFactory::CreateProviderL(
    CPosLandmarkDatabase& aDb,
    CPosLmSearchCriteria& aCriteria )
    {
    CLmkListProviderBase* provider =
        CLmkSearchedLmListProvider::NewL( aDb, aCriteria );
    return provider;
    }

// ---------------------------------------------------------
// LmkListProviderFactory::CreateProviderL
// ---------------------------------------------------------
//
EXPORT_C CLmkListProviderBase* LmkListProviderFactory::CreateProviderL(
    CLmkParser& aParser )
    {
    CLmkListProviderBase* provider =
        CLmkParsedLmListProvider::NewL( aParser );
    return provider;
    }

//  End of File
