/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Blid application compass view's compass control class definition.
*
*/



#ifndef __CHTKNAVIGATIONCONTROL_H__
#define __CHTKNAVIGATIONCONTROL_H__

// SYSTEM INCLUDES
#include <lbspositioninfo.h>
#include <EPos_CPosLandmark.h>
#include <alf/alfcontrol.h>

//#ifdef RD_SCALABLE_UI_V2
#include <barsread.h>
#include <StringLoader.h>
#include <aknstyluspopupmenu.h>
//#endif // RD_SCALABLE_UI_V2

// USER INCLUDE
#include "CBlidBaseView.h"

//FORWARD DCLERATION
class MBlidLocation;
class MBlidRouter;
class MBlidEngObserver;
class MBlidSettings;
class CBlidEng;
class CEikonEnv;
class MKeyProcessor;
class CAlfEnv;
class CAlfImageVisual;
class CAlfTextVisual;
class CAlfBorderBrush;
class CAlfAnchorLayout;
class CAlfLayout;
class CAlfCurvePath;
class CAlfCurvePathLayout;
class CAlfDeckLayout;
class CAlfTextStyle;
class CBlidArrivedTonePlayerUtility;
class CAlfFrameBrush;
class CAlfImageLoaderUtil;
class CAlfTexture;

const TInt KNumTexts = 3;

/*! 
  @class CHtkNavigationControl
  
  @discussion 
  */
