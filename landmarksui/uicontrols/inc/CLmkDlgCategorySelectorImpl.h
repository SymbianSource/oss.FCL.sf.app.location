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
* Description:    LandmarksUi Content File -    Common UI's (dialog-based) category selector impl class.
*
*/







#ifndef CLMKDLGCATEGORYSELECTORIMPL_H
#define CLMKDLGCATEGORYSELECTORIMPL_H

//  INCLUDES
#include "CLmkDlgSelectorImplBase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Common UI's (dialog-based) category selector impl class.
*
*/
class CLmkDlgCategorySelectorImpl : public CLmkDlgSelectorImplBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aDb database reference
        * @param aShowEmptyCategories if ETrue also empty categories are shown
        * @return newly instantiated object
        */
        IMPORT_C static CLmkDlgCategorySelectorImpl* NewL(
                                    CPosLandmarkDatabase& aDb,
                                    TBool aShowEmptyCategories,
                                    TBool aShowBlankNaviPane = EFalse);

        /**
        * Destructor.
        */
        ~CLmkDlgCategorySelectorImpl();

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
        * @param aShowEmptyCategories
        * @return newly instantiated object
        */
        CLmkDlgCategorySelectorImpl( CPosLandmarkDatabase& aDb,
                                  	 TBool aShowEmptyCategories,
                                	 TBool aShowBlankNaviPane );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        ///
        TBool iShowEmptyCategories;
        TBool iShowBlankNaviPane;
    };

#endif      // CLMKDLGCATEGORYSELECTORIMPL_H

// End of File
