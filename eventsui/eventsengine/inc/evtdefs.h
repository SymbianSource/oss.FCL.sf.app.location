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
* Description:  Definition file for defining the Events unique Id and Status.
*
*/


#ifndef EVTDEFS_H
#define EVTDEFS_H

// extern data types
typedef TUint32 TEvtEventId;

// constants go here
/**
 * @enum TEvtEventStatus
 * Status of the Event.
 *
 */
enum TEvtEventStatus
    {
    /**
     * The Status of an Event when it is Active
     */
    EActive                               = 0,
    
    /**
     * The Status of an Event when it is in Draft
     */
    EDraft                                = 1,
    
    /**
     * The Status of an Event when it is complete
     */
    ECompleted                            = 2
    };
    
/**
 * @enum TEvtFireAccuracy
 * Accuracy of Fired Trigger.
 *
 */
enum TEvtFireAccuracy
    {
    /**
     * Trigger Fired with High Accuracy
     */
    EHighAccuracy                          = 0,
    
    /**
     * Trigger Fired with Mid Accuracy
     */
    EMidAccuracy,
    /**
     * Trigger Fired with Low Accuracy
     */
    ELowAccuracy
    };
    
/**
 * @enum TEvtConsoleAction
 * Action for the ConsoleUi.
 *
 */
enum TEvtConsoleAction
    {
    /**
     * No Action
     */
    EEvtActionNone                          = 0,
    
    /**
     * Action for ConsoleUi to handle Fired trigger
     */
    EEvtActionFired,
    
    /**
     * Action for ConsoleUi to change state
     */
    EEvtActionChangeState
    };
#endif // EVTDEFS_H
