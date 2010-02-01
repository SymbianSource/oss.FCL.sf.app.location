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
* Description:  Concrete Class for Location Events.
*
*/


#ifndef C_EVTEVENT_H
#define C_EVTEVENT_H

// System Includes
#include <e32base.h>
#include <lbs.h>

// User Includes
#include "evtdefs.h"

// Forward Declarations
class CEvtAction;

// extern data types

// global Functions

// constants go here

/**
 *  @class CEvtEvent
 *  Concrete class that encapsulates all the information related for
 *	Event.
 *
 *  This class is used to store the detailed information for an event.
 *
 *  @lib evtstorage.lib
 *  @since S60 v9.1
 */
class CEvtEvent : public CBase
    {
public:  
    /**
     * Constructs a new instance of Event.
     *
     * @return The new instance of Event object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtEvent* NewL();
    
    /**
     * Constructs a new instance of Event Object.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Event object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtEvent* NewLC();  

    /**
    * Destructor.
    */
    virtual ~CEvtEvent();

public: // Get and Set methods
    
    /**
     * Gets the Trigger id, identifying the Location Event.
     * This identifier is definded by Location Triggering Engine.   
     *
     * @since S60 v9.1     
     * @return Event identifier identifying the Location Event.
     */
    IMPORT_C TEvtEventId EventId() const;
 
    /**
     * Set the Trigger id, identifying the Location Event. The
     * identifer is definded by Location Triggering Engine.
     *
     * @since S60 v9.1   
     * @param[in] aId Event identifier identifying the Location Event.
     */
    IMPORT_C void SetEventId( const TEvtEventId aId );
    
    /**
     * Gets the Subject for the Location Events.
     * The Subject is defined by the Location EventsUi on Creation.
     * A reference to the object's internal string is returned.
     *
     * @since S60 v9.1    
     * @return Subject for the Location Event. If the Subject has not 
     *         been set for the Location Event, then a NULL
     *         string is returned.
     */
    IMPORT_C TPtrC Subject() const;

    /** 
     * Set the Subject String for the Location Event. The Subject is 
     * copied.
     *
     * @since S60 v9.1  
     * @param[in] aSubject Subject String for the Location Event. 
     *                  The Subject string is copied.
     * @leave KErrNoMemory If there is not enough memory to copy the string.
     */
    IMPORT_C void SetSubjectL( const TDesC& aSubject );
    
    /**
     * Gets the Place for the Location Events.
     * The Place is defined by the Location EventsUi on Creation.
     * A reference to the object's internal string is returned.
     *
     * @since S60 v9.1    
     * @return Place String for the Location Event. If the Place has not 
     *         been set for the Location Event, then a NULL
     *         string is returned.
     */
    IMPORT_C TPtrC Place() const;

    /** 
     * Set the Place String for the Location Event. The Place is 
     * copied.
     *
     * @since S60 v9.1  
     * @param[in] aPlace Place String for the Location Event. 
     *                  The Place string is copied.
     * @leave KErrNoMemory If there is not enough memory to copy the string.
     */
    IMPORT_C void SetPlaceL( const TDesC& aPlace );
    
    /**
     * Gets the Status of the Location Event.
     * The Status is defined by the Location EventsUi on Creation.
     * Status is updated as and when the Event moves to different state.
     *
     * @since S60 v9.1    
     * @return TEvtEventStatus Status for the Location Event.
     */
    IMPORT_C TEvtEventStatus EventStatus() const;

    /** 
     * Set the Status for the Location Event.
     *
     * @since S60 v9.1  
     * @param[in] aEventStatus Status for the Location Event. 
     */
    IMPORT_C void SetEventStatus( 
                            const TEvtEventStatus aEventStatus );
                            
    /**
     * Gets the Radius for the Location Events.
     * The Radius is defined by the Location EventsUi on Creation.
     * Radius is defined based on the place.
     *
     * @since S60 v9.1    
     * @return Radius for the Location Event.
     */
    IMPORT_C TReal Radius() const;

    /** 
     * Set the Radius for the Location Event.
     *
     * @since S60 v9.1  
     * @param[in] aRadius Radius for the Location Event. 
     */
    IMPORT_C void SetRadius( const TReal aRadius );
             
    /**
     * Gets the Repeast flag for the Location Event.
     * The Repeast is defined by the Location EventsUi on Creation.
     *
     * @since S60 v9.1    
     * @return TBool Repeat flag for the Location Event.
     */
    IMPORT_C TBool Repeat() const;

    /** 
     * Set the Repeat flag for the Location Event.
     *
     * @since S60 v9.1  
     * @param[in] TBool Repeat flag for the Location Event. 
     */
    IMPORT_C void SetRepeat( const TBool aRepeat );
        
    /**
     * Gets the Description for the Location Events.
     * The Description is defined by Location EventsUi on Creation.
     * A reference to the object's internal string is returned.
     *
     * @since S60 v9.1    
     * @return Description for the Location Event. If the Description 
     *          has not been set for the Location Event, then a NULL
     *         string is returned.
     */
    IMPORT_C TPtrC Description() const;

    /** 
     * Set the Description String for the Location Event. The  
     * Description is copied.
     *
     * @since S60 v9.1  
     * @param[in] aDescription Description String for the Location Event. 
     *                  The Description string is copied.
     * @leave KErrNoMemory If there is not enough memory to copy the string.
     */
    IMPORT_C void SetDescriptionL( const TDesC& aDescription );
    
    /**
     * Gets the Action for the Location Events.
     * The Action is defined by the Location EventsUi on Creation.
     * A reference to the object's action is returned.
     *
     * @since S60 v9.1    
     * @return Action for the Location Event. If the Action has 
     *          not been set for the Location Event, then a NULL
     *          is returned.
     */
    IMPORT_C CEvtAction& Action() const;

    /** 
     * Set the Action for the Location Event. The Action is 
     * copied.
     *
     * @since S60 v9.1  
     * @param[in] aAction Action for the Location Event. 
     *                  The ownership of Action is transferred to Event.
     * @leave KErrNoMemory If there is not enough memory to copy the string.
     */
    IMPORT_C void SetActionL( CEvtAction* aAction );

    /** 
     * Check if Action is assigned to the Event.
     *
     * @since S60 v9.1  
     * @TBool Returns ETrue if action is present Or else EFalse.
     */
    IMPORT_C TBool HasAction( );

    /** 
     * Set the Location for the Location Event.
     *
     * @since S60 v9.1  
     * @param[in] aLocation Location for the Location Event. 
     * @leave KErrNoMemory If there is not enough memory to copy the string.
     */
     void SetLocationL( TPosition& aLocation );
    
    /**
     * Gets the Location for the Location Events.
     * The Location is defined by the Location EventsUi on Creation.
     * A reference to the object's action is returned.
     *
     * @since S60 v9.1    
     * @return Location for the Location Event. If the Action has 
     *          not been set for the Location Event, then a NULL
     *          is returned.
     */
     IMPORT_C TPosition& Location() const;

private: // methods
    /**
     * Default C++ Constructor.
     */
    CEvtEvent();
    
    /**
     * Default Copy Constructor.
     */
    CEvtEvent(CEvtEvent& );

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data
    
    /**
     * System wide unique identifier identifying the Location Event. 
     * This identifier is defined by the Location Triggering Engine.
     *
     * Owns
     */
    TEvtEventId                      iEventId;
    
    /**
     * Event Subject.
     * Subject for the Location Event. The Subject is defined by the Location
     * Events Ui.
     * 
     * Owns
     */
    HBufC*                          iSubject;
    
    /**
     * Event Place.
     * Place for the Location Event. The String is defined by the Location
     * Events Ui.
     * 
     * Owns
     */
    HBufC*                          iPlace;
	
	/**
	 * Event Status.
	 */
	TEvtEventStatus					iEventStatus;
    
	/**
	 * Radius for Accuracy.
	 */
	TReal							iRadius;
    
	/**
	 * Repeat Event.
	 */
	TBool							iRepeat;
    
    /**
     * Event Description.
     * Description for the Location Event. The String is defined by the
     * Location Events Ui.
     * 
     * Owns
     */
    HBufC*                          iDescription;
	    
	/**
	 * Tone String for Event.
	 */
	CEvtAction*				        iAction;
    
    /**
     * lat-long information 
     * 
     */
    TPosition*             			iLocation;

    };

#endif // C_EVTEVENT_H
