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
* Description:  Central Repository keys for Events Storage API.
*                This key defines and updates the type of Change to Storage 
*                database.
*                
*
*/


#ifndef EVTSTORAGEPLUGINDOMAINCRKEYS_H
#define EVTSTORAGEPLUGINDOMAINCRKEYS_H

/**
 * UID value for the Central Repository Key used for Events Storage API.
 */
const TInt KCRUidEvtStorageDb = 0x2001E66D;

/**
 * Central repository Key identifer value for Events Storage database change.
 */
const TInt KEvtDbChangeType = 0x00000001;

/**
 * Central repository Key identifer value for Disclaimer Setting.
 */
const TInt KEvtDisclaimerSetting = 0x00000002;

/**
 * Central repository Key identifer value for Snooze Interval.
 */
const TInt KEvtSnoozeInterval = 0x00000003;

/**
 * Central repository Key identifer value for Snooze Period.
 */
const TInt KEvtSnoozePeriod = 0x00000004;

#endif      // EVTSTORAGEPLUGINDOMAINCRKEYS_H