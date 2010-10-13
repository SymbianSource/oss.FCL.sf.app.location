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
* Description:  Provides blid startup view class methods.
*
*/



// SYSTEM INCLUDES
#include <StringLoader.h>
#include <Blid.rsg>
#include <blid.mbg>
#include <akntitle.h> 
#include <aknnavide.h> 
#include <eikspane.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknUtils.h>
#include <AknsConstants.h>
 
#include <alf/alfenv.h>
#include <alf/alfanchorlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alftextstyle.h>
#include <alf/alfborderbrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfevent.h>
#include <alf/alftransformation.h>
#include <alf/alftexture.h>
#include <alf/alfdecklayout.h>
#include <alf/alfutil.h>
#include <alf/alfimagevisual.h>
#include <alf/alftexture.h>
#include <alf/alfimageloaderutil.h>
#include <alf/alfflowlayout.h>

// USER INCLUDES
#include "CBlidSatelliteControl.h"
#include "CBlidEng.h"
#include "CBlidDocument.h"
#include "MBlidLocation.h"
#include "CBlidBaseView.h"
#include "bliduiconsts.h"
#include "Debug.h"

TInt TimerCallBack(TAny* aPtr);

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CBlidSatelliteControl::NewL()
// ---------------------------------------------------------
//
CBlidSatelliteControl* CBlidSatelliteControl::NewL( CAlfEnv& aEnv,
		                                  const TRect& aRect,
		                                  CBlidBaseView& aView)
    {
    CBlidSatelliteControl* self = new (ELeave) CBlidSatelliteControl(aEnv,
    													   aView );
    CleanupStack::PushL(self);
    self->ConstructL( aRect );
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------
//
 //CBlidSatelliteControl::CBlidSatelliteControl()
// ---------------------------------------------------------
//
CBlidSatelliteControl::CBlidSatelliteControl(CAlfEnv& aEnv,
		    				   	   CBlidBaseView& aView ) 
    : CAlfControl( ),iEnv(aEnv),iView(aView)
    {
    iOnlineMode = EFalse;
    }


// ---------------------------------------------------------
// CBlidSatelliteControl::ConstructL()
// ---------------------------------------------------------
//
void CBlidSatelliteControl::ConstructL( const TRect& /*aRect*/ )
    {
    CAlfControl::ConstructL( iEnv );
    
    CBlidEng* engine = iView.BlidDocument()->Engine();
    iLocation = engine->LocationModel();

	iImagePath = HBufC::NewL( iEnv.TextureManager().ImagePath().Length() + 1 );
	iImagePath->Des().Copy( iEnv.TextureManager().ImagePath() );
	
	iAvailableNoteText = StringLoader::LoadL( R_BLID_NOTE_GPS_POSITIONING_AVAILABLE, CEikonEnv::Static() );
	iSearchNoteText = StringLoader::LoadL( R_BLID_WAITNOTE_SEARCHING_GPS_SATELLITES, CEikonEnv::Static() );
    
    iMainAnchorLayout = CAlfAnchorLayout::AddNewL( *this );
    
    iGlobeDeckLayout = CAlfDeckLayout::AddNewL( *this, iMainAnchorLayout );
    iAnimationVisual = CAlfImageVisual::AddNewL(*this, iGlobeDeckLayout );

    iDisplayIcon = 1;
    iPeriodic = CPeriodic::NewL(0); // neutral priority 
    iPeriodic->Start( TTimeIntervalMicroSeconds32( 200000 ),
                      TTimeIntervalMicroSeconds32( 200000 ),
                      TCallBack(Tick, this));
                      
	CalculateRects();
	SetAnchors();
    CreateGlobeVisualL();
    UpdateGlobeVisual();
	CreateSatelliteSignalVisualsL();
	
	}
    
// ----------------------------------------------------
// CBlidSatelliteControl::~CBlidSatelliteControl
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CBlidSatelliteControl::~CBlidSatelliteControl()
    {
    iTextures.Reset();
    iTextures.Close();
    delete iNavimessage;
    delete iImagePath;
    delete iSearchNoteText;
    delete iAvailableNoteText;
	iSignalBarRects.Reset();
	iSignalBarRects.Close();
	iSatelliteNumberRects.Reset();
	iSatelliteNumberRects.Close();
	delete iPeriodic;
	delete iTimer;
    }    
 
 // ---------------------------------------------------------
// CBlidSatelliteControl::Tick()
// ---------------------------------------------------------
//
TInt CBlidSatelliteControl::Tick(TAny* aObject)
    {
    ((CBlidSatelliteControl*)aObject)->UpdateGlobeVisual(); // cast, and call non-static function

    return KErrNone;
    }
    

// ---------------------------------------------------------
// CBlidSatelliteControl::UpdateL
// ---------------------------------------------------------
//
void CBlidSatelliteControl::UpdateL()
    {
    TInt satInView = iLocation->NumberOfSatellitesVisible();
    TInt satId = 0;
	DEBUG1(" CBlidSatelliteControl::UpdateL iErrorCode ::%d", iErrorCode );
	if( iOnlineMode )
		{
		CreateNaviPaneL( *iAvailableNoteText );
		// code for GPS fix received view			
	    for( TInt i = 0; i < satInView; i++ )
		    {
		    satId = iLocation->GetSatelliteNo( i );
			TBuf<5> satNumber;
			satNumber.Format(_L("%d"), satId);
			
			if( i < 16 )
				{
				DisplaySatellitesRowL( satNumber, iSignalStrengthRowOne[i], iSatNoRowOne[i], i );
				// Dimmed those satellites which are not used.
				if( !iLocation->IsSatellitesUsed( i ) )
					{
					iSignalStrengthRowOne[i]->SetOpacity( 0.3 );	
					}
				else
					{
					iSignalStrengthRowOne[i]->SetOpacity( 1.0 );						
					}					
				}

		    }

	    // Change soft key    
	    iView.CBAPtr()->SetCommandSetL( R_AVKON_SOFTKEYS_EXIT );
	    iView.CBAPtr()->DrawDeferred();	    
		// Start the timer for 2 seconds
	    if( iTimer )
	        {
	        delete iTimer;
	        iTimer = NULL;
	        }
	    iTimer = CPeriodic::NewL( EPriorityHigh );
	    if( iTimer )
	        {
		    TCallBack callback( TimerCallBack, this );
		    iTimer->Start( TTimeIntervalMicroSeconds32( 2000000 ),
	                       TTimeIntervalMicroSeconds32( 80000000 ), 
                           callback );
	        }
		// once timer completed then activate navigation view	        
		}
    else if( iLocation->WaitingGPSData() || iErrorCode == KErrTimedOut )
		{
		CreateNaviPaneL( *iSearchNoteText );
	    // code for searching GPS view
	    for( TInt i = 0; i < satInView; i++ )
		    {
		    satId = iLocation->GetSatelliteNo( i );
			TBuf<5> satNumber;
			satNumber.Format(_L("%d"), satId);
										
			if( i < 16 )
				{
				DisplaySatellitesRowL( satNumber, iSignalStrengthRowOne[i], iSatNoRowOne[i], i );
				}
		    }
		ChangeSoftKeyL();		    
		}
    }
    
// ----------------------------------------------------
// CBlidSatelliteControl::MakeTitleL
// ----------------------------------------------------
//
void CBlidSatelliteControl::MakeTitleL( TInt aResourceText )
    {
    HBufC* buf = StringLoader::LoadLC(aResourceText);       
    CAknTitlePane* title = static_cast<CAknTitlePane*>( StatusPane()->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    CleanupStack::Pop(); //buf
    title->SetText(buf); // Takes ownership of buf
    } 
    
// ----------------------------------------------------
// CBlidSatelliteControl::StatusPane
// ----------------------------------------------------
//
CEikStatusPane* CBlidSatelliteControl::StatusPane()
    {
    return CEikonEnv::Static()->AppUiFactory()->StatusPane();
    }
    
// ----------------------------------------------------
// CBlidSatelliteControl::SetErrorCode
// ----------------------------------------------------
//
void CBlidSatelliteControl::SetErrorCode( TInt aErrorCode )
	{
	iErrorCode = aErrorCode;	
	}
	
// ----------------------------------------------------
// CBlidSatelliteControl::SetOnlineMode
// ----------------------------------------------------
//
void CBlidSatelliteControl::SetOnlineMode( TBool aOnlineMode )
	{
	iOnlineMode = aOnlineMode;	
	}
	
// ----------------------------------------------------
// CBlidSatelliteControl::CreateNaviPaneL
// ----------------------------------------------------
//
void CBlidSatelliteControl::CreateNaviPaneL( const TDesC& aText )
    {
    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    // Fetch pointer to the default navi pane control
    CAknNavigationControlContainer* naviPane = 
        static_cast<CAknNavigationControlContainer*>(sp->ControlL( 
        TUid::Uid(EEikStatusPaneUidNavi)));
    
    naviPane->PushDefaultL(EFalse);
    if( iNavimessage )
    	{
    	delete iNavimessage;
    	iNavimessage = NULL;
    	}
	iNavimessage = naviPane->CreateNavigationLabelL( aText );
	naviPane->PushL( *iNavimessage );
    }
    
// ----------------------------------------------------
// CBlidSatelliteControl::CreateGlobeVisualL
// ----------------------------------------------------
//
void CBlidSatelliteControl::CreateGlobeVisualL( )
	{
	// code to load svg icon
	MAlfBitmapProvider* provider = NULL;
	
	// first creste the image loader utils and then call SetSize.	
    iView.ImageLoaderUtils()->SetSize( iGlobeRect.Size() );
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_01, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_01_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture1 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture1);
    												 
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_02, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_02_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture2 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture2);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_03, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_03_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture3 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture3);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_04, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_04_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture4 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture4);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_05, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_05_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture5 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture5);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_06, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_06_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture6 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture6);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_07, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_07_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture7 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture7);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_08, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_08_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture8 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture8);
	}
	
