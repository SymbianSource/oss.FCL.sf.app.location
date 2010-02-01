/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Interface for selector icon manager
*
*/








#ifndef MLMKSELECTORICONMGR_H
#define MLMKSELECTORICONMGR_H

//  INCLUDES
#include <badesca.h>

// FORWARD DECLARATIONS
class CAknIconArray;

// CLASS DECLARATION

/**
* MLmkSelectorIconMgr class
*/
class MLmkSelectorIconMgr
    {
    public:

        enum TIconType {
            EDefinedIcon = 0, // any non-default icon
            ELandmarkDefaultIcon,
            ECategoryDefaultIcon,
            ERestaurantDefaultIcon,
            EBusStopDefaultIcon
            };

    public: // New functions
        /**
        * Loads icon from file to icon array from parameter file and file
        * index. For subsequent calls icon identification data is cached
        * so same index in array is returned without reloading data from file.
        * NOTE: icon mask id must be next from icon id in icon mbg file.
        * Leaves with Symbian error code if error.
        * @param aMbmFileName icon file name
        * @param aIconIdInFile id number of icon in file
        * @return icon array index
        */
        virtual TInt GetIconL( const TDesC& aMbmFileName,
                               TInt aIconIdInFile ) = 0;

        /**
        * Loads icon based on icon type. Returns KErrNotFound if not available.
        * @param aIcon icon type.
        * @return icon array index.
        */
        virtual TInt GetDefaultIconL( TIconType aIcon ) = 0;

        /**
        * Sets the default icon for a given type.
        * If default icon has already been set this method
        * leaves with KErrAlreadyExists.
        * @param aIcon type of the icon
        * @param aMbmFileName icon file name
        * @param aIconIdInFile id number of icon in file
        */
        virtual void SetDefaultIconL( TIconType aIcon,
                                      const TDesC& aMbmFileName,
                                      TInt aIconIdInFile ) = 0;

        /**
        * Resets icon manager. Resets and destroyes icon array content and
        * deletes all icon manager data, including default icons.
        */
        virtual void ResetMgrL() = 0;

// TODO: some method like InvalidateLmItemIconsL should be added
// which could be used when database is updated. Otherwise more icons
// are just loaded to icon manager.
    };

#endif      // MLMKSELECTORICONMGR_H

// End of File
