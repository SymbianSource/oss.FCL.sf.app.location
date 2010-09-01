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
* Description:    LandmarksUi Content File -    Base class for common ui selectors (dialog-based).
*
*/







#ifndef CLMKDLGSELECTORIMPLBASE_H
#define CLMKDLGSELECTORIMPLBASE_H

//  INCLUDES
#include "CLmkSelectorImplBase.h"
#include "MLmkNavigationObserver.h"
#include "MLmkDlgMenuObserver.h"
#include <eikcmobs.h>       // MEikCommandObserver

// FORWARD DECLARATIONS
class MObjectProvider;
class CEikonEnv;
class CLmkSelectorDialog;
class TCoeHelpContext;
class CEikTextListBox;

// CLASS DECLARATION

/**
* Base class for common ui selectors (dialog-based).
* During implementation it was noticed that pre-selection indexes
* cause difficulties and some refactoring could be done to make the
* code easier to read and maintain.
*
*/
class CLmkDlgSelectorImplBase
: public CLmkSelectorImplBase,
  public MEikCommandObserver,
  public MLmkNavigationObserver,
  public MLmkDlgMenuObserver
    {
    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        ~CLmkDlgSelectorImplBase();

    public: // New functions
        /**
        * Selector implementation parent receives observed events when
        * any of the base class's observation methods is executed.
        * @param aParent parent of this selector implementor
        */
        void SetSelectorImplParent( CLmkDlgSelectorImplBase* aParent );

        /**
        * Object provider mechanism.
        * @param aParent parent object
        */
        IMPORT_C void SetMopParent( MObjectProvider* aParent );

        /**
        * Runs single item selection dialog.
        * @param aOpenedItem contains output selection id
        * @param aAlwaysModifyOpened if ETrue then highlighted item
        *        is set to aOpenedItem even if dialog was cancelled
        * @return !=0 if accepted, 0 if canceled
        */
        virtual TInt ExecuteL( TPosLmItemId& aOpenedItem,
                               TBool aAlwaysModifyOpened = EFalse );

        /**
        * Runs multiple item selection dialog.
        * aSelectedItems is used as both input and output parameter
        * (i.e. if a non-empty array is given then those items are
        * marked selected by default.
        * @param aSelectedItems contains input/output selection id(s)
        * @param aAlwaysModifySelected if ETrue then selected items
        *        are set to aSelectedItems even if dialog was cancelled
        * @return !=0 if accepted, 0 if canceled
        */
        virtual TInt ExecuteL( RArray<TPosLmItemId>& aSelectedItems,
                               TBool aAlwaysModifySelected = EFalse );

        /**
        * Runs single item selection dialog. This method is mainly
        * targeted for internal use within LmkUi component.
        * @param aOpenedItem contains output selection index
        * @return !=0 if accepted, 0 if canceled
        */
        virtual TInt ExecuteL( TInt& aOpenedItem );

        /**
        * Runs multiple item selection dialog. This method is mainly
        * targeted for internal use within LmkUi component.
        * aSelectedItems is used as both input and output parameter
        * (i.e. if a non-empty array is given then those items are
        * marked selected by default.
        * @param aSelectedItems contains input/output selection indexes
        * @return !=0 if accepted, 0 if canceled
        */
        virtual TInt ExecuteL( CArrayFix<TInt>& aSelectedItems );

         /**
      	* Stores the empty dialog label
        * @param aLabelRes Label resource id
        */
        virtual void SetEmptyDlgLabel( TInt aLabelPrimRes ,TInt aLabelSecRes );

       // Added for accepting dynamically changing title pane parameter
		virtual TInt ExecuteL( CArrayFix<TInt>& aSelectedItems, TPtrC aTitlePaneText );

        /**
        * Template method implemented by the subclass and called by
        * the base class.
        */
        virtual void GetDlgResources( TBool aIsSingleSelector,
                                      TInt& aTitlePaneResource,
                                      TInt& aMenuBarResource,
                                      TInt& aDialogResource ) const = 0;



        /**
        * Template method called by the base class,
        * implemented by the subclass if dialog has a help.
        * Base class implementation returns null help context.
        * @return aContext help context
        */
        virtual TCoeHelpContext HelpContext() const;

        virtual void StopExecutionL();

        virtual void SetDialogTitleL(const TDesC& aTitle);

    public: // From CLmkSelectorImplBase
        MLmkListMemento* MementoL();
        TInt ListVisibleItemCount();
        TInt ListMarkedItemCountL();

    protected: // From MEikCommandObserver
        void ProcessCommandL( TInt aCommandId );

    protected: // From MLmkDlgMenuObserver
        void DynInitDlgMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    protected: // From MLmkListProviderObserver
        void HandleListProviderEvent( TLmkListProviderEventType aEvent );
        void HandleListProviderError( TInt aError );

    protected: // From MLmkNavigationObserver
        TKeyResponse HandleNavigationEventL( const TKeyEvent& aKeyEvent );
        void HandleLaunchingEventL();
        void HandleClosingEventL();

        /**
        * Check whether Coordinates has to be hidden.
        */
        void CheckHideCoordinateL( );

    protected: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb
        * @return newly instantiate object
        */
        CLmkDlgSelectorImplBase( CPosLandmarkDatabase& aDb );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();

    private: // New functions

        /**
        * Internal helper function which attempts to read indexes
        * corresponding to stored lm item ids and to set them as
        * selected in the listbox.
        */
        void RetrySetPreSelectedL();

    protected: // Data
        ///Ref: parent selector
        CLmkDlgSelectorImplBase* iSelectorImplParent;

        ///Ref: mop parent
        MObjectProvider* iMopParent;

        ///Own: Note that iDialog is a dynamic object which can be accessed
        ///     between HandleLaunchingEventL() & HandleClosingEventL().
        CLmkSelectorDialog* iDialog;
        TInt iCurrentLabelPrimResource;
        TInt iCurrentLabelSecResource;

        //It will be ETrue when help has to be hidden.
        TBool iIsHideHelp;

    private: // Data
        // These should not be used by subclasses, their content is
        // guaranteed to be correct only within this class.

        ///Latest provider event recorded by this class
        TLmkListProviderEventType iDlgSelectorImplBaseLatestListEvent;

        ///Ref: items to be set marked when provider gets ready if it
        ///     wasn't ready when ExecuteL() was called
        const RArray<TPosLmItemId>* iInputSelectedItems;

        // user-specified title for the dialog
        HBufC* iTitle;
        
    };

#endif      // CLMKDLGSELECTORIMPLBASE_H

// End of File
