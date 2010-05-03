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

#include <hbglobal.h>

// separator
const QString KSeparator(",");

// space
const QString KSpace(" ");

//Icon Paths

// dummy image for a location
const QString KDummyImage("qtg_large_maps");

//Contacts collection icon
const QString KCollectionsContacts("qtg_small_contacts");

//Contacts collection icon
const QString KCollectionsCalendar("qtg_small_calendar");

//Contacts type prefered icon
const QString KContactPrefIcon("qtg_mono_favourites");
//Contacts type home icon
const QString KContactHomeIcon("qtg_mono_home");
//Contacts type work icon
const QString KContactWorkIcon("qtg_mono_work");


// strings used to differentiate the contact address type
const QString KContactHome("Contact Home");
const QString KContactWork("Contact Work");
const QString KContactsString("Others");


/** 
 * Defines view type in location picker
*/
enum TViewType
{
    /** LocationPicker Content */
    ELocationPickerContent,
    /** Collection list Content*/
    ELocationPickerCollectionListContent,
    /** Search View */
    ELocationPickerSearchView,
    /** Collection content */
    ELocationPickerCollectionContent
};


#endif // LOCATIONPICKERTYPES_H

