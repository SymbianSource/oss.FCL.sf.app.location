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
* Description:  Registration information for a Location based Application.
*
*/


#ifndef C_LCREGAPPINFO_H
#define C_LCREGAPPINFO_H

// SYSTEM INCLUDES
#include <e32base.h>

// USER INCLUDE 
#include "lcipcparams.h"

// FORWARD DECLARATIONS

/**
 *  Location Centre registration info.
 *
 *	This class along with base class CLcAppInfo stores all the 
 *	details of a registerd service with Location centre.
 *
 *  @since S60 v5.0
 */
 
class CLcRegAppInfo: public CLcAppInfo
    {
public:
    /**
     * Constructs a new instance of Location centre registration object.
     *
     * @return The new instance of Location centre registration object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcRegAppInfo* NewL();
    
    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server
     */
    ~CLcRegAppInfo();
    
public:
    /**
     * This function returns the corresponding file name of a
     * registered service(application/document/web url) with full path name
     *
     * @since S60 v5.0
     * @param None.
     * @return TFileName Returns the file name with full path.
     */
	TFileName FileName();

    /**
     * This function set the file name for registered
     * service(application/document/web url) with full path name
     * It leaves with KErrBadName if the filename length is greater then 
     * 256 character.
     *
     * @since S60 v5.0
     * @param[ in ] aFile	Filename with full path.
     */
	void SetFileNameL( const TDesC& aFile );

    /**
	 * This function says if the registered service
	 * is present or absent at terminal
     *
     * @since S60 v5.0
     * @param None.
     * @return TBool	ETrue if present or EFalse.
     */
	TBool IsAppPresent( );

    /**
     * This function set's the service present or absent
     * status to true or false
     *
     * @since S60 v5.0
     * @param[ in ] aPresenceStatus		ETrue if present or EFalse
     */
	void SetFilePresenceStatus( TBool aPresenceStatus );

    /**
     * If the registered service present in removable media
     * then the current MMC id of the terminal is set through
     * this function for it.
     *
     * @since S60 v5.0
     * @param[ in ] aUid Contains the MMC id 
     */
	void SetMmcId( TUint aUid );

    /**
     * Return Mmc id associated with a registered service 
     *
     * @since S60 v5.0
     * @param None
     * @return TUint Mmc id set for this appinfo
     */
	TUint MmcId();

    /**
     * This function stores all the language specific id provided through
     * the registration file for a service in a array.
     *
     * @since S60 v5.0
     * @param[ in ] aNameLang	contains the language code
     * @return Return KErrNotFound if the Id is not valid one.
     */
	TInt SetNameLanguage( const TDesC& aNameLang );

    /**
     * This function stores all the language specific name
     * provided through the registration file for a service
     * in a RpointerArray.
     *
     * @since S60 v5.0
     * @param[ in ] aName Contains the language 
     *					  name
     */
	void SetAppNameL( const TDesC& aName );
	
    /**
     * This function check the current language code
     * with present language code with this registered service
     * and set the name accordingly.
     *
     * @since S60 v5.0
     * @param aFs Contains the file server session reference
     */
	void SetAppLangSpecificNameL( RFs& aFs );

    /**
     * Stores the index order of the app info 
     *
     * @since S60 v5.0
     * @param [ in ]  aIndex 
     */
	void SetArrayIndex( TInt aIndex );

    /**
     * 
     * @since S60 v5.0
     * @param None.
     * @return Order index of the app info 
     */
	TInt Index();

    /**
     * This function checks the count of the app name array
     * and return ETrue if count is greater than zero or EFalse
     *
     * @since S60 v5.0
     * @param None.
     * @return Etrue if the count of language name
     *		   array count is greater than zero or EFalse 
     */
	TBool IsAppNamePresent();	

private:
    /**
     * C++ Default constructor
     */
    CLcRegAppInfo();
	
private:
	// stores the file name for a reg app info 
	TFileName					iFileName;	
	
	// stores the registered service presence status 
	TBool						iFilePresenceStatus;
	
	// stores the MMC id of a lc reg app info
	TUint						iMmcId;
	
	// contains all the different language specific name  
	RPointerArray<HBufC>		iAppNameArray;
	
	// contains all the different language id for a registered file
	RArray<TUint32>				iAppLangArray;
	
	// sets the order index of app info
	TInt						iIndex;
    };

#endif // C_LCREGAPPINFO_H
