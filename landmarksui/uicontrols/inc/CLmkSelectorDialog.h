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
* Description:    LandmarksUi Content File -    CLmkSelectorDialog class
*
*/








#ifndef CLMKSELECTORDIALOG_H
#define CLMKSELECTORDIALOG_H

//  INCLUDES
#include "MLmkListProviderObserver.h"
#include <aknselectionlist.h>
#include "MLmkListMemento.h"

// FORWARD DECLARATIONS
class MLmkNavigationObserver;
class TCoeHelpContext;
class CLmkListProviderBase;

// CLASS DECLARATION

/**
* CLmkSelectorDialog class
*/
class CLmkSelectorDialog
    : public CAknSelectionListDialog,
      public MLmkListProviderObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aOpenedItemIndex
        * @param aLbModel listbox model, ownership is not transferred
        * @param aMenuBarResourceId
        * @param aCmdObserver, ownership is not transferred
        * @param aMemento , maintains reference of memento.
        * @return newly instantiated object
        */
        static CLmkSelectorDialog* NewL( TInt& aOpenedItemIndex,
                                         MDesCArray* aLbModel,
                                         TInt aMenuBarResourceId,
                                         MEikCommandObserver* aCmdObserver,
                                         MLmkListMemento* aMemento = NULL, CLmkListProviderBase* aListProvider = NULL);

        /**
        * Destructor.
        */
        virtual ~CLmkSelectorDialog();

    public: // New functions
    	void MakeEmptyTextListBoxL(TInt aResourcePrimText ,TInt aResourceSecText, TBool aRequestFromFindBox = EFalse ) const;
    	/**
        * Enable/Disable the find box in list dialog.
        * @return ETrue to enable find box, EFalse otherwise
        */
		void ShowFindBox( TBool aVisible );
        /**
        * Tells whether this dialog has a find box or not.
        * @return ETrue if has find box, EFalse otherwise
        */
        TBool HasFindBox() const;

        /**
        * Number of list items.
        * @return number of list items
        */
        TInt NumberOfItems() const;

        /**
        * Currently highlighted item's index.
        * @return highlighted item's index
        */
        TInt CurrentItemIndex() const;

        /**
        * Selection indexes. This class does not support multiselection,
        * subclasses may support.
        * @return selection indexes, ownership is not transferred
        */
        const CArrayFix<TInt>* SelectionIndexes() const;

        /**
        * Set help context.
        * @param aContext
        */
        void SetHelpContext( TCoeHelpContext aContext );

        /**
        * Set observer for navigation events.
        * @param
        */
        void SetNavigationObserver( MLmkNavigationObserver& aObserver );

               // For Landscape Support
		void HandleResourceChange (TInt aType);

		 /**
      	* Stores the empty dialog label
        * @param aLabelRes Label resource id
        */
		void SetDlgEmptyResource(TInt aEmptyPrimLabel,TInt aEmptySecLabel);
		/**
      	* Enables or Disables left softkey
        * @param aEnable ETrue for visible, EFalse for invisible
        */
		void EnableLeftSoftKey(TBool aEnable);
		 /**
        * Updates the MSK for the dialog
        */
		virtual void UpdateMskL();
		void PrepareLC(TInt aDlgResourceId);

	private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TInt aMenuBarResourceId);

		void SetCbaResourceAndCommandId(TInt aaDlgResourceId);

    public: // From CAknSelectionListDialog
        CEikListBox* ListBox() const;
        CAknSearchField* FindBox() const;
#ifdef RD_SCALABLE_UI_V2
    protected: // from MCoeControlObserver
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );
#endif //RD_SCALABLE_UI_V2

    public: // From MLmkListProviderObserver
        void HandleListProviderEvent( TLmkListProviderEventType aEvent );
        void HandleListProviderError( TInt aError );

    protected: // From CCoeControl
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

    protected: // From CAknSelectionListDialog
        void PreLayoutDynInitL();
        void PostLayoutDynInitL();

    protected: // From CAknDialog
        TBool OkToExitL( TInt aButtonId );

    protected: // New functions

        /**
        * Internal method which notifies navigation observer if key event
        * is navigation event.
        * @param aEkeyEvent
        * @param aType
        * @return EKeyWasConsumed if navigation observer consumed the event
        */
        TKeyResponse KeyEventForNavigationL( const TKeyEvent& aKeyEvent,
                                             TEventCode aType );

    protected:

        /**
        * C++ default constructor.
        */
        CLmkSelectorDialog( TInt& aOpenedItemIndex,
                            MDesCArray* aLbModel,
                            MEikCommandObserver* aCmdObserver,
                            MLmkListMemento* aMemento = NULL, CLmkListProviderBase* aListProvider = NULL );

    protected:    // Data
        TCoeHelpContext iHelpContext;
        /// Ref: navigation observer
        MLmkNavigationObserver* iNavigationObserver;
        MLmkListMemento* iMemento;
        TBool iExited;
        TInt iDlgEmptyLabelPrimResource;
        TInt iDlgEmptyLabelSecResource;
     	TInt iMskCmdId;
     	TInt iCbaResourceId;
#ifdef RD_SCALABLE_UI_V2
        CAknListBoxFilterItems* iFilter;
#endif //RD_SCALABLE_UI_V2
		CLmkListProviderBase* iListProvider;
    };

#endif      // CLMKSELECTORDIALOG_H

// End of File
