/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   'Set Place' component for retrieving place.
*
*/


#ifndef EVTMGMTUIPLACECOMPONENT_H_
#define EVTMGMTUIPLACECOMPONENT_H_

#include <e32base.h>        // CBase

#include "evtmgmtuimapnavigationadapter.h"
#include "evtmgmtuilocationserviceadapter.h"
#include "evtmgmtuiwaitdlglauncher.h"
#include "evtmgmtuilocsettinglauncherobserver.h"

class TCoordinate;
class CEvtMgmtUiLocSettingsLauncher;
class CAknIconArray;

/**
 *  @class CEvtMgmtUiPlaceComponent
 *  Class that represents 'Set place' component of editor
 *
 *  This class acts as 'set place' component of editor.
 *  It  is used to retrieve location details for which event will be set.
 *  Location details can be retrieved from current location, map or landmark
 *
 *  @since S60 v9.1
 */

class CEvtMgmtUiPlaceComponent: public CBase,
                                public MEvtMgmtUiMapAndNavigationObserver,
                                public MEvtMgmtUiLocationServiceObserver,
                                public MEvtMgmtUiWaitDialogLauncherObserver,
                                public MEvtMgmtUiLocSettingsLauncherObserver
    {
private:
    /*
     *  'Set place' popup dialog options
     */
    enum TEvtMgmtUiSetPlace
       {
       /*
        * It indicates that 'Current location' is selected
        * from popup dialog
        */
       EEvtMgmtUiSetPlaceCurrentPlace,
       /*
        * It indicates that 'Landmark' is selected
        * from popup dialog
        */
       EEvtMgmtUiSetPlaceLandmark,
       /*
        * It indicates that 'Map' is selected
        * from popup dialog
        */
       EEvtMgmtUiSetPlaceFromMap,
       };
    
    public:  // Constructor and destructor  
      /**
       * Static Two phase constructor
       * @param[in] aPrevPlace - place of existing event
       *                         empty string for newly create event     
       * @param[in/out] aTriggerLocation - location of the existing event is passed.
       *                                   after successful completion of retrieval operation
       *                                   new location is set to it.    
       */
      static CEvtMgmtUiPlaceComponent* NewLC(const TPtrC aPrevPlace,  
              TCoordinate& aTriggerLocation );

      static CEvtMgmtUiPlaceComponent* NewL(const TPtrC aPrevPlace,  
                   TCoordinate& aTriggerLocation );

      /**
       * Virtual destructor
       */
      virtual ~CEvtMgmtUiPlaceComponent(); 
      
    public:// new functions
     /**
     * It is used as a wrapper class to trap leaving function DisplayPlacePopupDlgL()
     *
     * @since S60 v5.0
     * 
     * @param[in] aStatus - status which will contain success/failure state
     *                       based on retrieval of location is successful or failed
     */
     void DisplayPlacePopupDlg( TRequestStatus& aStatus );    
     
     /**
     * It is used to retrieve place details
     * 
     * @since S60 v5.0
     * 
     * @ret place details retrieved
     */
     TPtrC Place() const;
     
     /**
      * It is used to retrieve location details
      * 
      * @since S60 v5.0
      * 
      * @ret location details retrieved
      */
     TCoordinate& TriggerLocation() const;
  
     /**
      * It is used to launch position settings
      * 
      * @since S60 v5.0
      */
     void LaunchPositioningSettingsL();
           
    public: //Derived
       
       /**
         * Derived from MEvtMgmtUiMapAndNavigationObserver
         */
       void HandleMapAndNavigationResponseL( TInt aErrorCode, const TPtrC aPlace, TCoordinate aTriggerLocation  );
       
       /**
        * Derived from MEvtMgmtUiLocationServiceObserver
        */
       void HandleLocationServiceResponseL( const TInt aStatus );

       /**
        * Derived from MEvtMgmtUiLocationServiceObserver
        */      
       void HandleLocationServiceErrorL( TInt aErrorCode );
       
       /**
       * Derived from MEvtMgmtUiWaitDialogLauncherObserver
       */ 
       void HandleDialogDismissed( TInt aButtonId );
       
       /**
         * Derived from MEvtMgmtUiLocSettingsLauncherObserver
         */ 
       void NotifyLocSettingLaucherChangeL( TInt aErrorCode );

    private://new functions
        /*
         * It is used to display accuracy note
         */
        void DisplayAccuracyNoteL();
        /*
         * It is used to display accuracy note
         */
        void DisplayAccuracyNoteL(TPosition& aPos);
        /*
         * It is used to get the accuracy string
         */
        void GetAccuracyStringL( TReal& aAccuracy, HBufC*& aAccuracyString );
        /*
         * Gets the place details from retrieved position information.
         */
        TInt GetPlaceL();
        /*
         * Resolves the Error code
         */
        void ResolveErrorL( TInt aErrorCode );
       
    private:  // Constructor
        /**
         * C++ Default constructor
         */       
        CEvtMgmtUiPlaceComponent( TCoordinate& aTriggerLocation );
  
        /**
         * Second phase of the two phase constructor
         */ 
         void ConstructL( const TPtrC aPrevPlace );
         
    private: //new function
        
        /**
         * It is used to display 'set place' popup dialog and retrieve place 
         * using selected method.
         *
         * @since S60 v5.0
         * 
         * @ret it returns the option selected.
         *      it leaves with KErrCancel, if popup dialog is cancelled.
         *      it may leave with other errors, if retrieval of place fails.
         */
        TInt DisplayPlacePopupDlgL();
           
        /*
         * It is used to display error note.
         * 
         * @since S60 v5.0
         * 
         * @param[in] aResourceId - resource ID of the text to be displayed on error note.
         */
        void DisplayErrorNoteL(TInt aResourceId );
        
       
       /**
        * It retrieves place details from landmark     
        * 
        * @since S60 v5.0
        *       
        */
       void PlaceFromLandmarkL();
         
       /**
         * It returns total number of landmarks stored in landmark database.     
         * If there is any error in retrieving details, it will return zero landmark.
         * 
         * @since S60 v5.0
         * 
         * @ret total number of landmarks stored in landmark database
         */
       TInt LandmarksL();       

       /**
        * It displays message query if there is no positioning method enabled.     
        * 
        * @since S60 v5.0
        * 
        * @ret If pressed Ok, it will return true.
        *      If pressed Close, it will return false.
        */
       TBool DisplayNoPositioningMethodInfoNoteL( TInt aResourceId );
       
       /**
        * It initiates set place request from current location.
        * If there is no positioning method enabled, it will leave with KErrCancel.
        * Otherwise it will request current location and will launch busy dialog.     
        * 
        * @since S60 v5.0
        *        
        */
       void PlaceFromCurrentLocationL();
       
       /**
        * Update icons
        */
       void UpdateIconsL( CAknListQueryDialog& aDialog );    
        
       /**
        * Append icon to icon array. CAknIconArray owns icons so it is 
        * easier to it leave safely.
        * @param aIcons icon array
        * @param aIconFileWithPath icon file name with path
        * @param aSkinID skin id of the icon
        * @param aIconGraphicsIndex picture index
        * @param aIconGraphicsMaskIndex mask index
        */
       void AddIconL( CAknIconArray&       aIcons,
                      const TDesC&         aIconFileWithPath,
                      TAknsItemID          aSkinID,
                      TInt                 aIconGraphicsIndex,
                      TInt                 aIconGraphicsMaskIndex );
                            
       /**
        * Gets Events Management UI icon file path and name. Ownership
        * is transferred, item is left in the cleanup stack.
        * @return TFileName* The full name of the Events UI icon file.
        */        
       TFileName* IconFileNameLC() const;
      /**
       * It will create new trigger;    
       */
      //void CreateTriggerL();
    private:
        /**
         * Place value
         * owns
         */
        HBufC*                              iPlace;
        /*
         *  location details 
         */
        TCoordinate&                        iTriggerLocation;
       
        /*
         *  status of retrieval of place 
         *  Not own
         */
        TRequestStatus*                     iStatus;
        
        /*
         * object used to retrieve place details from map 
         * Owns
         */
        CEvtMgmtUiMapNavigationAdapter*     iMapAdapter;
        /*
         * object used to retrieve place details from current location
         * Owns
         */
        CEvtMgmtUiLocationServiceAdapter*   iLocationServiceAdapter;
        /**
          * Wait dialog launcher
          * Owns
          */ 
       CEvtMgmtUiWaitDialogLauncher*        iWaitDialogLauncher;   
       /*
        * object used to display position settings
        * Owns
        */
       CEvtMgmtUiLocSettingsLauncher*       iPosSetLauncher;
       
       TBool                                iIsDlgOutstanding;
    };

#endif //EVTMGMTUIPLACECOMPONENT_H_
