/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Application's adapter class to Map and Navigation use cases
*
*/


#ifndef CEVTMGMTUIMAPNAVIGATIONADAPTER_H
#define CEVTMGMTUIMAPNAVIGATIONADAPTER_H

// INCLUDES
#include <AiwCommon.h>
#include <e32std.h>

class CAiwServiceHandler;
class TCoordinate;

/**
* The class CEvtMgmtUiMapNavigationAdapter contains the implemenation 
* for all the uses cases of Map and Navigation.
* Functionality:
*   To check map service providers
*   To show location on map
*   To navigate to the location
*/

class CEvtMgmtUiMapNavigationAdapter : public CBase, 
                                       public MAiwNotifyCallback
	{

	public: // Constructors and destructor
		/**
        * Symbian 1st phase static constructor. 
        *       
        */        
        static CEvtMgmtUiMapNavigationAdapter* NewL();
         
        static CEvtMgmtUiMapNavigationAdapter* NewLC();
        /**
        * Destructor.
        */
        ~CEvtMgmtUiMapNavigationAdapter();		
    
    public: // New methods       	
        
    	/**
    	 * It will return total map service providers.
    	 * 
    	 * @since S60 v9.1
    	 * 
    	 * @ret total number of map service providers
    	 */
    	TInt MapServiceProvidersL();
    	
    	/**
         * It will show location on map
         * 
         * @since S60 v9.1
         */
    	void ShowOnMapL( const TCoordinate& aCoordinate );
    	
    	/**
         * It will navigate to location
         * 
         * @since S60 v9.1
         */
    	void NavigateToPlaceL( const TCoordinate& aCoordinate );
   
    public: // from MAiwNotifyCallback
           /**
           * The callback function for the get landmark from map use case
           * This function gets a callback once the user selects a location
           * from map to save as a landmark.
           */
           TInt HandleNotifyL( TInt aCmdId, 
                               TInt aEventId, 
                               CAiwGenericParamList &aEventParamList, 
                               const CAiwGenericParamList &aInParamList );
           	
    private:
    	/**
        * C++ default constructor.                 
        */
        CEvtMgmtUiMapNavigationAdapter(); 
        
        /**
        * Symbian 2nd phase constructor. 
        */
        void ConstructL();
    	    	
    private: // Data    	
    	// Owns : Pointer to AIW service handler to send service commands     	
    	CAiwServiceHandler*     iAiwServiceHandler;
    	
    	// Owns : Generic parameter input list     
    	CAiwGenericParamList*   iInList;
    	
    	// Owns : Generic parameter output list    	     	
        CAiwGenericParamList*   iOutList;      
	};

#endif //CEVTMGMTUIMAPNAVIGATIONADAPTER_H

// End of file

