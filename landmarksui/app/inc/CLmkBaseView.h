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
* Description:    LandmarksUi Content File -    Declares base view for Landmarks application.
*
*/







#ifndef CLMKBASEVIEW_H
#define CLMKBASEVIEW_H

// INCLUDES
#include "MLmkKeyProcessor.h"
#include <aknview.h>
#ifdef RD_SCALABLE_UI_V2
#include <eiklbo.h>
#endif//RD_SCALABLE_UI_V2

// FORWARD DECLARATIONS
class CLmkBaseContainer;
class CEikMenuPane;
class CLmkAppUi;
class CLmkMapNavigationInterface;

/*
* This enum specifies, what item to display in the current view.
* The information is used by containers and corresponding views.
*/
enum TLmkMskDispItem
	{
	EContextMenu,//display context menu
	ELabel,      //display label
	ENoMsk,	     // no MSK
	EListEmpty
	};

// CLASS DECLARATION
/**
*  Base view class.
*/
#ifdef RD_SCALABLE_UI_V2
class CLmkBaseView :
public CAknView,
public MLmkKeyProcessor,
public MEikListBoxObserver
#else
class CLmkBaseView : public CAknView, public MLmkKeyProcessor
#endif//RD_SCALABLE_UI_V2
    {
    public: // Constructors and destructor
        /**
        * C++ constructor
        * @param aKeyProcessor A pointer to MLmkKeyProcessor
        * @return newly instantiated object
        */
        CLmkBaseView( MLmkKeyProcessor& aKeyProcessor );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aResId Resource id
        */
        void BaseConstructL( TInt aResId );

        /**
        * Destructor.
        */
        ~CLmkBaseView();

    protected: // From MLmkKeyProcessor
        TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        MAknTabObserver* TabObserver();

    protected: // New methods

        CLmkAppUi& LmkAppUi() const;

        /**
        * Handle help requests.
        * @param aMenuPane A pointer to CEikMenuPane
        */
        void HandleHelpFeature( CEikMenuPane* aMenuPane ) const;

        /**
        * Check whether Coordinates has to be hidden.
        */
        void CheckHideCoordinateL( );

  protected://New methods for MSK

  		/**
        * Enables the MSK context menu, when some items are marked.
        * overriden by respective view classes to support the
        * view based enabling of the MSK menu.
        * @param aEnable Etrue to enable context menu
        *                EFalse to disable context menu
        */
        virtual void EnableMskMenuL(TBool aEnable);

        /**
        * Enables the MSK lable.
        * overriden by respective view classes to support the
        * view based enabling of the MSK label.
        * @param aEnable Etrue to enable label
        *                EFalse to disable label
        */
        virtual void EnableMskLabelL(TBool aEnable);
	public:
		/**
        * Decides what to be displayed for MSK.
        * It can be context menu or label or nothing.
        * @param aMskDispItem
        */
		virtual void UpdateMskViewL(TLmkMskDispItem aMskDispItem);


	public : // for landscape support
		void Update();
    #ifdef RD_SCALABLE_UI_V2
    protected://for touch support

    	//from MEikListBoxObserver
    	void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    	void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    #endif//RD_SCALABLE_UI_V2
    protected: // Data
        /// Own: container
        CLmkBaseContainer* iBaseContainer;
        /// Ref: reference to key processor
        MLmkKeyProcessor& iKeyProcessor;
        //Own: Pointer to map and navigation interface
        CLmkMapNavigationInterface* iMapNavInterface;

        //It will be ETrue, when launched from location centre.
        TBool isEmbeddedLaunch;

        //It will be ETrue when coordinates has to be hidden.
        TBool iIsHideCoordinate;

        //It will be ETrue when help has to be hidden.
        TBool iIsHideHelp;
    };

#endif // CLMKBASEVIEW_H

// End of File
