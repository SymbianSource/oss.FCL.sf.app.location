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
* Description:    LandmarksUi Content File -    CLmkDlgLmSelectorImpl class
*
*/







#ifndef CLMKDLGLMSELECTORIMPL_H
#define CLMKDLGLMSELECTORIMPL_H

//  INCLUDES
#include "CLmkDlgSelectorImplBase.h"
#include "MLmkListProviderObserver.h"
#include <eikcmobs.h>       // MEikCommandObserver

// FORWARD DECLARATIONS
class CPosLmSearchCriteria;

// CLASS DECLARATION

/**
* CLmkDlgLmSelectorImpl class
*/
class CLmkDlgLmSelectorImpl : public CLmkDlgSelectorImplBase
    {
    public:  // Constructors and destructor
        /**
        * Create CLmkDlgLmSelectorImpl object
        * @param aDb
        * @return newly instantiated object
        */
        IMPORT_C static CLmkDlgLmSelectorImpl* NewL(
                            CPosLandmarkDatabase& aDb );

        /**
        * Create CLmkDlgLmSelectorImpl object
        * @param aDb
        * @param aCriteria
        * @return newly instantiated object
        */
        IMPORT_C static CLmkDlgLmSelectorImpl* NewL(
                                            CPosLandmarkDatabase& aDb,
                                            CPosLmSearchCriteria* aCriteria );

        /**
        * Destructor.
        */
        ~CLmkDlgLmSelectorImpl();

    public: // From CLmkSelectorImplBase
        void SetupListProviderL();
        const CLmkListProviderBase& ListProvider() const;
        void GetDlgResources( TBool aIsSingleSelector,
                              TInt& aTitlePaneResource,
                              TInt& aMenuBarResource,
                              TInt& aDialogResource ) const;

    protected: // From MLmkListProviderObserver
        void HandleListProviderEvent( TLmkListProviderEventType aEvent );
        void HandleListProviderError( TInt aError );

    protected:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb
        * @return newly instantiated object
        */
        CLmkDlgLmSelectorImpl( CPosLandmarkDatabase& aDb );

        /**
        * C++ default constructor.
        * @param aDb
        * @parma aCriteria
        * @return newly instantiated object
        */
        CLmkDlgLmSelectorImpl( CPosLandmarkDatabase& aDb,
                               CPosLmSearchCriteria* aCriteria );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /**
        * Updates the Navi Label, based on the category id
        * Used only when gets ELmkEventCategoryUpdated event.
        */
        void UpdateNaviLabelL();
        /**
        * This function is called to check if the criteria based category
        * exists in the Landmarks database or not, this check is required,
        * since the category might have been deleted from the landmarks,
        * Hence the Category contents dialog which is open needs to be
        * closed.
        */
        TBool CheckDbIfCriteriaCategoryExistsL();
    private:    // Data
        ///Own:
        CPosLmSearchCriteria* iCriteria;
    };

#endif      // CLMKDLGLMSELECTORIMPL_H

// End of File
