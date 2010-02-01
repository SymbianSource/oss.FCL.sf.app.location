/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Central Repository Key defintion for ordering of Location
*                based Applications and Content/Services in Location Centre.
*
*/


#ifndef LCPRIVATECRKEYS_H_
#define LCPRIVATECRKEYS_H_

// System Includes
#include <e32base.h>

/**
 * The category UID for the keys in this header file.
 * UID of the Location Centre Central Repository Key. This key defines the
 * position of the Location based Applications and Contents/Services in
 * Location Centre and the value for Location Centre Server Shutdown timer value.
 */
const TUid KCRUidLocationCentre = { 0x10283137 };


/**
 * Enumeration list of values for the @p KCRUidLocationCentreOrder key.
 *
 * Location Centre displays the list of Location based Application and 
 * Services/Contents registered with it. Each Location based Application or 
 * Content/Service can be present in either the Top, Middle or Bottom portion of
 * the list. The exact location and order of display of these Applications
 * and Services/Contents depends upon whether they are pre-installed in the
 * phone image or whether they are dynamically installed. Those Applications and
 * Contents/Services which come with the phone image can occur only in the Top
 * or Bottom portion of the list while those Applications and Contents/Services
 * which are dynamically installed are shown in the Middle portion of the list.
 * 
 * The enumeration @p TLcOrderValues defines the values and format of Top, Middle
 * and Bottom keys.
 */
enum TLcOrderValues
	{
	/**
	 * Top Key for list of Location based Applications and Contents/Services which
	 * are shown at the top of Location Centre list.
	 * This key is a Read only key and the default values for the Keys is configured
	 * by the device manufacturer while flashing the device. They cannot be dynamically
	 * configured. The keys contains a UNICODE ( UCS -2 ) string which contains a
	 * list of Unique Identifers for Location based Application and Contents/Services
	 * in the order in which the device manufacturer wants these Applications and
	 * Contents/Services to be displayed.
	 *
	 * The format of this Key is defined as
	 * | Count | Length(1) | UID(1) | Length(2) | UID(2) | ... | Length( Count ) | UID( Count ) |
	 * where,
     * Count   - ( 4 bytes ) - The number of Location based Applications or Contents / Services
     *                         which are predefined to be displayed at the top of
     *                         Location Centre.
     * Length(n) - ( 4 bytes ) - Length of the UID for the Location based Application or
     *                         Content/Service encoded as as a string in the UNICODE ( UCS -2)
     *                         format.
     * String(n) - ( Length(n) * 2 bytes ) - UID of the Location based Application or Content/Service
     *                         encoded as a string in the UNICODE ( UCS-2 ) format. Here each character
     *                         of the UID takes 2 bytes. Hence, over all the length of the UID string
     *                         is 2 * Length(n). 
	 */
	ELcTop															= 0x00000001,
	
	/**
	 * Middle Key for list of Location based Applications and Contents/Services which
	 * are shown in the middle of Location Centre list.
	 * 
	 * This key takes a Boolean value indicating whether there are any Location based
	 * Applications or Contents/Services which have been dynamically installed or not.
	 * The values which this key can take are
	 * ETrue, If there are certain Location based Applications or Contents/Service
	 *        which have been dynamically installed on the terminal.
	 * EFalse, If there are no Location based Applications or Contents/Service
	 *        which have been dynamically installed on the terminal.
	 * The default value for this Key is EFalse.
	 *
	 * Incase there are any dynamically installed Applications or Contents/Services then
	 * these components would be displayed in the middle of Location Centre in the order
	 * of their registration.
     */
	ELcMiddle														= 0x00000002,
	
	/**
	 * Bottom Key for list of Location based Applications and Contents/Services which
	 * are shown at the bottom of Location Centre list.
	 * This key is a Read only key and the default values for the Keys is configured
	 * by the device manufacturer while flashing the device. They cannot be dynamically
	 * configured. The keys contains a UNICODE ( UCS -2 ) string which contains a
	 * list of Unique Identifers for Location based Application and Contents/Services
	 * in the order in which the device manufacturer wants these Applications and
	 * Contents/Services to be displayed.
	 *
	 * The format of this Key is defined as
	 * | Count | Length(1) | UID(1) | Length(2) | UID(2) | ... | Length( Count ) | UID( Count ) |
	 * where,
     * Count   - ( 4 bytes ) - The number of Location based Applications or Contents / Services
     *                         which are predefined to be displayed at the bottom of
     *                         Location Centre.
     * Length(n) - ( 4 bytes ) - Length of the UID for the Location based Application or
     *                         Content/Service encoded as as a string in the UNICODE ( UCS -2)
     *                         format.
     * String(n) - ( Length(n) * 2 bytes ) - UID of the Location based Application or Content/Service
     *                         encoded as a string in the UNICODE ( UCS-2 ) format. Here each character
     *                         of the UID takes 2 bytes. Hence, over all the length of the UID string
     *                         is 2 * Length(n). 
	 */	
	ELcBottom														= 0x00000003		
	};

/**
 * Value for Location Centre shutdown timer in milliseconds.
 */
const TInt KLcShutdownTimerValue                           = 0x00000004;

#endif      // LCPRIVATECRKEYS_H_
          
// End of File
