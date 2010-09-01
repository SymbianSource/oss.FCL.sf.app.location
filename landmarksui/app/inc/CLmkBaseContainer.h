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
* Description:    LandmarksUi Content File -    LMK application base container
*                class definition.
*
*/






#ifndef CLMKBASECONTAINER_H
#define CLMKBASECONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <akntabgrp.h>


#include "MLmkMskObserver.h"
#include "CLmkBaseView.h"


// FORWARD DECLARATIONS
class MLmkKeyProcessor;
class CAknNavigationDecorator;
class CEikStatusPane;
class CEikListBox;
class CEikMenuBar;
class CAknNavigationControlContainer;
class CAknViewAppUi;
class CLmkAppSelectorImplBase;



// CLASS DECLARATION
/**
*  CLmkBaseContainer  container control class.
* Inheriting from MLmkMSKObserver to get events from selectors, for changing
* middle softkeys.
*/
class CLmkBaseContainer : public CCoeControl, public MLmkMskObserver
    {
    public: // Constructors and destructor
        /**
         * Destructor.
         */
        ~CLmkBaseContainer();

    protected: // Constructors and destructor
        /**
         * Constructor
         * @param aKeyProcessor forward key events
         * @param aHelpContext reference to help context
         * @return newly instantiated object
         */
        CLmkBaseContainer( MLmkKeyProcessor& aKeyProcessor,
                           const TDesC& aHelpContext );

        /**
         * Destructor
         */
        void BaseConstructL();

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
    public:

    	/**
        * Return reference to SelectorImpl
        * @return CLmkAppSelectorImplBase reference
        */
        CLmkAppSelectorImplBase& SelectorImpl();

    protected: // new functions
        /**
         * Get application UI. Ownership is not transferred.
         * @return pointer to CAknViewAppUi
         */
        CAknViewAppUi* ViewAppUi();

        /**
         * Get navi pane reference.
         * @return navi pane reference
         */
        CAknNavigationControlContainer& NaviPane();

         //Code added by gaurav for Tab Icons -start-
        /**
         * Add new tabs to tabgroup
         * @param aTabGroup
         */
        void AddTabsL(CAknTabGroup& aTabGroup);

        HBufC* GetBitmapFilenameLC();

        //-end-

        /**
         * Create CAknNavigationDecorator, ownership passed to caller.
         * This method creates a tab group or empty navi pane if
         * KZeroResourceId given.
         * @param aResource resource id
         * @return new instance of object. NULL, if something fails
         */
        CAknNavigationDecorator* CreateNaviPaneL( TInt aResource );

        /**
         * Appui's status pane getter
         * @return Statuspane pointer
         */
        CEikStatusPane* StatusPane();

        /**
         * Return view's menubar
         * @return CEikMenuBar
         */
        virtual CEikMenuBar* MenuBar();

        /**
         * Return container's context specific menubar resource id
         * @return resource id
         */
        virtual TInt ContextSpecificMenuBar();

        /**
         * Return container's options menubar resource id
         * @return resource id
         */
        virtual TInt OptionsMenuBar();

	public:

        /**
        * This functions finds, what MSK item to display.
        * It is purely based on whether the item is marked or
        * there are no items in the list.
        * @return TLmkMskDispItem
        */
        virtual TLmkMskDispItem ResolveMskDisplayItem();

	protected:

        /**
        * This function sets the list box observer to CEikListBox class
        * @param aObserver MEikListBoxObserver derived class
        */
        #ifdef RD_SCALABLE_UI_V2
        virtual void SetListBoxObserver(MEikListBoxObserver* aObserver);
        #endif//RD_SCALABLE_UI_V2

    protected: //data

        ///Ref:
        CAknNavigationControlContainer* iNaviPane;

        // Ref:
        MLmkKeyProcessor& iKeyProcessor;

        // Own:
        TCoeContextName iContextName;

		/*
		* This member is moved from CLmkLbWithFilterContainer class to this
		* class.
		*/
         /// Own.
        CLmkAppSelectorImplBase* iSelector;

        TBool iSearchInitiated;
    };
#endif // CLMKBASECONTAINER_H


// End of File

