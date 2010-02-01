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
* Description:  Class for the Location Events Action.
*
*/


#ifndef C_EVTACTION_H
#define C_EVTACTION_H

// System Includes

// User Includes
#include "evtdefs.h"

// Forward Declarations

// extern data types

// global Functions
/**
 * @enum TEvtActionType
 * Action Type for an Event.
 *
 */
enum TEvtActionType
    {
    /**
     * The Action for the Event is to play Alarm Tone.
     */
    EAlarm                               = 0
    };
        
/**
 *  @class CEvtAction
 *  Class that represents the action for an Event
 *
 *  This class encapsultes the Action information like action type
 *  and generic string for action. This class allows for Internalizing
 *	Externalizing the action.
 *
 *  @lib evtstorage.lib
 *  @since S60 v9.1
 */
class CEvtAction : public CBase
    {
public:  
    /**
     * Constructs a new instance of Event Action.
     *
     * @return The new instance of Event Action object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtAction* NewL();
    
    /**
     * Constructs a new instance of Event Action.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Event object.
     * @leave System wide error code if the object creation fails.
     */
    IMPORT_C static CEvtAction* NewLC();  

    /**
    * Destructor.
    */
    virtual ~CEvtAction();

public: // Get and Set methods
    
    /**
     * Gets the Action id, identifying the Location Event's Action.
     * This identifier is definded by Storage Db on creation.   
     *
     * @since S60 v9.1     
     * @return Identifier identifying the Location Event's Action.
     */
    TInt64 Id() const;
 
    /**
     * Set the Action id, identifying the Location Event's Action. 
     * This identifier is definded by Storage Db on creation.
     *
     * @since S60 v9.1   
     * @param[in] aId Action identifier identifying the Location 
     * 					Event's Action.
     */
    void SetId( const TInt64 aId );
    
    /**
     * Gets the Trigger id, identifying the Location Event to which
     * Action is associated with.
     * This identifier is definded by Storage Db on creation.   
     *
     * @since S60 v9.1     
     * @return Event identifier identifying the Location Event.
     */
    TEvtEventId EvtId() const;
 
    /**
     * Set the Trigger id, identifying the Location Event to which
     * Action is associated with.
     * This identifer is definded by Storage Db on creation.
     *
     * @since S60 v9.1   
     * @param[in] aId Action identifier identifying the Location 
     * 					Event's Action.
     */
    void SetEvtId( const TEvtEventId aId );
    
    /**
     * Gets the Action type for the Location Event.
     * The Type is defined when an event is created.
     *
     * @since S60 v9.1    
     * @return TEvtActionType Type of Location Event.
     */
    IMPORT_C TEvtActionType Type() const;

    /** 
     * Set the Action type for the Location Event.
     *
     * @since S60 v9.1  
     * @param[in] TEvtActionType Type for Location Event. 
     */
    void SetType( const TEvtActionType aType );
    
    /**
     * Gets the Action for the Location Events.
     * The Action is defined by the Location EventsUi on Creation.
     * A reference to the object's internal string is returned.
     *
     * @since S60 v9.1    
     * @return Action String for the Location Event.
     */
    TPtrC Action() const;

    /** 
     * Set the Action String for the Location Event. The Action is 
     * copied.
     *
     * @since S60 v9.1  
     * @param[in] aAction Action String for the Location Event. 
     *                  The Action string is copied.
     * @leave KErrNoMemory If there is not enough memory to copy the string.
     */
    void SetActionL( const TDesC& aAction );

private: // methods
    /**
     * Default C++ Constructor.
     */
    CEvtAction();

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data
	
	/**
	 * Action Id for an Event.
	 */
	TInt64					iId;
	
	/**
	 * Event Id to which the Action is associated with.
	 */
	TEvtEventId				iEvtId;
	
	/**
	 * Action Type for an Event.
	 */
	TEvtActionType				    iType;
	
	/**
	 * Action String for an Event.
	 */
	HBufC*				            iActionString;
    };

#endif // C_EVTACTION_H
