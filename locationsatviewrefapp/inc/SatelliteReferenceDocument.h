/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

/*
* ============================================================================
*  Name     : SatelliteReferenceDocument
*  Part of  : Satellite Reference App
* ============================================================================
*/

#ifndef SATELLITE_REFERENCE_DOCUMENT_H
#define SATELLITE_REFERENCE_DOCUMENT_H

// INCLUDES
#include <AknDoc.h>
   

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CSatelliteReferenceDocument application class.
*/
class CSatelliteReferenceDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CSatelliteReferenceDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CSatelliteReferenceDocument();

    private:

        /**
        * EPOC default constructor.
        */
        CSatelliteReferenceDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

