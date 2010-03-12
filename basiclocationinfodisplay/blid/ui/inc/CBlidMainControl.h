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



#ifndef __CBLIDMAINCONTROL_H__
#define __CBLIDMAINCONTROL_H__

// SYSTEM INCLUDES
#include <alf/alfcontrol.h>

// USER INCLUDE
// INCLUDES
//#include "CBlidBaseContainer.h"
//#include "CBlidBaseView.h"

//FORWARD DCLERATION
class CEikStatusPane;
class CAlfEnv;
class CBlidBaseView;
class CAlfAnchorLayout;
class CAlfTextVisual;
class MBlidLocation;
class CBlidLocSettingsLauncher;
class CAknMessageQueryDialog;
class CAlfImageLoaderUtil;
class CAlfImageVisual;
class CAlfTexture;

/*! 
  @class CBlidMainControl
  
  @discussion 
  */
class CBlidMainControl : public CAlfControl
    {
	public:
		 /*!
		  @function NewL
		   
		  @discussion Create a CBlidMainControl object, which will draw itself to aRect
          * Two-phased constructor
    	  * @param aKeyProcessor
		  * @param aParent
		  * @param aRect A rectangle for drawing.
		  * @return The newly created object.

		  @result a pointer to the created instance of CBlidMainControl
		  */
		    static CBlidMainControl* NewL( CAlfEnv& aEnv,
		                                const TRect& aRect,
		                                CBlidBaseView& aView );

		 /*!
		  @function ~CBlidMainControl
		  
		  @discussion Destroy the object and release all memory objects
		  */
		     ~CBlidMainControl();

	  
	private:
		 /*!
		  @function CBlidMainControl
		  
		  @discussion Perform the first phase of two phase construction 
		  */
		    CBlidMainControl(  CAlfEnv& aEnv,
		    				   CBlidBaseView& aView );

		 /*!
		  @function ConstructL
		  
		  @discussion  Perform the second phase construction of a CBlidMainControl object
		  */
		    void ConstructL( const TRect& aRect );
		    
	public:    
		/*!
		@function UpdateL
		  
		@discussion  Called by Location model to update the current location
		*/
		void UpdateL();
		
		/*!
		@function MakeTitleL
		  
		@discussion  Set the title
		@param aResourceText
		*/
		void MakeTitleL( TInt aResourceText );
		
		/*!
		@function SetErrorCode
		  
		@discussion  Sets the error code
		@param aErrorCode  
		*/
		void SetErrorCode( TInt aErrorCode );
		
		/*!
		@function SetOnlineMode
		 
		@discussion  Sets the online mode
		@param aOnlineMode
		*/
		void SetOnlineMode( TBool aOnlineMode );
		
		/*!
		@function CreateNaviPaneL
		  
		@discussion  Create the navigation pane
		*/
		void CreateNaviPaneL();
		
		/*!
		@function LaunchPositioningSettingsViewL
		  
		@discussion  Launches the positioning settings view
		*/
		void LaunchPositioningSettingsViewL( );
		
		/*!
		@function DeleteDialogResource
		  
		@discussion  Delete the dialog resources
		*/
		void DeleteDialogResource( );
		
		/*!
		@function UpdateDisplayLayoutL
		  
		@discussion  Update the diaply layout when there is achange in app resource
		*/
		void UpdateDisplayLayoutL();	
		
	private:
		/*!
		@function StatusPane
		  
		@discussion  Retruns the status pane
		*/
		CEikStatusPane* StatusPane() const;
		
		/*!
		@function DispMsgQueryWithLinkL
		  
		@discussion  Displays a message query with a link
		@param aHeadingText
		@param aMsgQueryText
		@param aLinkText
		@param aLinkShow
		@param aCallBack
		*/
		TInt DispMsgQueryWithLinkL(TInt aHeadingText,
								 TInt aMsgQueryText,
								 TInt aLinkText,
								 TBool aLinkShow,
								 TCallBack aCallBack );
		
		/*!
		@function CreateSearchingTextL
		  
		@discussion  Creates the searching text visual
		*/
		void CreateSearchingTextL();
		
		/*!
		@function CreateAnimationL
		  
		@discussion  Creates the image visuals for animation
		*/
		void CreateAnimationL();
		
		/*!
		@function UpdateAnimation
		  
		@discussion  Changes the image visual in orser to animate
		*/
		void UpdateAnimation();
		
		/*!
		@function Tick
		  
		@discussion  Timer callback
		@param aObject
		*/
        static TInt Tick(TAny* aObject); // functions for TCallBack protocol

	private:
        /// Reference to the view to handle resource change
        CAlfEnv& iEnv;
        CBlidBaseView& iView;
		CAlfAnchorLayout* iMainLayout;
		CAlfTextVisual* iSearchTextVisual;
		TInt  iErrorCode;
		TBool  iOnlineMode;	
		MBlidLocation* iLocation;
        ///Own: count for 30 sec PSY unavailaility
        static TInt iPSYTimeoutCount;
        TInt iButtonId;       
        CBlidLocSettingsLauncher*	iLauncher;
        TBool isSettingsLaunched;
        HBufC* iHeadPaneText;
        HBufC* iMsgQueryText;
        HBufC* iLinkText;
        HBufC* iMsgQText;
        TBool isDialogLaunched;

	    CPeriodic* iPeriodic; // periodic timer active object
        TInt iDisplayIcon;    // Current svg animation icon
        CAlfImageVisual* iAnimationVisual;
	    TRect iAnimationRect;
	    RPointerArray<CAlfTexture> iTextures;
	    TBool isAccessoryRequired;
	    
	public:        
        CAknMessageQueryDialog*		iDialog;
    };

#endif // __CBLIDMAINCONTROL_H__
