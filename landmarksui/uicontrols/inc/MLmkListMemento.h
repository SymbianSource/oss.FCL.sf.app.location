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
* Description:    LandmarksUi Content File -    Interface for storing and restoring listbox UI status
*
*/







#ifndef MLMKLISTMEMENTO_H
#define MLMKLISTMEMENTO_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CEikListBox;
class CLmkListProviderBase;

// CLASS DECLARATION

/**
* MLmkListMemento class. Interface for storing and restoring
* listbox UI status (e.g. current item index).
* Methods in this class are non-leaving since this functionality is
* just supplementary, error situations should be handled by memento.
*/
class MLmkListMemento
    {
    public: // Constructors and destructors
        /**
        * Destructor
        */
        virtual ~MLmkListMemento() {};

    public: // New functions
        /**
        * Store listbox state
        * @param aListbox
        * @param aProvider
        * @param aFiltered
        */
        virtual void Store( const CEikListBox& aListBox,
                            const CLmkListProviderBase& aProvider,
                            TBool aFiltered ) = 0;

        /**
        * Restore listbox state
        * @param aListbox
        * @param aProvider
        * @param aFiltered
        */
        virtual void Restore( const CEikListBox& aListBox,
                              const CLmkListProviderBase& aProvider,
                              TBool aFiltered ) = 0;
    };

#endif      // MLMKLISTMEMENTO_H

// End of File