// ----------------------------------------------------
// CBlidSatelliteControl::UpdateGlobeVisual
// ----------------------------------------------------
//
void CBlidSatelliteControl::UpdateGlobeVisual( )
	{
    if(KNumOfStartUpIcon > iDisplayIcon)
        {
        iDisplayIcon++;
        }
    else
        {
        iDisplayIcon = 1;
        }
    //iAnimationVisual->SetSize( animationRect.Size() );
    iAnimationVisual->SetImage( TAlfImage( *iTextures[iDisplayIcon-1] ) );//N   
    }
    
// ----------------------------------------------------
// CBlidSatelliteControl::CreateSatelliteSignalVisualsL
// ----------------------------------------------------
//
void CBlidSatelliteControl::CreateSatelliteSignalVisualsL( )
	{
	TInt typefaceStyleId = iEnv.TextStyleManager().CreatePlatformTextStyleL(EAknLogicalFontPrimarySmallFont, EAlfTextStyleSmall);
    CAlfTextStyle* style1 = iEnv.TextStyleManager().TextStyle( typefaceStyleId );
    style1->SetBold( ETrue );
    // Set the text font size in pixel
    style1->SetTextPaneHeightInPixels( 26, ETrue );
	
	TRgb rgb;
	AknsUtils::GetCachedColor ( AknsUtils::SkinInstance(),
								rgb,
								KAknsIIDQsnTextColors,
								EAknsCIQsnTextColorsCG1 
								);
	
	CreateSignalLayoutL( style1, rgb );
	}
	
