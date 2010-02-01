/*
* ============================================================================
*  Name        : lclocationappinfo.h
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
* Description:  Location Application Information structures.
*
*/

/**
 *  @file lclocationappinfo.h
 * 
 *  This file provides the information about the components registered with
 *  Location Centre.
 *
 *  @note The components which can register with Location Centre are of
 *  two types
 *  - Applications
 *  - Location based services / contents.
 *
 *  These Client Applications or Services/Contents which register with Location
 *  Centre will be called Location based Applications henceforth.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */

#ifndef C_LCLOCATIONAPPINFO_H
#define C_LCLOCATIONAPPINFO_H

// SYSTEM INCLUDES
#include <e32base.h>

// USER INCLUDES
#include "lclocationappfilter.h"

// FORWARD DECLARATIONS
class CGulIcon;

/**
 *  @class CLcLocationAppInfo
 *  Location Application information.
 *
 *  The class defines the information pertaining to an application registered 
 *  with Location Centre.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
class CLcLocationAppInfo : public CBase
    {
public:
    /**
     * @enum TLcSystemCharacteristics
     * System Characteristics for the applications registered with Location
     * Centre.
     *
     * These characteristics can be combined and used to form a bit-mask
     * which can then be used while retrieving/displaying registered
     * applications from Location Centre. 
     * The system characteristics are defined by Location Centre when an
     * application registers with it.
     */
    enum TLcSystemCharacteristics
        {
        /**
         * This characteristic can be used to retrieve/display all Location
         * Applications regardless of their System arrtibutes. This
         * characteristic cannot be combined with any other value.
         * This will be used as the default System characteristic.
         */
        ESysCharNone                                = 0,
        
        /**
         * This characteristic is associated with Location based Applications that
         * are installed on the ROM.
         */
        ESysCharRomBased                            = 1
        };
        
    /**
     * @enum TLcApplicationCharacteristics
     * Application Characteristics for the applications registered with 
     * Location Centre.
     * 
     * The application characteristics are defined by the Location based Applications
     * when they register with Location Centre. A Location Application can
     * define one or more characteristics when they register. These
     * characteristics can be combined and used to form a bit-mask which can
     * then be used while retrieving/displaying registered applications from
     * Location Centre.
     */
     enum TLcApplicationCharacteristics
        {
        /**
         * This characteristics is used for Location based Applications which do
         * not wish to publish any characteristics. These Location based Applications
         * would get displayed in Location Centre only when Location Centre is
         * launched from the application shell or when the user of the Location
         * Centre API does not specify any other Application characteristics in
         * filtering. This characteristic cannot be combined with any other value.
         * This will be used as the default Application characteristic.             
         */
        EAppCharNone                                = 0,
        
        /**
         * This characteristic is used for Location based Applications which have
         * Settings related functionality. These Location based Applications are
         * used to configure the user's terminal or the remote server for
         * all Location related features.
         */
        EAppCharSettings                            = 1,
        
        /**
         * This characteristic is used for Location based Applications which have
         * Personal Data Management related functionality. These Location
         * based Applications enable the user to manage his personal information.
         */
        EAppCharPersonalDataManagement              = 2,
        
        /**
         * This characteristic is used for Location based Applications which have
         * Navigation related functionality. These Location based Applications
         * help the terminal user to navigate from one location to another.
         */
        EAppCharNavigation                          = 4,
        
        /**
         * This characteristic is used for Location based Applications which are
         * related to Location based events. These Location based Applications are
         * generally based to trigger certain events based on the terminal's
         * current Location.
         */
        EAppCharLocationBasedEvents                 = 8,
        
        /**
         * This characteristic is used for Location based Applications which have
         * Remote positioning related functionality. These Location 
         * based Applications are generally involved in determining the current
         * location of other terminals or objects.
         */
        EAppCharRemotePositioning                   = 16,
        
        /**
         * This characteristic is used for Location based Applications which are
         * related to Network services. These Location based Applications are
         * generally involved in the exchange of Location related information
         * and Location related services over the network.
         */
        EAppCharNetworkService                      = 32,
        
        /**
         * This characteristic is used for Location based Applications which are
         * used to send Location enhanced messages.
         */
        EAppCharLocationEnhancedMessages            = 64,
        
        /**
         * This characteristic is used for Location based Applications which
         * provide Location based content which is stored on the terminal. 
         */
        EAppCharOnDeviceLocationBasedContent        = 128,
    
        /**
         * This characteristic is used for Location based Applications which
         * provide Location based content that is stored remotely and needs
         * to be accessed from the remote server. These Location based Applications
         * would need connection to the network to access the Location based
         * content.
         */
        EAppCharRemoteLocationBasedContent          = 256,
        
        /**
         * This characteristic is used for Location based Applications which
         * position a particular geographic location of a map.
         */
        EAppCharMaps                                = 512
        };
                
    /**
     * @enum TLcLaunchMode
     * The mode in which Location Centre needs to launch the application
     * contained in the Location Application information object.
     */
    enum TLcLaunchMode
        {
        /**
         * The corresponding application would be launched in the mode
         * specified by it during registration with Location Centre.
         */
        EDefaultMode                            = 0,
        
        /**
         * The corresponding application would be launched as a stand
         * alone application.
         */
        EStandAloneMode                         = 1,
        
         /**
         * The corresponding application would be launched as a
         * chained application.
         */
        EChainedMode                            = 2           
        };

