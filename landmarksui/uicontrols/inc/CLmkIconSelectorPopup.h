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
* Description:    LandmarksUi Content File -    Icon selector popup.
*
*/








#ifndef __CLMKICONSELECTORPOPUP_H__
#define __CLMKICONSELECTORPOPUP_H__

//  INCLUDES
#include <AknGrid.h>

// CLASS DECLARATION
/**
* CLmkIconSelectorPopup class
*/
class CLmkIconSelectorPopup : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aGrid
        * @return
        */
        static CLmkIconSelectorPopup* NewL( CAknGrid& aGrid );

    public: // From CAknPopupList
		TBool ExecuteLD();

		void HandleScreenSizeChange();
    protected:
        /**
        * Destructor.
        */
        ~CLmkIconSelectorPopup();

   	private:
		/**
		 * By default Symbian 2nd phase constructor is private.
		 */
        void ConstructL( CAknGrid& aGrid );

        /**
        * C++ default constructor.
        */
        CLmkIconSelectorPopup();

    private: //new functions
        /**
         * Load graphics
         */
    	void LoadGraphicsL();

        /**
         * Add Icons to Popup
         * @param number of icons
         */
		void AddDataL( TInt aNum );

        /**
         * Set layout to cells in the grid.
         */
        void ApplySelGridGraphicStyleL();

    private:    // Data
        TInt iNumOfColumns;
        TInt iNumOfRows;
        TSize iSizeOfItems;

        ///Ref:
        CAknGrid* iGrid;

        // Ref:
        CEikonEnv*      iEnv;
    };

#endif      // __CLMKICONSELECTORPOPUP_H__

// End of File
