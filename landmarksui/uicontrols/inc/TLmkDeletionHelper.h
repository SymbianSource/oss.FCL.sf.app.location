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
* Description:    LandmarksUi Content File -    This class helps updating the listbox after deleting item(s).
*
*/







#ifndef TLMKDELETIONHELPER_H
#define TLMKDELETIONHELPER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CEikListBox;

// CLASS DECLARATION

/**
* This class offers an easy way to handle list update when user is
* deleting items. This class does not own heap data.
*/
class TLmkDeletionHelper
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TLmkDeletionHelper();

        /**
        * Destructor.
        */
        virtual ~TLmkDeletionHelper();

    public: // New functions

        /**
        * Store list information when use has requested deleting one
        * or more items.
        * @param aListBox reference to the listbox
        * @param aMultipleItems ETrue if user is deleting multiple items
        */
        void StoreListInformation( CEikListBox& aListBox,
                                   TBool aMultipleItems );

        /**
        * Updates the list using AknListBoxUtils if delete happened,
        * i.e. StoreListInformation() has been previously called.
        * @return ETrue if list was updated, EFalse otherwise
        */
        TBool UpdateListIfDeleteHappenedL();

    private:    // Data
        /// marked ETrue when starting to delete
        TBool iUserRequestedDelete;

        /// required by AknListBoxUtils
        TInt iCurrentItemIndexBeforeRemoval;

        /// required by AknListBoxUtils
        TBool iCurrentItemWasRemoved;

        ///Ref: required by AknListBoxUtils
        const CArrayFix<TInt>* iIndexesToRemovedItems;

        ///Ref: list to be updated
        CEikListBox* iListBox;
    };

#endif      // TLMKDELETIONHELPER_H

// End of File