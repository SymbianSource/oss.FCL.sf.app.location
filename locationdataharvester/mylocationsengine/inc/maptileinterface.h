/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Maptile interface header implementation.
*
*/

#ifndef __MAPTILEINTERFACE_H__
#define __MAPTIlEINTERFACE_H__

#include <e32std.h> 
#include <e32base.h>
// Header for landmarks db
#include<EPos_CPosLandmarkDatabase.h>
#include <maptilegeocoderplugin.h>


 
/**
 * MMapTileObserver class.
 * Observer class for notifying the maptile related events 
 *
 */
class MMapTileObserver
    {
public:   
    
   /** Informs once maptile is retrived from the REST server
    * 
    * @param[in] aErrCode Error code
    * @param[in] aMapTilePath Path where the maptile image staroed
    * 
    */    
    virtual void MapTilefetchingCompleted( TInt aErrCode,const TDesC& aMapTilePath ) = 0;
    
    /** Informs once geocoding completed 
     * 
     * @param[in] aLatitude converted latitude value
     * @param[in] aLongitude converted longitude value
     * 
     */       
    virtual void RestGeoCodeCompleted( TReal aLatitude,TReal aLongitude ) = 0;
    
    };



/** 
 * Class used for interacting with maptile and geocoder components .
 * This class raises the request for geocoding service and maptile
 * fetching service.
 */
class CMapTileInterface : public CBase, MMaptileGeocoderObserver
{
public:

   /**
    * Factory method
    */
    static CMapTileInterface* NewL(); 
    
    /**
     * Interface for requesting maptile image for a landmark object
     * 
     * @param[in] aLandmark Landmark containing address details
     * @param[in] aFilePath Path where the maptile image to be stored
     * @param[in] aObserver Observer for callback notification
     */
    void GetMapTileImageL(  CPosLandmark*  aLandmark , 
            const TDesC& aFilePath,   MMapTileObserver* aObserver  );
    
    /**
     * Interface for requesting maptile image for a landmark object
     * 
     * @param[in] aAddressDetails containing address details
     * @param[in] aFilePath Path where the maptile image to be stored
     * @param[in] aObserver Observer for callback notification
     */
    void GetMapTileImageL(const TDesC& aAddressDetails,
            const TDesC& aFilePath, MMapTileObserver* aObserver);
    
    /**
     * Interface for requesting  for a landmark object.
     * @return CPosLandmark object.
     */
    CPosLandmark* GetLandMarkDetails();
        
    
    /**
     * Constructor
     */
    CMapTileInterface();
    
    /**
     * Destructor
     */
    ~CMapTileInterface();

private:

    /**
     * Function for Retrieving latitude & longitude information
     * 
     * @param[in] aLatitude latitude value
     * @param[in] aLongitude Longitude value
     */
	void GetMapTileL( TReal aLatitude, TReal aLongitude	);
	
    /**
     * Interface for requesting  for a landmark object.
     * @param[in] aAddressInfo , geo-code address details
     */
    void SetLandMarkDetailsL(MAddressInfo& aAddressInfo);
    
protected: 
	
    /**
     * Derived from MMaptileGeocoderObserver
     */
    void GeocodingCompleted( TInt aErrorcode, MAddressInfo& aAddressInfo );
    
    /**
     * Derived from MMaptileGeocoderObserver
     */
    void MapTileFetchingCompleted( TInt aErrorcode, const TDesC& aMapImagePath );
    
private:
    
    /**
     * Two phase construction
     */
    void ConstructL();
    
private:
	
    //MaptileGeocode plugin interface instance
    CMaptileGeocoderPlugin* iMaptileGeocoder;
    //Flag to check street availability
    TBool iStreetAvailable;
    //Maptile image path
	HBufC* iFilePath;
	//Maptile observer
	MMapTileObserver* iObserver ;
	//pointer to landmark  from address information.
	CPosLandmark  *iLandmark;

    };
#endif // MAPTILEINTERFACE_H

