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
* Description:    LandmarksUi Content File -    Store and restore views last state.
*
*/








#ifndef CLMKLMITEMLISTMEMENTO_H
#define CLMKLMITEMLISTMEMENTO_H

//  INCLUDES
#include "MLmkListMemento.h"
#include <e32base.h>
#include <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CAknFilteredTextListBoxModel;

// CLASS DECLARATION

/**
* CLmkLmItemListMemento class
*/
class CLmkLmItemListMemento : public CBase, public MLmkListMemento
    {
    public: // Constructors and destructor

        /**
        *
        */
        static CLmkLmItemListMemento* NewL();

        /**
        * Destructor.
        */
        ~CLmkLmItemListMemento();

    public: // New functions

		TBool IsModelAvailable();
		void  UpdateModelAvailability(TBool aModelAvailable);
       /**
        * Returns memorized item id for specific purposes.
        * Mainly used for setting up category contents view.
        * @return item id, KPosLmNullItemId if not applicable
        */
        IMPORT_C TPosLmItemId MemorizedItemId() const;

    public: // From MLmkListMemento

        void Store( const CEikListBox& aListBox,
                    const CLmkListProviderBase& aProvider,
                    TBool aFiltered );

        void Restore( const CEikListBox& aListBox,
                      const CLmkListProviderBase& aProvider,
                      TBool aFiltered );

    protected: // Constructors and destructor

        /**
        * C++ constructor.
        */
        CLmkLmItemListMemento();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

    private: // New functions

        void StoreFiltered( const CEikListBox& aListBox,
                            const CLmkListProviderBase& aProvider,
                            const CAknFilteredTextListBoxModel& aModel );

        void RestoreFiltered( const CEikListBox& aListBox,
                              const CLmkListProviderBase& aProvider,
                              const CAknFilteredTextListBoxModel& aModel );

        void StoreNonFiltered( const CEikListBox& aListBox,
                               const CLmkListProviderBase& aProvider );

        void RestoreNonFiltered( const CEikListBox& aListBox,
                                 const CLmkListProviderBase& aProvider );

    private: // data

        TInt iMemorizedCurrentItemIndex;

        TPosLmItemId iMemorizedCurrentItemId;

        TInt iMemorizedTopItemIndex;
        TBool iModelAvailable;
    };

#endif      // CLMKLMITEMLISTMEMENTO_H

// End of File
