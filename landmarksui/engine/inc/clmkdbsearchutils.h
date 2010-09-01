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
* Description:    LandmarksUi Content File -    This file contains search utilities to query the landmarks Db.
*				 For example getting all landmarks under a category, Name of a
*				 particular category etc.
*
*/







#ifndef CLMKDBSEARCHUTILS_H
#define CLMKDBSEARCHUTILS_H

//Include files
#include "MLmkAOOperationObserver.h"

// Forward declaration
class CPosLandmark;
class CPosLandmarkDatabase;
class CPosLandmarkSearch;
class CPosLmCategoryCriteria;
class MLmkDbSearchUtilsObserver;
class CLmkAOOperation;

class CLmkDbSearchUtils : public CActive, public MLmkAOOperationObserver
	{
	public: // Constructors and destructor
		/**
		* Create CLmkDbSearchUtils object
		*
		*/
		IMPORT_C static CLmkDbSearchUtils* NewL();

		/**
		* Destructor.
		*/
		IMPORT_C ~CLmkDbSearchUtils();

	private:
	    /**
		* Default C++ constructor
		*/
		CLmkDbSearchUtils();

		/*
		* Symbian two phase constructor
		*/
		void ConstructL();

	public:
		/**
		* Searches the db and gets the landmark under a mentioned
		* category Id. This is a asynchronous request. The observers
		* HandleSearchNotifyL is called when the search is complete
		*
		* @param aObserver observer to get the asynchronous callabck
		* @param aCategoryId required category id
		*/
		IMPORT_C void GetLandmarksUnderCategoryL(MLmkDbSearchUtilsObserver* aObserver,
									   			 TPosLmItemId aCategoryId );
        /**
        * Get the name of the required category.
        *
        * @param aCategoryId the category id whose name is required
        */
	    IMPORT_C HBufC* GetCategoryNameL(TPosLmItemId aCategoryId);

	    /**
	    * Returns the CPosLandmark object for a particular landmark Id.
	    *
	    * @param aLandmarkId is the db id of the required landmark
	    */
	    IMPORT_C CPosLandmark* GetLandmarkForIdL(TPosLmItemId aLandmarkId);

	    /**
	    * Gets the landmarks whose item id's are specified in a asynchronus
	    * way. The observer gets a callback once all the landmark objects
	    * have been retreived.
	    *
	    * @param aObserver is the pointer to the observer which makes the request
	    * @param aLmItemId is the Landmarks Item array which as the item id's
	    * @param aArray is the out parameter which holds all the landmark objects
	    */
	    IMPORT_C void GetLandmarksL(MLmkDbSearchUtilsObserver* aObserver,
	    							RArray<TPosLmItemId> aLmItemId,
	    							RPointerArray<CPosLandmark>* aArray);

	private: // From MLmkAOOperationObserver
		/**
		* Callback from
		*/
		void HandleOperationL(TOperationTypes aType,
							  TReal32 aProgress,
							  TInt aStatus );

	private:// From CActive
		void RunL();
		void DoCancel();


	private: // Data
		/// Observer of the search
		MLmkDbSearchUtilsObserver* iObserver;

		//Owns:reference to the landmark database
		CPosLandmarkDatabase* iDb;

		//Owns:landmark search variable
		CPosLandmarkSearch* iSearch;

		// Owns:criteria for search
		CPosLmCategoryCriteria* iCriteria;

		// Owns:pointer to the search AO object
		CLmkAOOperation* iSearchAO;

		// Owns:Pointer to the array in which the landmark objects need to be pushed
		RPointerArray<CPosLandmark>* iLmkArray;

		// Array of landmark item id's which need to be obtained
		RArray<TPosLmItemId> iLmItemId;

		// Index to iLmItemId for the landmarks search
		TInt iSearchIndex;
	};

#endif // CLMKDBSEARCHUTILS_H


