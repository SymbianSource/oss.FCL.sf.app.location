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
* Description: defines types and constants for location picker
*
*/

#ifndef LOCATIONPICKERTYPES_H
#define LOCATIONPICKERTYPES_H

// Defines total number of main views ( all, collectionlist, search )
const int KNumberOfMainViews = 3;

// separator
const QString KSeparator(",");

// space
const QString KSpace(" ");

// Application Title
const QString KApplicationTitle("Select Location");

// Application Title
const QString KNoLocations("No Location entries present");

// space
const QString KContactCollection("Contact addresses");

// strings used to differentiate the contact address type
const QString KContactPrefered("Contact Prefered");
const QString KContactHome("Contact Home");
const QString KContactWork("Contact Work");
const QString KContactsString("Others");
const QString KContactsCollection("Contact addresses");
const QString KSortBy("Sort by");
const QString KAscend("Ascending");
const QString KDescend("Descending");

//Icon Paths

// dummy image for a location
const QString KDummyImage(":/qtg_large_maps.png");

//All tab icon
const QString KAllTab(":/qtg_mono_location.png.png");

//COllection tab icon
const QString KCollectionTab(":/qtg_mono_location_collection.png");

//Search tab icon
const QString KSearchTab(":/qtg_mono_search.png");

//Contacts collection icon
const QString KCollectionsContacts(":/qtg_small_contacts.png");

//Contacts collection icon
const QString KCollectionsCalendar(":/qtg_small_calendar.png");

//Contacts type prefered icon
const QString KContactPrefIcon(":/qtg_mono_favourites.png");
//Contacts type home icon
const QString KContactHomeIcon(":/qtg_mono_home.png");
//Contacts type work icon
const QString KContactWorkIcon(":/qtg_mono_work.png");

/** Defines view type in location picker
*/
enum TViewType
{
    /** All View */
    ELocationPickerAllView,
    /** Collection list view */
    ELocationPickerCollectionListView,
    /** Search View */
    ELocationPickerSearchView,
    /** Collection content view */
    ELocationPickerCollectionContentView
};


#endif // LOCATIONPICKERTYPES_H