// ----------------------------------------------------
// CBlidSatelliteControl::CreateSignalLayoutL
// ----------------------------------------------------
//
void CBlidSatelliteControl::CreateSignalLayoutL( CAlfTextStyle* aStyle, TRgb aRgb )
	{
	for( TInt i = 0; i < 16; i++ )
		{
		iSignalStrengthRowOne[i] = CAlfImageVisual::AddNewL( *this, iMainAnchorLayout );
		
		iSatNoRowOne[i] = CAlfTextVisual::AddNewL( *this, iMainAnchorLayout );
		iSatNoRowOne[i]->SetColor( aRgb );	
		iSatNoRowOne[i]->SetTextStyle( aStyle->Id() );
		iSatNoRowOne[i]->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
		}
	}
	
// ---------------------------------------------------------
// CBlidSatelliteControl::DisplaySatellitesRowL
// ---------------------------------------------------------
//
void CBlidSatelliteControl::DisplaySatellitesRowL( const TDesC& aSatNo,
												   CAlfImageVisual* aImageVisual,
												   CAlfTextVisual* aTextVisual,
												   TInt aIndex )
	{
	// code to load svg icon
	MAlfBitmapProvider* provider = NULL;
	// first creste the image loader utils and then call SetSize.	
    iView.ImageLoaderUtils()->SetSize( iSignalBarRects[0].Size() );
	
	aTextVisual->SetTextL( aSatNo );
	
	switch( iLocation->GetSatelliteSignalStrength( aIndex ) )
		{
		case EBlidSignalStrengthZero:
			{
			                                                                  
		    // then create the bitmap provider
		    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSignalBar01, // svg icon skin constant id defiened in Aknsconstant.h file
		    												 			iView.ImagePathName(),        // mif file name with path.
		    												 			EMbmBlidQgn_graf_blid_signal_bar_01, // bitmap id defiened in mbg file
		    												 			EMbmBlidQgn_graf_blid_signal_bar_01_mask ); // bitmap mask id defiened in mbg file
		    												 
			    												 
			// now using that bitmap provider create the texture.
			CAlfTexture & texture = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault );	
					                                                                  
			aImageVisual->SetImage(TAlfImage(texture));		                                                                  
			break;							
			}
		case EBlidSignalStrengthOne:
			{
		    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSignalBar02, // svg icon skin constant id defiened in Aknsconstant.h file
		    												 			iView.ImagePathName(),        // mif file name with path.
		    												 			EMbmBlidQgn_graf_blid_signal_bar_02, // bitmap id defiened in mbg file
		    												 			EMbmBlidQgn_graf_blid_signal_bar_02_mask ); // bitmap mask id defiened in mbg file
		    												 
			    												 
			// now using that bitmap provider create the texture.
			CAlfTexture & texture = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault );	
					                                                                  
			aImageVisual->SetImage(TAlfImage(texture));		                                                                  
			break;							
			}
		case EBlidSignalStrengthTwo:
			{
		    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSignalBar03, // svg icon skin constant id defiened in Aknsconstant.h file
		    												 			iView.ImagePathName(),        // mif file name with path.
		    												 			EMbmBlidQgn_graf_blid_signal_bar_03, // bitmap id defiened in mbg file
		    												 			EMbmBlidQgn_graf_blid_signal_bar_03_mask ); // bitmap mask id defiened in mbg file
		    												 
			    												 
			// now using that bitmap provider create the texture.
			CAlfTexture & texture = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault );	
					                                                                  
			aImageVisual->SetImage(TAlfImage(texture));		                                                                  
			break;							
			}
		case EBlidSignalStrengthThree:
			{
		    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSignalBar04, // svg icon skin constant id defiened in Aknsconstant.h file
		    												 			iView.ImagePathName(),        // mif file name with path.
		    												 			EMbmBlidQgn_graf_blid_signal_bar_04, // bitmap id defiened in mbg file
		    												 			EMbmBlidQgn_graf_blid_signal_bar_04_mask ); // bitmap mask id defiened in mbg file
		    												 
			    												 
			// now using that bitmap provider create the texture.
			CAlfTexture & texture = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault );	
					                                                                  
			aImageVisual->SetImage(TAlfImage(texture));		                                                                  
			break;							
			}
		case EBlidSignalStrengthFour:
			{
		    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSignalBar05, // svg icon skin constant id defiened in Aknsconstant.h file
		    												 			iView.ImagePathName(),        // mif file name with path.
		    												 			EMbmBlidQgn_graf_blid_signal_bar_05, // bitmap id defiened in mbg file
		    												 			EMbmBlidQgn_graf_blid_signal_bar_05_mask ); // bitmap mask id defiened in mbg file
		    												 
			    												 
			// now using that bitmap provider create the texture.
			CAlfTexture & texture = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault );	
					                                                                  
			aImageVisual->SetImage(TAlfImage(texture));		                                                                  
			break;							
			}
		default:
			{
			break;							
			}
		}
	}

