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
* Description:   	LandmarksUi Content File
*				Application's (view-based) category selector impl class.
*
*/







#ifndef CLMKAPPCATEGORYSELECTORIMPL_H
#define CLMKAPPCATEGORYSELECTORIMPL_H

//  INCLUDES
#include "CLmkAppLmItemSelectorImpl.h"
#include <coecobs.h>

// FORWARD DECLARATIONS
class CLmkAppLmSelectorImpl;
class CAknSearchField;

// CLASS DECLARATION

/**
*  Application's (view-based) category selector impl class.
*
*/
class CLmkAppCategorySelectorImpl : public CLmkAppLmItemSelectorImpl
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aDb database reference
        * @param aSender landmark sender reference
        * @param aFindBox
        * @return newly instantiated object
        */
        IMPORT_C static CLmkAppCategorySelectorImpl* NewL(
                                    CPosLandmarkDatabase& aDb,
                                    CLmkSender& aSender,
                                    TBool aFindBox );

        /**
        * Destructor.
        */
        ~CLmkAppCategorySelectorImpl();

    public: // From CLmkSelectorImpl
        void SetupListProviderL();
        const CLmkListProviderBase& ListProvider() const;
        IMPORT_C TInt CurrentCategoryId();

    public: //from MLmkMapAndNavigationObserver
        void HandleLmCreated(TPosLmItemId aNewLmkItemId);

    protected: //from MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                              TReal32 aProgress,
                              TInt aStatus );

    protected: // from MCoeControlObserver
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );

    protected: // From MEikCommandObserver
        void ProcessCommandL( TInt aCommandId );

    protected: // From CLmkAppLmItemSelectorImpl
        void ChangeIconL( TPosLmItemId aId, TInt aIconIndex, TInt aMaskIndex);
        void ChangeIconsL( RArray<TPosLmItemId>& aIdArray, TInt aIconIndex, TInt aMaskIndex);

    protected: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb database reference
        * @param aSender landmark sender reference
        * @param aFindBox
        */
        CLmkAppCategorySelectorImpl( CPosLandmarkDatabase& aDb,
                                     CLmkSender& aSender,
                                     TBool aFindBox );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
    	CAknSearchField* iFindBox;
    };

#endif      // CLMKAPPCATEGORYSELECTORIMPL_H

// End of File
