/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Events Info Note Interface definition
*
*/

#ifndef EVTINFONOTEINTERFACE_HRH
#define EVTINFONOTEINTERFACE_HRH

/**
 * Interfaces for display the Events UI trigger fire Information
 */
const TUid KEvtInfoNote                 = { 0x2001E66C };
const TUid KEvtInfoNoteChannel          = { 0x10009D48 };


/**
 * Enumeration value for the result from the Event info note
 */
enum TEvtInfoNoteResult
    {
    /**
     * Enumeration value requesting the display of the trigger
     */
    EOpenTrigger    = 6000,
    
    /**
     * Enumeration value request the client application to
     * expand the description field
     */
    EShowDetails,
    
    /**
     * Enumeration value requesting to snooze the trigger
     */
    ESnoozeTrigger,
    
    /**
     * Enumeration value requesting closure of the info note
     */
    ECloseInfoNote
    };

#endif // EVTINFONOTEINTERFACE_HRH
