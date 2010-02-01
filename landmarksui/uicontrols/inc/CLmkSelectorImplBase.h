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
* Description:    LandmarksUi Content File -    Selector functionality common to both application (view-based) and
                 common ui components (dialog-based).
*
*/







#ifndef CLMKSELECTORIMPLBASE_H
#define CLMKSELECTORIMPLBASE_H

//  INCLUDES
#include "MLmkListProviderObserver.h"
#include <EPos_Landmarks.h> // Lm typedefs, constants etc.

// FORWARD DECLARATIONS
class CPosLandmarkDatabase;
class CLmkListProviderBase;
class CLmkSelectorIconMgr;
class CAknIconArray;
class CLmkUiUtils;
class MLmkListMemento;
class CEikListBox;
class CEikonEnv;

// CLASS DECLARATION

/**
*  Selector functionality common to both application (view-based) and
*  common ui components (dialog-based).
*
*/
class CLmkSelectorImplBase
: public CBase,
  public MLmkListProviderObserver
    {
    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        ~CLmkSelectorImplBase();

    public: // New functions
        /**
        * Sets memento to be used by this class. Memento's
        * lifetime must be at least as long as this class's lifetime.
        * @param aMemento
        */
        IMPORT_C void SetMemento( MLmkListMemento& aMemento );

        /**
        * Creates a memento (but does not set it with SetMemento()).
        * @return created memento, ownership is transferred
        */
        virtual MLmkListMemento* MementoL() = 0;

        /**
        * Template method implemented by the subclass and called by
        * the base class.
        */
        virtual void SetupListProviderL() = 0;

        /**
        * Template method implemented by the subclass and called by
        * the base class. Needed since composite selector doesn't
        * have own list provider but it utilizes other providers.
        * @return CLmkListProviderBase reference
        */
        virtual const CLmkListProviderBase& ListProvider() const = 0;

        /**
        * Return number of visible items
        * @return TInt
        */
        virtual TInt ListVisibleItemCount() = 0;

        /**
        * Return number of marked items
        * @return TInt
        */
        virtual TInt ListMarkedItemCountL() = 0;

    protected: // New functions
        /**
        * Helper method which creates icon array and icon manager, sets
        * default icons and gives icon manager reference to list provider.
        * @return created icon array, left in the cleanup stack
        */
        CAknIconArray* SetupIconMgrLC();

        /**
        * Helper method for storing listbox state.
        * @param aListbox
        * @param aProvider
        * @param aFiltered
        */
        void Store( const CEikListBox& aListBox,
                    const CLmkListProviderBase& aProvider,
                    TBool aFiltered );

        /**
        * Helper method for restoring listbox state
        * @param aListbox
        * @param aProvider
        * @param aFiltered
        */
        void Restore( const CEikListBox& aListBox,
                      const CLmkListProviderBase& aProvider,
                      TBool aFiltered );

    protected: // From MLmkListProviderObserver
        void HandleListProviderEvent( TLmkListProviderEventType aEvent );

    protected: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb
        * @return newly instantiated object
        */
        CLmkSelectorImplBase( CPosLandmarkDatabase& aDb );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();

    protected: // Data

        ///Ref:
        CPosLandmarkDatabase& iDb;

        ///Ref:
        MLmkListMemento* iMemento;

        ///Own:
        CLmkListProviderBase* iListProvider;

        ///Own:
        CLmkSelectorIconMgr* iIconMgr;

        ///Own:
        CLmkUiUtils* iLmkUiUtils;

    private: // Internal classes

        // This struct-like class is used for postponing memento.
        // It does not own any heap data.
        class TLmkRestoreData
            {
            public: // Constructor:
                TLmkRestoreData();

            public: // Public data:
                TBool iRestoreRequested;
                const CEikListBox* iListBox;
                const CLmkListProviderBase* iProvider;
                TBool iFiltered;
            };

    private: // Data
        // These should not be used by subclasses, their content is
        // guaranteed to be correct only within this class.

        ///Latest provider event recorded by this class
        TLmkListProviderEventType iSelectorImplBaseLatestListEvent;

        ///Contains information for possible postponed memento operation
        TLmkRestoreData iRestoreData;

    protected:
        // Ref
        CEikonEnv*      iEnv;
    };

#endif      // CLMKSELECTORIMPLBASE_H

// End of File
