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

#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkCategory.h>
#include <EPos_CPosLmTextCriteria.h>
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_CPosLmDatabaseManager.h>
#include <lbsposition.h>
#include <mylocations.rsg>
#include <barsread.h>
#include <barsc.h>
#include "mylocationsdatabasemanager.h"
#include "mylocationlogger.h"
#include "mylocationsdefines.h"
//Custom landmark database for storing the my locations details
_LIT( KMyLocationsDatabaseUri, "file://c:MyLocationsLandmarks.ldb" );
// separator
_LIT( KSeparator, ",");
// space
_LIT( KSpace, " ");

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::ConstructL()
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::ConstructL()
{
    __TRACE_CALLSTACK;//Open and intialize Landmark DB
    iLandmarkDb = CPosLandmarkDatabase::OpenL();
    ExecuteAndDeleteLD(iLandmarkDb->InitializeL());

    // create landmarks lookup database.
    iLandmarksLookupDb = CLookupDatabase::NewL(KLandmarksLookupDatabaseName);
    User::LeaveIfError( iLandmarksLookupDb->Open() );
    
    CPosLmDatabaseManager* dbManager = CPosLmDatabaseManager::NewL();
    CleanupStack::PushL(dbManager);

    //Create custom landmark database for storing my locations data
    if (!dbManager->DatabaseExistsL(KMyLocationsDatabaseUri))
    {
        HPosLmDatabaseInfo* dbInfo = HPosLmDatabaseInfo::NewLC(
                KMyLocationsDatabaseUri);
        dbManager->CreateDatabaseL(*dbInfo);
        CleanupStack::PopAndDestroy(dbInfo);
    }

    CleanupStack::PopAndDestroy(dbManager);

    //Open and initialize the custom landmark database
    iMyLocationsLandmarksDb = CPosLandmarkDatabase::OpenL(
                                        KMyLocationsDatabaseUri);
    if (iMyLocationsLandmarksDb->IsInitializingNeeded())
    {
        ExecuteAndDeleteLD(iMyLocationsLandmarksDb->InitializeL());
    }

    // create mylocations lookup database.
    iMylocationsLookupDb
            = CLookupDatabase::NewL(KMylocationsLookupDatabaseName);

    // Create category manager for mylocations
    iMyLocationsCatManager = CPosLmCategoryManager::NewL(
            *iMyLocationsLandmarksDb);

    // Create category manager for landmarks
    iLandmarksCatManager = CPosLmCategoryManager::NewL(*iLandmarkDb);

    //open the lookup database
    User::LeaveIfError( iMylocationsLookupDb->Open() );

    // open file session
    User::LeaveIfError(iFsSession.Connect());

    // Add contacts, calendar and history categories
    AddMylocationsCategoryL(ESourceLandmarksContactsCat);
    //close the lookup database
    iMylocationsLookupDb->Close();

}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CMyLocationsDatabaseManager()
// Default constructor.
// -----------------------------------------------------------------------------
//
CMyLocationsDatabaseManager::CMyLocationsDatabaseManager() : iLandmarkDb( NULL ),
                iMyLocationsLandmarksDb( NULL ), 
                iLmContactsCatId( 0 ), iLandmarksLookupDb( NULL ), 
                iMylocationsLookupDb( NULL ), iMyLocationsCatManager( NULL ), 
                iLandmarksCatManager( NULL )
{
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::_CMyLocationsDatabaseManager()
// Destructor.
// -----------------------------------------------------------------------------
//
CMyLocationsDatabaseManager::~CMyLocationsDatabaseManager()
{
    __TRACE_CALLSTACK;// delete member variables.
    if (iLandmarksLookupDb)
    {
        iLandmarksLookupDb->Close();
        delete iLandmarksLookupDb;
    }
    if (iMylocationsLookupDb)
    {
        iMylocationsLookupDb->Close();
        delete iMylocationsLookupDb;
    }

    delete iLandmarksCatManager;

    delete iLandmarkDb;

    delete iMyLocationsCatManager;

    delete iMyLocationsLandmarksDb;
  
    // close the file session
    iFsSession.Close();

}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::AddMylocationsCategoryL()
// Adds the category to the mylocations and landmarks database..
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::AddMylocationsCategoryL( const TUint32 aCategoryType )
{
    __TRACE_CALLSTACK;//Open the resource file
    RResourceFile resourceFile;
    resourceFile.OpenL( iFsSession, KMyLocationsResourceFile );
    CleanupClosePushL( resourceFile );
    
    // Get the category name
    HBufC8* dataBuffer = NULL;
    if (aCategoryType == ESourceLandmarksContactsCat)
    {
        dataBuffer = resourceFile.AllocReadLC(R_LOCINT_LIST_CATEGORY_CONTACTS);
    }
    
    TResourceReader resReader;
    resReader.SetBuffer(dataBuffer);
    TPtrC resData = resReader.ReadTPtrC();

    TLookupItem lookupItem;
    lookupItem.iLmId = 0;
    lookupItem.iSource = aCategoryType;
    lookupItem.iUid = 0;

    RArray<TLookupItem> itemArray;
    CleanupClosePushL(itemArray);
    iMylocationsLookupDb->FindEntriesBySourceTypeL(lookupItem.iSource,
            itemArray);

    // Get the category from mylocations lookup table
    if (itemArray.Count() == 0)
    {
        // category not found, so create one.
        CPosLandmarkCategory *category = CPosLandmarkCategory::NewL();
        CleanupStack::PushL(category);
        category->SetCategoryNameL(resData);

        if ( aCategoryType == ESourceLandmarksContactsCat)
        {
            TPosLmItemId landmarksCatId = 0;
            // Add category to landmarks database
            TRAPD ( error, ( landmarksCatId = iLandmarksCatManager->AddCategoryL( *category ) ) );
            if (error == KErrNone || error == KErrAlreadyExists)
            {
                landmarksCatId = iLandmarksCatManager->GetCategoryL(resData);
            }
            lookupItem.iUid = landmarksCatId;
        }

        // Add the catefory to mylocations database
        TPosLmItemId myLocationsCatId = 0;
        // Add category to mylocations database
        TRAPD ( error, ( myLocationsCatId = iMyLocationsCatManager->AddCategoryL( *category ) ) );
        if (error == KErrNone || error == KErrAlreadyExists)
        {
            myLocationsCatId = iMyLocationsCatManager->GetCategoryL(resData);
        }
        // create this entry in mylocations lookup table
        lookupItem.iSource = aCategoryType;
        lookupItem.iLmId = myLocationsCatId;
        iMylocationsLookupDb->CreateEntryL(lookupItem);

        CleanupStack::PopAndDestroy(category);
    }
    else
    {
        // category found in lookup table,
        // update the corresponding category in Mylocations and landmarks database with 
        // current localized string  


        lookupItem.iLmId = itemArray[0].iLmId;
        lookupItem.iUid = itemArray[0].iUid;
        if ( aCategoryType == ESourceLandmarksContactsCat)
        {
            CPosLandmarkCategory *category =
                    iLandmarksCatManager->ReadCategoryLC(lookupItem.iUid);
            category->SetCategoryNameL(resData);
            TRAP_IGNORE ( ( iLandmarksCatManager->UpdateCategoryL( *category ) ) );
            CleanupStack::PopAndDestroy(category);
        }
        // update category in mylocations db
        CPosLandmarkCategory *category2 =
                iMyLocationsCatManager->ReadCategoryLC(lookupItem.iLmId);
        category2->SetCategoryNameL(resData);
        TRAP_IGNORE ( ( iMyLocationsCatManager->UpdateCategoryL( *category2 ) ) );
        CleanupStack::PopAndDestroy(category2);
    }

    CleanupStack::PopAndDestroy(&itemArray);

    if ( dataBuffer )
    {
        CleanupStack::PopAndDestroy(dataBuffer);
    }
    
    if (aCategoryType == ESourceLandmarksContactsCat)
        iLmContactsCatId = lookupItem.iUid;
    
    // Close the resource file
    CleanupStack::PopAndDestroy( &resourceFile );

}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::UpdateDatabaseL()
// Updates the location into the landmark database and lookup table. Based on 
// the entry source type and the entry change type the updation can be 
// addition/modification/deletion.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::UpdateDatabaseL(CPosLandmark* aLandmark,
        const TUint32 aUid, const TUint32 aSourceType, const TEntryChangeType aChangeType)
{
    __TRACE_CALLSTACK;//open the lookup database
    User::LeaveIfError(iMylocationsLookupDb->Open());
    switch (aChangeType)
    {
    // if the entry is added
    case EEntryAdded:
    {
        // Handle this entry in the lookup table and update landmarks db.
        HandleEntryAdditionL(aLandmark, aUid, aSourceType);
        break;
    }
        // if the entry is modified
    case EEntryModified:
    {
        // Handle this entry in the lookup table and update landmarks db.
        HandleEntryModificationL(aLandmark, aUid, aSourceType);
        break;
    }
        // if the entry is deleted
    case EEntryDeleted:
    {
        // Handle this entry in the lookup table and update landmarks db.
        HandleEntryDeletionL(aUid, aSourceType);
        break;
    }
    }
    //close the lookup database
    iMylocationsLookupDb->Close();

}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CheckIfDuplicateExistsL()
// Checks if this landmark is already present in database. If present returns the landmark id, else 0
// -----------------------------------------------------------------------------
//
TPosLmItemId CMyLocationsDatabaseManager::CheckIfDuplicateExistsL(
                                    const CPosLandmark* aLandmark)
{
    __TRACE_CALLSTACK;// Stores the found duplicate landmark's id. 
    TPosLmItemId retId = 0;

    // Initially filter only the landmarks which are nearer to the current landmark.
    // Then we can make a duplicate check on each of the found landmarks.

    // create a search object.
    CPosLandmarkSearch* search = CPosLandmarkSearch::NewL(
            *iMyLocationsLandmarksDb);
    CleanupStack::PushL(search);

    TBuf<KMaxAddressLength> lmAddress;
    GetLandmarkFullAddress(lmAddress, aLandmark);

    // Create the search criterion
    CPosLmTextCriteria* crit = CPosLmTextCriteria::NewLC();
    crit->SetTextL(lmAddress);
    crit->SetAttributesToSearch(CPosLandmark::ELandmarkName);

    // Start the search and execute it at once.
    ExecuteAndDeleteLD(search->StartLandmarkSearchL(*crit));
    CleanupStack::PopAndDestroy(crit);

    // Retrieve an iterator to access the matching landmarks.
    CPosLmItemIterator* iter = search->MatchIteratorL();
    CleanupStack::PushL(iter);

    // Iterate the search matches.
    TPosLmItemId lmId;

    while ((lmId = iter->NextL()) != KPosLmNullItemId)
    {
        //Found duplicate entries.
        // Get the corresponding id in landmarks db
        RArray<TLookupItem> itemArray;
        CleanupClosePushL(itemArray);
        iMylocationsLookupDb->FindEntriesByLandmarkIdL(lmId, itemArray);
        if (itemArray.Count())
        {
            if (itemArray[0].iSource == ESourceLandmarks)
            {
                // return id only if the source is from landmarks database.
                retId = itemArray[0].iUid;
                CleanupStack::PopAndDestroy(&itemArray);
                break;
            }
        }
        CleanupStack::PopAndDestroy(&itemArray);
    }

    CleanupStack::PopAndDestroy(iter);
    CleanupStack::PopAndDestroy(search);

    return retId;
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CompareLandmarks()
// Compares two landmarks. Only the text fields, landmark name, street, city, state country and 
// postal code are compared.
// -----------------------------------------------------------------------------
//
TBool CMyLocationsDatabaseManager::CompareLandmarks(
        const CPosLandmark* aLandmark1, const CPosLandmark* aLandmark2)
{
    __TRACE_CALLSTACK;// Compare landmark names
    TPtrC name1, name2;
    aLandmark1->GetLandmarkName(name1);
    aLandmark2->GetLandmarkName(name2);
    if (name1 != name2)
    {
        return EFalse;
    }

    // Compare street info
    TPtrC street1, street2;
    aLandmark1->GetPositionField(EPositionFieldStreet, street1);
    aLandmark2->GetPositionField(EPositionFieldStreet, street2);
    if (street1 != street2)
    {
        return EFalse;
    }

    // Compare City info
    TPtrC city1, city2;
    aLandmark1->GetPositionField(EPositionFieldCity, city1);
    aLandmark2->GetPositionField(EPositionFieldCity, city2);
    if (city1 != city2)
    {
        return EFalse;
    }

    // compare state info
    TPtrC state1, state2;
    aLandmark1->GetPositionField(EPositionFieldState, state1);
    aLandmark2->GetPositionField(EPositionFieldState, state2);
    if (state1 != state2)
    {
        return EFalse;
    }

    // compare postal code
    TPtrC postalCode1, postalCode2;
    aLandmark1->GetPositionField(EPositionFieldPostalCode, postalCode1);
    aLandmark2->GetPositionField(EPositionFieldPostalCode, postalCode2);
    if (postalCode1 != postalCode2)
    {
        return EFalse;
    }

    // compare country name
    TPtrC country1, country2;
    aLandmark1->GetPositionField(EPositionFieldCountry, country1);
    aLandmark2->GetPositionField(EPositionFieldCountry, country2);
    if (country1 != country2)
    {
        return EFalse;
    }

    return ETrue;
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::HandleEntryAdditionL()
// Handles the entry addition in lookup table and landmarks db
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::HandleEntryAdditionL(CPosLandmark* aLandmark,
        const TUint32 aUid, const TUint32 aSourceType)
{
    __TRACE_CALLSTACK;
    if ( aSourceType == ESourceLandmarks )
    {
        AddToMylocationsDbL(aLandmark, aUid, aSourceType);
        return;
    }
    if ( aSourceType == ESourceLandmarksCategory )
    {
        CreateCategoryL(aUid);
        return;
    }
    // Create a lookup item
    TLookupItem lookupItem;
    lookupItem.iUid = aUid;
    lookupItem.iSource = aSourceType;

    // check if this landmark is already present in database
    TPosLmItemId dupLmId = CheckIfDuplicateExistsL(aLandmark);
    if (dupLmId)
    {
        // landmark already present in db. get the details
        CPosLandmark* dupLandmark = iLandmarkDb->ReadLandmarkLC(dupLmId);
        if (dupLandmark)
        {
            if (aSourceType == ESourceContactsPref || aSourceType
                    == ESourceContactsWork || aSourceType
                    == ESourceContactsHome)
            {
                dupLandmark->AddCategoryL(iLmContactsCatId);
            }
            // update the landmark object in the db
            iLandmarkDb->UpdateLandmarkL(*dupLandmark);
            CleanupStack::PopAndDestroy(dupLandmark);
        }

        // point the lookup item's landmark uid to the existing landmark.
        lookupItem.iLmId = dupLmId;
    }
    else // it is a new entry, so add into the database
    {
        if (aSourceType == ESourceContactsPref || aSourceType
                == ESourceContactsWork || aSourceType == ESourceContactsHome)
        {
            aLandmark->AddCategoryL(iLmContactsCatId);
        }
        // add the landmark into the db. 
        // point the lookup item's landmark uid to the newly created landmark in the db.
        lookupItem.iLmId = iLandmarkDb->AddLandmarkL(*aLandmark);
    }

    // create the entry in the lookup table.
    iLandmarksLookupDb->CreateEntryL(lookupItem);

}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::HandleEntryModificationL()
// Handles the entry modification in the lookup table and landmarks db.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::HandleEntryModificationL(
        CPosLandmark* aLandmark, const TUint32 aUid, const TUint32 aSourceType)
{
    __TRACE_CALLSTACK;
    if ( aSourceType == ESourceLandmarks )
    {
        ModifyMylocationsDbL( aLandmark, aUid, aSourceType );
        return;
    }

    TLookupItem lookupItem;
    lookupItem.iUid = aUid;
    lookupItem.iSource = aSourceType;


    if (aSourceType == ESourceContactsPref || aSourceType
            == ESourceContactsWork || aSourceType == ESourceContactsHome)
    {
        // Behavior: If a contact is modified, 
        // If this entry is not present in lookup table. add the entry and update the landmarks db.
        // If this entry is already present in lookup table, check if the location info is modified or not.
        // If the location info is modified, delete the landmark from db and add the new landmark
        // into the db. 
        // Before deletion make sure that the landmark is not being refered by other lookup entries.

        // find the entry in the lookup table.
        if (iLandmarksLookupDb->FindEntryL(lookupItem))
        {
            // check if the location info is modified by comparing the new landmark with the existing landmark
            CPosLandmark* existingLandmark = NULL;
            TRAPD( error, ( existingLandmark = 
                   CheckAndReadLandmarkL( iLandmarkDb, lookupItem.iLmId ) ) );
            CleanupStack::PushL(existingLandmark);
            if (error == KErrNotFound)
            {
                // Landmarks item deleted. So delete corresponding lookup entries.
                RArray<TLookupItem> itemArray;
                CleanupClosePushL(itemArray);
                iLandmarksLookupDb->FindEntriesByLandmarkIdL(lookupItem.iLmId,
                        itemArray);
                for (TInt i = 0; i < itemArray.Count(); i++)
                {
                    iLandmarksLookupDb->DeleteEntryL(itemArray[i]);
                }
                CleanupStack::PopAndDestroy(&itemArray);

                // Add the entry into the lookup table and update landmarks db.
                HandleEntryAdditionL(aLandmark, aUid, aSourceType);
                
                CleanupStack::PopAndDestroy(existingLandmark);
                return;
            }

            if (!CompareLandmarks(existingLandmark, aLandmark))
            {
                // landmarks are not same, means location information is modified.

                // Check if the new landmark is already in db.
                TPosLmItemId dupLmId = CheckIfDuplicateExistsL(aLandmark);
                if (dupLmId)
                {
                    // landmark already present in db. get the details
                    CPosLandmark* dupLandmark = iLandmarkDb->ReadLandmarkLC(
                            dupLmId);
                    if (dupLandmark)
                    {
                        // add category.
                        dupLandmark->AddCategoryL(iLmContactsCatId);

                        // update the landmark object in the db
                        iLandmarkDb->UpdateLandmarkL(*dupLandmark);
                    }
                    CleanupStack::PopAndDestroy(dupLandmark);

                    // update the lookup item to refer to the newly created landmark.
                    lookupItem.iLmId = dupLmId;
                    iLandmarksLookupDb->UpdateEntryL(lookupItem);
                }
                else
                {
                    // landmark not already present in db.
                    // Create a new entry in the db
                    aLandmark->AddCategoryL(iLmContactsCatId);
                    lookupItem.iLmId = iLandmarkDb->AddLandmarkL(*aLandmark);
                    // update the lookup table
                    iLandmarksLookupDb->UpdateEntryL(lookupItem);
                }
            }
            else
            {
                // landmarks are same, means location not modified. So return.
                CleanupStack::PopAndDestroy(existingLandmark);
                return;
            }

            // delete the existing landmark only if it not being refered by other lookup entries.

            // Check if any other entries are refering this landmark.
            RArray<TLookupItem> itemArray;
            CleanupClosePushL(itemArray);
            iLandmarksLookupDb->FindEntriesByLandmarkIdL(
                    existingLandmark->LandmarkId(), itemArray);

            if (itemArray.Count())
            {
                // There are other lookup entries refering this landmark. So do not delete the landmark

                // If none of these lookup item's source type is contacts, disassociate 'iLmContactsCatId' category
                // from this landmark.
                TInt i = 0;
                while (i < itemArray.Count())
                {
                    if (itemArray[i].iSource == ESourceContactsPref
                            || itemArray[i].iSource == ESourceContactsWork
                            || itemArray[i].iSource == ESourceContactsHome
                            )
                    {
                        // a lookup item exists which is from contacts, so 'iLmContactsCatId' is still valid.
                        break;
                    }
                    i++;
                }
                if (i == itemArray.Count())
                {
                    // no lookup items from contacts exists refering this landmark.
                    // so disassociate 'iLmContactsCatId' from this landmark

                    existingLandmark->RemoveCategory(iLmContactsCatId);
                    iLandmarkDb->UpdateLandmarkL(*existingLandmark);
                }
            }
            else
            {
                // no other lookup entry is refering this landmark. 

                // check if any other categories is associated with this landmark.
                // Assume this landmark is associated with a history entry or a user created landmark entry.
                // there is a chance that this landmark is still valid.
                // Do not delete the landmark in this case.
                RArray<TPosLmItemId> categoryIdArray;
                CleanupClosePushL(categoryIdArray);
                existingLandmark->GetCategoriesL(categoryIdArray);
                if (categoryIdArray.Count() == 1)
                {
                    // only one category i.e, 'iLmContactsCatId' is associated.
                    // delete the landmark.
                    iLandmarkDb->RemoveLandmarkL(existingLandmark->LandmarkId());
                }
                CleanupStack::PopAndDestroy(&categoryIdArray);
            }
            CleanupStack::PopAndDestroy(&itemArray);
            CleanupStack::PopAndDestroy(existingLandmark);

        }
        else // entry not present in lookup table
        {
            // Add the entry into the lookup table and update landmarks db.
            HandleEntryAdditionL(aLandmark, aUid, aSourceType);
        }
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::HandleEntryDeletionL()
// Handles the entry deletion in lookup table and landmarks db.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::HandleEntryDeletionL(const TUint32 aUid,
                                                const TUint32 aSourceType)
{
    __TRACE_CALLSTACK;
    TLookupItem lookupItem;
    lookupItem.iUid = aUid;
    lookupItem.iSource = aSourceType;

    if (aSourceType == ESourceContactsPref || aSourceType
            == ESourceContactsWork || aSourceType == ESourceContactsHome)
    {
        // Behavior: In the context of contacts, if a contact is deleted, the user is not interested in
        // that contact's data, hence its address as well. So, delete the corresponding entries from 
        // both lookup table and iLandmarkDb.  
        // Before deleting the entry from iLandmarkDb, make sure that this entry is not being refered by
        // other entries of the lookup table. If it is being refered by other entries in lookup table, then
        // do not delete the landmark.

        // Find the corresponding landmark uid
        if (iLandmarksLookupDb->FindEntryL(lookupItem))
        {
            // delete the lookup entry.
            iLandmarksLookupDb->DeleteEntryL(lookupItem);

            // Check if any other entries are refering this landmark.
            RArray<TLookupItem> itemArray;
            CleanupClosePushL(itemArray);
            iLandmarksLookupDb->FindEntriesByLandmarkIdL(lookupItem.iLmId,
                    itemArray);

            if (itemArray.Count())
            {
                // There are other lookup entries refering this landmark. So do not delete the landmark

                // If none of these lookup item's source type is contacts, disassociate 'iLmContactsCatId' category
                // from this landmark.
                TInt i = 0;
                while (i < itemArray.Count())
                {
                    if (itemArray[i].iSource == ESourceContactsPref
                            || itemArray[i].iSource == ESourceContactsWork
                            || itemArray[i].iSource == ESourceContactsHome)
                    {
                        // a lookup item exists which is from contacts/calendar, so 'iLmContactsCatId' is still valid.
                        break;
                    }
                    i++;
                }
                if ( i == itemArray.Count() )
                {
                    // no lookup items from contacts exists refering this landmark.
                    // so disassociate 'iLmContactsCatId' from this landmark

                    CPosLandmark* landmark = iLandmarkDb->ReadLandmarkLC(
                            lookupItem.iLmId);
                    landmark->RemoveCategory(iLmContactsCatId);
                    iLandmarkDb->UpdateLandmarkL(*landmark);
                    CleanupStack::PopAndDestroy(landmark);
                }
            }
            else
            {
                // no other lookup entry is refering this landmark. 

                // check if any other categories is associated with this landmark.
                // Assume this landmark is associated with a history entry or a user created landmark entry.
                // there is a chance that this landmark is still valid.
                // Do not delete the landmark in this case.
                CPosLandmark* landmark = iLandmarkDb->ReadLandmarkLC(
                        lookupItem.iLmId);
                RArray<TPosLmItemId> categoryIdArray;
                CleanupClosePushL(categoryIdArray);
                landmark->GetCategoriesL(categoryIdArray);
                if (categoryIdArray.Count() == 1)
                {
                    // only one category i.e, 'iLmCalendarCatId' is associated.
                    // delete the landmark.
                    iLandmarkDb->RemoveLandmarkL(lookupItem.iLmId);
                }

                CleanupStack::PopAndDestroy(&categoryIdArray);
                CleanupStack::PopAndDestroy(landmark);
            }
            CleanupStack::PopAndDestroy(&itemArray);
        }
    }

    else if (aSourceType == ESourceLandmarks)
    {
        // Landmarks item deleted. So delete corresponding lookup entries.
        RArray<TLookupItem> itemArray;
        CleanupClosePushL(itemArray);
        iLandmarksLookupDb->FindEntriesByLandmarkIdL(aUid, itemArray);
        for (TInt i = 0; i < itemArray.Count(); i++)
        {
            iLandmarksLookupDb->DeleteEntryL(itemArray[i]);
        }

        CleanupStack::PopAndDestroy(&itemArray);

        DeleteFromMylocationsDbL(aUid, aSourceType);
    }
}


// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::GetLandmarkFullAddress()
// Gets the comma separated full address of the given landmark.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::GetLandmarkFullAddress(
        TBuf<KMaxAddressLength>& aLandmarkAddress,
        const CPosLandmark* aLandmark)
{
    TPtrC tempStr;
    TInt retStatus;
    TBool addressEmtpy = ETrue;
    retStatus = aLandmark->GetLandmarkName(tempStr);
    if (retStatus == KErrNone && tempStr.Length())
    {
        aLandmarkAddress.Copy(tempStr);
        addressEmtpy = EFalse;
    }

    retStatus = aLandmark->GetPositionField(EPositionFieldStreet, tempStr);
    if (retStatus == KErrNone && tempStr.Length())
    {
        if (!addressEmtpy)
        {
            aLandmarkAddress.Append(KSeparator);
            aLandmarkAddress.Append(KSpace);
            aLandmarkAddress.Append(tempStr);
        }
        else
        {
            aLandmarkAddress.Copy(tempStr);
            addressEmtpy = EFalse;
        }
    }

    retStatus = aLandmark->GetPositionField(EPositionFieldCity, tempStr);
    if (retStatus == KErrNone && tempStr.Length())
    {
        if (!addressEmtpy)
        {
            aLandmarkAddress.Append(KSeparator);
            aLandmarkAddress.Append(KSpace);
            aLandmarkAddress.Append(tempStr);
        }
        else
        {
            aLandmarkAddress.Copy(tempStr);
            addressEmtpy = EFalse;
        }
    }

    retStatus = aLandmark->GetPositionField(EPositionFieldState, tempStr);
    if (retStatus == KErrNone && tempStr.Length())
    {
        if (!addressEmtpy)
        {
            aLandmarkAddress.Append(KSeparator);
            aLandmarkAddress.Append(KSpace);
            aLandmarkAddress.Append(tempStr);
        }
        else
        {
            aLandmarkAddress.Copy(tempStr);
            addressEmtpy = EFalse;
        }
    }

    retStatus = aLandmark->GetPositionField(EPositionFieldCountry, tempStr);
    if (retStatus == KErrNone && tempStr.Length())
    {
        if (!addressEmtpy)
        {
            aLandmarkAddress.Append(KSeparator);
            aLandmarkAddress.Append(KSpace);
            aLandmarkAddress.Append(tempStr);
        }
        else
        {
            aLandmarkAddress.Copy(tempStr);
            addressEmtpy = EFalse;
        }
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CheckCategoryAvailabilityL()
// Checks if given category id is found in the database pointed by category manager.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::CheckCategoryAvailabilityL(
        CPosLmCategoryManager* aCategoryManager, const TUint32 aCategoryId)
{
    __TRACE_CALLSTACK;
    CPosLandmarkCategory *category = aCategoryManager->ReadCategoryLC(
            aCategoryId);
    CleanupStack::PopAndDestroy(category);
}
// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CheckAndReadLandmarkL()
// Checks if given landmark id is found in the database and returns the read landmark.
// -----------------------------------------------------------------------------
//
CPosLandmark* CMyLocationsDatabaseManager::CheckAndReadLandmarkL(
        CPosLandmarkDatabase* aDb, const TUint32 aLmId)
{
    __TRACE_CALLSTACK;
    CPosLandmark* lm = aDb->ReadLandmarkLC(aLmId);
    CleanupStack::Pop(lm);    
    return lm;
}
// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::AddToMylocationsDbL()
// Adds the entry into the mylocations database and updates the lookup table.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::AddToMylocationsDbL(CPosLandmark* aLandmark,
        const TUint32 aUid, const TUint32 aSourceType)
{
    __TRACE_CALLSTACK;
    if (aSourceType == ESourceLandmarks)
    {
        CPosLandmark* landmark = iLandmarkDb->ReadLandmarkLC(aUid);
        RArray<TPosLmItemId> catArray;
        CleanupClosePushL( catArray );
        landmark->GetCategoriesL(catArray);
       

        // add the categories in the mylocations database for this landmark
        for (TInt i = 0; i < catArray.Count(); i++)
        {
            TLookupItem lItem;
            lItem.iUid = catArray[i];
            lItem.iSource = ESourceLandmarksUserCat;
            lItem.iLmId = 0;
            if (!iMylocationsLookupDb->FindEntryL(lItem))
            {
                lItem.iSource = ESourceLandmarksContactsCat;
                if (!iMylocationsLookupDb->FindEntryL(lItem))
                {
                    // means this is global category, so just add it
                    lItem.iLmId = lItem.iUid;
                }
            }
            TRAP_IGNORE( aLandmark->AddCategoryL( lItem.iLmId ) );
        }

        CleanupStack::PopAndDestroy( &catArray );       
        TLookupItem lookupItem;
        lookupItem.iUid = aUid;
        lookupItem.iSource = aSourceType;
        TRAP_IGNORE( lookupItem.iLmId = iMyLocationsLandmarksDb->AddLandmarkL( *aLandmark ) );
        CleanupStack::PopAndDestroy(landmark);
        iMylocationsLookupDb->CreateEntryL(lookupItem);
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::ModifyMylocationsDbL()
// Adds the entry into the mylocations database and updates the lookup table.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::ModifyMylocationsDbL(CPosLandmark* aLandmark,
        const TUint32 aUid, const TUint32 aSourceType)
{
    __TRACE_CALLSTACK;
    TLookupItem lookupItem;
    lookupItem.iUid = aUid;
    lookupItem.iSource = aSourceType;
    iMylocationsLookupDb->FindEntryL(lookupItem);
    iMylocationsLookupDb->DeleteEntryL(lookupItem);
    iMyLocationsLandmarksDb->RemoveLandmarkL(lookupItem.iLmId);
    AddToMylocationsDbL(aLandmark, lookupItem.iUid, lookupItem.iSource);
}

// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::DeleteFromMylocationsDbL()
// Deletes the entry from the mylocations database and updates the lookup table.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::DeleteFromMylocationsDbL(const TUint32 aUid,
                                                           const TUint32 aSourceType)
{
    __TRACE_CALLSTACK;
    TLookupItem lookupItem;
    lookupItem.iUid = aUid;
    lookupItem.iSource = aSourceType;
    iMylocationsLookupDb->FindEntryL(lookupItem);

    iMyLocationsLandmarksDb->RemoveLandmarkL(lookupItem.iLmId);
    iMylocationsLookupDb->DeleteEntryL(lookupItem);
}
// -----------------------------------------------------------------------------
// CMyLocationsDatabaseManager::CreateCategoryL()
// Creates a new category in Mylocations Db and adds a corresponding entry in 
// mylocations lookup table.
// -----------------------------------------------------------------------------
//
void CMyLocationsDatabaseManager::CreateCategoryL(const TUint32 aUid)
{
    __TRACE_CALLSTACK;// Read the category.
    CPosLandmarkCategory *category = iLandmarksCatManager->ReadCategoryLC(aUid);

    TLookupItem lookupItem;
    lookupItem.iUid = aUid;
    lookupItem.iSource = ESourceLandmarksUserCat;
    // Add category to landmarks database
    TRAPD ( err, (lookupItem.iLmId = iMyLocationsCatManager->AddCategoryL( *category ) ) );

    if (err == KErrNone)
    {
        iMylocationsLookupDb->CreateEntryL(lookupItem);
    }
    CleanupStack::PopAndDestroy(category);

}


// End of file
