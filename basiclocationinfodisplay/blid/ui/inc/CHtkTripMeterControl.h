/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Blid application Tripmeter view's control class definition.
*
*/



#ifndef __CHTKTRIPMETERCONTROL_H__
#define __CHTKTRIPMETERCONTROL_H__

// SYSTEM INCLUDES
#include <alf/alfcontrol.h>

// USER INCLUDE

//FORWARD DCLERATION
class MBlidLocation;
class MBlidRouter;
class MBlidSettings;
class MKeyProcessor;
class CAlfEnv;
class CBlidEng;
class CAknNavigationDecorator;
class CAknTabGroup;
class CEikStatusPane;
class CBlidBaseView;
class CAlfImageVisual;
class CAlfTextVisual;
class CAlfAnchorLayout;
class CAlfDeckLayout;
class CAlfCurvePathLayout;
class CRepository;
class CAlfTextStyle;
class CAlfFrameBrush;
class CAlfImageLoaderUtil;
class CAlfTexture;

/*! 
  @class CHtkTripMeterControl
  
  @discussion HUITK-based control that displays the text "Hello World".
  */
class CHtkTripMeterControl : public CAlfControl
    {
	public:
		 /*!
		  @function NewL
		   
		  @discussion Create a CHtkTripMeterControl object, which will draw itself to aRect
		  @param aRect the rectangle this view will be drawn to
		  @result a pointer to the created instance of CHtkTripMeterControl
		  */
	    static CHtkTripMeterControl* NewL(CAlfEnv& aEnv,
		    							MKeyProcessor& aKeyProcessor,
                                        MObjectProvider* aParent,
		                                const TRect& aRect,
                                        MBlidLocation* aLocation,
                                        MBlidRouter* aRouter,
                                        CBlidEng& aEngine,
                                        CBlidBaseView& aView);

		 /*!
		  @function ~CHtkTripMeterControl
		  
		  @discussion Destroy the object and release all memory objects
		  */
		~CHtkTripMeterControl();

	  
	private:
		 /*!
		  @function CHtkTripMeterControl
		  
		  @discussion Perform the first phase of two phase construction 
		  */
	    CHtkTripMeterControl(CAlfEnv& aEnv,
	    					MKeyProcessor& aKeyProcessor,
                            MBlidRouter& aRouter,
                            CBlidBaseView& aView,
                            CBlidEng& aEngine );

		 /*!
		  @function ConstructL
		  
		  @discussion  Perform the second phase construction of a CHtkTripMeterControl object
		  */
		void ConstructL(CAlfEnv& aEnv,
						const TRect& aRect,
                        MBlidLocation* aLocation,
                        CBlidEng& aEngine );
                        
                        
	public:	    //from CAlfControl
		/**
         * Handle key event
         */
		TBool OfferEventL( const TAlfEvent& aEvent );
		/**
         * Process command
         */	
		void ProcessCommandL(TInt aCommand);
		
    public: // New functions
        /**
         * Update the display, called whenever a position update is received
         */
        void UpdateL();
        
    public: 
    	/**
         * Retruns ETrue if trip is started
         */
	    TBool HasTripStarted();
	    /**
         * Checks if GPS Data was lost
         */
	    TBool WasGPSDataLost();
	    /**
         * Sets GPS Data availability
         */
	    void SetGPSDataAvailability(TBool aAvailability);

        /**
         * Make title
         * @param aResourceText resource to create title
         */
        void MakeTitleL( TInt aResourceText );
		
		/**
         * Returns Navigation decorator
         */
		CAknNavigationDecorator* GetNiviDecorator( );

        /**
         * Create CAknNavigationDecorator, ownership passed to caller.
         * @param aDefaultNaviPane, ETrue if default, EFalse if tabgroup
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
         * Saves the odometer value
         */
		void SaveOdometerValue();
		/**
         * Reset Odometer Distance
         */
		void ResetOdometerDistanceL( );
		/**
         * Change trip focus
         */
		void ChangeTripFocus( TBool aSTripDimmed );
		/**
         * Rechange trip focus
         */
		void ReChangeTripFocus( );
		/**
         * Update layout
         */
		void UpdateLayoutL( );
		/**
         * Displays info popup
         */
		void DisplayInfoPopUpMsgL();
		
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
         * Creates the trip visuals
         */
        void CreateTripVisualsL( );
		/**
         * Returns tripmeter value as descriptor
         */
		void TripmeterFormatL( TDes& aTripmeterString );
		/**
         * Returns total trip time as descriptor
         */
		void TotalTimeFormat( TDes& aTotalTimeString );
		/**
         * Returns odometer value as descriptor
         */
		void OdometerFormat( TDes& aOdometerString );
		/**
         * Returns current trip time as descriptor
         */
		void TripTimeFormat( TDes& aTripTimeString );
		/**
         * Set the value of tripmeter
         */
		void SetTRipMeterValueL( );
		/**
         * Set the value of odometer
         */
		void SetOdoMeterValueL( );
		/**
         * Set the trip time
         */
		void SetTripTimeL( );
		/**
         * Set the trip speed
         */
		void SetTripSpeed( );
		/**
         * Create speed visuals
         */
		void CreateSpeedVisualsL();
		/**
         * Rotates the speed dial
         */
		void RotateSpeedDialL( int aThetaPrevious, int aThetaFinal );
		/**
         * Ceates the speeed format
         */
		void CreateSpeedFormatsL();
		/**
         * Change the speedmeter scale
         */
		void ChangeSpeedometerScaleL();
		/**
         * Formats the odometer distance
         */
		void FormatOdometerDistance( TReal aLastodometerValue );
		/**
         * Launches device not available note
         */
		void LaunchDeviceNotAvlNoteL();
		/**
         * Displays the current signal strength
         */
		void DisplaySatelliteSignalStrength( TInt aSatCount );
		/**
         * Launches info query dialog
         */
		void LaunchInfoQueryDlgL();		
		/**
         * Update visual image
         */
        void UpdateVisualImage();
        /**
         * Create Popup visuals
         */
        void CreatePopUpVisualL();
        /**
         * Create all signal textures
         */
		void CreateSignalTexturesL();
		/**
         * Create popup image tesxture
         */
		void CreatePopupImgTextureL();		 
        /**
         * Called when timer is expired
         */
        static TInt Tick(TAny* aObject); // functions for TCallBack protocol
        /**
         * Calculate the Layout Rects
         */
        void CalculateRects();
        /**
         * Set Anchors for Visuals
         */
        void SetAnchors();
	private:
        /// Ref: aRouter
        MBlidRouter& iRouter;
        
        MBlidLocation* iLocation;

        /// Own: Decorator class for navigation pane controls.
        CAknNavigationDecorator* iDecoratedTabGroup;        

        /// Reference to the view to handle resource change
        CBlidBaseView& iView;
        
        /// Reference to the engine
        CBlidEng& iEngine;
                
		//Own: Layout for all the visuals.
		CAlfAnchorLayout* iTripLayout;
		
		CAlfEnv& iEnv;
		
		CAlfImageVisual* iTripDeckVisual;
		CAlfImageVisual* iOdoDeckVisual;
		CAlfImageVisual* iBigTripVisual;
		CAlfDeckLayout* iTripDistLayout;
		CAlfDeckLayout* iOdoLayout;
		CAlfImageVisual* iTripDialVisual;
		CAlfImageVisual* iTripTimeVisual;
		//CAlfDeckLayout* iTripTimeLayout;
		CAlfDeckLayout* iBigTripDecLayout;
    	/// Ref: A pointer to MBlidSettings model
        MBlidSettings* iSettingsModel;
        
        HBufC* iDurationFormatLong;
        
        HBufC* iSpeedFormatKmph;
        
        HBufC* iSpeedFormatMph;
        
        HBufC* iDataBlank;
        
        HBufC* iTripDistance;
        
        HBufC* iOdometerDistance;
        
		//CAlfCurvePathLayout* iSpeedCurve;
		//CAlfDeckLayout* iMileLayout;
		CAlfTextVisual* iMileVisual[9];
		TInt iSpeedCount;
		TReal32 iPrevSpeed;
		TInt iMaxScale;
		TInt iPrevAngle;
		TInt iNextAngle;
		
		CAlfAnchorLayout* iPointerLayout;
		
		CAlfTextVisual* iTripMeter;
		
		CAlfTextVisual* iOdoMeter;
		
		CAlfTextVisual* iTriptime;
		
		CAlfTextVisual* iSpeedFormatVisual;
		
		TReal32 iCurrSpeed;
		
		TBool iIsLessThanInitial;
		
		CRepository* iRepository;
		
		TReal iLastodometerValue;
		
		TReal iOdoValueToStore;
		
		CAlfTextStyle* iStyle1;
		
		CAlfAnchorLayout* iPopUpLayout;
		
		CAlfTextVisual* iPopUpMsg;
		
		CAlfImageVisual* iPopImage;
		
		TBool isSatelliteInfoLaunch;
		
		CAlfDeckLayout* iPopupDeck;
		
		CAlfFrameBrush* iFrameBrush;
		
		CAlfImageVisual* iImageVisual;
		
	    CPeriodic* iPeriodic; // periodic timer active object
        TInt iDisplayIcon;    // Current svg animation icon
	    RPointerArray<CAlfTexture> iPopupTextures;
	    RPointerArray<CAlfTexture> iSignalTextures;
	    CAlfTexture* iPopupImgTexture;	    	    
		
		HBufC*	iImagePath;
		
		TBool iSTripDimmed;
		
		CAlfAnchorLayout* iPopUpAnchor;
		
		TRect iBigTripRect;
		
		TRect iTripDialRect;
		
		TRect iTripDeckRect;
		
		TRect iTripMeterRect;
		
		TRect iOdoDeckRect;
		
		TRect iOdoMeterRect;
		
		TRect iSpeedFormatRect;
		
		TRect iTripTimeRect;
		
		TRect iClockImageRect;
		
		TRect iValueRect1;
		
		TRect iValueRect2;
		
		TRect iValueRect3;
		
		TRect iValueRect4;
		
		TRect iValueRect5;
		
		TRect iValueRect6;
		
		TRect iValueRect7;
		
		TRect iValueRect8;
		
		TRect iValueRect9;		
		
		TRect iSignalStrengthRect;												
		
		TBool isDeviceUnavailableDisplayed;											
		TBool iIsDownPressed;										;
    };

#endif // __CHTKTRIPMETERCONTROL_H__
