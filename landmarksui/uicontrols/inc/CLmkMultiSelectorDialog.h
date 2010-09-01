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
* Description:    LandmarksUi Content File -    Multi selector dialog.
*
*/








#ifndef CLMKMULTISELECTORDIALOG_H
#define CLMKMULTISELECTORDIALOG_H

//  INCLUDES
#include "CLmkSelectorDialog.h"
#include "LmkConsts.h"

// FORWARD DECLARATIONS
class CLmkSender;
class MLmkDlgMenuObserver;

// CLASS DECLARATION

/**
* CLmkMultiSelectorDialog class
* Don't have default icons, user should set icon array self
*/
class CLmkMultiSelectorDialog : public CLmkSelectorDialog
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aCurrentItemIndex
        * @param aSelectionIndexArray
        * @param aLbModel listbox model, ownership is not transferred
        * @param aMenuBarResourceId
        * @param iOkOptionsMenuResourceId
        * @param aCmdObserver, ownership is not transferred
        * @param aMenuObserver, ownership is not transferred
        * @return newly instantiated object
        */
        static CLmkMultiSelectorDialog* NewL( TInt& aCurrentItemIndex,
                                         CArrayFix<TInt>* aSelectionIndexArray,
                                         MDesCArray* aLbModel,
                                         TInt aMenuBarResourceId,
                                         TInt iOkOptionsMenuResourceId,
                                         MEikCommandObserver* aCmdObserver,
                                         MLmkDlgMenuObserver* aMenuObserver,
                                         MLmkListMemento* aMemento = NULL );

        /**
        * Destructor.
        */
        virtual ~CLmkMultiSelectorDialog();

    public: // New functions

        /**
        * This method prepares the dialog to use "Send as..." menu.
        * Note: this function requires that command ELmkCmdSendDummy exists
        * in the menu pane, since it determines the position of "Send as...".
        * @param aSender reference
        * @param aSendUiMenuPaneResource, must contain ELmkCmdSendDummy
        * @param aMenuTextResource customized "Send as..." substitute
        */
        void SetupSendMenu( CLmkSender& aSender,
                            TInt aSendUiMenuPaneResource,
                            TInt aMenuTextResource = KZeroResourceId );
		/**
        * Updates MSK of the dialog.
        */
        void UpdateMskL();
	private:
		TBool IsCurrentItemMarked();

    protected: // From MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );

    protected: // From CAknSelectionListDialog
        void SelectionListProcessCommandL( TInt aCommand );
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );
        void PreLayoutDynInitL();
        void PostLayoutDynInitL();
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        TBool OkToExitL( TInt aButtonId );

    protected:
        /**
        * C++ default constructor.
        */
        CLmkMultiSelectorDialog( TInt& aCurrentItemIndex,
                                 CArrayFix<TInt>* aSelectionIndexArray,
                                 MDesCArray* aLbModel,
                                 TInt aMenuBarResourceId,
                                 TInt iOkOptionsMenuResourceId,
                                 MEikCommandObserver* aCmdObserver,
                                 MLmkDlgMenuObserver* aMenuObserver,
                                 MLmkListMemento* aMemento = NULL );

    protected:

        TInt iMenuBarResourceId;
        TInt iOkOptionsMenuResourceId;
        TInt iSendUiMenuPaneResourceId;
        TInt iMenuTextResourceId;

    private:
    	void ConstructL(TInt aMenuBarResourceId);
    private:

        /// Ref:
        CArrayFix<TInt> *iSelectionIndexArray;
        /// Ref:
        CLmkSender* iLmkSender;
        /// Ref:
        MLmkDlgMenuObserver* iMenuObserver;
        TBool iExited;
        TBool iIsToClearSelection;
    };

#endif      // CLMKMULTISELECTORDIALOG_H

// End of File
