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
* Description:    LandmarksUi Content File -    Application UI class for Landmark message viewer.
*
*/








#ifndef CLMKMSGVIEWERAPPUI_H
#define CLMKMSGVIEWERAPPUI_H

// INCLUDES
#include "MLmkAOOperationObserver.h"
#include <aknappui.h>

// FORWARD DECLARATIONS
class CLmkParser;
class CPosLandmarkDatabase;
class CAknWaitDialog;
class CLmkSender;
class CLmkMsgViewerAppView;

// CLASS DECLARATION

/**
* Application UI class for Landmark message viewer.
*/
class CLmkMsgViewerAppUi
: public CAknAppUi,
  public MLmkAOOperationObserver
    {
    public: // Constructors and destructor

        /**
         * C++ default constructor.
         */
        void ConstructL();

        /**
         * Destructor.
         */
        ~CLmkMsgViewerAppUi();

    public: // from CCoeControl
      /*
       * Helper function to implement the dynamic resource changes
       */
      void HandleResourceChangeL(TInt aType);

    public: // New functions
        TBool ProcessCommandParametersL( TApaCommand aCommand,
	                                     TFileName& aDocumentName,
	                                     const TDesC8& aTail );

        /**
        * Opens a landmark package file and starts processing it.
        * @param aFilename full file name
        */
        void OpenL( const TDesC& aFilename );

        // Added for FILE_HANDLE change
        void OpenL(RFile& aFile);

        /**
        * Returns a reference to landmark sender.
        * @return landmark sender
        */
        CLmkSender& LmkSender() const;

    protected: // From MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                               TReal32 aProgress,
                               TInt aStatus );

    private: // From CEikAppUi
        void HandleCommandL( TInt aCommand );
		void OpenFileL( const TDesC& aFileName );

        // Added for FILE_HANDLE change
        void OpenFileL( RFile& aFilHandle );

    private: // New functions
        /**
        * Helper function which performs the steps following
        * database initialization.
        * @param aStatus result of db initialization
        */
        void HandleDbInitFinishedL( TInt aStatus );

        /**
        * Helper function which performs the steps following
        * asynchronous parse operation.
        * @param aStatus result of parse operation
        */
        void HandleParseFinishedL( TInt aStatus );

        /**
        * Helper function which creates and executes a landmark viewer.
        */
        void ExecuteLmViewerL();

        /**
        * Helper function which creates and executes a lm package viewer.
        */
        void ExecutePkgViewerL();

        /**
        * Used to callback by the idle timer in order to launch the dialog
        * @since 3.0
        */
        static TInt CallBackFuncL(TAny *aPtr);

        /**
        * This function accesses the document and launches the content
        * using the UI Dialog.
        * return: ETrue When file handle is not valid
        *         EFalse When file handle is valid
        * @since 3.0
        */
        TBool LaunchDialogL();

        /**
        * This function closes wait note.
        * @since 3.0
        */
        void CloseWaitNoteL();

        /** Common part of both OpenL(...) */
        void DoOpenL();

    private: //Data
        /// Owns: parser
        CLmkParser* iParser;
        /// Owns: landmark database
        CPosLandmarkDatabase* iDb;
        /// Owns: wait dialog
        CAknWaitDialog* iWaitDialog;
        /// Owns: landmark sender
        CLmkSender* iLmkSender;
        /// Owns: empty view
        CLmkMsgViewerAppView* iAppView;

        CPeriodic* iIdleTimer;
        /// result of parsing
        TBool iDataReadyForView;
        //
        TBool iIsDbOpen;
    };

#endif // CLMKMSGVIEWERAPPUI_H

// End of File