public: // Exported Functions
    /**
     * Constructs a new instance of Location Application information object.
     *
     * @return The new instance of Location Application information object.
     * @leave System wide error code if the object creation fails.
     */
    IMPORT_C static CLcLocationAppInfo* NewL();
    
    /**
     * Constructs a new instance of Location Application information object.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Location Application information object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CLcLocationAppInfo* NewLC();        

    /**
     * Constructs a new instance of Location Application information object.
     *
     * @param[in] aLocAppInfo Location Application Information from which the
     *                        object should be constructed. 
     * @return The new instance of Location Application information object.
     * @leave System wide error code if the object creation fails.
     */
    IMPORT_C static CLcLocationAppInfo* NewL( 
                            const CLcLocationAppInfo&  aLocAppInfo );
    
    /**
     * Constructs a new instance of Location Application information object.
     * Leaves the created instance on the cleanup stack.
     *
     * @param[in] aLocAppInfo Location Application Information from which the
     *                        object should be constructed.         
     * @return The new instance of Location Application information object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CLcLocationAppInfo* NewLC(
                            const CLcLocationAppInfo&  aLocAppInfo );    

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Application
     * information object.
     */
    virtual ~CLcLocationAppInfo();

    /**
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
    IMPORT_C TPtrC Id() const;
    
    /**
     * Gets the Logical name for the Location Application.
     * The name is defined by the Location Centre on registration with
     * Location Centre. This is the name which would be displayed by
     * Location Centre for this application.
     * A reference to the object's internal string is returned.
     *
     * @since S60 v5.0     
     * @return Logical name for the Location Application. If the name 
     *         has not been set for the Location Application, then a NULL
     *         string is returned.
     */
    IMPORT_C TPtrC Name() const;
      
    /**
     * Gets the Icon for the Location Application. 
     * This is the icon that would be displayed in Location Centre for this
     * application. If no icon is defined then a default icon would be used.
     *
     * The ownership of the icon would be transferred back to the caller
     * function.
     *
     * @since S60 v5.0     
     * @return - Icon for the Location Application. 
     *         - NULL, If the Icon has not been set for the Location 
     *                 based Application
     * @leave System wide error codes if the icon allocation or copying
     *        fails.
     */
    IMPORT_C CGulIcon* IconL() const;

    /**
     * Mode in which the Location Application would be launched. 
     * If the mode defined is EDefaultMode, then the default mode specified
     * by the application when registering with Location Centre is used.
     *
     * @since S60 v5.0     
     * @return Mode in which the Location Application would be launched.       
     */
    IMPORT_C TLcLaunchMode LaunchMode() const;

    /**
     * Returns the System characteristics for the Location Application
     *
     * @since S60 v5.0
     * @return - System Characteristics for the Location
     *           Application. All the system characteristics for the 
     *           Location Application would be combined to form an integer
     *           bit-mask and this function returns the bit-mask. The
     *           individual System characteristics are defined by
     *           @ref TLcSystemCharacteristics enumeration.
     */
    IMPORT_C TUint32 SystemCharacteristics() const;

    /**
     * Returns the Application characteristics for the Location Application
     *
     * @since S60 v5.0
     * @return - Application Characteristics for the Location
     *           Application. All the application characteristics for the 
     *           would be combined to form an integer bit-mask and this
     *           function returns the current bit-mask. The individual
     *           Application characteristics are defined by
     *           @ref TLcApplicationCharacteristics enumeration.
     */
    IMPORT_C TUint32 ApplicationCharacteristics() const;
                           
                                                            
