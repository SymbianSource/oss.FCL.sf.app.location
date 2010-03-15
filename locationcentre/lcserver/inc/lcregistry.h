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
* Description:  Location Centre Server object.
*
*/


#ifndef C_LCREGISTRY_H
#define C_LCREGISTRY_H


// SYSTEM INCLUDES
#include <e32base.h>
#include <f32file.h>

//USER INCLUDE
#include "lcregistrationupdatenotifier.h"
#include "lcipcparams.h"

// FORWARD DECLARATIONS
class MLcRegistryObserver;
class CLcRegistrationParser;
class CLcRegistryUpdate;
class CLcRegAppOrder;
class CLcRegAppInfo;
class CLcRegAppStore;
class RApaLsSession;

/**
 *  Location Centre server's registration handler.
 *
 *  This class is inherited from CActive and MLcRegistryUpdateNotifier. This
 *	class handles all the basic logic(dynamic and static registration handling).
 *
 *  @since S60 v5.0
 */
class CLcRegistry : public CActive, 
					public MLcRegistryUpdateNotifier
	{
public:
    /**
     * Constructs a new instance of CLcRegistry.
     *
     * @return The new instance of CLcRegistry object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcRegistry* NewL( MLcRegistryObserver& aRegistryObserver );
    
    /**
     * Constructs a new instance of CLcRegistry.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of CLcRegistry object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcRegistry* NewLC( MLcRegistryObserver& aRegistryObserver );        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server registry handler
     */
    virtual ~CLcRegistry();
    

public:  //interface method from MLcRegistryUpdateNotifier
    /**
     * 
     */
	void HandleDynamicRegistrationL( );


public: //interface to be called by other class
    /**
     * This function retuns all the present registered service
     * like application/document/webservice to le server engine
     *
     * @since S60 v5.0
     * @param appInfoArray[ out ] contains array of registration app info pointer
     * @return If no application is registered return KErrNotFound.
     */
	TInt GetAllRegisteredAppsList( RPointerArray<CLcAppInfo>& appInfoArray);	
	
    /**
     * This function find the corresponding app info 
     * with respect to incomming application Uuid.
     *
     * @since S60 v5.0
     * @param aAppSid[ in ]
     * @param aLcRegAppInfo[ out ]      
     * @return If the app info related to this Uuid is not found return KErrNotFound
     */
	TInt GetApplicationInfo( const TDesC& 	aAppUid,
						     CLcBasicAppInfo*&	aLcRegAppInfo );  

    
private:
    /**
     * C++ Default constructor
     */
    CLcRegistry( MLcRegistryObserver& aRegistryObserver );
    
    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();


private: // From CActive
    /**
     * Inherited from CActive
     */
    void RunL();

    /**
     * Inherited from CActive
     */
    void DoCancel();		

    /**
     * Inherited from CActive
     */
    TInt RunError( TInt aError );
	
private:
    /**
     * This function checks wheather any update is needed
     * to already stored registered appinfo list.If it is then
     * start the system drive scan and update registered app info accordingly 
     *
     * @since S60 v5.0
     * @param None.
     * @return None.
     */
	void CheckForUpdationL();

    /**
     * This function scan all the system drives which are
     * mounted at present in the system and collects all
     * the registered file names present in the location centre's private
     * directory.
	 *
     * @since S60 v5.0
     * @param None.
     * @return None.
     */
	void ScanDrivesAndGetFileNameL();

    /**
     * Validate Application/Document through appshell/doc handler.
     * 
     * @since S60 v5.0
     * @param aLcRegAppInfo [ in ]		Pointer to regappinfo.
     * @return None
     */
	void ValidateRegisteredServiceL(CLcRegAppInfo& aApplicationInfo );		

    /**
     * This function notifies registration server engine that
     * processing of all the registration file has been done successfully.
     * 
     * @since S60 v5.0
     * @param None.
     * @return None.
     */
	void FinalRegistrationUpdateL();

    /**
     * Issue a request to Active scheduler.
     * 
     * @since S60 v5.0
     * @param None.
     * @return None.
     */
	void IssueRequest();
	
    /**
     * Get the current MMC Id for the device.
     * 
     * @since S60 v5.0
     * @param aUid [out] It holds current MMC id of the terminal.
     * @return  Return system error if not executed successfully
     *			else return KErrNone.
     */
	TInt GetMmcId( TUint& aUid );
	
    /**
     * This function parse a registration file and return the 
     * CLcRegAppInfo* and then it validates the application
     * or document and fetch the order and finally append this
     * CLcRegAppInfo to appinfo array.     
     * 
     * @since S60 v5.0
     * @param aLcRegAppInfo		Pointer reference to regappinfo.
     * @return KErrBadName if the regappinfo file name is not correct one.
     */
	CLcRegAppInfo* CreateAppRegInfoL( TFileName&	aFileName );

    /**
     * This function insert single registration info into
     * app info array in specific order.Order is fetched for
     * preinstalled application from central repository and for
     * post installation as registration/deregistration order.
     * 
     * @since S60 v5.0
     * @param aLcRegAppInfo [ in ]		Pointer to regappinfo.
     * @return None
     */
	void InsertAppRegInfoL( CLcRegAppInfo*		aAppInfo );
	
    /**
     * This function checks wheather certain registration file 
     * is present in the removable media like MMC.
     * 
     * @since S60 v5.0
     * @param aFileName[ in ]		Name of the file to check if in MMC.
     * @return If file is in MMC then return ETrue else EFalse.
     */
	TBool IsInMMc( TFileName& aFileName );

    /**
     * This function checks presence for a certain registration file
     * in the stored app info array and return the corresponding app info
     * 
     * @since S60 v5.0
     * @param aFileName[ in ]				Name of the file to check if
     *										present in old list.
     * @param alcRegAppInfoPtr[ out ]		filled the appropriate reg app
     *										info to this pointer.
     * @return 					On successfull 
     *							execution return ETrue else EFalse.
     */
	TBool DoesApplicationExists( TFileName& 		aFileName,
								 CLcRegAppInfo*& 	alcRegAppInfoPtr );

   /**
    * Find the content type from file useing content recognizer
    *
    * @since S60 v5.0
    * @param aDownload[ in ] 			The actual download.
    * @param aContentType[ out ] 		The mime type
    * @param aApplicationInfo[ in ] 	The application info ptr.    
    */
	void FindContentTypeFromFileL( RApaLsSession apaSession,
								   TUint8*& aContentTypeString,
								   CLcRegAppInfo* aApplicationInfo );
	
   /**
	* Checks wheather same UUID based service is already
	* present in the current registered app info array.
	*
	* @since S60 v5.0
	* @param aLcRegAppInfo 	The application info ptr.    
	*/
	TBool DoesSameUuidPresent( CLcRegAppInfo* aLcRegAppInfo );
	
    /**
     * This function compare time stamp between two registration
     * info and return ETrue if same or EFalse.
     * 
     * @since S60 v5.0
     * @param aFileName[ in ]		Name of the current file to compare.
     * @return ETrue else EFalse.
     */
	TBool IsAppUpgraded( TFileName& aFileName );
	
    /**
     * This function sets name for certains application name 
     * if it is not provided with the registration file
     * 
     * @since S60 v5.0
     * @param appInfo[ in ]		Pointer to registration info
     *							needs to fetch application caption name.
     */
	void SetAbsentAppNameL( CLcRegAppInfo*    aAppInfo );							   
	
    /**
     * This function checks registered service type
     * like  application/service/webservice.If it is an application
     * then the function fetches it's exe's full path and if it
     * is document takes it's application data and if it is weburl
     * then takes it's regiatration file and according to presence of
     * above mentioned fles presence in ROM or else where set the system
     * characteristics. 
     * 
     * @since S60 v5.0
     * @param alsSession[ in ]		Reference to RApaLsSession.
     * @param aAppInfo[ in ]		Reference to application info.     
     */
	void SetAppSysCharacteristicsL( RApaLsSession&		alsSession,
								 	CLcRegAppInfo&		aAppInfo );
	

private: // Data  members
    /**
     * Array of current Location Centre application information objects.
     * Owns
     */
    RPointerArray<CLcRegAppInfo>    iAppInfoArray;

    /**
     * Array of intermediate Location Centre application information objects.
     * Owns
     */
    RPointerArray<CLcRegAppInfo>    iAppInfoNewArray;

	//Own:File server reference.
	RFs								iFs;
	
	//Reference to observer class.	
	MLcRegistryObserver&			iRegistryObserver;
	
	//contains the list of registered file names
	RArray<TFileName>				iFileNameArry;
	
	//own:pointer to registry update
	CLcRegistryUpdate*				iLcRegUpdate;
	
	//own:pointer to registry app order settings class
	CLcRegAppOrder*					iLcRegAppOrder;
	
	// Private srorage pointer
	CLcRegAppStore*					iRegAppStore;
		
	// mentain the number of file count	
	TInt 							iregFileCount;	
	
	// boolean variable to decide registry updation start.	
	TBool							iRegistryHasToUpdate;
	};
	
#endif // C_LCREGISTRY_H

