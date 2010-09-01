/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Blid application document class definition.
*
*/


#ifndef CBLIDDOCUMENT_H
#define CBLIDDOCUMENT_H

// INCLUDES
#include <AknDoc.h>
   

// FORWARD DECLARATIONS
class CEikAppUi;
class CBlidEng;
// CLASS DECLARATION

/**
*  CBlidDocument application class.
*/
class CBlidDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
         * Two-phased constructor.
         */
        static CBlidDocument* NewL(CEikApplication& aApp);

        /**
         * Destructor.
         */
        virtual ~CBlidDocument();

    private:

        /**
         * C++ default constructor.
         */
        CBlidDocument(CEikApplication& aApp);

        /**
         * By default Symbian 2nd phase constructor is private.
         */        
        void ConstructL();

    public: // New functions
        /**
         * Creates the Blid engine owned by this object.
         *
         */
        void CreateEngineL();

        /**
         * Returns a pointer to blid application engine.
         */
        CBlidEng* Engine();

    private: //From CAknDocument
        /**
         * Stores the document of application.
         */
        void StoreL( CStreamStore& aStore, 
            CStreamDictionary& aStreamDic ) const;
        /**
         * Restores the document of application
         */
        void RestoreL( const CStreamStore& aStore,
            const CStreamDictionary& aStreamDic );
            
        /**
         * Restores the document's state from the specified file, 
         * or creates a new default document
         */
        CFileStore* OpenFileL(TBool aDoOpen, const TDesC& aFileName, RFs& aFs);

    private: // From CEikDocument
        /**
         * From CEikDocument, create CLocAppUi "App UI" object.
         */
        CEikAppUi* CreateAppUiL();

    private: // Data
        /// Own: A pointer to Blid's engine
        CBlidEng* iEngine; 
    };

#endif // CBLIDDOCUMENT_H

// End of File
