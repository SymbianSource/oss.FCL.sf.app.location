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
* Description:  Basic Classes for the Location Events.
*
*/


#ifndef C_EVTBASICEVENTINFO_H
#define C_EVTBASICEVENTINFO_H

// System Includes
#include <e32base.h>

// User Includes
#include "evtdefs.h"

// Forward Declarations

// extern data types

// global Functions

// constants go here
/**
 *  @class CEvtBasicEventInfoInfo
 *  Class for the basic Location Event information.
 *
 *  This class is used to store the basic information for an event.
 *
 *  @lib evtstorage.lib
 *  @since S60 v9.1
 */
class CEvtBasicEventInfo : public CBase
    {
public:  
    /**
     * Constructs a new instance of CEvtBasicEventInfo.
     *
     * @return The new instance of CEvtBasicEventInfo object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtBasicEventInfo* NewL();
    
    /**
     * Constructs a new instance of CEvtBasicEventInfo Object.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of CEvtBasicEventInfo object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtBasicEventInfo* NewLC();  

    /**
    * Destructor.
    */
    virtual ~CEvtBasicEventInfo();

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
     * @return TEvtBasicEventStatus Status for the Location Event.
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
    
private: // methods
    /**
     * Default C++ Constructor.
     */
    CEvtBasicEventInfo();
    
    /**
     * Default Copy Constructor.
     */
    CEvtBasicEventInfo(CEvtBasicEventInfo& );

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
    TEvtEventId                          iEventId;
    
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
     * Repeat Event.
     */
    TBool                           iRepeat;
	
	/**
	 * Event Status.
	 */
	TEvtEventStatus					iEventStatus;

    };

#endif // C_EVTBASICEVENTINFO_H
