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
* Description:  Location Centre API implementation.
*
*/


#ifndef C_LCSERVICEIMPL_H
#define C_LCSERVICEIMPL_H

// SYSTEM INCLUDES
#include <e32base.h>

// USER INCLUDES
#include "lcservice.h"
#include "lclocationappinfo.h"
#include "lcclientsession.h"
#include "lcnotification.h"
#include "lcasyncoperation.h"

// FORWARD DECLARATIONS
class MLcAppExitObserver;
class TLcLocationAppFilter;
class CLcAppLauncher;
class CBufFlat;
class CLcAppInfo;
class CLcNotificationAO;
class CLcBasicAppInfo;
class CLcLocationAppInfoArray;
class CAknPopupList;
class CLcPopupListBox;

/**
 *  Implementation of Location Centre Interface class CLcService.
 *
 *  This class provides the implementation of Location Centre API.
 *  This is needed to ensure that the implementation details of Location
 *  Centre API is completely abstracted from CLcService.
 */
NONSHARABLE_CLASS( CLcServiceImpl )  : public CBase,
                                       public MLcNotification,
                                       public MLcAsynOperationObserver
    {
public: // Exported Functions
    /**
     * Constructs a new instance of Location Centre interface.
     *
     * @return The new instance of Location Centre interface object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServiceImpl* NewL();
    
    /**
     * Constructs a new instance of Location Centre interface.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Location Centre interface object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServiceImpl* NewLC();        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * interface.
     */
    virtual ~CLcServiceImpl();

    /**
     * Launches Location Centre as a pop-up dialog with a list of
     * Location based Application.
     *
     * Other documentation present in the corresponding function for
     * CLcService class.
     */                        
    void LaunchLocationCentreL( 
        const TLcLocationAppFilter&       aLocationAppFilter,
        CLcLocationAppInfo::TLcLaunchMode aLaunchMode = CLcLocationAppInfo::EDefaultMode,
        MLcAppExitObserver*               aChainedAppExitObserver = NULL );
                            
    /**
     * Launches Location Centre with the specified array of Location
     * based Applications in a pop-up dialog.
     *
     * Other documentation present in the corresponding function for
     * CLcService class.
     */                        
    void LaunchLocationCentreL(
          const RPointerArray<CLcService::CLcLaunchParam>& aIncludeAppArray,
          MLcAppExitObserver*                              aChainedAppExitObserver = NULL );
  
    /**
     * Launches Location Centre without the specified array of
     * Location based Applications in a pop-up dialog.
     *
     * Other documentation present in the corresponding function for
     * CLcService class.
     */                        
    void LaunchLocationCentreL(
        const RArray<TPtrC>&              aExcludeAppArray,
        CLcLocationAppInfo::TLcLaunchMode aLaunchMode = CLcLocationAppInfo::EDefaultMode,
        MLcAppExitObserver*               aChainedAppExitObserver = NULL );
 
     /**
     * Launches Location Centre as a pop-up dialog with a filtered list of
     * Location based Applications and the user can select a Location
     * based Application from the pop-up list displayed.
     *
     * Other documentation present in the corresponding function for
     * CLcService class.
     */     
    TPtrC SelectLocationApplicationL( 
                const TLcLocationAppFilter&         aLocationAppFilter );
       
    /**
     * Launches Location Centre with based Applications in a pop-up
     * dialog and the user can select a Location based Application
     * from the pop-up list displayed.
     *
     * Other documentation present in the corresponding function for
     * CLcService class.
     */                        
    TPtrC SelectLocationApplicationL( const RArray<TPtrC>&  aAppArray,
                                            TBool           aIncludeFlag );
 
    /**
     * Gets a filtered list of Location based Applications.
     *
     * Other documentation present in the corresponding function for
     * CLcService class.
     */
     CLcLocationAppInfoArray* GetLocationApplicationsL(
                     const TLcLocationAppFilter&    aLocationAppFilter );
    
    /**
     * Gets a filted list of Location based Applications.
     *
     * Other documentation present in the corresponding function for
     * CLcService class. 
     */
     void GetLocationApplications( 
                            TRequestStatus&             aStatus,
                      const TLcLocationAppFilter&       aLocationAppFilter,
                            CLcLocationAppInfoArray*&   aAppInfoArray );
    
    /**
     * Cancels an outstanding @ref GetLocationApplications request.
     */
    void CancelGetLocationApplications();
    
    /**
     * Launches a Location based Application in the desired mode. 
     *
     * Other documentation present in the corresponding function for
     * CLcService class.
     */
    void LaunchLocationApplicationL( 
                const TDesC&                      aAppIdentifier,
                CLcLocationAppInfo::TLcLaunchMode aLaunchMode = CLcLocationAppInfo::EDefaultMode,
                MLcAppExitObserver*               aChainedAppExitObserver = NULL );
 
    /**
     * Sets an observer for notifying changes to the Location Centre server.
     *
     * Other documentation present in the corresponding function for
     * CLcService class.  
     */
    void SetObserverL( MLcNotification&     aObserver );
                                            
    /**
     * Removes the observer that has already been set using the @ref SetObserverL method.
     *
     * Other documentation present in the corresponding function for
     * CLcService class.
     */
    TInt RemoveObserver();   

public: // Inherited from the base classes
    /**
     * Inherited from MLcNotification
     */
    void LcStatusChangedL( TInt aErrorCode );
    	
	/**
	 * Inherited from MLcAsynOperationObserver
	 */
	void OperationComplete( TInt aError );            
    
private:
    /**
     * Default C++ Constructor.
     */
    CLcServiceImpl();
                           
    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();
    
    /**
     * Leaving form of GetLocationApplications. This is needed because the API
     * does not leave whereas we need certain leaving operations to be performed
     * in the GetLocationApplications asynchronous version.
     */
	void GetLocationApplicationsL( 
	                TRequestStatus&             aStatus,
	          const TLcLocationAppFilter&       aLocationAppFilter,
	                CLcLocationAppInfoArray*&   aAppInfoArray );
	                
    /**
     * Launches Location Centre with the supplied Applications in a pop-up
     * dialog and the user can select a Location based Application from the pop-up
     * list displayed.
     *
     * @param[in] aAppInfoArray Application Information array. The ownership of this
     *                          array is transferred to the pop-up.
     * @return TPtrC, Identifier corresponding to the element that has been selected.
     */ 
    TPtrC SelectLocationApplicationL( CLcLocationAppInfoArray*   aAppInfoArray );
       
private:
    /**
     * Enumeration for the outstanding Pop-up operation.
     */
    enum TLcPopUpOutStanding
        {
        /**
         * Filtered based Applications
         */
        ELcPopupWithFiltering,
        ELcPopupWithSpecifiedApps,
        ELcPopupWithoutSpecifedApps
        };
        
private: // Data        
    /**
     * Location Centre server client side session handle
     */
    RLcClientSession                iSession;
    
    /**
     * Location based Application's launcher.
     * This object can be used to launch all Location based Applications,
     * contents and services in the standalone mode or in the chained mode.
     * Owns
     */
    CLcAppLauncher*                 iAppLauncher;
        
    /**
     * Observer for notifications
     */
    MLcNotification*                iObserver;
    
    /**
     * Active Object for notification requests
     *
     * Owns
     */
    CLcNotificationAO*              iNotificationAO;
    
    /**
     * Request Status on which the Asynchronous request would be completed.
     */
    TRequestStatus*             	iStatus;
    
    /**
     * Location Centre Asynchronous operation handler.
     *
     * Owns
     */
    CLcAsyncOperation*				iAsyncOperation;
    
    /**
     * Location Centre pop-up.
     *
     * Owns.
     */
    CAknPopupList*                  iPopUp;
    
    /**
     * Location Centre pop-up list box.
     *
     * Owns.
     */
    CLcPopupListBox*                iPopUpListBox;
    
    /**
     * Idenfier of the Selected Application.
     */
    HBufC*                          iSelectedAppId;
    
    /**
     * Pop-up Operation outstanding
     */
    TLcPopUpOutStanding             iPopupOutStanding;
    
    /**
     * Filter conditions.
     * Owns.
     */
    TLcLocationAppFilter*           iFilter;
    
    /**
     * Array of applications which need to be included/excluded in the filtering
     * operation.
     *
     * Owns.
     */
    RArray<TPtrC>                   iAppArray;
    };
    

#endif // C_LCSERVICEIMPL_H
