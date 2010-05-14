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
* Description: Maptile interface source implementation for maptile
*              service.
*
*/

#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmItemIterator.h>
#include <EPos_CPosLmOperation.h>
#include <f32file.h>
#include <bautils.h>
#include "maptileinterface.h"
#include "mylocationlogger.h"

//Invalid latitude & longitude value
const TReal KInvalidLatitudeLongitude = 200.00;
const TInt KStreetLvelZoom = 14;
const TInt KCityLevelZoom = 7;
const TInt KMapTileSize= 256;

// -----------------------------------------------------------------------------
// CMapTileInterface::NewL()
// Creates and returns the instance of CMapTileInterface
// -----------------------------------------------------------------------------
//
CMapTileInterface* CMapTileInterface::NewL()
{
    CMapTileInterface* self = new (ELeave) CMapTileInterface();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CMapTileInterface::~CMapTileInterface()
// Destructor
// -----------------------------------------------------------------------------
//
CMapTileInterface::~CMapTileInterface()
{
    __TRACE_CALLSTACK;
   
    delete iFilePath;
    
    delete iMaptileGeocoder;
    
    delete iLandmark;
}    


// -----------------------------------------------------------------------------
// CMapTileInterface::CMapTileInterface()
// Constructor
// -----------------------------------------------------------------------------
//
CMapTileInterface::CMapTileInterface() :
           iMaptileGeocoder( NULL ), 
           iFilePath( NULL ),
           iObserver(NULL),
           iLandmark(NULL)
{
}

// -----------------------------------------------------------------------------
// CMapTileInterface::ConstructL()
// Two phase constructor
// -----------------------------------------------------------------------------
//
void CMapTileInterface::ConstructL()
{
    __TRACE_CALLSTACK;

    iMaptileGeocoder = CMaptileGeocoderPlugin::NewL();
    
    iMaptileGeocoder->SetMaptileGeocoderObserver( *this );
}
// -----------------------------------------------------------------------------
// CMapTileInterface::GetMapTileImageL()
// Interface for requesting maptile image for a landmark object
// -----------------------------------------------------------------------------
//
void CMapTileInterface::GetMapTileImageL(const TDesC& aAddressDetails,
        const TDesC& aFilePath, MMapTileObserver* aObserver)
{
    __TRACE_CALLSTACK;
    iObserver = aObserver;
    delete iFilePath;
    iFilePath = NULL;    
    iFilePath = HBufC::NewL(aFilePath.Length());
    iFilePath->Des().Copy(aFilePath);
    iStreetAvailable = EFalse;
    TConnectionOption conn = ESilent;
    iMaptileGeocoder->GetCoordinateByAddressL(aAddressDetails, conn);
}
// -----------------------------------------------------------------------------
// CMapTileInterface::GetMapTileImageL()
// Interface for requesting maptile image for a landmark object
// -----------------------------------------------------------------------------
//
void CMapTileInterface::GetMapTileImageL(CPosLandmark* aLandmark,
        const TDesC& aFilePath, MMapTileObserver* aObserver)
{
    __TRACE_CALLSTACK;
    iObserver = aObserver;
    delete iFilePath;
    iFilePath = NULL;    
    iFilePath = HBufC::NewL(aFilePath.Length());
    iFilePath->Des().Copy(aFilePath);

    TPtrC getStreet;
    aLandmark->GetPositionField(EPositionFieldStreet, getStreet);

    if (getStreet.Length() > 0)
    {
        iStreetAvailable = ETrue;
    }
    TConnectionOption conn = ESilent;
    iMaptileGeocoder->GetCoordinateByAddressL(*aLandmark, conn);
}

// -----------------------------------------------------------------------------
// CMapTileInterface::GetMapTileImageL()
// Function for Retrieving latitude & longitude information
// -----------------------------------------------------------------------------
//
void CMapTileInterface::GetMapTileL(TReal aLatitude, TReal aLongitude)
{
    __TRACE_CALLSTACK;//Notification to observer , to update contact db,
    iObserver->RestGeoCodeCompleted(aLatitude, aLongitude);
    TInt zoom = KCityLevelZoom;
    TInt size = KMapTileSize;
    if (iStreetAvailable)
    {
        zoom = KStreetLvelZoom;
    }
    iStreetAvailable = EFalse;
    TMapTileParam mapTileparam(aLatitude, aLongitude, zoom, size);
    iMaptileGeocoder->GetMapTileByGeoCodeL( mapTileparam, *iFilePath );
}

// -----------------------------------------------------------------------------
// CMapTileInterface::MapComplete()
// Maptile retrievel notification
// -----------------------------------------------------------------------------
//
void CMapTileInterface::MapTileFetchingCompleted( TInt aErrCode, const TDesC& aMapTilePath )
{
    __TRACE_CALLSTACK;
    MYLOCLOGSTRING1("MapComplete() status-%d ",aErrCode );
    iObserver->MapTilefetchingCompleted( aErrCode, aMapTilePath );

}

// -----------------------------------------------------------------------------
// CMapTileInterface::GeocodeComplete()
// Geocode completion  notification
// -----------------------------------------------------------------------------
//
void CMapTileInterface::GeocodingCompleted(TInt aErrorcode,
        MAddressInfo& aAddressInfo)
{
    __TRACE_CALLSTACK;
    TReal latitude, longitude;
    latitude=aAddressInfo.GetLatitude();
    longitude=aAddressInfo.GetLongitude();
    
    MYLOCLOGSTRING1("GeocodeCompleted() status-%d",aErrorcode);
    MYLOCLOGSTRING1("GeocodeCompleted() latitude-%f",latitude );
    MYLOCLOGSTRING1("GeocodeCompleted() longitude-%f",longitude );
	
    if (aErrorcode == KErrNone)
    {
        if ( latitude != KInvalidLatitudeLongitude
                && longitude != KInvalidLatitudeLongitude)
        {           
            TRAP_IGNORE( SetLandMarkDetailsL(aAddressInfo) );            
            TRAPD( error, GetMapTileL(latitude, longitude) );
            if ( error != KErrNone )
            {
                //Log error message
				 MYLOCLOGSTRING1("GetMapTileL() status-%d",error);
				 iObserver->MapTilefetchingCompleted(error, KNullDesC);
            }
        }
        else
        {
            iObserver->MapTilefetchingCompleted(KErrGeneral, KNullDesC);
        }
    }
    else
    {
        iObserver->MapTilefetchingCompleted(aErrorcode, KNullDesC);

    }
}

// -----------------------------------------------------------------------------
// CMapTileInterface::GetLandMarkDetails()
// return pointer to CPosLandmark 
// -----------------------------------------------------------------------------
//
CPosLandmark* CMapTileInterface::GetLandMarkDetails()
{
    return iLandmark;  
               
}
// -----------------------------------------------------------------------------
// CMapTileInterface::SetLandMarkDetails()
// create CPosLandmark details from MAddressInfo 
// -----------------------------------------------------------------------------
//
void CMapTileInterface::SetLandMarkDetailsL(MAddressInfo& aAddressInfo)
{
    if(iLandmark)
    {
        delete iLandmark;
        iLandmark=NULL;
    }
    TReal latitude,longitude;
    latitude=aAddressInfo.GetLatitude();
    longitude=aAddressInfo.GetLongitude();    
    TLocality position(TCoordinate(latitude,longitude),0);
    iLandmark=CPosLandmark::NewL();
    //latitude and longitude
    iLandmark->SetPositionL(position);
    
    //street
    TPtrC tempText=aAddressInfo.GetThoroughfareName();
    if(tempText.Length()>0)
    {
        iStreetAvailable=ETrue;
    }
    iLandmark->SetPositionFieldL(EPositionFieldStreet, tempText);
    //postal code
    tempText.Set(aAddressInfo.GetPincode());
    iLandmark->SetPositionFieldL(EPositionFieldPostalCode, tempText);
    //city
    tempText.Set(aAddressInfo.GetCity());
    iLandmark->SetPositionFieldL(EPositionFieldCity, tempText);
    //state
    tempText.Set(aAddressInfo.GetState());
    iLandmark->SetPositionFieldL(EPositionFieldState, tempText);
    //country
    tempText.Set(aAddressInfo.GetCountryName());
    iLandmark->SetPositionFieldL(EPositionFieldCountry, tempText); 
    
}
//end of line
