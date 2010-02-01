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
* Description:  Utility class which provides the Application, Service and File
*                launching services to Location Centre.
*
*/


#ifndef LCAPPLAUNCHER_H
#define LCAPPLAUNCHER_H

// SYSTEM INCLUDES
#include <e32std.h>
#include <aknserverapp.h>

// USER INCLUDES

// FORWARD DECLARATIONS
class CEikonEnv;
class MLcAppExitObserver;
class CSchemeHandler;
class CDocumentHandler;

/**
 * Lc Launch Application Service class. This class provides the return path
 * for the Application termination call issued from an application launched
 * in the chanied mode
 */
class RLcLaunchAppService : public RAknAppServiceBase
	{
public: // Inherited from Base classes

    /**
     * Imherited from RAknAppServiceBase
     */
	TUid ServiceUid() const;	
	};

/** 
 * Chained Application launcher for Location Centre. This class almost acts
 * like a duplication for CAknLaunchAppService class. This is required because
 * CAknLaunchAppService doesnt take any command line arguments.
 */
class CLcLaunchAppService : public CBase
	{
public:	
    /**
     * Static two phases constructor
     */
	static CLcLaunchAppService* NewL();
	
	/**
	 * C++ Destructor
	 */
	~CLcLaunchAppService();
	
	/**
	 * Launches an Application in the chained mode.
	 *
     * @since S60 v5.0
     * @param aApplicationUid UID of the application that needs to be
     *                        launched.    
     * @param aExitObserver   Application Exit Observer     
     * @param aCmdLineArg     Command line arguments which needs to be passed
     *                        when the application is launched.
	 */
    void LaunchAppL( const TUid& 						aAppUid, 
    					   MAknServerAppExitObserver& 	aObserver, 
    				 const TDesC&               		aExitObserver );
	    				 
    				 
private:
    /**
     * Generates a Server name. This function has been duplicated
     * from CAknLaunchAppService
     *
     * @since S60 v5.0
     * @param[out] aServerName	  		 Application Server name   
     * @param[in]  aAppServerUid  		 Application server UID    
     * @param[in]  aServerDifferentiator Server differentiator    
     */
    void ServerName( TName& aServerName, TUid aAppServerUid, TUint aServerDifferentiator );
    
    /**
     * Generates a Server name and a differentiator. This function has been duplicated
     * from CAknLaunchAppService
     *
     * @since S60 v5.0
     * @param[in]  aServerName	  		 Application Server name   
     * @param[in]  aAppServerUid  		 Application server UID    
     * return Server differentiator      
     */
    TUint GenerateServerDifferentiatorAndName( TName& aServerName, TUid aAppServerUid );
    
private: // Data members
	/**
	 * App Server exit monitor
	 * Owns
	 */    
	CApaServerAppExitMonitor*	iMonitor;
	
	/**
	 * Service class for establishing communication with the Application server
	 */
	RLcLaunchAppService			iService;	    				 	
	};
	
/**
 *  Utility class which provides Application, Content and Services
 *  launch functionality to Location Centre.
 *
 *  @lib lcutils.lib
 *  @since S60 v5.0
 */
