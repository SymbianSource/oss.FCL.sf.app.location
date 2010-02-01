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
* Description:  Location Centre API interface.
*
*/
/**
 *  @file lcservice.h
 *
 *  This file provides the interface class to Location Centre. Using this interface
 *  the client application can operate upon components which have registered with
 *  Location Centre.
 *  The components which can register with Location Centre are of two types
 *  - Applications
 *  - Location based services / contents.
 *
 *  These Client Applications or Services/Contents which register with Location
 *  Centre will be called Location based Application henceforth.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */

#ifndef C_LCSERVICE_H
#define C_LCSERVICE_H

// SYSTEM INCLUDES
#include <e32base.h>

// USER INCLUDES
#include "lclocationappinfo.h"

// FORWARD DECLARATIONS
class CLcServiceImpl;
class MLcAppExitObserver;
class MLcNotification;
class TLcLocationAppFilter;

/**
 *  @class CLcService
 *  Interface to Location Centre.
 *
 *  This class allows the Client application to incorporate Location Centre
 *  and Location Centre related functionalities into their application.
 *  The following are the functionalities exposed by this API.
 *
 *  - Get the list of Location based Applications registered with Location Centre.
 *
 *  - Launch Location Centre with a customized set of Location based Application.
 *
 *  - Launch a particular Location based Application.
 *
 *  - Register for notification of changes to the state of the Location
 *    based Applications.
 *
 *  In additon to the above functions, the user can also customize the list
 *  of Location based Applications using the @ref TLcLocationAppFilter filter.
 *
 *  Since, this class acts as a Client side interface to the Location Centre
 *  server, it is recommended that the Client application creates the
 *  instance of this class during its construction and delete it only on
 *  Exit.
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
class CLcService : public CBase
    {
public:
    /**
     * @class CLcLaunchParam
     * Contains additional parameters for customized Location Centre
     * launching. It specifies the Location based Application that needs to be
     * displayed in Location Centre and the mode of launching for each of
     * these Location based Application when a User selects any of them. This
     * array of these structures is to be passed as an input to the 
     * overloaded @ref LaunchLocationCentreL function
     */
    class CLcLaunchParam : public CBase
        {
        public:
            /**
             * Constructs a new instance of Launch parameter object.
             *
             * @since S60 v5.0              
             * @param[in] aAppId        Identifier for the Location based Application.
             *                          The identifier string is copied.
             * @param[in] aLaunchMode   Launch mode when the terminal user selects
             *                          this Location based Application. The values
             *                          which this parameter can assume is defined
             *                          by the @ref CLcLocationAppInfo::TLcLaunchMode
             *                          enumeration.
             *                          If the user doesn't define a value for this
             *                          parameter then @ref CLcLocationAppInfo::EDefaultMode
             *                          is assumed. 
             * @return The new instance of Launch parameter object.
             * @leave System wide error code if the object creation fails.         
             */        
            IMPORT_C static CLcLaunchParam* NewL
                ( const TDesC&     aAppId,
                        CLcLocationAppInfo::TLcLaunchMode aLaunchMode = 
                                                CLcLocationAppInfo::EDefaultMode );
            
            /**
             * Constructs a new instance of Launch parameters object.
             * Leaves the created instance on the cleanup stack.
             *
             * @since S60 v5.0             
             * @param[in] aAppId        Identifier for the Location based Application.
             *                          The identifier string is copied.             
             * @param[in] aLaunchMode   Launch mode when the terminal user selects
             *                          this Location based Application. The values
             *                          which this parameter can assume is defined
             *                          by the @ref CLcLocationAppInfo::TLcLaunchMode
             *                          enumeration.
             *                          If the user doesn't define a value for this
             *                          parameter then @ref CLcLocationAppInfo::EDefaultMode
             *                          is assumed. 
             * @return The new instance of Launch parameter object.
             * @leave System wide error code if the object creation fails.         
             */                                                
            IMPORT_C static CLcLaunchParam* NewLC
                ( const TDesC&     aAppId,
                        CLcLocationAppInfo::TLcLaunchMode aLaunchMode = 
                                                CLcLocationAppInfo::EDefaultMode );
                                                
            /**
             * Virtual Destructor
             */
            virtual ~CLcLaunchParam();
                                                            
       	public: // Non Exported Methods                 
            /**
             * @internal 
             * Gets the Unique identifier identifying the Location Application.
             * This identifier is defined by the Location Application on registration
             * with Location Centre.
             * This is the identifier that must be used by the client of this API for 
             * any operation on the particular Location Application.
             * A reference to the object's internal string is returned.     
             *
             * @since S60 v5.0     
             * @return Unique identifier identifying the Location Application. If the
             *         value of the identifier has not been set then a NULL string
             *         is returned.
             */                                   
            TPtrC  Id() const;
            
            /**
             * @internal
             * Mode in which the Location Application would be launched. 
             * If the mode defined is @ref CLcLocationAppInfo::EDefaultMode, then
             * the default mode specified by the application when registering with
             * Location Centre is used.
             *
             * @since S60 v5.0     
             * @return Mode in which the Location Application would be launched.       
             */            
            CLcLocationAppInfo::TLcLaunchMode  LaunchMode() const;
        
        private:
        	/**
        	 * C++ Default Constructor
        	 */
        	CLcLaunchParam( CLcLocationAppInfo::TLcLaunchMode aLaunchMode );
        	
        	/**
        	 * Second phase of the Symbian two phase construction.
        	 */
        	void ConstructL( const TDesC& 	aAppId );        	
        	     
        private:
	        /**
	         * System wide unique identifier identifying the Location based
	         * Application. This identifier is defined by the Location based
	         * Application on registration with Location Centre.
	         * Owns
	         */               
	        HBufC*                               iId;
	        
	        /**
	         * Mode in which the Location based Application would be launched. If
	         * the mode defined is @ref CLcLocationAppInfo::EDefaultMode, then
	         * the mode specified by the Location based Application when
	         * registering with Location Centre is used.
	         * If no value is specified for this paramter then it takes 
	         * @ref CLcLocationAppInfo::EDefaultMode as its value.
	         */            
	        CLcLocationAppInfo::TLcLaunchMode     iLaunchMode;
        
        };