public: // Non exported methods
 
    /**
     * @internal
     * Set the Unique identifier identifying the Location Application. The
     * identifer is copied.
     *
     * @since S60 v5.0     
     * @param[in] aId Unique identifier identifying the Location Application.
     * @leave KErrNoMemory If there is not enough memory to copy the string.     
     * @leave KErrAlreadyExists If the value of the Location applicaton UID
     *                          has already been set.       
     */
    void SetIdL( const TDesC& aId );

    /**
     * @internal    
     * Set the Logical name for the Location Application. The name is 
     * copied.
     *
     * @since S60 v5.0     
     * @param[in] aName Logical name for the Location Application. 
     *                  The logical string is copied.
     * @leave KErrNoMemory If there is not enough memory to copy the string.
     * @leave KErrAlreadyExists If the value of the Location applicaton name.
     *                          has already been set.
     */
    void SetNameL( const TDesC& aName );

    /**
     * @internal    
     * Set the Icon for the Location Application.
     *
     * @since S60 v5.0     
     * @param[in] aIconType The type of the file which contains the Icon.
     * @param[in] aIconData Opaque data field which contains the actual data information.
     * @param[in] aIconId	Id of the Icon in the Icon file.
     * @leave KErrAlreadyExists If the value of the Location applicaton Icon
     *                          has already been set.         
     */	
	void SetIconL( 		 TInt		aIconType,
				   const TDesC&		aIconData,
				   		 TInt		aIconId = 0 );
    
    /**
     * @internal    
     * Sets the Mode in which the Location Application would be launched.
     *
     * @since S60 v5.0     
     * @param[in] aLaunchMode Mode in which the Location Application would be
     *                        launched. 
     */
    void SetLaunchMode( TLcLaunchMode  aLaunchMode );
    
    /**
     * @internal    
     * Sets the system characteristics for the Location Application.
     *
     * @since S60 v5.0     
     * @param[in] aSysCharacteristics System Characteristics for the Location
     *                                Application.
     */
    void SetSystemCharacteristics( TUint32   aSysCharacteristics);

    /**
     * @internal    
     * Sets the Application characteristics for the Location Application.
     *
     * @since S60 v5.0     
     * @param[in] aAppCharacteristics Application Characteristics for the Location
     *                                Application.
     */
    void SetApplicationCharacteristics( TUint32   aAppCharacteristics);

private:
    /**
     * Default C++ Constructor.
     */
    CLcLocationAppInfo();
    
    /**
     * Overloaded Copy Constructor.
     *
     * @param[in] aLCAppInfo Location Application information object from which
     * the new object should be constructred.
     */
    CLcLocationAppInfo( 
                const CLcLocationAppInfo&     aLCAppInfo );
    
    /**
     * Overloaded Assignment operator. By default, prohibit assignment
     * operator
     *
     * @param[in] aLCAppInfo Location Application information object from which 
     * the new value needs to be assigned.
     * @return The Location Application information object after assigment.
     */
    CLcLocationAppInfo& operator= ( 
                const CLcLocationAppInfo&   aLCAppInfo );
                           
    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();
    
   /**
    * Second phase overloaded constructor
    */
   void ConstructL( const CLcLocationAppInfo&   aLCAppInfo ); 
    
private: // Data
    /**
     * System wide unique identifier identifying the Location Application. This
     * identifier is defined by the Location Application on registration with
     * Location Centre.
     *
     * Owns
     */
    HBufC*                          iId;
    
    /**
     * Application Name.
     * Logical name for the Location Application. The name is defined by the
     * Location Centre on registration with Location Centre. This is the name
     * which would be displayed by Location Centre for this application.
     * 
     * Owns
     */
    HBufC*                          iApplicationName;
    
    /**
     * Application Icon Data.
     * Icon for the Location Application. This is the icon that would be displayed
     * in Location Centre for this application. If no icon is defined then a
     * default icon would be used.
     *
     * Owns
     */
    HBufC*                       	iApplicationIconData;
    
	/**
	 * Application Icon Type.
	 */
	TInt							iApplicationIconType;
	
	/**
	 * Application Icon Id.
	 */
	TInt							iIconId;
	
    /**
     * Mode in which the Location Application would be launched. If the mode
     * defined is EDefaultMode, then the default mode specified by the 
     * application when registering with Location Centre is used.
     */
    TLcLaunchMode                   iLaunchMode; 
     
   /**
    * Bit-mask containing the System characteristics for the corresponding
    * Location application
    */
    TUint32                         iSystemCharacteristics;
    
   /**
    * Bit-mask containing the Application characteristics for the corresponding
    * Location application
    */
    TUint32                         iAppCharacteristics;
          
    /**
     * Reserved for future use.
     */           
    TAny*                           iReserved;
    };

