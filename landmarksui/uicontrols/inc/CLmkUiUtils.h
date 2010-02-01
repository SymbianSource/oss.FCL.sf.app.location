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
* Description:    LandmarksUi Content File -    Utils class for ui classes.
*
*/







#ifndef CLMKUIUTILS_H
#define CLMKUIUTILS_H

//  INCLUDES
#include <e32base.h>
#include <aknnavide.h>

// FORWARD DECLARATIONS
class CEikStatusPane;
class CAknTitlePane;
class CAknContextPane;
class CEikImage;
class CPosLandmark;

// Added for testing
class MLmkEditorUiBuilder;
class MLmkFieldData;
class CEikonEnv;


// CLASS DECLARATION
/**
* CLmkUiUtils class
* - Store and restore navipane.
* - Store and restore titlepane.
* - Read resource file.
* - etc.
*/
class CLmkUiUtils : public CBase
    {
    public:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @return created CLmkCategoryContentsView object
        */
        static CLmkUiUtils* NewL();

        /**
        * Destructor.
        */
        ~CLmkUiUtils();

    private: // Constructors
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CLmkUiUtils();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // New functions
        /**
        * Change title pane
        * @param aTitle, take ownership
        */
        IMPORT_C static void ChangeTitlePaneL( HBufC* aTitle );

        /**
        * Change title pane. If KZeroResourceId is given then
        * title pane is not changed.
        * @param aResourceText resource containing title text
        */
        IMPORT_C static void ChangeTitlePaneL( TInt aResourceText );

        /**
        * Find file from a correct drive, i.e. from drive where dll
        * being executed is located.
        * @param aFileName, file name which is updated to contain drive
        */
        IMPORT_C static void GetFileWithCorrectDriveL( TFileName& aFileName );



        // Added for testing
        /**
        * Change the icon in Landmark's name label field on a need basis
        *
        */

        IMPORT_C static void ChangeLmNameLabelIconL ( MLmkEditorUiBuilder& aUiBuilder,
                                               MLmkFieldData& nameField);


        /**
        * Method for getting Avkon icon file name with correct
        * path information.
        * @return file name
        */
        static TFileName* AvkonIconFileLC();

        /**
        * Method for getting LmkUi icon file name with correct
        * path information.
        * @return file name
        */
        static TFileName* LmkUiIconFileLC();

        /**
        * Store current title pane.
        */
        void StoreTitlePaneL();

        /**
        * Restore old title pane.
        *
        */
        void RestoreOldTitlePane();

        /**
        * Read LmkUi resource file.
        */
    	void ReadLmkUiResourceFileL();

        /**
        * Store current navi pane.
        */
        void StoreNaviPaneL();

        /**
        * Create empty/default navipane.
        */
        void CreateDefaultNaviPaneL();

        /**
        * Create navi label.
        * @param aLabel
        */
        void CreateNaviLabelL( const TDesC& aLabel );

        /**
        * Restore old navi pane.
        */
        void RestoreOldNaviPaneL();

        /**
        * Swap new context icon and save old icon.
        * @param aNewImage
        */
        void SwapNewContextIconL( CEikImage* aNewImage );

        /**
        * Set old context icon.
        */
        void SetOldContextIcon();

        /**
        * Update new context image.
        * @param aNewImage
        */
        void UpdateContextIconL( CEikImage* aNewImage );

        /**
        * Finds if web url field is empty. Ignored http://
        * @param aUrl web url.
        */
        static TBool FindIfWebAddrFieldIsEmpty (const TDes& aUrl);

    private:  // New functions
        /**
        * Return status pane.
        * @return status pane, ownership is not transferred
        */
        static CEikStatusPane* StatusPane();

        /**
        * Set context pane.
        * @param aContextPane
        */
        void SetContextPaneL();

    private:    // Data
        ///Ref:
        CAknContextPane* iContextPane;

        ///Own:
        CEikImage* iOldImage;

        /// TInt variable
        TInt iResourceOffset;

        /// Own: Title pane text to restore on exit
        HBufC* iStoredTitlePaneText;

        ///Ref:
        CAknTitlePane* iTitlePane;

        ///Own:
        CAknNavigationDecorator* iStoredDecorator;

        //Own:
        CAknNavigationDecorator* iNaviLabel;

        //Ref:
        CAknNavigationControlContainer* iNaviPane;

    };

#endif      // CLMKUIUTILS_H

// End of File