class CHtkNavigationControl : public CAlfControl
    {
	public:
		 /*!
		  @function NewL
		   
		  @discussion Create a CHtkNavigationControl object, which will draw itself to aRect
		  @param aRect the rectangle this view will be drawn to
		  @result a pointer to the created instance of CHtkNavigationControl
		  */
		    static CHtkNavigationControl* NewL(CAlfEnv& aEnv,
					                           MObjectProvider* aParent,
							                   const TRect& aRect,
					                           MBlidLocation& aLocation,
					                           MBlidRouter& aRouter,
					                           CBlidEng& aEngine,
					                           CBlidBaseView& aView);

		 /*!
		  @function NewLC
		   
		  @discussion Create a CHtkNavigationControl object, which will draw itself to aRect
		  @param aRect the rectangle this view will be drawn to
		  @result a pointer to the created instance of CHtkNavigationControl
		  */
		    static CHtkNavigationControl* NewLC(CAlfEnv& aEnv,
					                            MObjectProvider* aParent,
							                    const TRect& aRect,
					                            MBlidLocation& aLocation,
					                            MBlidRouter& aRouter,
					                            CBlidEng& aEngine,
					                            CBlidBaseView& aView);


		 /*!
		  @function ~CHtkNavigationControl
		  
		  @discussion Destroy the object and release all memory objects
		  */
		     ~CHtkNavigationControl();

	  
	private:
		 /*!
		  @function CHtkNavigationControl
		  
		  @discussion Perform the first phase of two phase construction 
		  */
		    CHtkNavigationControl(CAlfEnv& aEnv,
                                  MBlidLocation& aLocation,
                                  CBlidBaseView& aView,
                                  CBlidEng& aEngine );

		 /*!
		  @function ConstructL
		  
		  @discussion  Perform the second phase construction of a CHtkNavigationControl object
		  */
		    void ConstructL(CAlfEnv& aEnv,
		                    const TRect& aRect,
                            MBlidRouter& aRouter,
                            CBlidEng& aEngine);

	private:    
        /** 
         * Add new tabs to tabgroup
         * @param aTabGroup
         */
        void AddTabsL(CAknTabGroup& aTabGroup);
        
        /**
         * Appui's status pane getter
         * @return Statuspane pointer
         */
        CEikStatusPane* StatusPane();
        /**
         * Creates navigation visuals
         */
        void CreateNavigationVisualsL( );
        /**
         * Creates navigation text visuals
         */						     
		void CreateNavigationTextVisualsL( CAlfTextStyle* aStyle1 );
		/**
         * Creates disstance visuals
         */
		void CreateDistanceVisualL( CAlfTextStyle* aStyle1 );
		/**
         * Play arrived tone
         */
		void PlayArrivedToneL();
		/**
         * Calculate if user has arrived to destination
         */
		void CalculateIfArrivedL();
		/**
         * Creates application strings
         */
		void CreateApplicationStringsL( );
		/**
         * Converts durtion into descriptor
         */
		void DurationFormat( TDes& aDurationString );
		/**
         * Converts distance unit into descriptor
         */
		void DistanceUnit( TDes& aUnitString );
		/**
         * Converts distance into descriptor
         */
		void DistanceFormat( TDes& aDistanceString );
		/**
         * Calculates durtion
         */
		void CalculateDurationL( TCourse aCourse );
		/**
         * Calculates distance
         */
		void CalculateDistanceL( TPosition aPosition );
		/**
         * Converts speed format into descriptor
         */
		void SpeedFormat( TDes& aSpeedString, TDes& aSpeedFormatString);
		/**
         * Creates speed visuals
         */
		void CreateSpeedVisualL( CAlfTextStyle* aStyle1 );
		/**
         * Prepares for dynamic move
         */
		void PrepareDynamicMoveL( );
		/**
         * Sets the landmark name
         */                             
		void SetLandMarksNameL();
		/**
         * Displays the current signal strength
         */
		void DisplaySatelliteSignalStrength( TInt aSatCount );
		/**
         * Creates textures for signal images
         */
		void CreateSignalTexturesL();
		/**
         * Create popup image texture
         */
		void CreatePopupImgTextureL();
		/**
         * Creates accuracy ring texture
         */
		void CreateAccTextureL();

       /*
        * This function checks wheather
        * distance to destination is less than 10 meter
        * and return ETrue or EFalse depends upon the condition.
        */        
        TBool IsInArrivedZone();
		/**
         * Creates indicator visual
         */      
        void CreateIndicatorVisualL();
        /**
         * Launches device not available note
         */
        void LaunchDeviceNotAvlNoteL();
        /**
         * Launches infor query dialog
         */
        void LaunchInfoQueryDlgL();
		/**
         * Creates animation
         */
        void CreateAnimationL();
        /**
         * Creates popup visuals
         */
        void CreatePopUpVisualL();
        /**
         * Updates visual image when timer is expired
         */
        void UpdateVisualImage();
        /**
         * Called when timer is expired
         */
        static TInt Tick(TAny* aObject); // functions for TCallBack protocol
        /**
         * Launches stylus activated popup
         */
        void LaunchStylusPopupMenuL( TPointerEvent aPointerEvent );
        /**
         * Calculate the Layout Rects
         */
        void CalculateRects();
        /**
         * Set Anchors for Visuals
         */
        void SetAnchors();
        
	public:	    //from CAlfControl
		/**
         * Handles the key events
         */
		TBool OfferEventL( const TAlfEvent& aEvent );
		/**
         * Notify about control visibility
         */
		void NotifyControlVisibility( TBool aIsVisible, 
		       						  CAlfDisplay& aDisplay );
					
		
	public:
        /**
         * Update controls
         */ 
        void UpdateL();
        /**
         * Clears the navigation info
         */
        void ClearNavigationInfoL();
		/**
         * Set update option
         */
		void SetUpdateOptionL( TBool aIsDestSet );        
		/**
         * Draw curves
         */
		void DrawCurvesL( );
		/**
         * Sets the flag visual
         */
		void SetFlagVisualL();
		/**
         * Sets the accuracy visual
         */
		void SetAccRacyVisualL();

        /**
         * Create CAknNavigationDecorator, ownership passed to caller.
         * @param aDefaultNaviPane, ETrue if default, EFalse if tabgroup
         * @return new instance of object. NULL, if something fails
         */
        void CreateNaviPaneL( TBool aDefaultNaviPane );


        /**
         * Activate navigation decorator.
         * Method can called if tab needs to be refreshed.
         * @param aNaviDecorator    navigation decorator
         * @param aTabId            activated tabulator id             
         */
        void ActivateTabL( CAknNavigationDecorator& aNaviDecorator, 
                            TInt aTabId );
		/**
         * Retruns the navi decorator
         */
		CAknNavigationDecorator* GetNiviDecorator( );
		/**
         * Change the flag visual image when reaching destination
         */
        void ChangeFlagVisualImageL();
        /**
         * Check if user has arrived to the destination
         */
        TBool HasArrivedDestination();        
        /**
         * Make title
         * @param aResourceText resource to create title
         */
        void MakeTitleL( TInt aResourceText );
        /**
         * Stop the arrrival tone player
         */
        void StopPlayerL(  ); 
        /**
         * Update the layout
         */
        void UpdateLayoutL();
        /**
         * Displays the info popup msg
         */
        void DisplayInfoPopUpMsgL();
        
	private:
		
        /// Ref: A reference to MBlidLocation model
        MBlidLocation& iLocationModel;

        /// Ref: A pointer to MBlidRouter model
        MBlidRouter* iRouter;

        /// Reference to the view to handle resource change
        CBlidBaseView& iView;
        
        /// Reference to the engine
        CBlidEng& iEngine;

        /// Own: Decorator class for navigation pane controls.
        CAknNavigationDecorator* iDecoratedTabGroup;        
        
		//Own: Layout for all the visuals.
		CAlfAnchorLayout* iLayout;

		//Own: Text visuals. 
		CAlfTextVisual* iLabel;
		
		//Own: Text visuals. 
		CAlfTextVisual* iDistanceLabel;

		//Own: Text visuals. 
		CAlfTextVisual* iSpeedLabel;

		//Own: Text visuals. 
		CAlfTextVisual* iSpeedFormatLabel;

		CAlfEnv& iEnv;
		
		CAlfImageVisual* iImageVisual;
		
        TInt iIndex;
        
        /// Own: Position information 
        TPosition iPositionInfo;

        /// Own: Course information
        TCourse iCourse;
        
        /// Ref: A pointer to MBlidSettings model
        MBlidSettings* iSettings;        
        
        // To determine if user has arrived to destination
        static TBool iHasArrived;
        
        //variable to check wheather user arrived to destination
        TBool iTimeFirstArrived;
        
        //Boolean to determine whether the arrived tone has palyed/not         
        static TBool       iHasToPlayArrivedTone;

        /// Own: Metric distance functionality text string 
        HBufC* iDistanceFormatKm;

        /// Own: Metric distance functionality text string 
        HBufC* iDistanceFormatM;

        /// Own: Imperial distance functionality text string 
        HBufC* iDistanceFormatMiles;

        /// Own: Imperial distance functionality text string 
        HBufC* iDistanceFormatFt;

        /// Own: Duration functionality text string 
        HBufC* iDurationFormatLong;

        /// Distance variable
        TReal32 iDistance;

        /// Duration variable
        TTime iDuration;

        /// Duration in days
        TInt iDurationInDays;

        /// Duration in seconds
        TReal64 iDurationInSeconds;

        /// Own: Speed's title text string
        HBufC* iBlidSpeed;
        
        /// Speed variable
        TReal iSpeed;

        /// Is speed data available
        TBool iIsSpeed;

        /// Own: Metric speed's format text string
        HBufC* iSpeedFormatKmph;

        /// Own: Imperial speed's format text string
        HBufC* iSpeedFormatMph;

        // Own: No Data indicator
        HBufC* iDataBeingCalculated;

       // Own: No Data indicator
        HBufC* iDataBlank;
        
		
		TBool iIsDestinationSet;
		
		CAlfCurvePath* iCurvePath;		
		
		CAlfCurvePathLayout* iCurveLayout;
		
		HBufC8* iFlagText;
		HBufC8* iAccText;
		
		CAlfImageVisual*  iFlagVisual;
		
		CAlfImageVisual*  iAccVisual;		
		
		CAlfImageVisual*  iNorthVisual;
		
		CAlfDeckLayout*  idecLayout;
		
		CAlfDeckLayout*  iNdecLayout;
		
		TReal iHeading;
		
		TRect iAccuracyRingRect;
		
		TPoint* iCharPoint;
		
		CAlfDeckLayout* iAccLayout;
		
        CPeriodic* iTimer;
        
        TInt iIsFirstDistance;
        TReal32 iFirstDistance;
    	HBufC*	iImagePath;
    	TSize iCurveSize;
    	TSize iFlagSize;
    	TSize iAccuracyringcurveSize;   
        
        CAlfDeckLayout* iCurveDecLyt;
        
        CAlfAnchorLayout* iIndiAccAnchor;
        
        CAlfImageVisual* iIndicatorVisual;
        
        CAlfDeckLayout* iFlagdeck;
        
        CAlfDeckLayout* iActualNdeck;
		
		CBlidArrivedTonePlayerUtility* iToneUtility;
		
		TBool isStopUpdate;
		
		CAlfAnchorLayout* iPopUpLayout;
		
		CAlfTextVisual* iPopUpMsg;
		
		CAlfImageVisual* iPopImage;
		
		TBool isSatelliteInfoLaunch;
		
		CAlfDeckLayout* iPopupDeck;
		
		CAlfFrameBrush* iFrameBrush;
		
		CAlfAnchorLayout* iPopUpAnchor;

		TRgb	iVisualColor;
		
		TBool iMskInfoVisible;
		
		CAlfImageVisual* iNavigationRing;

	    CPeriodic* iPeriodic; // periodic timer active object
        TInt iDisplayIcon;    // Current svg animation icon
	    RPointerArray<CAlfTexture> iTextures;
	    RPointerArray<CAlfTexture> iPopupTextures;
	    RPointerArray<CAlfTexture> iSignalTextures;
	    CAlfTexture* iAccTexture;
	    CAlfTexture* iPopupImgTexture;
				
		MAlfBitmapProvider* iProvider;
		
		TRect iSignalStrengthRect;
		
		TRect iLmkNameRect;
		
		TRect iDistanceRect;
		
		TRect iSpeedBgRect;
		
		TRect iSpeedRect;
		
		TRect iSpdFormatRect;
		
		TRect iNavRingBgRect;
		
		TRect iAccRingRect;
		
		TRect iDirectionIndRect;
		
		TRect iNorthRect;

		TRect iFlagRect;

		CAknStylusPopUpMenu* iMenu;
		
		TBool isDeviceUnavailableDisplayed;
    public:        
        TBool iTimerComplete;
        static TBool       iInArrivedZone;
        TBool iIsDownPressed;
    };

#endif // __CHTKNAVIGATIONCONTROL_H__
