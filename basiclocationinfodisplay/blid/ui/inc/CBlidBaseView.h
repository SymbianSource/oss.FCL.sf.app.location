/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares main view for blid application.
*
*/


#ifndef CBLIDBASEVIEW_H
#define CBLIDBASEVIEW_H

// INCLUDES
#include <aknview.h>
#include <featmgr.h>
#include <bldvariant.hrh>

#ifdef RD_SCALABLE_UI_V2
#include <eikclb.h>
#endif // RD_SCALABLE_UI_V2

#include "MKeyProcessor.h"
#include "MBlidEngObserver.h"
#include "CLmkLandmarkSelectorDlg.h"
#include "blidcommonconsts.h"

// FORWARD DECLARATIONS
class CEikonEnv;
class CBlidBaseContainer;
class CEikMenuPane;
class MBlidRouter;
class MBlidLocation;
class CBlidEng;
class CBlidCurrentPositionDlg;
class CBlidDocument;
class CAlfImageLoaderUtil;

// CLASS DECLARATION
/**
*  Base view class.
* 
*/
class CBlidBaseView : public CAknView, public MKeyProcessor, 
                                       public MBlidEngObserver
                                       #ifdef RD_SCALABLE_UI_V2                                       
                                       ,public MEikListBoxObserver
                                       #endif // RD_SCALABLE_UI_V2
    {
    public: // Constructors and destructor
        /**
         * C++ constructor
         * @param aKeyProcessor A pointer to MKeyProcessor
         */
        CBlidBaseView( MKeyProcessor& aKeyProcessor );

        /**
         * By default Symbian 2nd phase constructor is private.
         * @param aResId Resource id
         */
        void BaseConstructL( TInt aResId );

        /**
         * Destructor.
         */
        ~CBlidBaseView();

    public: // From MKeyProcessor
        TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        MAknTabObserver* TabObserver();

    public: // From MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    public: //From MBlidEngObserver
        void NotifyL( const TBool aOnlineMode );
        void NotifyErrorL( TInt aErrorCode );
        void Update(CBlidBaseContainer* aContainer);
        CEikButtonGroupContainer* CBAPtr() const;
        
    protected: // New methods  
        /**
         * Handle help requests
         *
         * @param aMenuPane A pointer to CEikMenuPane
         */
        void HandleHelpFeatureL( CEikMenuPane* aMenuPane ) const;

        /**
        * Insert waypoint to MBlidRouter's array
        * @return ETrue if waypoint inserted,
        *         EFalse if waypoint not inserted
        */
        TBool InsertWaypointL();

        /**
        * Select waypoint from the list
        *
        * @return index of list
        */
        TInt SelectWaypointL();

        /**
         * Check if waypoint name exist
         * @param aName 
         * @return TBool 
         */
        TBool CheckIfExistL( const TDes& aName, TInt& aErrorCode, TInt& aRemovedIndex );
        
        /**
         * Launches the Landmark Dialog
         * 
         */
        TInt LaunchLandmarksDialogL();
        
        /**
         * Launches confirmation dialog
         * 
         */
        TInt NotifyUserErrorL(TInt resourceId);
        
        void SaveCurrentPositionAsLandmarkL();
        
        TInt LaunchManualWaypointDialogL();
        
        /**
         * Make visible Middle Soft Key Label/Icon.
         * 
         */
        void SetMSKVisible(); 
        
        #ifdef RD_SCALABLE_UI_V2        
        /**
        * From MEikListBoxObserver.
        * Handles listbox events.
        * @param aListBox Listbox where the event occurred.
        * @param aEventType Event type.
        */
        void HandleListBoxEventL( CEikListBox* aListBox, 
            TListBoxEvent aEventType );

        /**
        * Called by HandleListBoxEventL to handles list box selections.
        * This is Pure virtual function so every derived view class
        * need defination for this function
        */
        virtual void HandleListBoxSelectionL() = 0;                                   
        #endif // RD_SCALABLE_UI_V2        
        
        void ShowCurrentPositionInfoL();
    
    public: //New Method
        /**
         * Changes the MSK visibility option depending upon some condition.
         * 
         */
        void SetMSKNotVisible();
        
        /**
         * Update Middle Soft Key label/Icon depending upon GPS data Availability
         * 
         */
        void UpdateMskStateL();
        
        void SaveCurrentPositionL();
        
        CBlidDocument* BlidDocument();
        
        void ActivateSatelliteViewL( );
        
        TBool IsSatViewActive( );
        
        TBool IsForeGroundApp( );
        
        void ExitMainApplicationL( TInt aButtonId );
        
        void LaunchTripContextMenuL();
        
        void LaunchNavigationContextMenuL();
        
        void LaunchSatelliteInfoDlgL();
        
        /**
         * Sets the Middle Soft Key label after reding from the resource ID.
         * @param aResourceId
         * @param aCommandId
         */        
        void SetMiddleSoftKeyLabelL(const TInt aResourceId, const TInt aCommandId );
        
        void RemoveMiddleSoftKeyLabel( const TInt aCommandId );
        
        void ActivateNavigationViewL( );
        
        void ChangeViewSoftKeyL( );
        
        TRect ApplicationRect() const;
        
        CAlfImageLoaderUtil* ImageLoaderUtils( ) const;
        
        TFileName ImagePathName( ) const;

        
    #ifdef RD_SCALABLE_UI_V2        
    public:
       /**
        * Depends upon the Lm feature availability
        * return ETrue or EFalse
        * @return Tbool
        */
        TBool IsLmFeatureAvailable();
        
       /**
        * Depends upon the Lm availability in DB
        * return ETrue or EFalse
        * @return Tbool
        */
        TBool IsLmAvailableL();
        
       /**
        * Depends upon saved Waypoint availability
        * return ETrue or EFalse
        * @return Tbool
        */
        TBool IsWpAvailable();
        
    #endif // RD_SCALABLE_UI_V2    
    
	public://new function added for HTK        
		void ActivateCorrectViewL( TInt aIndex );
		
        TBool IsSettingsViewActive();        		
        
        CCoeEnv* CoeEnv();        		
    

    protected: // Data
        /// Not owned: reference to key processor
        MKeyProcessor& iKeyProcessor;

        // Not owned: A pointer to location model
        MBlidLocation* iLocation;

        // Not owned: A pointer to router model
        MBlidRouter* iRouter;

        // Not owned: A pointer to blid's engine
        CBlidEng* iEngine;
        
        /// Own: Landmark Dialog
        CLmkLandmarkSelectorDlg* iLandmarkInfo;
        
        ///Own: count for 30 sec PSY unavailaility
        static TInt iPSYTimeoutCount;
        
        ///Own: Manual waypoint default name
        TBuf<KBlidWaypointNameMaxLen> iManualWaypointName;
        
        static TBool iOnline;
        
        TInt iGPSTimeoutCount;
        
        //Own: Boolean value holds whether the MSK is visible or not.
        TBool iMSKVisible;
        
        TBool	iIsSettingsViewActive;                 
        
	private:
		CBlidCurrentPositionDlg* iPopupDialog;
		
        TFileName iConFileName;
        
        CAlfImageLoaderUtil* iImageLoaderUtil;  
		
    };

#endif //CBLIDBASEVIEW_H

// End of File
