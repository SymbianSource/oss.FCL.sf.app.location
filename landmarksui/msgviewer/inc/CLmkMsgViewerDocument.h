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
* Description:    LandmarksUi Content File -    Landmark message viewer application document class definition.
*
*/







#ifndef CLMKMSGVIEWERDOCUMENT_H
#define CLMKMSGVIEWERDOCUMENT_H

// INCLUDES
#include <eikdoc.h>

// FORWARD DECLARATIONS
class CEikAppUi;
class RFile;

// CLASS DECLARATION

/**
*  CLmkMsgViewerDocument application class.
*/
class CLmkMsgViewerDocument : public CEikDocument
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CLmkMsgViewerDocument* NewL( CEikApplication& aApp );

        /**
         * Destructor.
         */
        virtual ~CLmkMsgViewerDocument();

    public: // New functions

       /* GetLandmarkContentFileHandle This function is a get function to
        * get the file-handle of the file content. This is used by AppUI
        * @since 3.0
        */
        RFile& GetLandmarkContentFileHandle();

        /**
        * IsFileHandleValid() This function is a get function to
        * get the flag whether File's handle is valid
        * AppUi uses this method.
        * @since 3.0
        */
        TBool IsFileHandleValid() const;

    public: // Methods from base class CEikDocument

        /**
        * Takes care of opening file.
        */
       CFileStore* OpenFileL( TBool aDoOpen,
                              const TDesC& aFilename,
                              RFs& aFs );

       void OpenFileL(CFileStore*& aFileStore,  RFile& aFile);

    private:

        /**
         * C++ default constructor.
         */
        CLmkMsgViewerDocument( CEikApplication& aApp );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private: // From CEikDocument
        /**
         * From CEikDocument, create CLocAppUi "App UI" object.
         */
        CEikAppUi* CreateAppUiL();

    private: // Data

    	// File handle to the Landmarks Package content
        RFile iLandmarkContentFileHandle;

        //File Path if the file launched with full filepath.
        HBufC* iFileFullPath;

        //Was file launched with full-path.
        TBool iIsFileFullPathValid;

        //OpenFileL updates this flag after getting the file handle.
        TBool iIsFileHandleValid;
    };

#endif // CLMKMSGVIEWERDOCUMENT_H

// End of File