/**
 *  @class CLcLocationAppInfoArray
 *  Array of CLocationAppInfo objects.
 *
 *  Place holder for the information about the applications registered with
 *  Location Centre. The class contains a list of individual Location Application
 *  information items.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
class CLcLocationAppInfoArray : public CBase
    {
public:
    /**
     * Constructs a new instance of Location Application information array.
     *
     * @return The new instance of Location Application information array.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CLcLocationAppInfoArray* NewL();
    
    /**
     * Constructs a new instance of Location Application information array.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Location Application information array.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CLcLocationAppInfoArray* NewLC();        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Application
     * information array. This includes all the Location Application
     * information objects that it owns.
     */
    IMPORT_C virtual ~CLcLocationAppInfoArray();
    
    /**
     * Returns the number of Location Application information objects in the
     * array.
     *
     * @since S60 v5.0     
     * @return The number of objects.      
     */               
    IMPORT_C TInt   Count() const;
    
    /**
     * Appends a Location Application information object to the array.
     * The ownership of the inserted object is transferred.
     *
     * @since S60 v5.0      
     * @param[in] aLCAppInfo Location Application information object which is
     *                       to be inserted.
     * @leave System wide error code if the operation fails.      
     */      
    IMPORT_C void   AppendL( CLcLocationAppInfo*     aLCAppInfo );
   
    /**
     * Removes a Location Application information object corresponding to the
     * specified array position from the array.
     * The function does not delete the object which is removed but transfers
     * it back to the caller. The ownership of the returned object is also
     * transferred to the caller.
     *
     * @since S60 v5.0      
     * @param[in] aIndex Index of the requested object in the array. The 
     *                   position is relative to zero, i.e. zero implies
     *                   the object pointer at the beginning of the array.
     * @return Pointer to the requested object.
     * @panic @ref TLCPanicCodes::ELcArrayOutofBounds, if there is no object
     *             corresponding to the index.          
     */
    IMPORT_C CLcLocationAppInfo* Remove( TInt    aIndex );
    
    /**
     * Resets the Location Application Info Array. It also deletes all the
     * Location Application Information objects present in the array.
     *
     * @since S60 v5.0
     */
    IMPORT_C void Reset();
            
    /**
     * Returns a Location Application information object corresponding to the
     * specified array position.
     * Only a reference to the internal object is returned, the ownership is 
     * not transferred.
     *
     * @since S60 v5.0      
     * @param[in] aIndex Index of the requested object in the array. The 
     *                   position is relative to zero, i.e. zero implies
     *                   the object pointer at the beginning of the array.
     * @return Reference to the requested object.
     * @panic @ref TLcPanicCodes::ELcArrayOutofBounds, if there is no object
     *             corresponding to the index.      
     */     
    IMPORT_C CLcLocationAppInfo& operator[]( TInt aIndex ) const;
                    
private:
    /**
     * Default C++ Constructor.
     */
    CLcLocationAppInfoArray();

     /**
     * Overloaded Copy Constructor. By default, prohibit copy constructor
     *
     * @param[in] aLCAppInfoArray Location Application information array from which
     * the new object should be constructred.
     */
    CLcLocationAppInfoArray( 
                const CLcLocationAppInfoArray&  aLCAppInfoArray );
    
    /**
     * Overloaded Assignment operator. By default, prohibit assignment
     * operator
     *
     * @param[in] aLCAppInfoArray Location Application information array from which 
     * the new value needs to be assigned.
     * @return The Location Application information array after
     * assigment
     */
    CLcLocationAppInfoArray& operator=( 
                const CLcLocationAppInfoArray&  aLCAppInfoArray );
                                       
    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // Data   
    /**
     * Array of Location Centre application information objects.
     *
     * Owns
     */
    RPointerArray<CLcLocationAppInfo>     iAppInfoArray;
    
    /**
     * Reserved for future use.
     */           
    TAny*                               iReserved;
    };

#endif // C_LCLOCATIONAPPINFO_H
