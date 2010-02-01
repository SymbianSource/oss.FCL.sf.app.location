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
* Description:  Parameters for the IPC message handling between the 
*                Location Centre API and Location Centre Server.
*
*/


#ifndef LCIPCPARAMS_H
#define LCIPCPARAMS_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <lclocationappfilter.h>
#include <lclocationappinfo.h>

// USER INCLUDES

// FORWARD DECLARATIONS
class CLcAppInfo;

// ENUMERATION DECLARATIONS
/**
 * Enumeration defining the kind of application that this structure holds.
 */
enum TLcAppType
    {
    /**
     * The structure holds an S60 Java based Application.
     */
    ELcNativeApplication,
    
    /**
     * The structure holds a Web URL.
     */
    ELcWebUrl,
    
    /** 
     * The structure holds a document.
     */
    ELcDocument
    };

// CLASS DECLARATIONS
/**
 * Array of identifiers which is passed to Location Centre server for
 * filtering.
 *
 * Derived from RPointerArray. So all functions of the Arrar are available
 * through this interface. 
 */
class RLcIpcAppIdArray : public RPointerArray< HBufC >
    {
public:    
    /**
     * Externalizes the contents of this Array into a buffer.
     *
     * @since S60 v5.0
     * @param aWriteStream  Buffer onto which the contents of this array
     *                      would be externalized.
     */
    IMPORT_C void ExternalizeL( RWriteStream&   aWriteStream );
   
    /**
     * Internalized the contents of the buffer into this array.
     * 
     * @since S60 v5.0
     * @param aReadStream   Buffer from which the contents of this
     *                      array would be populated.
     */
    IMPORT_C void InternalizeL( RReadStream&    aReadStream );
    
    /**
     * Returns the Length of the buffer needed to pack this array of
     * Location based Application's identifiers.
     *
     * @since S60 v5.0
     * @return TInt, Length of the Buffer which is needed to pack this
     *               array.
     */
    IMPORT_C TInt BufferLength();
        
    /**
     * Resets and destroys the Application identifer array.
     * There is no need to call close on the array after this.
     *
     * @param aIdArray Generic pointer to the array
     */
    IMPORT_C static void ResetAndDestroyIdArray( TAny* aIdArray );
        
    };
    
/**
 * Application information array.
 *
 * Derived from RPointerArray. So all functions of the Arrar are available
 * through this interface.
 */  
class RLcIpcAppInfoArray : public RPointerArray< CLcAppInfo >
    { 
public:
    /**
     * Externalizes the contents of this Array into a buffer.
     *
     * @since S60 v5.0
     * @param aWriteStream  Buffer onto which the contents of this array
     *                      would be externalized.
     */
    IMPORT_C void ExternalizeL( RWriteStream&   aWriteStream );
   
    /**
     * Internalized the contents of the buffer into this array.
     * 
     * @since S60 v5.0
     * @param aReadStream   Buffer from which the contents of this
     *                      array would be populated.
     */
    IMPORT_C void InternalizeL( RReadStream&    aReadStream );
    
    /**
     * Returns the Length of the buffer needed to pack this array of
     * Location based Applications.
     *
     * @since S60 v5.0
     * @return TInt, Length of the Buffer which is needed to pack this
     *               array.
     */
    IMPORT_C TInt BufferLength();
        
    /**
     * Resets and destroys the Application information array.
     * There is no need to call close on the array after this.
     *
     * @param aIdArray Generic pointer to the array
     */
    IMPORT_C static void ResetAndDestroyAppArray( TAny* aIdArray );        
    };

/**
 * Basic Information for a Location based Applicatiuon. This class provides sufficient
 * information for launching a Location based Application
 */
