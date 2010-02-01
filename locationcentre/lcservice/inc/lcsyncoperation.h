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
* Description:  Handles all the Synchronous operations with the Location
*                Centre Client Session
*
*/


#ifndef LCSYNCOPERATION_H
#define LCSYNCOPERATION_H

// SYSTEM INCLUDES
#include <e32base.h>

// USER INCLUDES

// FORWARD DECLARATIONS
class RLcClientSession;
class CLcBasicAppInfo;
class CLcLocationAppInfoArray;
class TLcLocationAppFilter;

/**
 *  Handles all the Synchronous operations to the Location Centre Server.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( LcSyncOperation )
    {
public:
    /**
     * Gets a filtered list of Location based Applications.
     *
     * @param[in] aSession    			Reference to the Location Centre Client Session.
     * @param[in] aLocationAppFilter	Filtering parameters.
     * @return CLcLocationAppInfoArray* Array of Location based Applications and
     *                                  Contents/Services.
     */
     static CLcLocationAppInfoArray* GetLocationApplicationsL(
     					   RLcClientSession&   		aSession,
                     const TLcLocationAppFilter&    aLocationAppFilter );
    
    /**
     * Gets a filtered list of Location based Applications.
     *
     * The Client application can configure the list of Location based
     * Applications returned. This can be achieved by passing the list of Location
     * based Applications which the Client wants to be included/excluded in the
     * list.
     *
     * The ownership of the array of identifiers is not transferred.
     *        
     * @param[in] aSession    	Reference to the Location Centre Client Session.        
     * @param[in] aAppArray     Array of Location based Applications and Contents
     *                          or Services which need to be included or excluded.
     * @param[in] aIncludeFlag  Boolean flag which defines whether the set of
     *                          Location based Applications should be included or
     *                          excluded.
     * @return CLcLocationAppInfoArray object containing a list
     *         of applications registered with Location Centre.
     */
    static CLcLocationAppInfoArray* GetLocationApplicationsL(
     					   RLcClientSession&   		aSession,    
                          const RArray<TPtrC>&      aAppArray,
                                TBool               aIncludeFlag );
                                                          
    /**
     * Gets the Location Application Information corresponding to the Location
     * based Application or Content/Service based on the Identifer name.
     *
     * @param[in] aSession    Reference to the Location Centre Client Session.     
     * @param[in] aIdentifier Name of the Location based Identifier whose
     *                        information is requested.
     */
    static CLcBasicAppInfo* GetLocationAppInfoL(  	  
    								RLcClientSession&   aSession,
    						  const TDesC&				aIdentifier );
      
    /**
     * Parses the Location Application information present in the Read stream.
     *
     * Other documentation present in the corresponding function for
     * CLcService class.
     */
    static CLcLocationAppInfoArray* ParseLocAppBufferL( RReadStream&   aReadStream );
    };

#endif // LCSYNCOPERATION_H
