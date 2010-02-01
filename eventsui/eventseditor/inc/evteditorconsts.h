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
* Description:   it will contain constans for engine/editor
*
*/



#ifndef EVTMGMTUICONSTS_H_
#define EVTMGMTUICONSTS_H_

/*
 *  constants used in editor/engine
 */

/*
 * resource file containing all editor specific resources
 */
_LIT( KEvtEditorResFileName, "evtmgmteditorui.rsc" );

/*
 *  Mif containing all icons used in editor ui
 */
_LIT( KEvtMgmtUiIconFileName, "evteditor.mif" );

/*
 *  Maximum length of place attribute stored in local database
 */
const TInt KPlaceMaxLength = 256;

/*
 *  time out value for location acquisition
 *  If location is not retrived within this time period, it fails with time-out error
 */
const TInt64 KLocAcqTimeOut = 1000000*2*60 ; //2 minutes

/*
 * minimum value for radius in km
 */
const TReal KMinRadiusInKm = 0.1;

/*
 *  maximum value for radius in km 
 */
const TReal KMaxRadiusInKm = 1.5;

/*
 * Conversion from km to mile.
 */
const TReal KUnitConversion = 0.621371192237;

/*
 *  maximum limit for low accuracy
 * Required when place is set using 'Current Location'
 */
const TInt KLowAccuracyMaxLimit = 500;

/*
 * Maximum limit for medium accuracy
 * Required when place is set using 'Current Location'
 */
const TInt KMediumAccuracyMaxLimit = 1500;

/*
 *  prefix for tone recorded from eventsui application
 */
//TODO: tone name should be changed acc to draft
 _LIT(KRecorderFile, "soundclip");
 
 /*
  *  file extention for tone recorded from eventsui application
  */
 _LIT(KRecorderFileEnd, ".amr");
 
#endif /*EVTMGMTUICONSTS_H_*/
