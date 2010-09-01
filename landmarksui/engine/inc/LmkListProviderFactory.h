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
* Description:    LandmarksUi Content File -    List provider factory
*
*/








#ifndef LMKLISTPROVIDERFACTORY_H
#define LMKLISTPROVIDERFACTORY_H

//  INCLUDES
#include "landmarks.hrh"
#include <e32base.h>

// FORWARD DECLARATIONS
//class CLmkLmItemListProvider;
class CLmkListProviderBase;
class CPosLandmarkDatabase;
class CPosLmSearchCriteria;
class CLmkParser;

//ENUMERATION
enum TLmkProviderType
    {
    ELmkNotShowEmptyCategories,
    ELmkShowEmptyCategories,
    ELmkShowExtendedCategories
    };

// CLASS DECLARATION

/**
* LmkListProviderFactory class
*/

class LmkListProviderFactory
    {
    public: // New functions
        /**
        * Create LmkListProviderFactory object
        * @param aDb
        * @param aType
        * @param aShowType has meaning only if
        *        aType == ECategorySelector
        * @return newly instantiated object
        */
        IMPORT_C static CLmkListProviderBase* CreateProviderL(
                                        CPosLandmarkDatabase& aDb,
                                        TLmkSelectorType aType,
                                        TLmkProviderType aShowType =
                                        ELmkNotShowEmptyCategories );

        /**
        * Creates filtered lm provider.
        * @param aDb
        * @param aCriteria
        * @return newly instantiated object
        */
        IMPORT_C static CLmkListProviderBase* CreateProviderL(
                                        CPosLandmarkDatabase& aDb,
                                        CPosLmSearchCriteria& aCriteria );

        /**
        * Creates parsered lm provider.
        * @param aParser
        * @return newly instantiated object
        */
        IMPORT_C static CLmkListProviderBase* CreateProviderL(
                                        CLmkParser& aParser );
    };

#endif      // LMKLISTPROVIDERFACTORY_H

// End of File