class CLcBasicAppInfo : public CBase
    {
public:
    /**
     * Static two phase constructor
     */
    inline static CLcBasicAppInfo* NewL();
    
    /**
     * Static two phased constructor. Leaves the object on the Cleanup stack.
     */
    inline static CLcBasicAppInfo* NewLC();
    
    /**
     * C++ Destructor
     */
    inline virtual ~CLcBasicAppInfo();
    
    /**
     * Retrieves the type of application stored in this structure.
     *
     * @since S60 v5.0     
     * @return, The Type of Application as defined by @p TLcAppType which is stored
     *          in this structure.
     */
    inline TUint32 ApplicationType() const;
    
    /**
     * Sets the Application type.
     * @since S60 v5.0     
     * @param aAppType The Type of Application as defined by @p TLcAppType which
     *                  is stored in this structure.
     */
    inline void SetApplicationType( TUint32      aAppType );
    
     /**
     * Mode in which the Location Application would be launched. 
     * This is the mode with which the application has registered with
     * Location Centre.
     *
     * @since S60 v5.0     
     * @return Mode in which the Location Application would be launched.       
     */
    inline TUint32 LaunchMode() const;

    /** 
     * Sets the Mode in which the Location Application would be launched.
     *
     * @since S60 v5.0     
     * @param[in] aLaunchMode Mode in which the Location Application would be
     *                        launched. 
     */
    inline void SetLaunchMode( TUint32  aLaunchMode );
        
    /**
     * Retrieves the Generic Application data stored in this structure.
     *
     * @since S60 v5.0     
     * @return, The Genereic data content for the Application.
     */    
    inline TPtrC ApplicationData() const;
    
    /**
     * Sets the Generic Application data.
     *
     * @since S60 v5.0     
     * @param aAppData The Genereic data content for the Application.
     */       
    inline void SetApplicationDataL( const TDesC&   aAppData );
    
    /**
     * Retrieves the command line parameters stored in this structure.
     *
     * @since S60 v5.0     
     * @return, The command line parameters for the Application.
     */      
    inline TPtrC CommandLineParamters() const;
    
    /**
     * Sets the Command line parameters.
     *
     * @since S60 v5.0     
     * @param aCmdLineParams The Command line parameters for the Application.
     */     
    inline void SetCommandLineParametersL( const TDesC&  aCmdLineParams );
        
    /**
     * Externalizes the contents of this Array into a buffer.
     *
     * @since S60 v5.0
     * @param aWriteStream  Buffer onto which the contents of this array
     *                      would be externalized.
     */
    IMPORT_C void ExternalizeL( RWriteStream&   aWriteStream );
   
    /**
     * Internalized the contents of the buffer into this array.
     * 
     * @since S60 v5.0
     * @param aReadStream   Buffer from which the contents of this
     *                      array would be populated.
     */
    IMPORT_C void InternalizeL( RReadStream&    aReadStream );
    
    /**
     * Returns the Length of the buffer needed to pack this structure.
     *
     * @since S60 v5.0
     * @return TInt, Length of the Buffer which is needed to pack this
     *               structure.
     */
    IMPORT_C TInt BufferLength();  
        
protected:
    /**
     * Default C++ constructor.
     */
    CLcBasicAppInfo();
     
protected:
    /**
     * Type of the application that is present in this structure.
     */
    TUint32                             iAppType;

    /**
     * Mode in which the Location Application would be launched. If the mode
     * defined is EDefaultMode, then the default mode specified by the 
     * application when registering with Location Centre is used.
     */
    TUint32                             iLaunchMode; 
    
    /**
     * Generic data content for all Location based Application. The nature of this
     * data-type depends on the kind of Application as specified by the earlier parameter.
     * If the iAppType is
     * ELcApplication - This string contains the SID of the application.
     * ELcWebUrl      - The string contains the URI of the page that needs to be launched.
     * ELcDocument    - The string contains the full path of the file which needs to be
     *                  opened.
     */
    HBufC*                              iAppData;

    /**
     * Command line parameters.
     */
    HBufC*                              iCmdLineParams;
    };
        
/**
 * Information of a Location based Application currently registered with
 * Location Centre.
 */
