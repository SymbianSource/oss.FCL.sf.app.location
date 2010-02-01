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
* Description:    LandmarksUi Content File -    This class provides utility methods for performing
*                landmark database related operations like adding, deleting
*                or updating a landmark and category etc.
*
*/







#ifndef CLMKDBUTILS_H
#define CLMKDBUTILS_H

//  INCLUDES
#include <lbsfields.h>
#include <EPos_Landmarks.h>
#include "landmarks.hrh"
#include "MLmkAOOperationObserver.h"

// FORWARD DECLARATIONS
class CPosLandmarkCategory;
class CPosLandmarkDatabase;
class CPosLmCategoryManager;
class CPosLmOperation;
class CPosLandmark;

// CLASS DECLARATION
/**
* This class provides utility methods for performing operations related to
* landmark database.
*
* Typical operations can be adding, deleting or updating landmark
* and creating a new category or renaming a category etc.
*
* All the methods of this class except InitializeDbL() assume that the database
* is opened and initialized.
*/

class CLmkDbUtils : public CBase, public MLmkAOOperationObserver
    {
	public:  // Constructors and destructor
		/**
		* Create CLmkDbUtils object
        * @param aDb -- landmark database which needs to be accessed via this class
        * @param aObserver -- calling class or object as observer for this class
        * @aProgressNote - Note showing the progress of landmark database related
        *                  operations
		* @return newly instantiated object
		*/
		IMPORT_C static CLmkDbUtils* NewL( CPosLandmarkDatabase& aDb,
                                           MLmkAOOperationObserver& aObserver,
                                           TBool aProgressNote );

		/**
		* Destructor.
		*/
		~CLmkDbUtils();

    private:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb -- landmark database
        * @param aObserver -- calling class passing itself as observer of this class
        * @param aProgressNote -- Note showing the progress of landmark database operations
        * @return newly instantiated object
        */
        CLmkDbUtils( CPosLandmarkDatabase& aDb,
                     MLmkAOOperationObserver& aObserver,
                     TBool aProgressNote );

        /**
		* By default Symbian 2nd phase constructor is private.
		*/
        void ConstructL();

    protected: //from MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                               TReal32 aProgress,
                               TInt aStatus );

    public: // New functions

        /**
        * Initializes landmark database.
        * This operation is asynchronous
        */
        IMPORT_C void InitializeDbL();

        /**
        * Compacts landmark database
        * This operation is asynchronous
        */
        IMPORT_C void CompactDbL();

        /**
        * Adds a new category to landmark database
        * This call is a synchronous one
        *
        * @param aCategory -- category which needs to be added to database
        * @param aDb       -- landmark database to which category needs to be added
        */
        IMPORT_C static void AddCategoryL( CPosLandmarkCategory& aCategory,
                                           CPosLandmarkDatabase& aDb );

        /**
        * Rename category
        * This call is synchronous
        * @param aCategoryName -- name of the category which needs to be renamed
        * @param aId           -- Unique identfier of the category in the database
        * @param aDb           -- handle to landmark database
        */
        IMPORT_C static void RenameCategoryL( TDesC& aCategoryName,
                                              TPosLmItemId aId,
                                              CPosLandmarkDatabase& aDb );

        /**
        * Get category name given category and this call is synchronous
        * @param aDb Handle to landmark database
        * @param aCategoryId Identifier of the category in database whose
        * @return aCategoryName
        */
        IMPORT_C static HBufC* CategoryNameL(
                                    CPosLandmarkDatabase& aDb,
                                    TPosLmItemId  aCategoryId );

        /**
        * Change the icon of category or landmark
        * This call is synchronous
        * @param aDb handle to landmark database
        * @param aId Identifier of the landmark or category whose icon needs to be changed
        * @param aItemType type of database item (landmark or category)
        * @param aMbmFileName Icon file name
        * @param aIconIndex Index of the icon in the icon file
        * @param aMaskIndex Index of icon mask in the icon file
        */
        IMPORT_C static void ChangeIconL( CPosLandmarkDatabase& aDb,
                                          const TPosLmItemId aId,
                                          TLmkItemType aItemType,
                                          const TDesC& aMbmFileName,
                                          TInt aIconIndex, TInt aMaskIndex);

        /**
        * Change multiple item's icon
        * This is a asynchronous call
        * @param aIdArray Array of landmark database items whose icon needs to be changed
        * @param aItemsType Landmark or category
        * @param aMbmFileName name of icon file
        * @param aIconIndex Index of the icon inside the icon file
        * @param aMaskIndex Index of icon mask inside the icon file
        */
        IMPORT_C void ChangeIconsL(
                                const RArray<TPosLmItemId>& aIdArray,
                                TLmkItemType aItemsType,
                                const TDesC& aMbmFileName,
                                TInt aIconIndex, TInt aMaskIndex);

        /**
        * Delete categories from landmark database
        * This is asynchronous method
        * @param aCategoryIdArray Array containing categories to be deleted
        */
        IMPORT_C void DeleteCategoriesL(
                                const RArray<TPosLmItemId>& aCategoryIdArray );

        /**
        * Delete a single landmark from database
        * This is a synchronous method
        * @param aId Identifier of landmark to be deleted
        * @param aDb Handle to landmark database
        */
        IMPORT_C static void DeleteLandmarkL( TPosLmItemId aId,
                                              CPosLandmarkDatabase& aDb );

        /**
        * Delete a set of selected landmarks from landmark database
        * This is an Asynchronous method
        * @param aLandmarkIdArray Array containing the landmarks to be deleted
        */
        IMPORT_C void DeleteLandmarksL(
                                const RArray<TPosLmItemId>& aLandmarkIdArray );

        /**
        * Adds a landmark to a category
        * This is  synchronous
        * @param aLandmarkId Identifier of landmark
        * @param aCategoryId Identifier of category
        * @param aDb
        */
        IMPORT_C static void AddLmToCategoryL( TPosLmItemId aLandmarkId,
                                               TPosLmItemId aCategoryId,
                                               CPosLandmarkDatabase& aDb );

        /**
        * Adds a single landmark to a set of selected categories
        * This is asynchronous call
        * @param aLandmarkId Unique identifier of landmark which need to be added to set of categories
        * @param aCategoryIdArray Array containing categories
        */
        IMPORT_C void AddLmToCategoriesL(
                                TPosLmItemId aLandmarkId,
                                const RArray<TPosLmItemId>& aCategoryIdArray );

        /**
        * Adds  multiple landmarks to one or more categories
        * This is an asynchronous call
        * @param aLandmarkIdArray Array of landmarks which need to be added to given categries
        * @param aCategoryIdArray Array of categories
        */
        IMPORT_C void AddLmsToCategoriesL(
                                const RArray<TPosLmItemId>& aLandmarkIdArray,
                                const RArray<TPosLmItemId>& aCategoryIdArray );

        /**
        * Cancel landmark database related operation
        * This cancels the Active Object created for landmark database related operation
        */
        IMPORT_C void CancelLmOperation();

        /**
        * It returns the category id,if given name
        */
    	IMPORT_C  TPosLmItemId CategoryIdL(CPosLandmarkDatabase& aDb, const TDesC&  aCategoryName );

		/**
        * Checks if this field exists in the landmark.
        */
		IMPORT_C TBool IsLmWithThisFieldExistL( TPositionFieldId  aFieldId);

		/**
        * Checks, if the url value is >150 , it trims the protocol
        * since it assumes,that the protocol is appended by the sender
        * @param landmark The landmark, from which the url field value
        *                 to be trimmed.
        */
		IMPORT_C static void RemoveDefaultProtocolL(TPtr& aWebUrl);
		/*
		* This function, appends the default protocol, if it is missing
		* from the url.
		*/
		IMPORT_C static void AddDefaultProtocolL(CPosLandmark* landmark);

    private:  // New functions
        /**
        * This call checks if an Active Object is already running or not
        * Leaves if an Active Object is already running
        */
        void LeaveIfInUseL();

        /**
        * Helper method. It pushes the aOperation to the cleanup
        * stack as the first step, so caller does not have to do it.
        * @param aOperation, ownership is transferred
        * @param aOpType
        */
        void SetupAndStartOperationL(
                            CPosLmOperation* aOperation,
                            MLmkAOOperationObserver::TOperationTypes aOpType );


    private:    // Data
        ///Ref:
        CPosLandmarkDatabase& iDb;

        ///Ref:
        MLmkAOOperationObserver* iObserver;

        TBool iProgressNote;

        ///Own:
        CLmkAOOperation* iAOOperation;

        ///Own:
        CPosLmCategoryManager* iCategoryMgr;
    };

#endif      // CLMKDBUTILS_H

// End of File
