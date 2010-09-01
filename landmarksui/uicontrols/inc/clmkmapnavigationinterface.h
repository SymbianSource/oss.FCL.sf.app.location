/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Application's interface class to Map and Navigation use cases
*
*/







#ifndef CLMKMAPNAVIGATIONINTERFACE_H
#define CLMKMAPNAVIGATIONINTERFACE_H

// INCLUDES
#include <AiwCommon.h>
#include <e32std.h>
#include <EPos_Landmarks.h>
#include <AknProgressDialog.h>
#include "mlmkdbsearchutilsobserver.h"
#include "CLmkAppSelectorImplBase.h"
#include "MLmkMapAndNavigationObServer.h"
// FORWARD DECLARATION
class CPosLandmark;
class CAiwServiceHandler;
class CLmkDbSearchUtils;
class CAknWaitDialog;
class CPosLandmarkDatabase;


/**
* The class CLmkMapNavigationInterface contains the implemenation
* for all the uses cases of Map and Navigation. It is a singleton class
*
*/
class CLmkMapNavigationInterface : public CBase,
								   public MAiwNotifyCallback,
								   public MLmkDbSearchUtilsObserver,
								   public MProgressDialogCallback
	{
	public:
	    // Enum for view type
		enum EViewType
			{
			EByLmkView, 	// For landmark view
			EByCategoryView // For category view
			};
	public: // Constructors and destructor
		/**
        * Symbian 1st phase static constructor.
        *
        * @param aViewType the view type (By Landmark or by category)
        */
        IMPORT_C static CLmkMapNavigationInterface* NewL( CPosLandmarkDatabase& aDb );

        /**
        * Destructor.
        */
       IMPORT_C ~CLmkMapNavigationInterface();

	public: // from MAiwNotifyCallback
		/**
		* The callback function for the get landmark from map use case
		* This function gets a callback once the user selects a location
		* from map to save as a landmark.
		*/
		IMPORT_C TInt HandleNotifyL( TInt aCmdId,
						            TInt aEventId,
						            CAiwGenericParamList &aEventParamList,
						            const CAiwGenericParamList &aInParamList );
	public: // from MProgressDialogCallback
		/**
		* Callback function for Progress dialog when ok to exit
		* the operation.
		*/
		void DialogDismissedL( TInt aButtonId );

    public: // New methods for Map & Navigation use cases

    	/**
    	* Shows the provided landmarks on map
    	*
    	* @param aArray pointer array of landmarks which are to be displayed on map
    	*
    	* @param aCommand command id for show on map
    	* @param aViewType
    	*/
    	IMPORT_C void ShowLandmarksOnMapL( RPointerArray<CPosLandmark> &aArray,
    									   TInt aCommand,
    									   EViewType aViewType );

    	/**
    	* Shows all the landmarks of a category on Map. The function searches
    	* Landmaks database through db search utils to get the landmaks in the
    	* category and then displays the map. While the search is being done a
    	* progress note is displayed.
    	*
    	* @param aCategoryId item id of the category whose landmarks need
    	*					 to be displayed on map
    	* @param aViewType
    	*
    	*/
    	IMPORT_C void ShowCategoryOnMapL( TPosLmItemId aCategoryId,
    									  TInt aCommand,
    									  EViewType aViewType  );

    	/**
    	* Displays map to allow navigation to the provided landmark
    	*
    	* @param aLandmark the Landmark to navigate to
    	* @param aCommand is the command id in the menu pane
    	*/
    	IMPORT_C void NavigateToLandmarkL( CPosLandmark* aLandmark, TInt aCommand );

    	/**
    	* This method is called for the get landmarks from map use case. The method
    	* sends the appropriate aiw command to get landmarks from map. Once user
    	* selects the opsition on map, a landmarks editor dialog will be displayed,
    	* populated with the appropriate position data of the position selected.
    	*
    	* @param aCommand menu command id for getting location from map
    	*/
    	IMPORT_C void GetLandmarkFromMapL( TInt aCommand );

    	/**
    	* This method is called for the get landmarks from map for saving a category.
    	* Once user selects the opsition on map, a landmarks editor dialog will be displayed,
    	* populated with the appropriate position data of the position selected.
    	*
    	* @param aCommand menu command id for getting location from map
    	* @param aCategoryId is the category in which the landmark needs to be saved
    	*/
    	IMPORT_C void GetLandmarkFromMapForCategoryL( TInt aCommand, TPosLmItemId aCategoryId );

    	/**
    	* Method returns the service command of the provided menu command.This
    	* function needs to be invoked in the HandleCommandL function when a
    	* menu option is selected. If the function returns anythin other than
    	* KAiwCommandNone then it is implied that some interest has been attached
    	* to the menu command and is a AIW operation.
    	*
    	* @param aCommand the command id for which the checks need to be made
    	*
    	* @return service id if valid, KAiwCmdNone otherwise
    	*/
    	IMPORT_C TInt GetServiceCmdByMenuCmd( TInt aCommand );

    	/**
    	* Method used to attach menu interest to AIW. If it is required to attach
    	* interest to any menu pane / command then it is require to invoke this
    	* function with the required menu pane with the required interest. The
    	* interest is an AIW interest that is defined in the resource file.
    	*
    	* @param aMenuPane is the id of the menu pane from the resource file
    	* 		 to which the interest needs to be attached
    	*
    	* @param aInterest is the id of the interest from the resource file
    	*/
    	IMPORT_C void AttachAIWInterestL( TInt aMenuPane, TInt aInterest );

    	/**
    	* Method used to attach menu panes which are specific to Map and Navigation.
    	* This function bascially handles the dimming of menu commands in menu panes
    	* and needs to be invoked in function "DynInitMenuPaneL" providing it with the
    	* resource id, menu pane and the base command, which is the command enum from
    	* which all Map and Navigation menu id's are declared (basically the offset).
    	*
    	* @param aResourceId resource id of the selected item
    	*
    	* @param aMenuPane is the pointer of the menu pane to which the submenu
    	*        will be attached
    	*
    	* @param aBaseCommand is the base command for map & navigation menu items
    	*
    	* @return ETrue if sub menu pane needs to be attached and will cascade the
    	*         sub menu, EFalse otherwise
    	*/
    	IMPORT_C TBool HandleMenuOperationL( TInt aResourceId, CEikMenuPane *aMenuPane, TInt aBaseCommand );

    	/**
    	* Method used to attach/edit menu panes. AVKON forms create
    	* its own menu panes due to which it is required to attach this to AIW's
    	* menu pane. In such cases this helper function can be used
    	*
    	* @param aResourceId resource id of the selected item
    	*
    	* @param aMenuPane is the pointer of the menu pane to which the submenu
    	*        will be attached
    	*/
    	IMPORT_C void AttachMenuPaneL( CEikMenuPane *aMenuPane, TInt aResourceId, TInt aBaseCommand );

    	/**
    	* Detaches a particualr interest from the service handler.
    	*
    	* @param aMenuResourceId is the menu id
    	* @param aInterestResourceId is the resource id of the interest
    	*/
    	IMPORT_C void DetachInterestL( TInt aMenuResourceId, TInt aInterestresourceId );

    	/**
    	* Sets the observer. It is called when map & navigation finishes
    	* any request related to landmark.
    	* @param aObserver is of type MLmkMapAndNavigationObserver
    	*/
    	IMPORT_C void SetObserver(MLmkMapAndNavigationObserver* aObserver);

    	/**
    	* Decreases the reference count, if it becomes zero, detroys self
    	*/
    	IMPORT_C void Release();

    public: // From CLmkDbSearchUtilsObserver
        /**
        * This is the callback function for the asynchronus search notification.
        * This is used for the "Show category on map" use case. This function is
        * the callaback to get all landmark id's under a particualre category.
        *
        * @param aArray is the array of the landmarks id's associated with the
        *				particular category
        */
    	void HandleCategorySearchNotifyL( RArray<TPosLmItemId> aArray );

    	/**
    	* This is from CLmkDbSearchUtilsObserver for landmarks search
    	* This search is not used and will have a dummy block
    	*/
    	void HandleLmkSearchNotifyL();
    private:
    	/**
        * C++ default constructor.
        */
        CLmkMapNavigationInterface( CPosLandmarkDatabase& aDb );

        /**
        * Symbian 2nd phase constructor.
        *
        * @param aViewType the view type (By Landmark or by category)
        */
        void ConstructL();

    private: // Private functions
    	/**
    	* This function is the santity check for the incomming landmarks for all
    	* use cases. The function does all the error checks and also does the
    	* error notification to the user through error notes and dialogs.
    	*
    	* @param aArray is the array of landmarks on which the checks need to made
    	*/
    	TBool CheckAndDisplayIfLandmarksEmptyL( RPointerArray<CPosLandmark> &aArray );

    	/**
    	* Methods saves the Landmark from the Get Landmarks from Map use case.
    	* This function opens an editor dialog to and populates it with the
    	* position information of the chosen landamark.
    	*
    	* @param aLandmark the landmark to be saved
    	*/
    	void SaveLandmarkL( CPosLandmark& aLandmark );
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
    	TBool IsLandmarkDataEmptyL( CPosLandmark& aLandmark );


    private: // Data
    	// Owns : Pointer to AIW service handler to send service commands
    	CAiwServiceHandler* iAiwServiceHandler;

    	// Owns : Generic parameter input list
    	CAiwGenericParamList*   iInList;

    	// Owns : Generic parameter output list
        CAiwGenericParamList*   iOutList;

        // Describes if view type is by Landmark view or by Category view
        EViewType iViewType;

        // Owns: Pointer to landmark search util
        CLmkDbSearchUtils* iSearchUtil;

        // Menu command for showing category on map
        TInt iCategoryCommand;

        //Owns: Category name, for by category view, to display in error notes
        HBufC* iCategoryName;

        // Category id to which the select from Map use case needs to save to
        TPosLmItemId iCategoryId;

        //Owns : processing wait note
        CAknWaitDialog* iWaitNote;

        // Array of Landmarks required for show category on use case
        RPointerArray<CPosLandmark> iLandmarkArray;

        //Ref:open handle to landmark database
        CPosLandmarkDatabase& iDb;

        //Ref
        MLmkMapAndNavigationObserver* iObserver;

        // Ref
        CEikonEnv*              iEnv;

        TInt iRefCnt;
	};

#endif //CLMKMAPNAVIGATIONINTERFACE_H

// End of file

