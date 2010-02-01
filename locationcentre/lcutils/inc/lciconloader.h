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
* Description:  Utility class for loading Application Icons
*
*/


#ifndef LCICONLOADER_H
#define LCICONLOADER_H

// SYSTEM INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CGulIcon;

/**
 *  Utility class for loading icons.
 *  
 *  This class encapsulated the icon loading functionality for Location Centre
 *  Application. Incase, there is no icon corresponding to the Location
 *  based Application or Content/Service a default icon wil be loaded.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
class LcIconLoader
    {
public:
	/**
	 * Loads an Icon for a Location based Application. If there is no icon
	 * corresponding to the Application, then a default Icon is loaded.
	 *
	 * @param[in] aAppUid UID for the application whose Icon is loaded.
	 */
	IMPORT_C static CGulIcon* LoadApplicationIconL( const TUid	aAppUid );
		
	/**
     * Loads the Icon for a Location based Application from an MIF Icon file.
     *
	 * If there is no icon loadable fron the requested file then a default
	 * icon is loaded.
	 *     
     * @param[in] aAppIconFile	Icon file name.
     * @param[in] aFrameNo		The Icon id of the icon when the Icon file
     *                          is a MIF File.
     * @return The new instance of Location Centre listbox object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CGulIcon* LoadMifFileIconL(
                          const TDesC&			aAppIconFile,
                                TInt			aFrameNo );
    
    /**
     * Loads the default icon for a Location based Application or Content/
     * Service.
		 *
     * @return The new instance of Location Centre listbox object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CGulIcon* LoadDefaultIconL();
    
private:	
	/**
	 * Loads an Icon for a Location based Application based on its UID. 
	 * If there is no icon corresponding to the Application UID,
	 * then a default Icon is loaded.
	 *
	 * @param[in] aAppUid UID for the application whose Icon is loaded.
	 */
	static CGulIcon*	LoadIconfromUIDL( const TUid	aAppUid );		    
		
    /**
     * Loads the Icon for a Location based Application from an Icon file.
     *
	 * If there is no icon loadable fron the requested file then a default
	 * icon is loaded.
	 *
     * @param[in] aAppIconFile	Icon file name.
     * @param[in] aFrameNo		The Icon id of the icon when the Icon file
     *                          is a MIF File.
     * @return The new instance of Location Centre listbox object.
     * @leave System wide error code if the object creation fails.         
     */
    static CGulIcon* LoadIconFromMifFileL( 		  
                          const TDesC&				aAppIconFile,
                                TInt				aFrameNo );              
    };
 
#endif // LCICONLOADER_H
