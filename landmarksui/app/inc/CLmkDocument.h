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
* Description:    LandmarksUi Content File -    Landmarks application document class definition.
*
*/







#ifndef CLMKDOCUMENT_H
#define CLMKDOCUMENT_H

// INCLUDES
#include <eikdoc.h>


// FORWARD DECLARATIONS
class CEikAppUi;
class CPosLandmarkDatabase;

// CLASS DECLARATION

/**
*  CLmkDocument application class.
*/
class CLmkDocument : public CEikDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aApp
        * @return newly instantiated object
        */
        static CLmkDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CLmkDocument();

    public: // New functions
        /**
        * Returns a reference to landmarks database.
        * @return database reference, not initialized necessarily
        */
        CPosLandmarkDatabase& LmDbL() const;

    private:
        /**
        * C++ default constructor.
        * @param aApp
        * @return newly instantiated object
        */
        CLmkDocument( CEikApplication& aApp );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // From CEikDocument
        /**
        * From CEikDocument, create CLocAppUi "App UI" object.
        * @return pointer to CEikAppUi object
        */
        CEikAppUi* CreateAppUiL();

    private: // Data
        ///Own:
        CPosLandmarkDatabase* iDb;
        TInt iError;
    };

#endif // CLMKDOCUMENT_H

// End of File
