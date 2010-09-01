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
* Description:    LandmarksUi Content File -    Landmark package viewer dialog.
*
*/







#ifndef CLMKDLGPACKAGEVIEWERIMPL_H
#define CLMKDLGPACKAGEVIEWERIMPL_H

//  INCLUDES
#include "CLmkDlgLmSelectorImpl.h"
#include "MLmkAOOperationObserver.h"
#include "clmkmapnavigationinterface.h"
#include "CLmkSaveLandmarkUtils.h"

// FORWARD DECLARATIONS
class CLmkParser;
class CLmkSender;
class CAknWaitDialog;

// CLASS DECLARATION

/**
*  Landmark package viewer dialog.
*/
class CLmkDlgPackageSelectorImpl
: public CLmkDlgLmSelectorImpl,
  public MLmkAOOperationObserver
    {
    public:  // Constructors and destructor

        /**
        * Create package viewer dialog
        * @param aDb database reference
        * @param aParser landmark parser reference
        * @param aSender landmark sender reference
        * @return newly instantiated object
        */
        IMPORT_C static CLmkDlgPackageSelectorImpl* NewL(
                                CPosLandmarkDatabase& aDb,
                                CLmkParser& aParser,
                                CLmkSender& aSender );

        /**
        * Destructor.
        */
        ~CLmkDlgPackageSelectorImpl();

    public: // From CLmkDlgSelectorImplBase
        void SetupListProviderL();
        void GetDlgResources( TBool aIsSingleSelector,
                              TInt& aTitlePaneResource,
                              TInt& aMenuBarResource,
                              TInt& aDialogResource ) const;
        TCoeHelpContext HelpContext() const;

    public: //from MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                              TReal32 aProgress,
                              TInt aStatus );

    protected: // From MEikCommandObserver
        void ProcessCommandL( TInt aCommandId );

    protected: // From MLmkDlgMenuObserver
        void DynInitDlgMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    protected: // From MLmkNavigationObserver
        void HandleLaunchingEventL();

    protected:  // Constructors and destructor
        /**
        * C++ default constructor.
        */
        CLmkDlgPackageSelectorImpl( CPosLandmarkDatabase& aDb,
                                  CLmkParser& aParser,
                                  CLmkSender& aSender );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New functions
        /**
        * Open landmark
        */
        void OpenLandmarkCmdL();

        /**
        * Save selected landmark(s)
        */
        void SaveLandmarksCmdL();

        /**
        * Save all landmarks in the package
        */
        void SaveAllLandmarksCmdL();

        /**
        * Sends the selected landmarks from the package
        */
        void SendSelectedLandmarksCmdL(TInt aCommandId);

        /**
        * Sends all the landmarks in package
        */
        void SendAllLandmarksCmdL(TInt aCommandId);

		/**
        *
        */
		void GetLandmarksInPackageL(RPointerArray<CPosLandmark> &aLmkArray);

    	/**
    	* This method is used to check if the supplied landmark has any valid
    	* coordinates or address. If either Latitude or Longitude are Nan and if
    	* none of the address fields have any valid entyr then the method returns
    	* ETrue otherwise returns EFalse.
    	*
    	* @param aLandmark is the landmark on which the check needs to be made
    	* @return ETrue if Landmark does not have any coordinates or address,
    	*               EFalse otherwise
    	*/
    	TBool IsLandmarkDataEmptyL( CPosLandmark* aLandmark );

    private:    // Data
        ///Ref: landmark parser
        CLmkParser& iParser;

        ///Ref: landmark sender
        CLmkSender& iSender;

        ///Own: iWaiNote
        CAknWaitDialog* iWaitNote;

        //Own: Pointer to map and navigation interface
        CLmkMapNavigationInterface* iMapNavInterface;

        // Ref: handle to landmark database
        CPosLandmarkDatabase& iDb;

        TCategoryName iNewCategoryFound;
        TInt iNoOfNewCategories;
        CLmkSaveLandmarkUtils* iSaveLmUtils;
        TOperationTypes iType;
        RArray<TUint> iTotalLms;

    };

#endif      // CLMKDLGPACKAGEVIEWERIMPL_H

// End of File
