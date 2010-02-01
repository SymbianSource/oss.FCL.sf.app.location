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
* Description:  Location Centre Server Engine Object.
*
*/


#ifndef C_LCSERVERENGINE_H
#define C_LCSERVERENGINE_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <lclocationappfilter.h>

// USER INCLUDES
#include "lcipcparams.h"
#include "lcregistryobserver.h"

// FORWARD DECLARATIONS
class CLcRegistry;
class MLcServerEngineObserver;

/**
 *  Location Centre server engine.
 *
 *  This class handles all the requests for the Location Centre Server. The
 *  requests from the Server session objects are transferred to this object
 *  which then handles the requests. There is only one instance of this
 *  class for the entire Server Engine. This class also contains the Location
 *  Centre registry and observes for anychanges to the Registry.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLcServerEngine ): public CBase,
                                      public MLcRegistryObserver
    {
public:
    /**
     * Constructs a new instance of Location Centre server engine.
     *
     
     * @param[in] aObserver Observer to the Location Centre Server
     *                      Engine.
     * @return The new instance of Location Centre server engine object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServerEngine* NewL( 
                    MLcServerEngineObserver&    aObserver );
    
    /**
     * Constructs a new instance of Location Centre server engine.
     * Leaves the created instance on the cleanup stack.
     *
     * @param[in] aObserver Observer to the Location Centre Server
     *                      Engine.     
     * @return The new instance of Location Centre server engine object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServerEngine* NewLC(
                    MLcServerEngineObserver&    aObserver );        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server engine
     */
    virtual ~CLcServerEngine();
    
    /**
     * Services the Client Request. The Engine provides a one place
     * destination for servicing all Client request. If the registry
     * is under updation, then the request is queued and the request
     * is served after the updation is completed.
     *
     * @param[in] aMessage The Message requets which needs to be served.
     */
    void DoServiceL( const RMessage2&      aMessage );

public: // Inherited from MLcRegistryObserver
    void RegistryUnderUpdation();
    
    void RegistryUpdated();
             
private:
    /**
     * C++ Default constructor
     */
    CLcServerEngine( MLcServerEngineObserver&    aObserver );
    
    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();
    
    /**
     * Services all the Requets which are outstanding. The implementation
     * of the Engine class ensures that all this function is called
     * only when the Engine is in a consistent state.
     */
    void ServiceRequests();
    
    /**
     * Services a client request which is outstanding.
     *
     * @param[in] aMessage Request message which is outstanding
     */
    void ServiceRequestL( RMessage2&      aMessage );
    
    /**
     * Filters the List of Location based Application based on the
     * System Characteristics and Application Characteristics.
     *
     * @param[in] aAppFilter            Filter properties that need to be applied
     *                                  on the list of Location based Applications.
     * @param[out] aFilteredAppArray    Array of Location based Applications after
     *                                  filtering.
     */
    void GetFilteredAppsL( 
            TLcLocationAppFilter&         aAppFilter,             
            RLcIpcAppInfoArray&           aFilteredAppArray );
    
    /**
     * Obtains a specified set of Location based Applications.
     *
     * @param[in] aSpecifedApps         List of Specified Applications whose
     *                                  description is sought.
     * @param[out] aFilteredAppArray    Array of specified Location based
     *                                  Applications.    
     */
    void GetSpecifiedAppsL( 
            RLcIpcAppIdArray&             aSpecifedApps,            
            RLcIpcAppInfoArray&           aSpecifiedAppArray );    
    /**
     * Obtains a list of Location based Application without a specified set
     * of Location based Applications.
     *
     * @param[in] aSpecifedApps         List of Specified Applications whose
     *                                  description is sought.
     * @param[out] aFilteredAppArray    Array of Location based Applications
     *                                  without the specified list.    
     */
    void GetWithoutSpecifiedAppsL( 
            RLcIpcAppIdArray&             aSpecifedApps,              
            RLcIpcAppInfoArray&           aSpecifiedAppArray );
    
    /**
     * Compares a filter value under specifed configurations.
     * In the usage of this function, the Engine passes the filter requested by
     * the Client application as the first parameter and compares it with
     * the filter of the Location based Application to determine whether the
     * application satisfies the required filter configuration.
     *
     * @param[in] aFilter           New filter parameter.
     * @param[in] aFiltertoCompare  Filter to which the filter passed in the first
     *                              argument must be compared with.
     * @param[in] aFilterConfig     Filter configuration which should be used for
     *                              comparison.
     */        
    TBool  Compare( TUint32         aFilter,
                    TUint32         aFiltertoCompare, 
                    TInt            aFilterConfig );
                    
	/**
	 * Compares the SID of the application being processed currently with that of
	 * the message which is handled. This is needed to ensure that the application
	 * which requested for the list of Location based Applications does not
	 * figure in the list requested.
	 *
	 * @param[in] aAppSid	SID of the application.
	 * @return TBool, ETrue if the SID matches
	 *                EFalse if the SID doesnt match
	 */                    
	TBool IsSameSID( const TDesC&		aAppSid );
    
private:
    /**
     * Location Centre Registry object.
     *
     * Owns.
     */
    CLcRegistry*                iRegistry;
    
    /**
     * Array of messages which need to be serviced. This class doesn't
     * own these message objects but just holds a reference to them.
     */
    RPointerArray< RMessage2 >  iMessageArray;
    
    /**
     * Observer to the Location Centre Server Engine class.
     */
    MLcServerEngineObserver&    iObserver;
    
    /**
     * Boolean value to indicate whether the Location Centre Registry
     * is currently under updation. This is required because we
     * cannot fetch the Registry values when its under updation.
     */
    TBool                       iRegistryUnderUpdate;
    
    /**
     * SID of the process which has initiated the current request
     */
    TUint32						iCurrentRequestorSid;
    
    };

#endif // C_LCSERVERENGINE_H
