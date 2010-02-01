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
* Description:  Blid application location request view's container 
*                class definition.
*
*/

#ifndef CBLIDBASECONTAINER_H
#define CBLIDBASECONTAINER_H

// INCLUDES
#include <coecntrl.h>
 
// FORWARD DECLARATIONS
class MKeyProcessor;
class CAknNavigationDecorator;
class CAknIconArray;
class CEikStatusPane;
class CEikListBox;
class CAknNavigationControlContainer;
class TAknsItemID;
class CAknTabGroup;

// CLASS DECLARATION
/**
*  CBlidBaseContainer  container control class.
*  
*/
class CBlidBaseContainer : public CCoeControl
    {
    public: // Constructors and destructor   
        /**
        * Destructor.
        */
        ~CBlidBaseContainer();

    protected: // construction
        CBlidBaseContainer( MKeyProcessor& aKeyProcessor, 
                            const TDesC& aHelpContext );

    public: // new functions
        /**
         * Activate navigation decorator.
         * Method can called if tab needs to be refreshed.
         * @param aNaviDecorator    navigation decorator
         * @param aTabId            activated tabulator id             
         */
        void ActivateTabL( CAknNavigationDecorator& aNaviDecorator, 
                            TInt aTabId );

    public: // from CCoeControl
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                    TEventCode aType);
        void HandleResourceChange(TInt aType);
       
    protected: // new functions
        /**
         * Create CAknNavigationDecorator, ownership passed to caller.
         * @param aDefaultNaviPane, ETrue if default, EFalse if tabgroup
         * @return new instance of object. NULL, if something fails
         */
        CAknNavigationDecorator* CreateNaviPaneL( TBool aDefaultNaviPane );
        
        /**
         *  Append icon to icon array. CAknIconArray owns icons so it is 
         *  easier to it leave safely.
         *
         *  @param aIcons                 Icon array
         *  @param aSkinID                TAknsItemID
         *  @param aIconFileWithPath      Icon file name with path
         *  @param aIconGraphicsIndex     Picture index.
         *  @param aIconGraphicsMaskIndex Mask index.
         */
        void AddIconL( CAknIconArray* aIcons,
                       const TAknsItemID aSkinID,
                       const TDesC& aIconFileWithPath,
                       TInt aIconGraphicsIndex,
                       TInt aIconGraphicsMaskIndex );

        /**
         * Make title
         * @param aResourceText resource to create title
         */
        void MakeTitleL( TInt aResourceText );
        
        /**
         * Make title
         * @param aListbox, listbox instance
         * @param aResourceText, resourse id
         */
        void MakeEmptyTextListBoxL( 
                CEikListBox* aListBox,
                TInt aResourceText );

        /**
         * Appui's status pane getter
         * @return Statuspane pointer
         */
        CEikStatusPane* StatusPane();

        /** 
         * Add new tabs to tabgroup
         * @param aTabGroup
         */
        void AddTabsL(CAknTabGroup& aTabGroup);
        
    public:        
        static void SetGPSAvailability(const TBool aAvailability);
        static TBool IsGPSAvailable();
        
    protected: //data
        MKeyProcessor& iKeyProcessor;
        TCoeContextName iContextName;
        static TBool iGPSAvailability;
    };
#endif // CBLIDBASECONTAINER_H


// End of File