public: // Exported Functions
    /**
     * Constructs a new instance of Location Centre interface.
     *
     * @return The new instance of Location Centre interface object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CLcService* NewL();
    
    /**
     * Constructs a new instance of Location Centre interface.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Location Centre interface object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CLcService* NewLC();        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * interface.
     */
    virtual ~CLcService();

    /**
     * Launches Location Centre as a pop-up dialog with a list of
     * Location based Application.
     *
     * All Location based Applications which can be launched in the
     * specified mode are displayed in the Location Centre pop-up
     * dialog. The user can select any of the Location based
     * Applications displayed in the pop-up list and the
     * corresponding Location based Application would be launched
     * in the specified launching mode.
     *
     * @since S60 v5.0      
     * @param [in] aLaunchMode Mode in which the the Location based
     *                         Applications displayed in the pop-up
     *                         must be launched. The default mode for
     *                         launching is the mode in which the
     *                         corresponding Location based Application 
     *                         has registered with Location Centre.       
     * @param[in] aChainedAppExitObserver Observer for notifications of
     *                                    chained application termination
     *                                    incase, the user launches any
     *                                    Location based Application from the
     *                                    pop-up in the chained mode. If
     *                                    the user doesn't specify any value
     *                                    for the observer then no callback
     *                                    would be given on chained
     *                                    application termination.
     * @leave KErrInUse - If there is an outstanding request present on Location Centre.     
     * @leave KErrNotFound - If no Location based Applications were found
     *                       with the required configuration.
     * @leave KErrNotSupported - If Location Centre cannot be launched. This
     *                           generally occurs when the Client application
     *                           does not have a UI context.
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future
     *                               requests to Location Centre would also
     *                               fail with the same error code.
     * @leave System wide error code if the operation fails due to some other
     *        reason.
     */                        
    IMPORT_C void LaunchLocationCentreL(
        CLcLocationAppInfo::TLcLaunchMode   aLaunchMode = CLcLocationAppInfo::EDefaultMode,
        MLcAppExitObserver*                 aChainedAppExitObserver = NULL );
                    
    /**
     * Launches Location Centre as a pop-up dialog with a filtered list of
     * Location based Application.
     *
     * The Client application can configure Location Centre to display 
     * only a subset of Location based Applications. This can be achieved
     * by passing an object of type @ref TLcLocationAppFilter as the
     * parameter to this function. Only those Location based Applications
     * which satisfy the filter conditions and can be launched in the 
     * specified mode are displayed in Location Centre. 
     * The user can select any of the Location based Applications displayed
     * in the pop-up list and the  Location based Application would be
     * launched in the specified launching mode.
     *
     * The ownership of the filter is not transferred to Location Centre.        
     *
     * @since S60 v5.0      
     * @param[in] aLocationAppFilter Filter that needs to be applied 
     *                               to customize the list of Location
     *                               based Applications to be displayed in
     *                               Location Centre.
     * @param [in] aLaunchMode Mode in which the the Location based
     *                         Applications displayed in the pop-up
     *                         must be launched. The default mode for
     *                         launching is the mode in which the
     *                         corresponding Location based application has
     *                         registered with Location Centre.         
     * @param[in] aChainedAppExitObserver Observer for notifications of
     *                                    chained application termination
     *                                    incase, the user launches any
     *                                    Location based Application from the
     *                                    pop-up in the chained mode. Incase,
     *                                    the user doesn't specify any value
     *                                    for the observer then no callback
     *                                    would be given on ahained
     *                                    application termination.
     * @leave KErrInUse - If there is an outstanding request present on Location Centre.     
     * @leave KErrNotFound - If no Location based Applications were found
     *                       with the required configuration.
     * @leave KErrNotSupported - If Location Centre cannot be launched. This
     *                           generally occurs when the Client application
     *                           does not have a UI context.
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future
     *                               requests to Location Centre would also
     *                               fail with the same error code.     
     * @leave System wide error code if the operation fails due to some other
     *        reason.
     */                        
    IMPORT_C void LaunchLocationCentreL( 
        const TLcLocationAppFilter&         aLocationAppFilter,
        CLcLocationAppInfo::TLcLaunchMode   aLaunchMode = CLcLocationAppInfo::EDefaultMode,
        MLcAppExitObserver*                 aChainedAppExitObserver = NULL );
                            
    /**
     * Launches Location Centre with the specified array of Location
     * based Applications in a pop-up dialog.
     *
     * The items of the array are first validated for their registration
     * with Location Centre and only those Location based Applications which are
     * registered with Location Centre and which can be launched in the
     * specified launch mode are displayed.
     * The user can select any of the Location based Application displayed in the 
     * pop-up list and the corresponding Location based Application would be opened
     * the mode defined in the corresponding @ref TLcLaunchParam
     * object.
     *
     * The ownership of the specified array is not tranferred to Location
     * Centre.
     *
     * @since S60 v5.0         
     * @param[in] aIncludeAppArray  Array of Location based Applications which
     *                              are to be displayed in Location Centre.
     * @param[in] aChainedAppExitObserver Observer for notifications of
     *                                    chained application termination
     *                                    incase, the user launches any
     *                                    Location based Application from the
     *                                    pop-up in the chained mode. Incase,
     *                                    the user doesn't specify any value
     *                                    for the observer then no callback
     *                                    would be given on chained
     *                                    application termination.
     * @leave KErrInUse - If there is an outstanding request present on Location Centre.     
     * @leave KErrNotFound - If none of the specified Location based
     *                       Applications were found registered with 
     *                       Location Centre.
     * @leave KErrNotSupported - If Location Centre cannot be launched. This
     *                           generally occurs when the Client application
     *                           does not have a UI context.
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future
     *                               requests to Location Centre would also
     *                               fail with the same error code.     
     * @leave System wide error code if the operation fails due to some other
     *        reason.
     */                        
    IMPORT_C void LaunchLocationCentreL(
                  const RPointerArray<CLcLaunchParam>& aIncludeAppArray,
                  MLcAppExitObserver*                  aChainedAppExitObserver = NULL );
  
    /**
     * Launches Location Centre without the specified array of
     * Location based Applications in a pop-up dialog.
     *
     * Only those Location based Applications which are not a part of the
     * specified array and which can be launched in the specified mode
     * are displayed in Location Centre. 
     * The user can select any of the Location based Application displayed in the 
     * pop-up list and the corresponding Location based Application would be opened
     * the mode defined in the specified mode.
     *
     * The ownership of the specified array is not tranferred to Location
     * Centre.
     *
     * @since S60 v5.0     
     * @param[in] aExcludeAppArray Array of identifiers of Location based
     *                             Application which are not to be displayed
     *                             in Location Centre.
     *                             These identifiers are to be defined by the
     *                             Location based Application on registration with
     *                             Location Centre.
     * @param [in] aLaunchMode Mode in which the all the Location
     *                         applications displayed in the pop-up
     *                         must be launched. The default mode for
     *                         launching is the mode in which the
     *                         corresponding application has registered
     *                         with Location Centre.         
     * @param[in] aChainedAppExitObserver Observer for notifications of
     *                                    chained application termination
     *                                    incase, the user launches any
     *                                    Location based Application from the
     *                                    pop-up in the chained mode. Incase,
     *                                    the user doesn't specify any value
     *                                    for the observer then no callback
     *                                    would be given on chained
     *                                    application termination.
     * @leave KErrInUse - If there is an outstanding request present on Location Centre.     
     * @leave KErrNotFound - If no Location based Applications were found
     *                       with the required configuration.
     * @leave KErrNotSupported - If Location Centre cannot be launched. This
     *                           generally occurs when the Client application
     *                           does not have a UI context.
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future
     *                               requests to Location Centre would also
     *                               fail with the same error code.        
     * @leave System wide error code if the operation fails due to some other
     *        reason.
     */                        
    IMPORT_C void LaunchLocationCentreL(
        const RArray<TPtrC>&              aExcludeAppArray,
        CLcLocationAppInfo::TLcLaunchMode aLaunchMode = CLcLocationAppInfo::EDefaultMode,
        MLcAppExitObserver*               aChainedAppExitObserver = NULL );
 
    /**
     * Launches Location Centre as a pop-up dialog with a list of
     * Location based Applications and the user can select a Location
     * based Application from the pop-up list displayed.
     * 
     * @since S60 v5.0
     * @return If the terminal User has selected a Location based Application,
     *         Unique identifier identifying the Location based Application
     *         which was selected.
     *         NULL string incase the terminal User cancelled the pop-up dialog.
     *         The string returned is a reference to this object's internal string. Hence,
     *         the Client Application must copy the string if it expects to retain
     *         it.
     * @leave KErrInUse - If there is an outstanding request present on Location Centre.     
     * @leave KErrNotFound - If no Location based Applications were found.
     * @leave KErrNotSupported - If Location Centre cannot be launched. This
     *                           generally occurs when the Client application
     *                           does not have a UI context.
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future
     *                               requests to Location Centre would also
     *                               fail with the same error code.    
     * @leave System wide error code if the operation fails due to some other
     *        reason.
     */     
    IMPORT_C TPtrC SelectLocationApplicationL();
 
     /**
     * Launches Location Centre as a pop-up dialog with a filtered list of
     * Location based Applications and the user can select a Location
     * based Application from the pop-up list displayed.
     * 
     * The Client application can configure Location Centre to display 
     * only a subset of Location based Applications. This can be achieved
     * by passing an object of type @ref TLcLocationAppFilter as the
     * parameter to this function. Only those Location based Applications
     * which satisfy the filter conditions are displayed in Location Centre. 
     * The user can select any of the Location based Applications displayed
     * in the pop-up list and the identifier corresponding to the selected
     * Location based Application would be returned back to the user.
     *
     * The ownership of the filter is not transferred to Location Centre.   
     *     
     * @since S60 v5.0
     * @param[in] aLocationAppFilter Filter that needs to be applied 
     *                               to customize the list of Location
     *                               based Applications to be displayed in
     *                               Location Centre.     
     * @return If the terminal User has selected a Location based Application,
     *         Unique identifier identifying the Location based Application
     *         which was selected.
     *         NULL string incase the terminal User cancelled the pop-up dialog.
     *         The string returned is a reference to this object's internal string. Hence,
     *         the Client Application must copy the string if it expects to retain
     *         it.     
     * @leave KErrInUse - If there is an outstanding request present on Location Centre.     
     * @leave KErrNotFound - If no Location based Applications were found.
     * @leave KErrNotSupported - If Location Centre cannot be launched. This
     *                           generally occurs when the Client application
     *                           does not have a UI context.
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future requests
     *                               to Location Centre would also fail with
     *                               the same error code.    
     * @leave System wide error code if the operation fails due to some other
     *        reason.
     */     
    IMPORT_C TPtrC SelectLocationApplicationL( 
                const TLcLocationAppFilter&         aLocationAppFilter );
       
    /**
     * Launches Location Centre with based Applications in a pop-up
     * dialog and the user can select a Location based Application
     * from the pop-up list displayed.
     *
     * The Client application can configure Location Centre to either
     * display only those Location based Applications specified in
     * the array or to display all Location based Applications other
     * than those specified in the array.
     * The user can select any of the Location based Application displayed
     * in the pop-up list and the identifier corresponding to the selected
     * Location based Application would be returned back to the user.
     *
     * The ownership of the specified array is not tranferred to Location
     * Centre.
     *
     * @since S60 v5.0         
     * @param[in] aArray  Array of Location based Applications which
     *                    are to be displayed in Location Centre.
     * @param[in] aIncludeFlag If the flag value is
     *                         - ETrue, then only those Location based
     *                           Applications specified in the array would
     *                           be displayed in Location Centre.
     *                         - EFalse, then all Location based Applications
     *                           other than those specified in the array
     *                           would be displayed in Location Centre.
     * @return If the terminal User has selected a Location based Application,
     *         Unique identifier identifying the Location based Application
     *         which was selected.
     *         The string returned is a reference to this object's internal string. Hence,
     *         the Client Application must copy the string if it expects to retain
     *         it.     
     *         NULL string incase the terminal User cancelled the pop-up dialog.
     * @leave KErrInUse - If there is an outstanding request present on Location Centre.     
     * @leave KErrNotFound - If none of the specified Location based
     *                       Applications were found registered with 
     *                       Location Centre.
     * @leave KErrNotSupported - If Location Centre cannot be launched. This
     *                           generally occurs when the Client application
     *                           does not have a UI context.
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future
     *                               requests to Location Centre would also
     *                               fail with the same error code.         
     * @leave System wide error code if the operation fails due to some other
     *        reason.
     */                        
    IMPORT_C TPtrC SelectLocationApplicationL(
                  const RArray<TPtrC>&      aAppArray,
                  TBool                     aIncludeFlag );

    /**
     * Gets a list of all Location based Applications.
     *
     * This function returns a list of all Location based Applications 
     * to the caller. 
     *
     * The ownership of the list of Location based Applications is
     * transferred to the caller.
     *       
     * @since S60 v5.0     
     * @param[in] aLocationAppFilter Filter that needs to be applied 
     *                               to customize the list of Location
     *                               Applications to be retrieved from
     *                               Location Centre.
     * @return CLcLocationAppInfoArray object containing a list
     *         of applications registered with Location Centre.
     * @leave KErrNotFound, If there are no Location based Applications
     *                      available.
     * @leave KErrInUse - If there is an outstanding request present on Location Centre.     
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future
     *                               requests to Location Centre would also
     *                               fail with the same error code.    
     * @leave System wide error code if the array retrieval fails for
     *        any other reason. 
     */
    IMPORT_C  CLcLocationAppInfoArray* GetLocationApplicationsL();
    
    /**
     * Gets a list of all Location based Applications.
     *
     * This is an asynchronous version and at any instant of time there
     * can be only one such request outstanding.
     *
     * The ownership of the list of Location based Applications is
     * transferred to the caller.     
     * 
     * @since S60 v5.0     
     * @param[out] aStatus Status variable on which the completion of the 
     *                     request would be communicated. The following
     *                     are the values which the variable can assume
     *                     on completion of the request.
     *                     Since, the CLcService class will use its internal
     *                     Active object for interacting with the Location
     *                     Centre server and not this aStatus directly, the
     *                     Client application must not use User::WaitforRequest
     *                     on this status variable.
     *                     - KErrNone, If the list of applications was
     *                                 successfully retrieved.
     *                     - KErrInUse, If there is an outstanding
     *                                  request present on Location Centre.
     *                     - KErrNotFound, If there are no Location
     *                                  based Applications available.
     *                     - KErrCancel, If an outstanding request is
     *                                  Cancelled.
     *                     - KErrServerTerminated - If the Location Centre
     *                                   server terminates prematurely. In this
     *                                   case, all future requests to Location
     *                                   Centre would also fail with the same
     *                                   error code.    
     *                     - System wide error code if the array retrieval
     *                       fails for any other reason.
     * @param[out] aAppInfoArray Reference to the pointer of Application
     *                           information array. This pointer would be
     *                           updated with the @ref CLcLocationAppInfoArray
     *                           object containing a list of applications
     *                           registered with Location Centre on the
     *                           asynchronous completion of this function.   
     */
    IMPORT_C  void GetLocationApplications( 
                            TRequestStatus&             aStatus,
                            CLcLocationAppInfoArray*&   aAppInfoArray );
 
     /**
     * Gets a filtered list of Location based Applications.
     *
     * The Client application can configure the list of Location based
     * Applications returned. This can be achieved by passing an object
     * of type @ref TLcLocationAppFilter as the parameter to this function.
     *
     * The ownership of the filter is not transferred to Location Centre.
     *   
     * The ownership of the list of Location based Applications is
     * transferred to the caller.
     *       
     * @since S60 v5.0     
     * @param[in] aLocationAppFilter Filter that needs to be applied 
     *                               to customize the list of Location
     *                               Applications to be retrieved from
     *                               Location Centre.
     * @return CLcLocationAppInfoArray object containing a list
     *         of applications registered with Location Centre.
     * @leave KErrNotFound If there are no Location based Applications
     *                     corresponding to the filter paramter.    
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future
     *                               requests to Location Centre would also
     *                               fail with the same error code.      
     * @leave System wide error code if the array retrieval fails for
     *        any other reason. 
     */
    IMPORT_C  CLcLocationAppInfoArray* GetLocationApplicationsL(
                     const TLcLocationAppFilter&         aLocationAppFilter );
    
    /**
     * Gets a filted list of Location based Applications.
     *
     * The Client application can configure the list of Location based
     * Applications returned. This can be achieved by passing an object
     * of type @ref TLcLocationAppFilter as the parameter to this function.     
     * This is an asynchronous version and at any instant of time there
     * can be only one such request outstanding.
     *
     * The ownership of the filter is not transferred to Location Centre.
     *     
     * The ownership of the list of Location based Applications is
     * transferred to the caller.     
     * 
     * @since S60 v5.0     
     * @param[out] aStatus Status variable on which the completion of the 
     *                     request would be communicated. The following
     *                     are the values which the variable can assume
     *                     on completion of the request.
     *                     Since, the CLcService class will use its internal
     *                     Active object for interacting with the Location
     *                     Centre server and not this aStatus directly, the
     *                     Client application must not use User::WaitforRequest
     *                     on this status variable.     
     *                     - KErrNone, If the list of applications was
     *                                 successfully retrieved.
     *                     - KErrInUse, If there is an outstanding
     *                                  request present.
     *                     - KErrCancel, If an outstanding request is
     *                                  Cancelled.
     *                     - KErrServerTerminated - If the Location Centre
     *                                   server terminates prematurely. In 
     *                                   this case, all future requests to
     *                                   Location Centre would also fail with
     *                                   the same error code.    
     *                     - System wide error code if the array retrieval
     *                       fails for any other reason.
     * @param[in] aLocationAppFilter Filter that needs to be applied 
     *                               to customize the list of Location
     *                               Applications to be retrieved from
     *                               Location Centre.     
     * @param[out] aAppInfoArray Reference to the pointer of Application
     *                           information array. This pointer would be
     *                           updated with the @ref CLcLocationAppInfoArray
     *                           object containing a list of applications
     *                           registered with Location Centre on the
     *                           asynchronous completion of this function.   
     */
    IMPORT_C  void GetLocationApplications( 
                            TRequestStatus&             aStatus,
                      const TLcLocationAppFilter&       aLocationAppFilter,
                            CLcLocationAppInfoArray*&   aAppInfoArray );
    
    /**
     * Cancels an outstanding @ref GetLocationApplications request.
     */
    IMPORT_C void CancelGetLocationApplications();
    
    /**
     * Launches a Location based Application in the desired mode. 
     * 
     * The identifier passed to te function is first validated for 
     * existence and registration with Location Centre. Incase, the 
     * Location based Application exists and its has already registered with
     * Location Centre, then the function launches the Location
     * Application in the desired mode.
     *
     * @since S60 v5.0     
     * @param[in] aAppIdentifier Unique identifier identifying the 
     *                           Location based Application.
     * @param [in] aLaunchMode Mode in which the all the Location based
     *                         Applications displayed in the pop-up
     *                         must be launched. The default mode for
     *                         launching is the mode in which the
     *                         corresponding Location based Application
     *                         has registered with Location Centre.         
     * @param[in] aChainedAppExitObserver Observer for notifications of
     *                                    chained application termination
     *                                    incase, the user launches any
     *                                    Location based Application from the
     *                                    pop-up in the chained mode. Incase,
     *                                    the user doesn't specify any value
     *                                    for the observer then no callback
     *                                    would be given on chained
     *                                    application termination. If a value
     *                                    is specified for the observer for
     *                                    Stand alone lauunching then the
     *                                    Observer value would be ignored.        
     * @leave KErrNotFound, If there is no Location based Application
     *                      corresponding to the identifer or if the Location
     *                      based Application cannot be launched in the
     *                      specified mode.
     * @leave KErrInUse - If there is an outstanding request present on Location
     *                    Centre.     
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future
     *                               requests to Location Centre would also fail
     *                               with the same error code.    
     * @leave System wide error code if the operation fails for any other
     *        reason.
     */
    IMPORT_C void LaunchLocationApplicationL( 
                const TDesC&                      aAppIdentifier,
                CLcLocationAppInfo::TLcLaunchMode aLaunchMode = CLcLocationAppInfo::EDefaultMode,
                MLcAppExitObserver*               aChainedAppExitObserver = NULL );
 
    /**
     * Sets an observer for notifying changes to the Location Centre server.
     * 
     * The notification can occur under the following conditions
     * - When a new Location based Application registers into Location Centre.
     * In this case, the Location based Application would be immediately available
     * for all Location Centre operations.
     * - When an already registered Location based Application de-registers.
     * In this case, the Location based Application will be removed from Location
     * Centre. Hence, it would not be available for any further Location Centre
     * operations.
     * - When a Location based Application is present on a removable media
     * (like Memory card) and the media is removed. In this case, for all practical
     * purposes the Location based Application behaves as in the previous context.
     * The difference being that the Location based Application does not get removed
     * from Location Centre but is marked as an Absent application. When the
     * removable media is re-inserted, the Location based Application immediately 
     * becomes available for all Location Centre operations.
     * - When an removable media containing an Absent Application is re-inserted. In
     * this case, the Location based Application would be immediately available for
     * all Location Centre operations.
     * - When the Location Centre server terminates pre-maturely. In this case, no
     * further notifications would be given to the Client application and all 
     * further requests to the Location Centre server would fail with 
     * @p KErrServerTerminated.
     *
     * @since S60 v5.0     
     * @param[in] aObserver Observer to which notification would be issued.
     * @leave KErrAlreadyExists If the observer has already been set.
     * @leave KErrServerTerminated - If the Location Centre server terminates
     *                               prematurely. In this case, all future requests to
     *                               Location Centre would also fail with the same
     *                               error code.    
     */
    IMPORT_C void SetObserverL( MLcNotification&     aObserver );
                                            
    /**
     * Removes the observer that has already been set using the @ref SetObserverL method.
     *
     * @since S60 v5.0   
     * @return KErrNone, If the observer removal was successful.
     *         KErrNotFound, If no observer was set previously.
     */
    IMPORT_C TInt RemoveObserver();
        
private:
    /**
     * Default C++ Constructor.
     */
    CLcService();

    /**
     * Overloaded Copy Constructor. By default, prohibit copy constructor
     *
     * @param[in] aLocationCentre Location Centre interface object from which the new
     * object should be constructed.
     */
    CLcService( const CLcService&     aLocationCentre );
    
    /**
     * Overloaded Assignment operator. By default, prohibit assignment
     * operator
     *
     * @param[in] aLocationCentre Location Centre interface object from which the new
     * object should be assigned.
     * @return the Location Centre interface object after assigment.
     */
    CLcService& operator= ( 
                const CLcService&   aLCAppInfo );
                           
    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();
    
private: // Data
    /**
     * Location Centre Implementation. Hides the actual implementation
     * of the Location Centre API.
     *
     * Owns
     */
    CLcServiceImpl*             iImplementation;
    };

#endif // C_LCSERVICE_H