class CLcAppLauncher : public CBase,
					   public MAknServerAppExitObserver
    {
public:
    /**
     * Constructs a new instance of Location Centre Application launcher.
     *
     * @return The new instance of Location Centre Application launcher object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CLcAppLauncher* NewL();
    
    /**
     * Constructs a new instance of Location Centre Application launcher.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Location Centre Application launcher object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CLcAppLauncher* NewLC();        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * interface.
     */
    virtual ~CLcAppLauncher();
   
    /**
     * Launchers an S60 Application or a Content/Service.
     *
     * This function acts a generic interface which the Location Centre
     * Utility class provides to the Location Centre Client Library. Using
     * this class Location Centre Client library can request the launch
     * of an S60 Application, a Java based Application, Location based
     * Content / Service.
     *
     * @since v5.0
     * @param   aAppType        Application Type
     * @param   aIdentifier     Application Identifier
     * @param   aStandAlone     Boolen value indicating whether the Application
     *                          is Launched in the Stand-alone mode or in the
     *                          Chained mode.
     * @param   aCmdLineArg     Command line arguments which needs to be passed
     *                          when the application is launched.
     * @param   aExitObserver   The observer to notify the termination of the
     *                          Launched application.  
     */
    IMPORT_C void LaunchLocationApplicationL(
                                TInt                    aAppType,
                          const TDesC&                  aIdentifer,
                                TBool                   aStandAlone,
                          const TDesC&                  aCmdLineArg,
                                MLcAppExitObserver*     aExitObserver );
    
private:                          
    /**
     * Launches an S60 Application as a stand-alone application.
     *
     * This function launches an S60 application using the S60 Application
     * architecture. Each S60 application is identified by a Unique
     * identifier ( the UID of the application ). This function launches the
     * application corresponding to the UID passed to it as a standalone
     * application.
     *
     * @since S60 v5.0
     * @param aApplicationUid UID of the application that needs to be
     *                        launched.    
     * @param aCmdLineArg     Command line arguments which needs to be passed
     *                        when the application is launched.  
     */
    void LaunchAppL(       TUid                    aApplicationUid,                               
                     const TDesC&                  aCmdLineArg );
                           
	 
    /**
     * Launches an S60 Application as a chained application.
     *
     * This function launches an S60 application using the S60 Application
     * architecture. Each S60 application is identified by a Unique
     * identifier ( the UID of the application ). This function launches the
     * application corresponding to the UID passed to it as a chained
     * application. In addition to that the caller function can also register
     * an observer to notify the termination of the launched application.
     *
     * @since S60 v5.0
     * @param aApplicationUid UID of the application that needs to be
     *                        launched.    
     * @param aCmdLineArg     Command line arguments which needs to be passed
     *                        when the application is launched.
     * @param aExitObserver   Application Exit Observer
     */	                           
	void LaunchAppAsChainedL( 	  TUid                    aApplicationUid,                               
                     		const TDesC&                  aCmdLineArg,
                     			  MLcAppExitObserver*     aExitObserver );                           
    
    /**
     * Launches a Web URL using the S60 Web browser.
     *
     * This function launched a Web URL as specified by the argument in
     * the S60 Web browser. This function invokes the existing instance
     * of the browser with the URL if the browser is already running. In case
     * the browser is not running, it launches the browser in the stand alone
     * mode with the specified URL. Incase, the Chained mode is requested, then
     * the browser is launched in the Embedded mode.
     *
     * @since S60 v5.0
     * @param aUrl The URL which needs to be launched in the browser.
     * @param aStandAlone     Boolen value indicating whether the Application
     *                        is Launched in the Stand-alone mode or in the
     *                        Chained mode.
     * @param aObserver       Observer to notify the termination of the
     *                        Web browser.
     */                            
    void LaunchWebBroserL( const TDesC&              aUrl,
                                 TBool               aStandAlone,
                                 MLcAppExitObserver* aObserver );
    
    /**
     * Launches content present in a file using an appropriate Application.
     *
     * This function launches the content using an appropriate S60 application. It
     * uses the S60 Document handler to load the corresponding application
     * with the specified file name.
     *
     * @since S60 v5.0
     * @param aFileName 	The name with full path of the file containing the content.
     * @param aStandAlone   Boolen value indicating whether the Application
     *                      is Launched in the Stand-alone mode or in the
     *                      Chained mode.     
     * @param aObserver 	The observer to notify the termination of the launched
     *                  	application.     
     */     
    void LaunchContentL( const TDesC&       		aFileName,
    						   TBool                aStandAlone,
    						   MLcAppExitObserver*  aObserver );
                                                                         
public: // Inherited from Base classes    
    /**
     * Inherited from MAknServerAppExitObserver
     * Notifies the termination of the Chained Application.
     *
     * @param aReason Reason for the application termination.
     */
    void HandleServerAppExit( TInt aReason );
    
private:
    /**
     * Default C++ Constructor.
     */
    CLcAppLauncher();

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();
                                                 
private:
    /**
     * Pointer to the Eikon Environment.
     */
    CEikonEnv*              iEnv;
    
    /**
     * Observer to notify the termination of the Location Application.
     */
    MLcAppExitObserver*     iExitObserver;
    
    /**
     * Launcher class used to launch the S60 browser as a chained
     * application.
     * Owns.
     */
    CSchemeHandler*         iBrowserLauncher;
    
    /**
     * Document handler used for launching documents.
     * Owns
     */
    CDocumentHandler*		iDocHandler;
    
    /**
     * Application Service class. This class is used to launch an application
     * as an embedded application
     */
    CLcLaunchAppService*	iAppService;
    
    /**
     * Flag to check whether an Embedded launch is outstanding 
     */
    TBool					isEmbeddedLaunchOutstanding;
    
    };          

#endif // LCAPPLAUNCHER_H
