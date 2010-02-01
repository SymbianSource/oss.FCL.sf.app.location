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
* Description:  Location Application Filters
*
*/

#ifndef T_LCLOCATIONAPPFILTER_H
#define T_LCLOCATIONAPPFILTER_H

/**
 *  @file lclocationappfilter.h
 * 
 *  This file provides the filters which can be used to customize the list
 *  of Location based Application which can be retrieved from Location Centre
 *  or which can be displayed in Location Centre.
 *
 *  @note The components which can register with Location Centre are of
 *  two types
 *  - Applications
 *  - Location based services / contents.
 *
 *  These Client Applications or Services/Contents which register with Location
 *  Centre will be called Location based Application henceforth.
 *
 *  @lib lcservice.lib
 *  @since S60 5.0
 */

// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  @class TLcLocationAppFilter
 *  Location Application Filter.
 *
 *  This class allows the client application to configure the list of Location
 *  based Applications that the client intends to obtain from Location Centre
 *  or the list of Location based Application that the client application wants
 *  Location Centre to show.
 *  The filtering is based on characteristics exhibited by the Location based
 *  Applications. These characteristics can be of two types
 *  - System Characteristics defined by 
 *  @ref CLcLocationAppInfo::TLcSystemCharacteristics and
 *  - Application Characteristics defined by 
 *  @ref CLcLocationAppInfo::TLcApplicationCharacteristics.
 *
 *  The default state of the filter class for various property is
 *  - System Characteristics is @ref TLcSystemCharacteristics::ESysCharNone
 *  - System Characteristic filter configuration is @ref EFilterStrict
 *  - Application Characteristic is @ref TLcApplicationCharacteristics::EAppCharNone
 *  - Application Characteristic filter configuration is @ref EFilterStrict
 *
 *  This class is expected to be is passed as an input argument to the
 *  @p GetLocationApplicationsL, @p GetLocationApplications and 
 *  @p LaunchLocationCentreL functions of @ref CLcService class.
 *  
 *  @see CLcService::GetLocationApplicationsL
 *  @see CLcService::GetLocationApplications
 *  @see CLcService::LaunchLocationCentreL
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
class TLcLocationAppFilter
    {
public:
     /**
      * @enum TLcFilterConfig
      * The mode in which the application filtering should be done. 
      * This enumeration is used by the client application to specify the way
      * in which filtering of applications should be done.
      */
     enum TLcFilterConfig
        {  
        /**
         * Location based Application that exhibit only the characteristics
         * specified by the client applications must be used. If the
         * System / Application characteristics stored in this object
         * contains a bitmask of more than one characteristic, then only
         * those Location based Application which satisfy all the mentioned
         * characteristics will be considered for filtering.
         * These characteristics are specified by the enumerations
         * @ref CLcLocationAppInfo::TLcSystemCharacteristics and
         * @ref CLcLocationAppInfo::TLcApplicationCharacteristics
         */
        EFilterStrict                            = 0,
        
        /**
         * Applications that exhibit only the characteristics specified by 
         * the client application alone or other characteristics in-addition
         * to the specified ones will be used for processing. If the
         * System / Application characteristics stored in this object
         * contains a bitmask of more than one characteristic, then
         * only those Location based Applications which satisfy all the
         * mentioned characteristics in addtion to optionally satisfying
         * other characteristics will be considered for filetering.
         * These characteristics are specified by the enumerations
         * @ref CLcLocationAppInfo::TLcSystemCharacteristics and
         * @ref CLcLocationAppInfo::TLcApplicationCharacteristics          
         */
        EFilterInclusive                         = 1,
        
         /**
         * Location based Application that exhibit characteristics other than
         * those specified by the client application must be used. If the
         * System / Application characteristics stored in this object
         * contains a bitmask of more than one characteristic, then only 
         *  Location based Application which do not satisfy any of the 
         * mentioned characteristics will be considered for filtering.          
         * These characteristics are specified by the enumerations
         * @ref CLcLocationAppInfo::TLcSystemCharacteristics and
         * @ref CLcLocationAppInfo::TLcApplicationCharacteristics          
         */
        EFilterExclusive                         = 2           
        };

public: // Exported Member Functions
    /**
     * Default Constructor
     */
    IMPORT_C TLcLocationAppFilter();
    
    /**
     * Accessor function which returns the currently configured System 
     * characteristics which will be used for filtering. All the system
     * characteristics would be combined to form an integer bit-mask and
     * this function retrieves the current bit-mask. The individual system
     * characteristics are defined by 
     * @ref CLcLocationAppInfo::TLcSystemCharacteristics enumeration.
     *
     * @note If the current configuration of the system characteristics is
     * the default value @ref CLcLocationAppInfo::ESysCharNone, 
     * then it should not be treated as a bit-mask but as an independent
     * value alone.
     *         
     * @since S60 5.0
     * @return Bit-mask holding the currently enabled System characteristics.
     */
    IMPORT_C TUint32 SystemCharacteristics() const;
    
    /**
     * Sets the requested system characteristic to the filter.
     *
     * All the system characteristics which the Client application intends
     * to be used for filtering must be combined to form a bit-mask
     * and this value is set to the filter.
     * The individual system characteristics are defined by 
     * @ref CLcLocationAppInfo::TLcSystemCharacteristics enumeration.
     *
     * @since S60 5.0     
     * @param[in] aSystemChar System Characteristic bit-mask value.
     * @return KErrArgument If the value of the input parameter is          
     *                      if its not a valid mask.
     */
    IMPORT_C TInt SetSystemCharacteristic( TUint32     aSystemChar );
    
    /**
     * Retrieves the current value of the System characteristics filter option.
     * The value of the filter is defined by the @ref TLcFilterConfig 
     * enumeration.
     *
     * @since S60 5.0      
     * @return Current System characteristics filter configuration.        
     */
    IMPORT_C TLcFilterConfig SysCharFilterConfiguration() const;
        
    /**
     * Sets the filter configuration option for system characteristics.
     * Location centre can be configured to treat the System
     * characteristics value based upon this flag.
     *
     * @since S60 5.0     
     * @param[in] aFilterConfig The mode in which the System characteristics 
     *                          value must be treated. The values which this
     *                          variable can assume is defined by the 
     *                          @ref TLcFilterConfig enumeration.  
     */
    IMPORT_C void SetSysCharFilterConfiguration( 
                            TLcFilterConfig   aFilterConfig );
    
    
    /**
     * Accessor function which returns the currently configured Application 
     * characteristics which will be used for filtering. All the application
     * characteristics would be combined to form an integer bit-mask and
     * this function retrieves the current bit-mask. The individual application
     * characteristics are defined by 
     * @ref CLcLocationAppInfo::TLcApplicationCharacteristics enumeration.
     *
     * @note If the current configuration of the application characteristics
     * is the default value @ref EAppCharNone, then it should not be
     * treated as a bit-mask but as an independent value alone.
     *
     * @since S60 5.0     
     * @return Bit-mask holding the currently enabled System 
     *         characteristics.
     */        
    IMPORT_C TUint32 ApplicationCharacteristics() const;
    
    /**
     * Sets the requested Application characteristic to the filter.
     *
     * All the Application characteristics which the Client application
     * intends to be used for filtering must be combined to form a bit-mask
     * and this value is set to the filter.
     * The individual system characteristics are defined by 
     * @ref CLcLocationAppInfo::TLcApplicationCharacteristics enumeration.
     *
     * @since S60 5.0     
     * @param[in] aAppChar Application Characteristic bit-mask value.
     * @return KErrArgument If the value of the input parameter is          
     *                      if its not a valid mask.
     */
    IMPORT_C TInt SetApplicationCharacteristics( TUint32     aAppChar );

    /**
     * Retrieves the current value of the Application characteristics
     * filter option. The value of the filter is defined by the 
     * @ref TLcFilterConfig enumeration.
     * 
     * @return Current Application characteristics filter configuration.        
     */        
    IMPORT_C TLcFilterConfig AppCharFilterConfiguration() const;
        
    /**
     * Sets the filter configuration option for application characteristics. 
     * Location centre can be configured to treat the Application
     * characteristics value based upon this flag.
     *
     * @since S60 5.0       
     * @param[in] aFilterConfig The mode in which the Application
     *                          characteristics value must be treated. The
     *                          values which this variable can assume is
     *                          defined by the @ref TLcFilterConfig
     *                          enumeration.
     */        
    IMPORT_C void SetAppCharFilterConfiguration( 
                        TLcFilterConfig     aFilterConfig );
            
private: // Data
    /**
     * System characteristics which must be used for Filtering Location
     * applications. This variable contains a Bitmask of the enumeration
     * CLcLocationAppInfo::TLcSystemCharacteristics.
     * If the Client application decides to set the default value or 
     * doesn't explicitly initialize this variable then the value used
     * would be ESysCharNone.
     */    
    TUint32                     iSystemChar;
    
    /**
     * Filtering option for System Characteristics.
     * If the client application decides to use the default value for this
     * filter then then value used would be EFilterStrict.
     */
    TLcFilterConfig             iSysFilterConfig;
    
    /**
     * Application characteristics which must be used for Filtering Location
     * applications. This variable contains a Bitmask of the enumeration
     * CLcLocationAppInfo::TLcApplicationCharacteristics.
     * If the Client application decides to set the default value or 
     * doesn't explicitly initialize this variable then the value used
     * would be EAppCharNone.
     */         
    TUint32                     iApplicationChar;
    
    /**
     * Filtering option for Application Characteristics.
     * If the client application decides to use the default value for this
     * filter then then value used would be EFilterStrict.
     */        
    TLcFilterConfig             iAppFilterConfig;
    
    /**
     * Reserved for future use.
     */
    TUint8                      iReserved[8];
    };

#endif // T_LCLOCATIONAPPFILTER_H
