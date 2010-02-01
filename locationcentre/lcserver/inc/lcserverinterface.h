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
* Description:  Private Interface to the Location Centre Server.
*
*/


#ifndef LCSERVERINTERFACE_H
#define LCSERVERINTERFACE_H
 
// SYSTEM INCLUDES
#include <e32base.h>
    
/**
 * Location Centre Server Name.
 *
 * The name begins with a ! to ensure that this server is protected. This
 * prevents faking of this server by any other malicious application.
 */
 _LIT( KLocationCentreServerName, "!LocationCentreServer" );

/**
 * Executable name of the server. This is required to be exposed because
 * the server is a transient server and not a permanent server and hence
 * has to be started by the Client. 
 */
 _LIT( KLocationCentreServerExe, "lcserver" );
 
/**
 * Location Centre server Version numbers
 */
const TUint KLcServerMajorVersionNumber = 1;
const TUint KLcServerMinorVersionNumber = 0;
const TUint KLcServerBuildVersionNumber = 0;
 
/**
 * IPC Message Types
 */
 enum TLcServerMsgType
    {
    /**
     * This message is used to get the length of the buffer that needs to
     * be passed to the Location Centre Server inorder to retieve information
     * about Location based Applications registered with Location Centre.
     * The Client also needs to specify the filtering that needs to be
     * applied to the set.
     * This message is to be used as a precurssor to the next message
     * ELcFilteredApps. This message can be used either in the synchronous or
     * the asynchronous variant.
     *
     * IPC Parameter Description
     * Arg1 - Package buffer Containing the Filter arguments
     * Arg2 - TDes, pointing to the buffer which would be filled with the
     *              Length of the Application information structure.
     */
    ELcFilteredAppsBufferLength                       = 1,
    
    /**
     * Retrives information regarding Location based Applications
     * registered with Location Centre. The Client also needs to specify
     * the filtering that needs to be applied to the set.This function can
     * be used either in the synchronous or the asynchronous variant.
     * The function takes a buffer which would be filled with the filtered
     * set of Location Based Application structures.
     *
     * IPC Parameter Description
     * Arg1 - Package buffer Containing the Filter arguments
     * Arg2 - TDes, pointing to the buffer which would be filled with the
     *              Application information structure.     
     */
    ELcFilteredApps                                   = 2,
    
    /**
     * Cancels an outstanding request for obtaining a filtered set of
     * Location based Applications and Contents/Services.
     */
    ELcCancelFilteredApps							  = 3,
    
    
    /**
     * This message is used to get the length of the buffer that needs to
     * be passed to the Location Centre Server inorder to retieve information
     * about Location based Applications registered with Location Centre.
     * The Client also needs to specify the set of Location based Applications
     * which would be used for filtering.
     * This message is to be used as a precurssor to the next message
     * ELcSpecifiedApps. This message can be used either in the synchronous or
     * the asynchronous variant.
     *
     * IPC Parameter Description
     * Arg1 - Package buffer Containing the array of Location based Applications
     *        which would be used for filtering.
     * Arg2 - TBool, Boolean value specifying whether the list of applications
     *        should be included or excluded.
     * Arg3 - TDes, pointing to the buffer which would be filled with the
     *              Length of the Application information structure.
     */
    ELcSpecifiedAppsBufferLength                      = 4,
    
    /**
     * Retrives information regarding Location based Applications
     * registered with Location Centre. The Client also needs to specify the
     * set of Location based Applications which would be used for filtering.
     * This function can be used either in the synchronous or the asynchronous
     * variant. The function takes a buffer which would be filled with the
     * filtered set of Location Based Application structures.
     *
     * IPC Parameter Description
     * Arg1 - Package buffer Containing the array of Location based Applications
     *        which would be used for filtering.
     * Arg2 - TBool, Boolean value specifying whether the list of applications
     *        should be included or excluded.
     * Arg2 - TDes, pointing to the buffer which would be filled with the
     *              Application information structure.     
     */
    ELcSpecifiedApps                                  = 5,
    
    /**
     * Gets the Location Application information structure's length
     *
     * IPC Parameter Description
     * Arg1 - Package buffer containing the Location based Application
     *        Identifier.
     * Arg2 - TDes, pointing to the buffer which would be filled with the
     *              Length of the Application information structure.     
     */
    ELcAppInfoLength                                  = 6,
    
    /**
     * Gets the Location Application information structure
     *
     * IPC Parameter Description
     * Arg1 - Package buffer containing the Location based Application
     *        Identifier.
     * Arg2 - TDes, pointing to the buffer which would be filled with the
     *              Application information structure.     
     */
    ELcAppInfo                                        = 7,
        
    /**
     * Registers an Observer which notifies any changes to the Location
     * Centre Registry. The Client Application will be notified incase
     * any changes occur before the Observer is removed using the
     * ELcRemoveObserver message.
     */
    ERegisterObserver                                 = 8,
    
    /**
     * Removes an Observer which is set using the ELcRegisterObserver
     * message.
     */
    ERemoveObserver                                   = 9
    };
        
#endif // LCSERVERINTERFACE_H