class CLcAppInfo : public CLcBasicAppInfo
    {
public:
    /**
     * Enumeration defining the Icon type present in this App Info
     */
    enum TLcIconFileType
        {
        EIconfromApp,
        EMifFile
        };
        
public:
    /**
     * Constructs a new instance of Location Application information object.
     *
     * @return The new instance of Location Application information object.
     */
    inline static CLcAppInfo* NewL();
 
     /**
     * Constructs a new instance of Location Application information object.
     *
     * @return The new instance of Location Application information object.
     */
    inline static CLcAppInfo* NewLC();
             
    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Application
     * information object.
     */
    inline virtual ~CLcAppInfo();

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
    inline TPtrC Id() const;
    
    /**
     * Set the Unique identifier identifying the Location Application. The
     * identifer is copied.
     *
     * @since S60 v5.0     
     * @param[in] aId Unique identifier identifying the Location Application.      
     */
    inline void SetIdL( const TDesC& aId );
                
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
    inline TPtrC Name() const;

    /**   
     * Set the Logical name for the Location Application. The name is 
     * copied.
     *
     * @since S60 v5.0     
     * @param[in] aName Logical name for the Location Application. 
     *                  The logical string is copied.
     */
    inline void SetNameL( const TDesC& aName );
    
    /**
     * Gets the Icon file name for the Location based Application.
     *
     * @since S60 v5.0     
     * @return The Icon file name for the Location based Application.
     */
    inline TPtrC IconFile() const;

    /**   
     * Sets the name of the Icon file for this Location based Application.
     *
     * @since S60 v5.0     
     * @param[in] aIconFile Full path for the Icon file.
     */
    inline void SetIconFileL( const TDesC& aIconFile );
        
    /**
     * Gets the Icon File type for the Location Application.
     * The Location Application can optionally have an icon defined
     * in the Registration file.
     *
     * @since S60 v5.0     
     * @return The Icon file type for the Location Application.
     */
    inline  TLcIconFileType IconFileType() const;

    /**   
     * Sets the Icon File type for the Location Application.
     * The Location Application can optionally have an icon defined
     * in the Registration file.
     *
     * @since S60 v5.0     
     * @param[in] aIconFileType The type of Icon file for this Location
     *                          Application
     */
    inline void SetIconFileType( const TLcIconFileType aIconFileType );
    
 
    /**
     * Gets the frame number for the Icon if the Icon file is a MIF file.
     *
     * @since S60 v5.0     
     * @return Frame Number      
     */
    inline TUint32 FrameNo() const;

    /** 
     * Sets the frame number for the Icon if the Icon file is a MIF file.
     *
     * @since S60 v5.0     
     * @param[in] aFrameNo Frame number for the icon.
     */
    inline void SetFrameNo( TUint32  aFrameNo );
    
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
    inline TUint32 SystemCharacteristics() const;

    /**
     * Sets the system characteristics for the Location Application.
     *
     * @since S60 v5.0     
     * @param[in] aSysCharacteristics System Characteristics for the Location
     *                                Application.
     */
    inline void SetSystemCharacteristics( TUint32   aSysCharacteristics );
    
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
    inline TUint32 ApplicationCharacteristics() const;

    /**    
     * Sets the Application characteristics for the Location Application.
     *
     * @since S60 v5.0     
     * @param[in] aAppCharacteristics Application Characteristics for the Location
     *                                Application.
     */
    inline void SetApplicationCharacteristics( TUint32   aAppCharacteristics );


    /**
     * Externalizes the contents of this Array into a buffer.
     *
     * @since S60 v5.0
     * @param aWriteStream  Buffer onto which the contents of this array
     *                      would be externalized.
     */
    void ExternalizeL( RWriteStream&   aWriteStream );
   
    /**
     * Internalized the contents of the buffer into this array.
     * 
     * @since S60 v5.0
     * @param aReadStream   Buffer from which the contents of this
     *                      array would be populated.
     */
    void InternalizeL( RReadStream&    aReadStream );
    
    /**
     * Returns the Length of the buffer needed to pack this structure.
     *
     * @since S60 v5.0
     * @return TInt, Length of the Buffer which is needed to pack this
     *               structure.
     */
    IMPORT_C TInt BufferLength();    
        
protected:
    /**
     * Default C++ Constructor.
     */
    inline CLcAppInfo();
    
protected:   
    /**
     * System wide unique identifier identifying the Location Application. This
     * identifier is defined by the Location Application on registration with
     * Location Centre.
     *
     * Owns
     */
    HBufC*                              iId;
    
    /**
     * Application Name.
     * Logical name for the Location Application. The name is defined by the
     * Location Centre on registration with Location Centre. This is the name
     * which would be displayed by Location Centre for this application.
     * 
     * Owns
     */
    HBufC*                              iApplicationName;
    
    /**
     * Icon File name.
     * The name of the Icon file if its specified in the Location Centre
     * Registration file for the Location Application.
     *
     * Owns
     */
    HBufC*                              iIconFile;
      
    /**
     * Icon file type.
     * The Location based Application can optionally specify an Icon file
     * which would be displayed for it in Location Centre.
     */
    TLcIconFileType                     iIconFileType;
    
    /**
     * Frame number incase the Icon file is a MIF file
     */
    TUint32                             iFrameNo;
     
   /**
    * Bit-mask containing the System characteristics for the corresponding
    * Location application
    */
    TUint32                             iSystemCharacteristics;
    
   /**
    * Bit-mask containing the Application characteristics for the corresponding
    * Location application
    */
    TUint32                             iAppCharacteristics; 
};
    
#include "lcipcparams.inl"
     
#endif // LCIPCPARAMS_H
