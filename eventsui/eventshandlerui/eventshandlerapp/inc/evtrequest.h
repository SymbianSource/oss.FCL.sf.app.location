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
* Description:  Container for Request and Type.
*
*/


#ifndef C_EVTREQUEST_H
#define C_EVTREQUEST_H

// System Includes
#include <e32base.h>

// User Includes
#include "evtrequesttype.h"
#include "evtevent.h"

/**
 *  @class CEvtRequest
 *  A class that encapsulates all the Ui Requests from Handler Server.
 *
 *  This class is used to store information about Ui Request type.
 *
 *  @since S60 v9.1
 */
class CEvtRequest : public CBase
    {
public:  
    /**
     * Constructs a new instance of CEvtRequest.
     *
     * @return The new instance of CEvtRequest object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtRequest* NewL();
    
    /**
     * Constructs a new instance of CEvtRequest Object.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of CEvtRequest object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtRequest* NewLC();  

    /**
    * Destructor.
    */
    virtual ~CEvtRequest();

public: // Get and Set methods
    
    /**
     * Event object for which we have got request.
     * This Ownership is NOT transferred.   
     *
     * @since S60 v9.1     
     * @return Event identifier identifying the Location Event.
     */
    IMPORT_C CEvtEvent& Event();
 
    /**
     * Set the Event Object for which the request has come. The
     * ownership of the aEvent will resides with CEvtRequest.
     *
     * @since S60 v9.1   
     * @param[in] aEvent Event object.
     */
    IMPORT_C void SetEventL( CEvtEvent* aEvent );
    
    /**
     * Gets the Type of Ui request associated with Event.
     *
     * @since S60 v9.1    
     * @return TEvtRequestType Type of Request.
     */
    IMPORT_C TEvtRequestType RequestType() const;

    /** 
     * Sets the type of the request associated with Event.
     *
     * @since S60 v9.1  
     * @param[in] aRequestType Type of Ui Request. 
     */
    IMPORT_C void SetRequestType( 
                            const TEvtRequestType aRequestType );

private: // methods
    /**
     * Default C++ Constructor.
     */
    CEvtRequest();
    
    /**
     * Default Copy Constructor.
     */
    CEvtRequest(CEvtRequest& );

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data
	    
	/**
	 * Event Object.
	 * Owned.
	 */
	CEvtEvent*				        iEvent;
    
	/**
	 * Request Type.
	 */
	TEvtRequestType					iRequestType;

    };

#endif // C_EVTREQUEST_H
