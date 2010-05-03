/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Database manager implementation for location picker and
*              maptile service.
*
*/

#ifndef __MYLOCATIONSDATABASEMANAGER_H__
#define __MYLOCATIONSDATABASEMANAGER_H__

// Landmarks
#include<epos_cposlandmarkdatabase.h>
#include <EPos_CPosLmCategoryManager.h>

// Lookup database
#include "mylocationslookupdb.h"

// Maximum string length of landmark address.
const TUint32 KMaxAddressLength = 255; 

/** Defines entry change type
*/
enum TEntryChangeType
    {
    /** Entry added */
    EEntryAdded,
    /** Entry modified */
    EEntryModified,
    /** Entry deleted */
    EEntryDeleted,
    /** Entry change unknown */
    EEntryUnknown
    };


/**
 * CMyLocationsDatabaseManager class.
 *  This class handles all the operations related to mylocations landmark database.
 *
 */
class CMyLocationsDatabaseManager : public CBase
    {
    public:
        // Constructors and destructor
    
        /**
         * ConstructL.
         * 2nd phase constructor.
         */
        void ConstructL();
    
        /**
         * CMyLocationsDatabaseManager.
         * C++ default constructor. 
         */
        CMyLocationsDatabaseManager();
    
        /**
         * ~CMyLocationsDatabaseManager.
         * Virtual Destructor.
         */
        virtual ~CMyLocationsDatabaseManager();
        
    public:
        /**
         * UpdateDatabaseL.
         * Updates the location into the landmark database and lookup table. 
         * Based on the entry source type and the entry change type the updation can be 
         * addition/modification/deletion.
         * 
         * @param[in] aLandmark Landmark object to be updated in the database.
         * @param[in] aUid Uid of the changed source entry.
         * @param[in] aSourceType Source type of the aUid.
         * @param[in] aChangeType Type of change to the entry ( added/modified/deleted )
         */
        void UpdateDatabaseL( CPosLandmark* aLandmark, const TUint32 aUid, 
                              const TUint32 aSourceType, const TEntryChangeType aChangeType );
        
        /** Gets the full address from a given landmark object
         *
         * @param[out] aLandmarkAddress comma separated landmark's full address
         * @param[in] aLandmark landmark object whose address needs to be found.
         */      
        void GetLandmarkFullAddress( TBuf<KMaxAddressLength>& aLandmarkAddress, 
                                     const CPosLandmark* aLandmark );

        
    private:
        
        /**
         * AddMylocationsCategory.
         * Adds the category to mylocations and landmarks database. 
         * Used to add history and others categories.
         * @param[in] aCategoryType defines the category type to be added.
         */
        void AddMylocationsCategoryL( const TUint32 aCategoryType );

        /**
         * CheckIfDuplicateExistsL.
         * Checks if this landmark is already present in database. 
         * If present returns the landmark id, else 0.
         * @param[in] aLandmark A landmark object to be checked for.
         * @returns If no duplicate is found in database returns 0 
         * else returns the id of the found landmark.
         */
        TPosLmItemId CheckIfDuplicateExistsL( const CPosLandmark* aLandmark );
        
        /**
         * CompareLandmarks.
         * Compares two landmarks. Only the text fields, landmark name, 
         * street, city, state country and postal code are compared. 
         * @param[in] aLandmark1 First landmark object to be compared.
         * @param[in] aLandmark2 Second landmark object to be compared.
         * @returns ETrue if the landmarks are same, else returns EFalse.
         */
       TBool CompareLandmarks( const CPosLandmark* alandmark1, 
                               const CPosLandmark* alandmark2 );
        
       /**
         * HandleEntryAdditionL.
         * Handles the entry addition in lookup table and landmarks db
         * @param[in] aLandmark Landmark object added.
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
         * @param[in] aSourceType source type of the aUid passed.
        */
       void HandleEntryAdditionL( CPosLandmark*  aLandmark, const TUint32 aUid,
                                  const TUint32 aSourceType );

       /**
         * HandleEntryModificationL.
         * Handles the entry modification in lookup table and landmarks db
         * @param[in] aLandmark Landmark object modified.
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
         * @param[in] aSourceType source type of the aUid passed.
        */
       void HandleEntryModificationL( CPosLandmark*  aLandmark, const TUint32 aUid,
                                      const TUint32 aSourceType );

       /**
         * HandleEntryDeletionL.
         * Handles the entry deletion in lookup table and landmarks db
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
         * @param[in] aSourceType source type of the aUid passed.
        */
       void HandleEntryDeletionL( const TUint32 aUid, const TUint32 aSourceType );

       /**
         * AddToMylocationsDbL.
         * Adds entry into mylocations db and also updates the lookup table
         * @param[in] aLandmark Landmark object to be added.
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
         * @param[in] aSourceType source type of the aUid passed.
        */
       void AddToMylocationsDbL( CPosLandmark*  aLandmark, const TUint32 aUid, 
                                 const TUint32 aSourceType );

       /**
         * ModifyMylocationsDbL.
         * Updates the entry into mylocations db and also updates the lookup table
         * @param[in] aLandmark Landmark object modified.
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
         * @param[in] aSourceType source type of the aUid passed.
        */
       void ModifyMylocationsDbL( CPosLandmark*  aLandmark, const TUint32 aUid, 
                                  const TUint32 aSourceType );

       /**
         * DeleteFromMylocationsDbL.
         * Deletes the entry from mylocations db and also updates the lookup table
         * @param[in] aUid Uid of the source entry corresponding to aLandmark..
         * @param[in] aSourceType source type of the aUid passed.
        */
       void DeleteFromMylocationsDbL( const TUint32 aUid, const TUint32 aSourceType );

       /**
         * CreateCategoryL.
         * Creates a new category in Mylocations Db and adds a corresponding entry in 
         * mylocations lookup table.
         * @param[in] aUid Uid of the source category.
        */
       void CreateCategoryL( const TUint32 aUid );

       /**
          * CheckCategoryAvailabilityL.
          * Checks whether a category is available in database pointed by category manager.
          * @param[in] aCategoryManager handle to the category manager
          * @param[in] aCategoryId Id of the category
         */
       void CheckCategoryAvailabilityL( CPosLmCategoryManager* aCategoryManager, 
                                        const TUint32 aCategoryId );

       /**
          * CheckAndReadLandmarkL.
          * Checks whether a category is available in database pointed by category manager.
          * @param[in] CPosLandmarkDatabase handle to the database manager
          * @param[in] aLmId Id of the landmark
          * @returns landmark object if found else NULL
         */
       CPosLandmark* CMyLocationsDatabaseManager::CheckAndReadLandmarkL( 
                       CPosLandmarkDatabase* aDb, const TUint32 aLmId );
       
    private:
        
        // Handle to the landmark database
        CPosLandmarkDatabase* iLandmarkDb;
        
        // Handle to the mylocations landmark database
        CPosLandmarkDatabase* iMyLocationsLandmarksDb;

        // Contacts category to be created for contacts and calendar 
        // related location entries in landmark database
        TPosLmItemId iLmContactsCatId;  
        
        // handle to landmarks lookup database.
        CLookupDatabase*  iLandmarksLookupDb;

        // handle to mylocations lookup database.
        CLookupDatabase*  iMylocationsLookupDb;

        // handle to mylocations category manager
        CPosLmCategoryManager* iMyLocationsCatManager;

        // handle to landmarks category manager
        CPosLmCategoryManager* iLandmarksCatManager;
        
        // handle to the file session
        RFs iFsSession;
       
    };

#endif  // __MYLOCATIONSDATABASEMANAGER_H__
