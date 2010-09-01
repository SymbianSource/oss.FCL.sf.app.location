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
* Description:  Blid application startup view's control class definition.
*
*/



#ifndef __CBlidSatelliteControl_H__
#define __CBlidSatelliteControl_H__

// SYSTEM INCLUDES
#include <alf/alfcontrol.h>

// USER INCLUDE
// INCLUDES
//#include "CBlidBaseContainer.h"
//#include "CBlidBaseView.h"

//FORWARD DCLERATION
class CAlfEnv;
class CBlidBaseView;
class MBlidLocation;
class CAlfAnchorLayout;
class CAlfTextVisual;
class CAlfDeckLayout;
class CAlfTextStyle;
class CAknNavigationDecorator;
class CAlfImageLoaderUtil;
class CAlfImageVisual;
class CAlfTexture;

/*! 
  @class CBlidSatelliteControl
  
  @discussion 
  */
class CBlidSatelliteControl : public CAlfControl
    {
	public:
		 /*!
		  @function NewL
		   
		  @discussion Create a CBlidSatelliteControl object, which will draw itself to aRect
          * Two-phased constructor
    	  * @param aKeyProcessor
		  * @param aParent
		  * @param aRect A rectangle for drawing.
		  * @return The newly created object.

		  @result a pointer to the created instance of CBlidSatelliteControl
		  */
		    static CBlidSatelliteControl* NewL( CAlfEnv& aEnv,
		                                const TRect& aRect,
		                                CBlidBaseView& aView );

		 /*!
		  @function ~CBlidSatelliteControl
		  
		  @discussion Destroy the object and release all memory objects
		  */
		     ~CBlidSatelliteControl();

	  
	private:
		 /*!
		  @function CBlidSatelliteControl
		  
		  @discussion Perform the first phase of two phase construction 
		  */
		    CBlidSatelliteControl(  CAlfEnv& aEnv,
		    				   CBlidBaseView& aView );

		 /*!
		  @function ConstructL
		  
		  @discussion  Perform the second phase construction of a CBlidSatelliteControl object
		  */
		    void ConstructL( const TRect& aRect );
		    
        /**
        * Enumerations for signal strength
        */
        enum TBlidSignalStrength
            {
            EBlidSignalStrengthZero,
            EBlidSignalStrengthOne,
            EBlidSignalStrengthTwo,
            EBlidSignalStrengthThree,
            EBlidSignalStrengthFour
            };  
		            
	public:    
		void UpdateL();
		void MakeTitleL( TInt aResourceText );
		void SetErrorCode( TInt aErrorCode );
		void SetOnlineMode( TBool aOnlineMode );
		void CreateNaviPaneL( const TDesC& aText );
		void ChangeNavigationViewL();
		void UpdateDisplayLayoutL();
		
		
	private:		
		void CreateGlobeVisualL( );
		void CreateSatelliteSignalVisualsL(  );
		void CreateSignalLayoutL( CAlfTextStyle* aStyle, TRgb aRgb );
		void DisplaySatellitesRowL( const TDesC& aSatNo,
						   		    CAlfImageVisual* aImageVisual,
									CAlfTextVisual* aTextVisual,
									TInt aIndex );
									
		void ChangeSoftKeyL();
		void CalculateRects();
		void SetAnchors();		
        static TInt Tick(TAny* aObject); // functions for TCallBack protocol
        void UpdateGlobeVisual();							
		
	private:
		CEikStatusPane* StatusPane();		
		
	private:
        /// Reference to the view to handle resource change
		CAlfEnv& 						iEnv;
        CBlidBaseView& 					iView;
		TInt  							iErrorCode;
		TBool  							iOnlineMode;
		CAlfAnchorLayout* 				iMainAnchorLayout;
		CAlfDeckLayout*					iGlobeDeckLayout;
		CAlfDeckLayout*					iSatelliteBarDeckLayout;
		CAlfDeckLayout* 				iFlowDeckOne;
		CAlfDeckLayout* 				iFlowDeckTwo;
		CAlfImageVisual* 				iSatelliteVisual[8];
		CAlfImageVisual* 				iSignalStrengthRowOne[17];
		CAlfTextVisual* 				iSatNoRowOne[17];
		CAknNavigationDecorator*		iNavimessage;
		MBlidLocation* 					iLocation;
		HBufC*							iImagePath;
		CPeriodic* 						iTimer;
		HBufC*							iAvailableNoteText;
		HBufC*							iSearchNoteText;
		
		RArray<TRect>					iSignalBarRects;
		RArray<TRect>					iSatelliteNumberRects;

	    CPeriodic* iPeriodic; // periodic timer active object
        TInt iDisplayIcon;    // Current svg animation icon
        CAlfImageVisual* iAnimationVisual;
	    RPointerArray<CAlfTexture> iTextures;
    	    
		TRect							iGlobeRect;
		TRect							iSatGridRect;														
    };	

#endif // __CBlidSatelliteControl_H__
