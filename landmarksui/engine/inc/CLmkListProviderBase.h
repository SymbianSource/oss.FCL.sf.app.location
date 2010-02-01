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
* Description:    LandmarksUi Content File -    Base class for landmark list provider
*
*/








#ifndef CLMKLISTPROVIDERBASE_H
#define CLMKLISTPROVIDERBASE_H

//  INCLUDES
#include "MLmkListProviderObserver.h"
#include <e32base.h>        // CBase

// FORWARD DECLARATIONS
class CLmkUiItemBase;
class MLmkSelectorIconMgr;

// CLASS DECLARATION

/**
* CLmkListProviderBase class
*/
class CLmkListProviderBase : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Don't delete during asynchronous operation, otherwise
        * observer(s) won't be notified!
        */
        ~CLmkListProviderBase();

    public: // New functions
        /**
        * Asynchronous initialization.
        */
        virtual void InitializeL() = 0;

        /**
        * Return item at the specified position within the array.
        * @param aIndex
        * @return CLmkUiItemBase pointer
        */
        virtual CLmkUiItemBase* ItemAtL( TInt aIndex ) = 0;

        /**
        * Returns the number of elements held in the array.
        * @return TInt
        */
        virtual TInt ItemCount() const = 0;

        virtual TBool StartSearchingLandmarksL(const TDesC& aSearchPattern,
    								  TBool aSearchOnlyInPreviousMatches) = 0;

        /**
        * Add observer
        * @param aObserver
        */
        IMPORT_C void AddObserverL( MLmkListProviderObserver& aObserver );

        /**
        * Remove observer
        * @return KErrNotFound if observer wasn't found, KErrNone otherwise
        */
        IMPORT_C TInt RemoveObserver( MLmkListProviderObserver& aObserver );

        /**
        * Must be called in order to have icons available. Can be called
        * repeatedly to take new icon mgr into use.
        * @param aIconMgr
        */
        IMPORT_C void SetIconMgrL( MLmkSelectorIconMgr& aIconMgr );

    protected:  // New functions
        /**
        * Notify observer
        * @param aEvent
        */
        void NotifyObservers( TLmkListProviderEventType aEvent );

        /**
        * Notify errors to observer
        * @param aError
        */
        void NotifyError( TInt aError );

    protected:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CLmkListProviderBase();

        /**
        * Symbian 2nd phase constructor
        */
        void BaseConstructL();

    protected:  // Data
        ///Ref:
        MLmkSelectorIconMgr* iIconMgr;

    private:    // Data
        // observers not own
        RPointerArray<MLmkListProviderObserver> iObservers;
    };

#endif      // CLMKLISTPROVIDERBASE_H

// End of File