// ----------------------------------------------------
// CBlidSatelliteControl::ChangeNavigationViewL
// ----------------------------------------------------
//
void CBlidSatelliteControl::ChangeNavigationViewL()
    {
    if (iView.IsForeGroundApp())
        {
        iView.ActivateNavigationViewL();
        }
    //delete the timer if present
    delete iTimer;
    iTimer = NULL;
    }

// ----------------------------------------------------
// CBlidSatelliteControl::ChangeSoftKeyL
// ----------------------------------------------------
//
void CBlidSatelliteControl::ChangeSoftKeyL()
	{
	iView.ChangeViewSoftKeyL();
	}

// ----------------------------------------------------
// CBlidSatelliteControl::UpdateDisplayLayoutL
// ----------------------------------------------------
//
void CBlidSatelliteControl::UpdateDisplayLayoutL( )
	{
	CalculateRects();
	SetAnchors();
	}

// ----------------------------------------------------
// CBlidSatelliteControl::CalculateRects
// ----------------------------------------------------
//	
void CBlidSatelliteControl::CalculateRects()
	{
	TAknLayoutRect mainBlid2Pane;
	TAknLayoutRect layoutWindow;
	TRect mainRect;
	TRect animationRect;
	TInt variety;

	if( Layout_Meta_Data::IsLandscapeOrientation() )
	    {
	    variety = 1;
	    }
	 else
	 	{
	 	variety = 0;
	 	}
	
	/////////////// calculate animation  rect /////////////////////////
    mainBlid2Pane.LayoutRect( iView.ApplicationRect(),
    							AknLayoutScalable_Apps::main_blid2_pane().LayoutLine() );
    							
	mainRect = mainBlid2Pane.Rect(); 							
    																				   
    mainBlid2Pane.LayoutRect( mainRect, 
    							AknLayoutScalable_Apps::blid2_gps_pane( 0 ).LayoutLine() );
    							
	animationRect =  mainBlid2Pane.Rect();   							
    							
	mainBlid2Pane.LayoutRect( animationRect, 
    							AknLayoutScalable_Apps::blid2_gps_pane_g1( variety ).LayoutLine() );
	
	iGlobeRect = mainBlid2Pane.Rect();
	
	/////////////// calculate satellite grid rect /////////////////////////
	mainBlid2Pane.LayoutRect( animationRect, 
    						  AknLayoutScalable_Apps::grid_blid2_satellite_pane( variety ).LayoutLine() );
    							
	iSatGridRect = mainBlid2Pane.Rect();
	
	/////////////// calculate cell rects /////////////////////////
	TAknLayoutScalableParameterLimits paramLimits;
	paramLimits = AknLayoutScalable_Apps::cell_blid2_satellite_pane_ParamLimits();
	
	TInt numberOfRows = paramLimits.LastRow() - paramLimits.FirstRow() + 1;
	TInt numberOfColumns = paramLimits.LastColumn() - paramLimits.FirstColumn() + 1;
	iSignalBarRects.Reset();
	iSatelliteNumberRects.Reset();
	for( TInt row = paramLimits.FirstRow() ; row <= paramLimits.LastRow(); ++row )
		{
		for( TInt column = paramLimits.FirstColumn() ; column <= paramLimits.LastColumn(); ++column )
			{
			layoutWindow.LayoutRect( iSatGridRect, AknLayoutScalable_Apps::cell_blid2_satellite_pane(0, column, row).LayoutLine() );
			TRect cellRect = layoutWindow.Rect();
			
			layoutWindow.LayoutRect( cellRect, AknLayoutScalable_Apps::cell_blid2_satellite_pane_g1(0).LayoutLine());
			TRect barRect = layoutWindow.Rect();
			iSignalBarRects.Append(barRect);
			
			// Save the layouts of the satellite numbers
			TAknLayoutText satNumberLayout;
			satNumberLayout.LayoutText(cellRect, AknLayoutScalable_Apps::cell_blid2_satellite_pane_t1(0).LayoutLine());			
			iSatelliteNumberRects.Append(satNumberLayout.TextRect());
			}
		}		
	}
	
