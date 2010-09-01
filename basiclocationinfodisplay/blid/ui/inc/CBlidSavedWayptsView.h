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
* Description:  Declares main view for Blid application.
*
*/

#ifndef BLIDSAVEDWAYPTSVIEW_H
#define BLIDSAVEDWAYPTSVIEW_H

// INCLUDES
#include "CBlidBaseView.h"
#include "MBlidRouter.h"

// FORWARD DECLARATIONS
class CBlidSavedWayptsContainer;
class MBlidSettings;


// CLASS DECLARATION

/**
*  CBlidSavedWayptsView view class.
*
*/
class CBlidSavedWayptsView : public CBlidBaseView
    {
    public: // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aKeyProcessor A pointer to MKeyProcessor
         * @return Created view object
         */
        static CBlidSavedWayptsView* NewLC( MKeyProcessor& aKeyProcessor );

        /**
         * Destructor.
         */
        ~CBlidSavedWayptsView();

    private: // constructor
        /**
         * C++ constructor
         * @param aKeyProcessor A pointer  to MKeyProcessor
         */
        CBlidSavedWayptsView( MKeyProcessor& aKeyProcessor );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    public: //From MBlidEngObserver
        void NotifyL( const TBool aOnlineMode );
        //From CBlidBaseView
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        void HandleWaypointsDeletionL();

    public: // From CAknView
        TUid Id() const;
        void HandleCommandL(TInt aCommand);

    #ifdef RD_SCALABLE_UI_V2
    private:
       /**
        * From CBlidBaseView.
        */
        void HandleListBoxSelectionL();
    #endif // RD_SCALABLE_UI_V2
            
    private: // From CAknView
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
        void DoDeactivate();
        TBool RenameWaypointL();
        TBool DeleteWaypointL(TInt aIndex);
        TBool DeleteWaypointsL(const CArrayFix<TInt>& aIndexes);

    private: // Data
        /// Own: A pointer to container 
        CBlidSavedWayptsContainer* iContainer;
    
        /// Not owned: A pointer to settings model
        MBlidSettings* iSettings;
        
        TVwsViewId iPreviousViewid;
        
    };

#endif // End of File
