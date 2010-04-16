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
* Description: LocationPickerDataManager private implementation
*
*/

#include <e32base.h>
#include <e32std.h>
#include <lbsposition.h>
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmCategoryManager.h>
#include <EPos_CPosLandmarkCategory.h>
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_CPosLmCategoryCriteria.h>
#include <HbIcon>

#include "locationpickerdatamanager_p.h"


// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::LocationPickerDataManagerPrivate()
// ----------------------------------------------------------------------------

LocationPickerDataManagerPrivate::LocationPickerDataManagerPrivate() :
        mModel( NULL ),
        mViewType( ELocationPickerAllView ),
        mIterator(NULL),
        mLandmarkDb(NULL),
        mLmCategoryManager(NULL),
        mLandmarkSearch(NULL)
{
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::LocationPickerDataManagerPrivate()
// ----------------------------------------------------------------------------

LocationPickerDataManagerPrivate::LocationPickerDataManagerPrivate( QStandardItemModel &aModel, TViewType aViewType ) :
        mModel( &aModel ),
        mViewType( aViewType ),
        mIterator(NULL),
        mLandmarkDb(NULL),
        mLmCategoryManager(NULL),
        mLandmarkSearch(NULL)
{
}


// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::~LocationPickerDataManagerPrivate()
// ----------------------------------------------------------------------------
LocationPickerDataManagerPrivate::~LocationPickerDataManagerPrivate()
{
    // delete the member variables;

    if( mIterator )
        delete mIterator;
    if ( mLandmarkDb )
        delete mLandmarkDb;
    if( mLmCategoryManager )
        delete mLmCategoryManager;
    if( mLandmarkSearch )
        delete mLandmarkSearch;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateModel()
// ----------------------------------------------------------------------------
bool LocationPickerDataManagerPrivate::populateModel( quint32 aCollectionId )
{
    bool retValue = false;
    TRAP_IGNORE( retValue = populateModelL( aCollectionId ) );
    return retValue;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateModel()
// ----------------------------------------------------------------------------
bool LocationPickerDataManagerPrivate::populateModelL( quint32 aCollectionId )
{
    // Handle to the landmark database
    mLandmarkDb = NULL;

    //Open and intialize Landmark DB
    mLandmarkDb = CPosLandmarkDatabase::OpenL();
    ExecuteAndDeleteLD( mLandmarkDb->InitializeL() );

    switch( mViewType )
    {
        case ELocationPickerAllView:
        case ELocationPickerSearchView:
             {
                 // Create an iterator for iterating the landmarks in the database
                 mIterator = mLandmarkDb->LandmarkIteratorL();

                 if( ( mIterator == NULL ) || (mIterator->NumOfItemsL() == 0) )
                 {
                     // no items in the landmark database, so return false.
                     return false;
                 }
                 CleanupStack::PushL(mIterator);
                 populateLandmarksL();
                 CleanupStack::Pop( mIterator );
             }
             break;
        case ELocationPickerCollectionListView:
             {

                 // Create category manager for landmarks
                 mLmCategoryManager = CPosLmCategoryManager::NewL( *mLandmarkDb );

                 if( !mLmCategoryManager )
                 {
                     return false;
                 }
                 // Create an iterator for iterating the referenced categories in the database
                 mIterator = mLmCategoryManager->ReferencedCategoryIteratorL();

                 if( ( mIterator == NULL ) || (mIterator->NumOfItemsL() == 0) )
                 {
                     // no items in the landmark database, so return false.
                     return false;
                 }
                 CleanupStack::PushL(mIterator);
                 populateCollectionsL();
                 CleanupStack::Pop( mIterator );

             }
             break;

        case ELocationPickerCollectionContentView:
             {

                 // create a search object.
                 mLandmarkSearch = CPosLandmarkSearch::NewL( *mLandmarkDb );
                 CleanupStack::PushL( mLandmarkSearch );

                 // Create the search criterion
                 CPosLmCategoryCriteria* criteria = CPosLmCategoryCriteria::NewLC();
                 criteria->SetCategoryItemId( aCollectionId );

                 // Start the search and execute it at once.
                 ExecuteAndDeleteLD( mLandmarkSearch->StartLandmarkSearchL( *criteria ) );
                 CleanupStack::PopAndDestroy( criteria );

                 // Retrieve an iterator to access the matching landmarks.
                 mIterator = mLandmarkSearch->MatchIteratorL();
                 if( ( mIterator == NULL ) || (mIterator->NumOfItemsL() == 0) )
                 {
                     // no landmarks in this collection
                     CleanupStack::Pop(mLandmarkSearch);
                     return false;
                 }
                 CleanupStack::PushL( mIterator );
                 populateLandmarksL();
                 CleanupStack::Pop( mIterator );
                 CleanupStack::Pop(mLandmarkSearch);
             }
             break;
    }
    return true;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateLandmarksL()
// ----------------------------------------------------------------------------

void LocationPickerDataManagerPrivate::populateLandmarksL()
{
    // Read each landmark in the database and copy to the model.
    TPosLmItemId lmId;
    while ((lmId = mIterator->NextL()) != KPosLmNullItemId )
    {
        CPosLandmark* readLandmark = mLandmarkDb->ReadLandmarkLC(lmId );

        if( readLandmark )
        {
            QString lmAddressLine1(" ");
            QString lmAddressLine2("");

            TPtrC tempStr;
            TInt retStatus;

            // Copy landmark name in string 1
            retStatus = readLandmark->GetLandmarkName( tempStr );
            if( retStatus == KErrNone && tempStr.Length() > 0)
            {
                lmAddressLine1 = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
            }

            // create address line 2
            bool addressEmtpy = true; // used to check if address line 2 is empty

            // get street
            retStatus = readLandmark->GetPositionField( EPositionFieldStreet, tempStr );
            if( retStatus == KErrNone && tempStr.Length() )
            {
                lmAddressLine2 = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
                addressEmtpy = EFalse;
            }

            // Get city
            retStatus =readLandmark->GetPositionField( EPositionFieldCity, tempStr );
            if( retStatus == KErrNone && tempStr.Length() )
            {
                if( !addressEmtpy )
                {
                    lmAddressLine2 = lmAddressLine2 + KSeparator;
                    lmAddressLine2 = lmAddressLine2 + KSpace;
                    lmAddressLine2 = lmAddressLine2 + QString( (QChar*)tempStr.Ptr(), tempStr.Length());
                }
                else
                {
                    lmAddressLine2 = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
                    addressEmtpy = EFalse;
                }
            }
            // Get State
            retStatus =readLandmark->GetPositionField( EPositionFieldState, tempStr );
            if( retStatus == KErrNone && tempStr.Length() )
            {
                if( !addressEmtpy )
                {
                    lmAddressLine2 = lmAddressLine2 + KSeparator;
                    lmAddressLine2 = lmAddressLine2 + KSpace;
                    lmAddressLine2 = lmAddressLine2 + QString( (QChar*)tempStr.Ptr(), tempStr.Length());
                }
                else
                {
                    lmAddressLine2 = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
                    addressEmtpy = EFalse;
                }
            }

            // get country
            retStatus =readLandmark->GetPositionField( EPositionFieldCountry, tempStr );
            if( retStatus == KErrNone && tempStr.Length() )
            {
                if( !addressEmtpy )
                {
                    lmAddressLine2 = lmAddressLine2 + KSeparator;
                    lmAddressLine2 = lmAddressLine2 + KSpace;
                    lmAddressLine2 = lmAddressLine2 + QString( (QChar*)tempStr.Ptr(), tempStr.Length());
                }
                else
                {
                    lmAddressLine2 = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
                    addressEmtpy = EFalse;
                }
            }

            // get contact address type
            QString contactAddressType;
            retStatus = readLandmark->GetLandmarkDescription( tempStr );
            if( retStatus == KErrNone && tempStr.Length() > 0)
            {
                contactAddressType = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
            }

            // set icons based on contact address type
            QVariantList icons;;
            if( contactAddressType == KContactHome )
            {
                icons << HbIcon(KDummyImage) << HbIcon(KContactHomeIcon);
            }
            else if( contactAddressType == KContactWork )
            {
                icons << HbIcon(KDummyImage) << HbIcon(KContactWorkIcon);
            }
            else
            {
                icons << HbIcon(KDummyImage) << HbIcon(KContactPrefIcon);
            }


            // create a list item and set to model
            QStringList addressData;
            addressData << lmAddressLine1 << lmAddressLine2;
            QStandardItem *modelItem = new QStandardItem();
            modelItem->setData(QVariant(addressData), Qt::DisplayRole);
            modelItem->setData( icons, Qt::DecorationRole );
            mModel->appendRow( modelItem );

            CleanupStack::PopAndDestroy( readLandmark );
        }
   }
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateCollectionsL()
// ----------------------------------------------------------------------------

void LocationPickerDataManagerPrivate::populateCollectionsL()
{
    // Read each categpry in the database and copy to the model.
    TPosLmItemId lmId;
    while ((lmId = mIterator->NextL()) != KPosLmNullItemId )
    {
        CPosLandmarkCategory* readCategory = mLmCategoryManager->ReadCategoryLC(lmId );

        if( readCategory )
        {
            QString categoryName("");

            TPtrC tempStr;
            TInt retStatus;

            retStatus = readCategory->GetCategoryName( tempStr );
            if( retStatus == KErrNone && tempStr.Length() > 0)
            {
                categoryName = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
                if(categoryName == KContactsString)
                {
                    categoryName = KContactsCollection;
                }
            }

            // create a list item and copy to model

            QString iconPath;
            QStandardItem *modelItem = new QStandardItem();
            modelItem->setData(QVariant(categoryName), Qt::DisplayRole);
            modelItem->setData( QIcon (KCollectionsContacts), Qt::DecorationRole );
            mModel->appendRow( modelItem );

            CleanupStack::PopAndDestroy( readCategory );
        }
    }
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::getData()
// ----------------------------------------------------------------------------

void LocationPickerDataManagerPrivate::getData( int aIndex, quint32& aValue )
{
    aValue = 0;
    RArray<TPosLmItemId> idArray;
    TRAPD( err, mIterator->GetItemIdsL( idArray, aIndex, 1 ) );
    if( err == KErrNone)
        aValue = (quint32) idArray[0];
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::getLocationItem()
// ----------------------------------------------------------------------------

void LocationPickerDataManagerPrivate::getLocationItem( quint32 aLmId, QLocationPickerItem& aItem )
{
    TRAPD( err, getLocationItemL( aLmId, aItem ) );
    if( err != KErrNone )
        aItem.mIsValid = false;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::getLocationItem()
// ----------------------------------------------------------------------------

void LocationPickerDataManagerPrivate::getLocationItemL( quint32 aLmId, QLocationPickerItem& aItem )
{
    //Open and intialize Landmark DB
    CPosLandmarkDatabase* landmarkDb = NULL;
    landmarkDb = CPosLandmarkDatabase::OpenL();

    ExecuteAndDeleteLD( landmarkDb->InitializeL() );

    CPosLandmark* readLandmark = landmarkDb->ReadLandmarkLC(aLmId );

    if( readLandmark )
    {
        // get position and store
        TLocality position;
        readLandmark->GetPosition( position );
        aItem.mLatitude = position.Latitude();
        aItem.mLongitude = position.Longitude();

        // get address fields
        TPtrC tempStr;
        TInt retStatus;

        // Copy landmark name in string 1
        retStatus = readLandmark->GetLandmarkName( tempStr );
        if( retStatus == KErrNone && tempStr.Length() > 0)
        {
            aItem.mName = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
        }

        // get street
        retStatus = readLandmark->GetPositionField( EPositionFieldStreet, tempStr );
        if( retStatus == KErrNone && tempStr.Length() )
        {
            aItem.mStreet = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
        }

        // Get city
        retStatus =readLandmark->GetPositionField( EPositionFieldCity, tempStr );
        if( retStatus == KErrNone && tempStr.Length() )
        {
            aItem.mCity = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
        }
        // Get State
        retStatus =readLandmark->GetPositionField( EPositionFieldState, tempStr );
        if( retStatus == KErrNone && tempStr.Length() )
        {
            aItem.mState = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
        }

        // get country
        retStatus =readLandmark->GetPositionField( EPositionFieldCountry, tempStr );
        if( retStatus == KErrNone && tempStr.Length() )
        {
              aItem.mCountry = QString( (QChar*)tempStr.Ptr(), tempStr.Length());
        }
    }
    aItem.mIsValid = true;
    CleanupStack::PopAndDestroy(readLandmark);
    delete landmarkDb;
}
