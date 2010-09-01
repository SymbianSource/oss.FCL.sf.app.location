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
* Description:    LandmarksUi Content File -    CLmkSelectorIconMgr class
*
*/








#ifndef CLMKSELECTORICONMGR_H
#define CLMKSELECTORICONMGR_H

//  INCLUDES
#include "MLmkSelectorIconMgr.h"
#include <e32base.h>
#include <AknsItemID.h>

// FORWARD DECLARATIONS
class CGulIcon;

// CLASS DECLARATION

/**
* CLmkSelectorIconMgr class
*/
class CLmkSelectorIconMgr : public CBase, public MLmkSelectorIconMgr
    {
    public:  // Constructors and destructor

        /**
        * EPOC constructor
        * @param aArray reference to icon array
        * @return newly instantiated object
        */
        static CLmkSelectorIconMgr* NewL( CAknIconArray& aArray );

        /**
        * Destructor.
        */
        ~CLmkSelectorIconMgr();

    public: // From MLmkSelectorIconMgr
        TInt GetIconL( const TDesC& aMbmFileName, TInt aIconIdInFile );
        TInt GetDefaultIconL( TIconType aIcon );
        void SetDefaultIconL( TIconType aIcon,
                              const TDesC& aMbmFileName,
                              TInt aIconIdInFile );
        void ResetMgrL();

    public: // New functions
        /**
        * Some Avkon dialogs use icon at index 0 and 1 for selection
        * marking. Icon manager reserves these indexes to be manually set.
        * @param aMbmFileName icon file path
        * @param aSkinID skin id corresponding to the icon
        * @param aIconId icon id in file
        * @param aMaskId icon mask id in file
        */
        void SetIconAtIndexZeroL( const TDesC& aMbmFileName,
                                  TAknsItemID aSkinID,
                                  TInt aIconId,
                                  TInt aMaskId );
        /**
        * Some Avkon dialogs use icon at index 0 and 1 for selection
        * marking. Icon manager reserves these indexes to be manually set.
        * @param aMbmFileName icon file path
        * @param aSkinID skin id corresponding to the icon
        * @param aIconId icon id in file
        * @param aMaskId icon mask id in file
        */
        void SetIconAtIndexOneL( const TDesC& aMbmFileName,
                                 TAknsItemID aSkinID,
                                 TInt aIconId,
                                 TInt aMaskId );

    private: // new functions
        /**
        * Loads icon from file.
        * @param aMbmFileName icon file path
        * @param aIconId icon id in file
        * @return icon, ownership is transferred to the caller
        */
        CGulIcon* LoadIconL( const TDesC& aMbmFileName, TInt aIconId );

        /**
        * Loads icon from file.
        * @param aMbmFileName icon file path
        * @param aSkinID skin id corresponding to the icon
        * @param aIconId icon id in file
        * @param aMaskId icon mask id in file
        * @return icon, ownership is transferred to the caller
        */
        CGulIcon* LoadIconL( const TDesC& aMbmFileName,
                             TAknsItemID aSkinID,
                             TInt aIconId,
                             TInt aMaskId );

        /**
        * Loads icon from file and appends it to the icon array.
        * @param aMbmFileName icon file path
        * @param aIconIdInFile icon id in file
        * @return array index of loaded icon
        */
        TInt LoadAndAppendIconL( const TDesC& aMbmFileName,
                                 TInt aIconIdInFile );

        /**
        * Defines icon in icon manager. This does not load icon to icon array.
        * This can be used to define icons to mgr without loading them to
        * memory..
        * NOTE: icon mask id must be next from icon id in icon mbg file.
        * Leaves with Symbian error code if error.
        * @param aMbmFileName icon file name
        * @param aIconIdInFile id number of icon in file
        * @return aIconIndex, icon manager id.
        */
        TInt CreateIconMgrItemL( const TDesC& aMbmFileName,
                                 TInt aIconIdInFile );

        /**
        * Appends an icon file to an internal array if it hasn't already
        * been added. Returns index of the icon file in the array.
        * @param aMbmFileName icon file path
        * @return icon file index
        */
        TInt AppendedIconFileIndexL( const TDesC& aMbmFileName );

    private:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aArray
        * @return newly instantiated object
        */
        CLmkSelectorIconMgr( CAknIconArray& aArray );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Type definitions
        class TIconData
            {
            public:
                TInt        iFileIndex;             // icon file
                TInt        iIconIndexWithinFile;   // index within file
                TIconType   iIconType;              // icon type
                TInt        iIconArrayId;           // index in icon array
            public:
                TIconData();
            };

    private:    // Data
        ///Own:
        CDesCArrayFlat* iIconFiles;

        ///Ref:
        CAknIconArray* iArray;

        RArray<TIconData> iCache;
    };

#endif      // CLMKSELECTORICONMGR_H

// End of File
