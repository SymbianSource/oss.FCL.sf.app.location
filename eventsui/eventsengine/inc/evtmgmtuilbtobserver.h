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
* Description:  It is observer class to handle lbt operations
*
*/


#ifndef EVTMGMTUILBTOBSERVER_H_
#define EVTMGMTUILBTOBSERVER_H_

#include <e32base.h>        // CBase

#include <lbtcommon.h>

class MEvtMgmtUiLbtObserver
    {
public:
    /*
     *  Type of Trigger Change
     */
    enum TLbtTriggerChangeType
        {
        /*
         * If multiple triggers are modified
         */
        ETriggerMultipleModified,
        /*
         * If a trigger is modified
         */
        ETriggerModified,
        /*
         * If a trigger is deleted
         */
        ETriggerDeleted
        };
public:
    /**
     * It will be used to notify observer that the trigger which is
     * owned by EventsUi has been modified outside, either by Manger Ui
     * or Event Handler
     * @param aTriggerId trigger id of newly created trigger
     * @param aType It will contain type of trigger change              
     */
    virtual void NotifyTriggerChangeL( const TLbtTriggerId &  aTriggerId,
    											TLbtTriggerChangeType aType )=0;
    
    /**
     * It will be used to notify observer that lbt operation
     * has be completed
     * @param aTriggerId trigger id of newly created trigger
     * @param aState It will contain the the operation type              
     */
    virtual void NotifyLbtChangeL(TLbtTriggerId &  aTriggerId,TInt aState )=0;
    
    /**
     * It will be used to notify observer that lbt operation
     * has be failed
     * @param aState It will contain the the operation type
     * @param aErrorCode It will contain the status of the operation
     *                KErrNone if successful otherwise error status code          
     */
    virtual void NotifyLbtError(TInt aErrorCode, TInt aState )=0;
    };



#endif //EVTMGMTUILBTOBSERVER_H_