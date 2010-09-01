/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -
*				Base class for application's (view-based) selectors.
*
*/







#ifndef CLMKAPPSELECTORIMPLBASE_H
#define CLMKAPPSELECTORIMPLBASE_H

//  INCLUDES
#include "CLmkSelectorImplBase.h"
#include <eikcmobs.h>
#include <eikmenup.h>
#include "landmarks.hrh"
#include "MLmkMapAndNavigationObServer.h"

#include <coecobs.h>

// FORWARD DECLARATIONS
class CEikTextListBox;
class CCoeControl;
class CAknSearchField;
class MLmkMskObserver;
#ifdef RD_SCALABLE_UI_V2
class CAknListBoxFilterItems;
#endif //RD_SCALABLE_UI_V2
// CLASS DECLARATION

/**
*  Base class for application's (view-based) selectors.
*
*/
class CLmkAppSelectorImplBase
: public CLmkSelectorImplBase,
  public MEikCommandObserver,
  public MLmkMapAndNavigationObserver
  #ifdef RD_SCALABLE_UI_V2
  ,public MCoeControlObserver
  #endif //RD_SCALABLE_UI_V2
  {
    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        ~CLmkAppSelectorImplBase();

    public: // New functions
        /**
        * Stores status to memento.
        * Can be used after memento has been set.
        */
        IMPORT_C void StoreMemento();

        /**
        * Restores status of a memento. If list is not ready yet then
        * restoration is postponed a while.
        * Can be used after memento has been set.
        */
        IMPORT_C void RestoreMemento();

        /**
        * Creates a list box.
        * Leaves with KErrNotSupported if wrong control type is given.
        * @param aParent parent for listbox control (usually container)
        * @param aControlType listbox type, i.e. EAknCt???ListBox
        * @param aFlags construction flags, e.g. EAknListBoxMarkableList
        * @param aEmptyLBResource empty listbox text
        * @return newly instantiated object
        */
        virtual CEikTextListBox& CreateListBoxL( CCoeControl* aParent,
                                                 TInt aControlType,
                                                 TInt aFlags,
                                                 TInt aEmptyLBPrimResource ,TInt aEmptyLBSecResource );

		/**
		* When the screen size changes the event needs to be propagated
		* to UI components in the selector classes. This will be triggered
		* from the UI Container classes
		*/
		virtual void HandleScreenSizeChange();

        IMPORT_C CAknSearchField* CreateFindBoxL(const CCoeControl &aParent);
        /**
		* IT returns an array of items marked.
		* @return Array of marked item Id's.
		*/
        IMPORT_C const CArrayFix<TInt>* MarkedIndexes();

        /**
		* @return Returns the current item index
		*/
        IMPORT_C TInt CurrentItemIndex();

         /**
        * Sets the observer, to be called , when MSK update is
        * required
        * @param aMskObserver reference
        */
        IMPORT_C void SetMskObserver(MLmkMskObserver* aMskObserver);
    public: // From CLmkSelectorImpleBase
        TInt ListVisibleItemCount();
        TInt ListMarkedItemCountL();

    public: // From MLmkListProviderObserver
        void HandleListProviderEvent( TLmkListProviderEventType aEvent );
        void HandleListProviderError( TInt aError );

    public: //from MLmkMapAndNavigationObserver
        void HandleLmCreated(TPosLmItemId /*aNewLmkItemId*/){}

    public :// New Method
    	CAknSearchField* FindBox();

    protected: // New functions
        /**
        * Helper function for making empty title
        * @param aListbox, listbox instance
        * @param aResourceText, resourse id
        */
         void MakeEmptyTextListBoxL( CEikTextListBox& aListBox,
                                    TInt aResourcePrimText, TInt aResourceSecText,
                                    TBool aRequestFromFindBox = EFalse ) const;


        /**
        * Helper function for creating and setting a listbox model
        * @param aListbox, listbox instance
        * @param aControlType, listbox control type
        * @param aProvider, engine for listbox model
        */
        virtual void CreateListBoxModelL( CEikTextListBox& aListBox,
                                          TInt aControlType,
                            CLmkListProviderBase& aProvider ) const;

        /**
        * Helper function which allows subclasses to handle ELmkEventListReady
        * in specific manner. By default list is reseted and redrawn.
        * @return ETrue if subclass handled the event, EFalse otherwise
        */
        virtual TBool HandleEventListReadyL();

        //Helper function to enable or disable find box
        void ShowFindBoxL( TBool aVisible );

        public: // New functions
        /**
        * Following are the helper wrapper functions which allow
        * the Landmarks UI control libary clients to setup AIW services
        * for invoking phone call. This will be used by Landmarks App or
        * Landmarks Message Viewer app
        *
        */

        virtual void AttachToAIWMenuL( TInt aMenuResourceId,
        					   TInt aInterestResourceId);

        virtual void InitializeMenuPaneL( CEikMenuPane& aMenuPane,
                                  TInt aMenuResourceId);

        virtual void ExecuteAIWCallCmdL( TInt aCommandId );

        virtual TBool IsLmWithThisFieldExistL ( TLmkAppCmdId  aFieldId);

        virtual void AttachInterestL ( TInt aInterestResourceId );

        /**
        * This acts as a wrapper interface, but the actual
        * derive class implements the functionality to call
        * proper call service handler.
        */
        virtual TInt ServiceCmdByMenuCmd( TInt aMenuCmd );

    #ifdef RD_SCALABLE_UI_V2
    protected: // from MCoeControlObserver
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );
	#endif //RD_SCALABLE_UI_V2

    protected: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb reference
        * @param aFindBox
        * @return newly instantiated object
        */
        CLmkAppSelectorImplBase( CPosLandmarkDatabase& aDb,
                                 TBool aFindBox );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();

    protected: // Data
        ///Own:
        CEikTextListBox* iListBox;

        TBool iFindBox;

        CAknSearchField* iSearchBox;
       // TInt iEmptyLBResource;
        TInt iEmptyLBPrimResource;
         TInt iEmptyLBSecResource;
        //ref
        MLmkMskObserver* iMskObserver;

	#ifdef RD_SCALABLE_UI_V2
        CAknListBoxFilterItems* iFilter;
    #endif //RD_SCALABLE_UI_V2

        TPosLmItemId iNewLmkItemId;
    };

#endif      // CLMKAPPSELECTORIMPLBASE_H

// End of File
