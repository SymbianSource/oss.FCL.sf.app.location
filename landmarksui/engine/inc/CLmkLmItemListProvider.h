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
* Description:    LandmarksUi Content File -    Abstract base class for landmark and category providers.
*
*/








#ifndef CLMKLMITEMLISTPROVIDER_H
#define CLMKLMITEMLISTPROVIDER_H

//  INCLUDES
#include "CLmkListProviderBase.h"
#include "MLmkAOOperationObserver.h"
#include "MLmkDbObserver.h"
#include "landmarks.hrh"
#include <EPos_Landmarks.h> // Lm typedefs, constants etc.
#include <EPos_CPosLandmark.h>

// FORWARD DECLARATIONS
class CPosLandmarkDatabase;
class CLmkDbUtils;
class CLmkDbInitializer;
class CLmkAOOperation;
class CAknWaitDialog;

// CLASS DECLARATION

/**
*  Abstract base class for landmark and category providers.
*  Each concrete provider is responsible of notifying observer about all
*  provider events.
*/
class CLmkLmItemListProvider
: public CLmkListProviderBase,
  public MLmkAOOperationObserver,
  public MLmkDbObserver
    {
    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        ~CLmkLmItemListProvider();

    public: // New methods
        /**
        * Any item which is not found is ignored.
        * @param aSelectedItems database ids for selected items
        * @return array containing selected items, ownership is transferred
        */
        IMPORT_C CArrayFix<TInt>* SelectedIndexesL(
                        const RArray<TPosLmItemId>& aSelectedItems ) const;

        /**
        * Any item which is not found is ignored.
        * @param aSelectedIndexes will be set by this method
        * @param aSelectedItems database ids for selected items
        */
        IMPORT_C void GetSelectedIndexesL( CArrayFix<TInt>& aSelectedIndexes,
                        const RArray<TPosLmItemId>& aSelectedItems ) const;

        /**
        * Leaves if appending an item to aSelectedItems fails.
		* Panics if any index is out of bounds.
        * @param aSelectedItems
        * @param aSelectedIndexes
        */
        IMPORT_C void GetSelectedItemIdsL(
                        RArray<TPosLmItemId>& aSelectedItems,
                        const CArrayFix<TInt>& aSelectedIndexes ) const;

		IMPORT_C void GetAllItemIdsL(
		    RArray<TPosLmItemId>& aSelectedItems ) const;

        /**
        * Returns KErrNotFound if item is not found.
        * @param aSelectedItem
        * @return error code
        */
        IMPORT_C TInt SelectedIndex( TPosLmItemId aSelectedItem ) const;

        /**
        * Panics if index is out of bounds.
        * @param aSelected
        * @param aIndex
        */
        IMPORT_C void GetSelectedItemId( TPosLmItemId& aSelected,
                                         TInt aIndex ) const;

        /**
        * Any item which is not found is ignored.
        * @param aSelectedItemsCount will set iSelectedItemsCount class variable by this method
        */
        IMPORT_C void SetSelectedItemsCount( TInt aSelectedItemsCount );

        /**
        * Any item which is not found is ignored.
        * @param aItemId will be removed from the iIdArray
        */
        IMPORT_C void RemoveItem( TPosLmItemId aItemId );

        /**
        * Any item which is not found is ignored.
        * @param aOperationCmd will set the Database Operation Command
        */
        IMPORT_C void SetOperationCmd( TLmkAppCmdId aOperationCmd );

        /**
        * Any item which is not found is ignored.
        * The Function must be used if no of selected items is greater than 10
        * @param aSelectedItems, the item count should be greater than 10
        * @param aType is a operation type
        * @param aCategoryId will set the category id to filter for deletion
        */
		IMPORT_C void RemoveItemsL( const RArray<TPosLmItemId>& aSelectedItems,
								MLmkAOOperationObserver::TOperationTypes aType,
								TPosLmItemId aCategoryId );

    public: // From CLmkListProviderBase
        void InitializeL();
        TInt ItemCount() const;
        TBool StartSearchingLandmarksL(
								const TDesC& aSearchPattern,
    							TBool aSearchOnlyInPreviousMatches);

    protected: // From MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                               TReal32 aProgress,
                               TInt aStatus );

    protected: // From MLmkDbObserver
        void HandleDatabaseEvent( TPosLmEvent& aEvent );

    protected:  // New functions
        /**
        * "Template method" implemented by subclasses.
        * List preparation cannot be done in construction phase, but
        * only after database initialization is complete.
        */
        virtual void PrepareListL() = 0;
        virtual TBool PrepareListL(const TDesC& aSearchPattern,
    							TBool aSearchOnlyInPreviousMatches) = 0;

        virtual void RemoveLandmarksL(const RArray<TPosLmItemId>& aSelectedItems ) = 0;

        /**
        * Internal helper method for performing steps after
        * database initialization.
        */
        void HandleDatabaseInitializedL();

        /**
        * Internal helper method which handles provider errors. It
        * resets iIdArray and notifies observers.
        * @param aError error code
        */
        void HandleError( TInt aError );

    protected:  // Constructors and destructor
        /**
        * C++ constructor.
        * @param aDb
        * @return newly instantiated object
        */
        CLmkLmItemListProvider( CPosLandmarkDatabase& aDb );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();

	    /**
	    * This function gets the landmark from the database &
	    * update its icon with the category icon, if it is associated
	    * with one category.
	    */
	    CPosLandmark* GetLandmarkLC(TInt aIndex);

    protected: // Data

        ///Ref: landmark database
        CPosLandmarkDatabase& iDb;

        /// Provide all db items or only the ones in the array?
        TBool iListAllItemsInDb;

        /// TPosLmItemId list
        RArray<TPosLmItemId> iIdArray;

        ///Own:
        CLmkDbUtils* iDbUtils;

        ///Own: helper for initialization
        CLmkDbInitializer* iInitializer;

		//own:  stores only those CPosLandmark objects, for which user has not set the icons
		//      explicitly
        RPointerArray<CPosLandmark>* iIconChangeLmkArray;

        ///Own:
        //CLmkAOOperation* iSearchAO;

        ///Own
        TInt iSelectedItemsCount;

        ///Own : stores the search result count
        TInt iCount;

        //Own
        TBool iRequestFromFindBox;

		//Own : set the progress note flag to start CAOOperation
		//		to get asynchronous search result
        TBool iOperationNotInUse;

        //Own : sets the operation given from the ui
        TLmkAppCmdId iOperationCmd;

        //Own : Operation type assign for operation
        MLmkAOOperationObserver::TOperationTypes iType;

        //Own : to store the
        RArray<TPosLmItemId> iSelectedForDelete;

 		//Own : no of landmarks to be deleted
        TInt iItemsToDelete;

       //Own : no of landmarks deleted while in the operation
        TInt iItemsDeleted;

        //Own is removelandmars operation going on.
        TBool iIsRemoveLandmarks;

        //Own : Waitnot for operations
        CAknWaitDialog* iWaitNote;

		//Own : CategoryId to remove landmarks from category operation
		TPosLmItemId iCategoryId;
    };

#endif      // CLMKLMITEMLISTPROVIDER_H

// End of File