// ----------------------------------------------------
// CBlidSatelliteControl::SetAnchors
// ----------------------------------------------------
//	
void CBlidSatelliteControl::SetAnchors()
	{
	// Anchor for Globe Animation
	TAlfRealPoint topAnimationPoint( iGlobeRect.iTl );
	TAlfRealPoint bottomAnimationPoint( iGlobeRect.iBr );

    iMainAnchorLayout->SetAnchor( EAlfAnchorTopLeft, 0, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topAnimationPoint.iX, topAnimationPoint.iY ) );
    iMainAnchorLayout->SetAnchor( EAlfAnchorBottomRight, 0, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomAnimationPoint.iX, bottomAnimationPoint.iY ) );
    
    // Set anchor for all signal strength images	
	for( TInt i = 0; i < 16; i++ )
		{
		TInt ordinal = i*2 + 1;
		
		TAlfRealPoint topPoint( iSignalBarRects[i].iTl );
		TAlfRealPoint bottomPoint( iSignalBarRects[i].iBr );
		
	    iMainAnchorLayout->SetAnchor(EAlfAnchorTopLeft, ordinal, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    	iMainAnchorLayout->SetAnchor(EAlfAnchorBottomRight, ordinal, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );		
		}
	// Set anchor for Satellite number text	
	for( TInt i = 0; i < 16; i++ )
		{
		TInt ordinal = i*2 + 2;
		
		TAlfRealPoint topPoint( iSatelliteNumberRects[i].iTl );
		TAlfRealPoint bottomPoint( iSatelliteNumberRects[i].iBr );
		
	    iMainAnchorLayout->SetAnchor(EAlfAnchorTopLeft, ordinal, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    	iMainAnchorLayout->SetAnchor(EAlfAnchorBottomRight, ordinal, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );		
		}    	
	
	}
	
// ----------------------------------------------------------------------------
// TimerCallBack
// ----------------------------------------------------------------------------
//
TInt TimerCallBack(TAny* aPtr)
    {
    CBlidSatelliteControl* ptr = static_cast<CBlidSatelliteControl*>(aPtr);
    if( ptr )
        {
		// Activate navigation view
		TRAP_IGNORE( ptr->ChangeNavigationViewL() );
        }
    return 0;
    }	
//End Of File



