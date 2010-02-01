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
* Description:  Events Storage database Observer
*
*/


#ifndef M_EVTDBOBSERVER_H_
#define M_EVTDBOBSERVER_H_
 

// Class Declaration
/**
 * Internal class to observe the change in Events Storage database.
 */
class MEvtDbObserver
    {
    public:
	    /**
	     * Notifies a change in the Events Storage database
	     */
	    virtual void HandleDbChangedL( ) = 0;
    };
    
#endif // M_EVTDBOBSERVER_H_

