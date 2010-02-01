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
class CPosLandmark;

class MEvtMgmtUiMapAndNavigationObserver
    {
public:
    /**
     * It will be used to notify observer that map operation
     * has been completed.
     * @param aPlace place retrieved from map
     * @param aErrorCode It will contain the status of the operation
     *                KErrNone if successful otherwise error status code          
     */
    virtual void HandleMapAndNavigationResponseL(TInt aErrorCode, const TPtrC aPlace, TCoordinate aTriggerLocation )=0;  
    };



/**
* The class CEvtMgmtUiMapNavigationAdapter contains the implemenation 
* for all the uses cases of Map and Navigation.
* Functionality:
*   To check map service providers
*   To show location on map
*   To navigate to the location
*   To retrieve location from map
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
		
		/**
        * Symbian 1st phase static constructor. 
        * Leaves the constructed object on Cleanup Stack.     
        */        
        static CEvtMgmtUiMapNavigationAdapter* NewLC();
        /**
        * Destructor.
        */
        ~CEvtMgmtUiMapNavigationAdapter();		
    
    public: // New methods     	
    
    	/**
    	* This method is called for the get landmarks from map use case.
    	* 
    	* @since S60 v9.1
    	*/
    	void PlaceFromMapL();
    	
    	/**
    	 * It will return place value recieved from map.
    	 * 
    	 * @since S60 v9.1
    	 * 
    	 * @ret selected place
    	 */
    	TPtrC Place() const;
    	
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
    	void ShowOnMapL();
    	
    	/**
         * It will navigate to location
         * 
         * @since S60 v9.1
         */
    	void NavigateToPlaceL();
    	
    	/**
         * It will set observer to notify completion of map operation
         * 
         * @since S60 v9.1
         * 
         *  @param [in] aObserver - observer to notify completion of map operation
         */
    	void SetObserver( MEvtMgmtUiMapAndNavigationObserver* aObserver );
    	
    	/**
         * It will set place
         * 
         * @since S60 v9.1
         * 
         * @param [in] aPlace - place of existing event
         *                    - empty for newly created event
         */
    	void SetPlaceL( const TPtrC& aPlace );
    	
    	/**
         * It will set location
         *  
         * @since S60 v9.1
         * 
         * param[in] aTriggerLocation - location of trigger of existing event
         *                            - NaN for newly created event
         */
    	void SetLocation( TCoordinate  aTriggerLocation );
    	
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
    	
    private: // Private functions
        
        /**
         * It will set place details from landmark recieved from map
         *  
         * @since S60 v9.1
         * 
         * @param[in] aLandmark - landmark selected on map      
         */
        void SetPlaceFromLandmarkL( CPosLandmark& aLandmark );
    	
    private: // Data    	
    	// Owns : Pointer to AIW service handler to send service commands     	
    	CAiwServiceHandler*     iAiwServiceHandler;
    	
    	// Owns : Generic parameter input list     
    	CAiwGenericParamList*   iInList;
    	
    	// Owns : Generic parameter output list    	     	
        CAiwGenericParamList*   iOutList;    
        
        // Owns : place details     
        HBufC*                  iPlace;       
        
        // Does not own : observer to notify completion of map operation            
        MEvtMgmtUiMapAndNavigationObserver* iObserver;
        
        // trigger location of the event
        TCoordinate            iTriggerLocation ;        
	};

#endif //CEVTMGMTUIMAPNAVIGATIONADAPTER_H

// End of file

