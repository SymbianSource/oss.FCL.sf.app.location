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


// SYSTEM INCLUDES
#include <coemain.h>
#include <akncontext.h> 
#include <aknnavide.h> 
#include <eikspane.h> 
#include <StringLoader.h> 
#include <akntitle.h> 
#include <akntabgrp.h>
#include <blid.mbg>
#include <Blid.rsg>
#include <aknlayoutscalable_avkon.cdl.h>
#include <centralrepository.h>
#include <aknnotedialog.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknUtils.h>
#include <AknsConstants.h>

#include <alf/alfenv.h>
#include <alf/alfanchorlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alfborderbrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfevent.h>
#include <alf/alftransformation.h>
#include <alf/alfimagevisual.h>
#include <alf/alftexture.h>
#include <alf/alfroster.h>
#include <alf/alflinevisual.h>
#include <alf/alfcurvepath.h>
#include <alf/alfcurvepathlayout.h>
#include <alf/alfimagebrush.h>
#include <alf/alfframebrush.h>
#include <alf/alfdecklayout.h>
#include <alf/alftextstyle.h>
#include <alf/alfutil.h>
#include <alf/alfimageloaderutil.h>
// USER INCLUDES
#include "CBlidBaseView.h"
#include "CBlidEng.h"
#include "CHtkTripMeterControl.h"
#include "Blid.hrh"
#include "CHtkTripMeterControl.h"
#include "MBlidRouter.h"
#include "MBlidSettings.h"
#include "MBlidLocation.h"
#include "e32const.h"
#include "Blidutils.h"
#include "bliduiconsts.h"
#include "BlidSettingsCrkeys.h"
#include "Debug.h"

_LIT( KText1,  " " );
_LIT( KDistanceFormat,"%.1f" );
const TInt KTimerInterval = 200000;

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CHtkTripMeterControl::NewL()
// ---------------------------------------------------------
//
CHtkTripMeterControl* CHtkTripMeterControl::NewL(CAlfEnv& aEnv,
				    							MKeyProcessor& aKeyProcessor,
		                                        MObjectProvider* /*aParent*/,
				                                const TRect& aRect,
		                                        MBlidLocation* aLocation,
		                                        MBlidRouter* aRouter,
		                                        CBlidEng& aEngine,
		                                        CBlidBaseView& aView)
    {
	CHtkTripMeterControl* self =
		new (ELeave) CHtkTripMeterControl( aEnv, aKeyProcessor, *aRouter, aView, aEngine );
	CleanupStack::PushL(self);
	self->ConstructL(aEnv, aRect, aLocation, aEngine );
    CleanupStack::Pop( self ); // self
	return self;
    }


// ---------------------------------------------------------
//
//CHtkTripMeterControl::CHtkTripMeterControl()
// ---------------------------------------------------------
//
CHtkTripMeterControl::CHtkTripMeterControl(CAlfEnv& aEnv,
					    					MKeyProcessor& /*aKeyProcessor*/,
				                            MBlidRouter& aRouter,
					                        CBlidBaseView& aView,
					                        CBlidEng& aEngine )
    : CAlfControl( ),iRouter( aRouter ),iView( aView ), iEngine(aEngine),iEnv(aEnv)

    {
    }


// ---------------------------------------------------------
// CHtkTripMeterControl::ConstructL()
// ---------------------------------------------------------
//
void CHtkTripMeterControl::ConstructL( CAlfEnv& aEnv,
										const TRect& /*aRect*/,
					                    MBlidLocation* aLocation,
					                    CBlidEng& aEngine )
    {
    
    CAlfControl::ConstructL( aEnv );
    
    iLocation = aLocation ;
    iSpeedCount = 0;
    iSettingsModel = aEngine.SettingsModel();
    
    iDurationFormatLong = CEikonEnv::Static()->AllocReadResourceL( 
                      R_QTN_TIME_DURAT_LONG_WITH_ZERO );

    iSpeedFormatKmph = CEikonEnv::Static()->AllocReadResourceL( R_BLID_METRIC_SPEED_FORMAT );
    
    iSpeedFormatMph = CEikonEnv::Static()->AllocReadResourceL( R_BLID_IMPERIAL_SPEED_FORMAT );                       

		
    /// item's formats
    iDataBlank = StringLoader::LoadL( R_BLID_BLANK, CEikonEnv::Static() );    
	iTripDistance = HBufC::NewL( KBlidLBItemMaxLen );
	iOdometerDistance = HBufC::NewL( KBlidLBItemMaxLen );
	iImagePath = HBufC::NewL( iEnv.TextureManager().ImagePath().Length() + 1 );
	iImagePath->Des().Copy( iEnv.TextureManager().ImagePath() );
	
	iRepository = CRepository::NewL( KCRUidBlidOdometerSettings );
	TInt error = iRepository->Get( KBlidOdometerSettingsValue, iLastodometerValue );
	if( error == KErrNone )
		{
		iOdoValueToStore = iLastodometerValue;
		FormatOdometerDistance( iLastodometerValue );
		}

    TInt textStyleId = iEnv.TextStyleManager().
        CreatePlatformTextStyleL( EAknLogicalFontDigitalFont, 
                                  EAlfTextStyleSmall );
    iStyle1 = iEnv.TextStyleManager().TextStyle( textStyleId );                                  
    iStyle1->SetBold( ETrue );
   	iStyle1->SetTextSizeInPixels( 22, ETrue );

    // Create a single text visual. The pointer to the visual is stored
    // as a member variable for later access.
	CreateNaviPaneL( EFalse );
    ActivateTabL( *iDecoratedTabGroup, EBlidNavigationView );
    
    iIsLessThanInitial = EFalse;
    iSTripDimmed = EFalse;
    
    CalculateRects();
    iTripLayout = CAlfAnchorLayout::AddNewL( *this );
    CreateTripVisualsL();// ordinal 0 to 8
	CreateSpeedVisualsL();// ordinal 9 to 17
	CreateSignalTexturesL();
	CreatePopupImgTextureL();
	// image visual for signal strength
	iImageVisual = CAlfImageVisual::AddNewL(*this, iTripLayout);// ordinal 18
	
	LaunchDeviceNotAvlNoteL( );
	SetAnchors();
    
    // Make the dial in zeroth position
    iNextAngle = -( ( 240.0 / iMaxScale ) * iCurrSpeed ) + 30;  	
	RotateSpeedDialL( iPrevAngle, iNextAngle );
	iPrevAngle = iNextAngle;	      
	isDeviceUnavailableDisplayed = EFalse;	      
	iIsDownPressed = EFalse;	      
    }


// ---------------------------------------------------------
// CHtkTripMeterControl::CreateSpeedVisualsL()
// ---------------------------------------------------------
//
void CHtkTripMeterControl::CreateSpeedVisualsL()
	{
	TInt j = 0;
	HBufC* lTextSpeed = HBufC::NewLC( 20 );
	
	for ( TInt i = 0; i < 9; i++ )
		{
		iMileVisual[i] = CAlfTextVisual::AddNewL( *this, iTripLayout );// ordinal 9 to 17
		lTextSpeed->Des().Zero();
		lTextSpeed->Des().AppendNum( j );
		j = j + 5;
		iMileVisual[i]->SetTextL( *lTextSpeed );
		iMileVisual[i]->SetTextStyle( iStyle1->Id() );
		iMileVisual[i]->SetColor( KRgbWhite );
		iMileVisual[i]->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
		iMileVisual[i]->SetPadding(TAlfBoxMetric(TAlfMetric(.04, EAlfUnitNormalized)));			
		}

	iMaxScale = j - 5;		
	CleanupStack::PopAndDestroy( lTextSpeed );
	}

// ----------------------------------------------------
// CHtkTripMeterControl::CreateGlobeVisualL
// ----------------------------------------------------
//
void CHtkTripMeterControl::CreatePopUpVisualL( )
	{
	// code to load svg icon
	MAlfBitmapProvider* provider = NULL;
	
	// first creste the image loader utils and then call SetSize.	
    iView.ImageLoaderUtils()->SetSize( TAlfRealSize( 60, 60 ) );
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_01, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_01_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture1 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iPopupTextures.Append(&texture1);
    												 
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_02, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_02_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture2 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iPopupTextures.Append(&texture2);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_03, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_03_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture3 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iPopupTextures.Append(&texture3);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_04, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_04_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture4 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iPopupTextures.Append(&texture4);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_05, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_05_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture5 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iPopupTextures.Append(&texture5);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_06, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_06_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture6 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iPopupTextures.Append(&texture6);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_07, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_07_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture7 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iPopupTextures.Append(&texture7);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSearching, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_searching_08, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_searching_08_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture8 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iPopupTextures.Append(&texture8);
	}
	
// ---------------------------------------------------------
// CHtkTripMeterControl::ChangeSpeedometerScaleL()
// ---------------------------------------------------------
//
void CHtkTripMeterControl::ChangeSpeedometerScaleL()
	{
	TInt j = 0;
	HBufC* lTextSpeed = HBufC::NewLC( 20 );
	for ( TInt i = 0; i < 9; i++ )
		{
		lTextSpeed->Des().Zero();
		if( iCurrSpeed > 40 &&
			iCurrSpeed <= 200 )
			{
			lTextSpeed->Des().AppendNum( j );
			j = j + 25;	
			}
		else if( iCurrSpeed <= 40 )
			{
			lTextSpeed->Des().AppendNum( j );			
			j = j + 5;		
			}
		iMileVisual[i]->SetTextL( *lTextSpeed );
		}
		
	if( iCurrSpeed > 40 &&
		iCurrSpeed <= 200 )
		{
		iMaxScale = j - 25;
		}
	else if( iCurrSpeed < 40 )
		{
		iMaxScale = j - 5;
		}

	CleanupStack::PopAndDestroy( lTextSpeed );

	}

// ---------------------------------------------------------
// CHtkTripMeterControl::CreateSpeedFormatsL()
// ---------------------------------------------------------
//
void CHtkTripMeterControl::CreateSpeedFormatsL()
	{
	iSpeedFormatVisual = CAlfTextVisual::AddNewL( *this, iTripLayout );// ordinal 18
	TInt typefaceStyleId = iEnv.TextStyleManager().CreatePlatformTextStyleL(EAknLogicalFontPrimarySmallFont, EAlfTextStyleSmall);
	
	if ( iSettingsModel->UnitOfMeasurement() == EUnitsMetric )
		{
		iSpeedFormatVisual->SetTextL( *iSpeedFormatKmph );		
		}
	else
		{
		iSpeedFormatVisual->SetTextL( *iSpeedFormatMph );	
		}
	iSpeedFormatVisual->SetTextStyle( typefaceStyleId );
	iSpeedFormatVisual->SetColor( KRgbWhite );
	iSpeedFormatVisual->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );	
	iSpeedFormatVisual->SetPadding(TAlfBoxMetric(TAlfMetric(.03, EAlfUnitNormalized)));
	}

// ---------------------------------------------------------
// CHtkTripMeterControl::CreateTripVisualsL()
// ---------------------------------------------------------
//
void CHtkTripMeterControl::CreateTripVisualsL( )
	{
	MAlfBitmapProvider* provider = NULL;
	
	iBigTripVisual = CAlfImageVisual::AddNewL(*this, iTripLayout);// ordinal 0

    // create the bitmap provider
    iView.ImageLoaderUtils()->SetSize( iBigTripRect.Size() );
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidTpmeter, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_tpmeter, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_tpmeter_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & tripTexture1 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault ); 
		                                                                  
	iBigTripVisual->SetImage(TAlfImage(tripTexture1));// big trip

	iTripDeckVisual = CAlfImageVisual::AddNewL(*this, iTripLayout);// ordinal 1	
	
    iView.ImageLoaderUtils()->SetSize( iTripDeckRect.Size() ); 
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidInfo, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_info, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_info_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & tripTexture2 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault ); 
			                                                                  
	iTripDeckVisual->SetImage( TAlfImage( tripTexture2 ) );//trip table

    iTripMeter = CAlfTextVisual::AddNewL( *this, iTripLayout );// ordinal 2
	TRgb rgb;
	AknsUtils::GetCachedColor ( AknsUtils::SkinInstance(),
								rgb,
								KAknsIIDQsnTextColors,
								EAknsCIQsnTextColorsCG1 
								);
								
    iTripMeter->SetColor( rgb );
    iTripMeter->SetClipping( EFalse );
	iTripMeter->SetTextStyle( iStyle1->Id() );	
	iTripMeter->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );

	iOdoDeckVisual = CAlfImageVisual::AddNewL(*this, iTripLayout );// ordinal 3
	
    iView.ImageLoaderUtils()->SetSize( iOdoDeckRect.Size() ); 
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidInfo, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_info, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_info_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & tripTexture6 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault ); 
                                                                	
	iOdoDeckVisual->SetImage( TAlfImage( tripTexture6 ) );//odometer table
	
    iOdoMeter = CAlfTextVisual::AddNewL( *this, iTripLayout );// ordinal 4
	iOdoMeter->SetColor( rgb );
    iOdoMeter->SetClipping( EFalse );
	iOdoMeter->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
	iOdoMeter->SetTextStyle( iStyle1->Id() );
	iOdoMeter->SetTextL( *iOdometerDistance );
	
	// Speed Format
	CreateSpeedFormatsL();// ordinal 5

	iTripDialVisual = CAlfImageVisual::AddNewL(*this, iTripLayout );// ordinal 6
	
    iView.ImageLoaderUtils()->SetSize( iTripDialRect.Size() ); 
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidTpPointer, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_tp_pointer, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_tp_pointer_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & tripTexture4 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault ); 
		                                                                 
	iTripDialVisual->SetImage( TAlfImage( tripTexture4 ) );//pointer
	
	iTripTimeVisual = CAlfImageVisual::AddNewL(*this, iTripLayout );// ordinal 7	

    iView.ImageLoaderUtils()->SetSize( iClockImageRect.Size() ); 
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidTime, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_time, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_time_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & tripTexture7 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault ); 
	
	iTripTimeVisual->SetImage( TAlfImage( tripTexture7 ) );//clock
	
	// trip time text	
    iTriptime = CAlfTextVisual::AddNewL( *this, iTripLayout );// ordinal 8
    iTriptime->SetColor( rgb );
    iTriptime->SetClipping( EFalse );
	iTriptime->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
	iTriptime->SetTextStyle( iStyle1->Id() );
	}

// ---------------------------------------------------------
// CHtkTripMeterControl::~CHtkTripMeterControl()
// ---------------------------------------------------------
//
CHtkTripMeterControl::~CHtkTripMeterControl()
    {
    delete iPeriodic;
    iPeriodic = NULL;
    iPopupTextures.Reset();
    iPopupTextures.Close();
	delete iDecoratedTabGroup;
	delete iDurationFormatLong;
	delete iDataBlank;
	delete iOdometerDistance;
	delete iTripDistance;
	delete iSpeedFormatKmph;
	delete iSpeedFormatMph;
	delete iRepository;
	delete iImagePath;
	iSignalTextures.Reset();
    iSignalTextures.Close();	
    }


// ----------------------------------------------------------------------------
// CHtkTripMeterControl::UpdateL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::UpdateL()
    {
    if((iEngine.AppMode() == CBlidEng::EAppForeground) && 
       (iSettingsModel->GetBacklightOption() == EBlidBackLightOnDuringNavigation) && !iView.IsSettingsViewActive() )
        {
        User::ResetInactivityTime();        
        }
    
	if( iLocation->IsGPSDataAvailable() )
		{
		isDeviceUnavailableDisplayed = EFalse;
		if( iView.CBAPtr() )
			{
			iView.CBAPtr()->SetCommandSetL( R_BLID_SOFTKEYS_OPTIONS_EXIT__CONTEXTOPTIONS );
			iView.CBAPtr()->DrawDeferred();
			}			    
		TInt satCount = 0;
		iLocation->GetNumOfSatellitesL( satCount );
		DisplaySatelliteSignalStrength( satCount );
		if( !iSTripDimmed )
			{
        	iTripLayout->SetOpacity( 1.0 );				
			}
        iPopUpLayout->SetOpacity( 0.0 );
		}
	else
		{
        TFileName imagepathname;		
		DisplaySatelliteSignalStrength( 0 );
        iTripLayout->SetOpacity( 0.3 );
        iPopUpLayout->SetOpacity( 1.0 );
		// for MSK enabled phone add MSK        
		if( iView.CBAPtr() )
			{
			iView.CBAPtr()->SetCommandSetL( R_BLID_OPTION_INFO_EXIT );
			iView.CBAPtr()->DrawDeferred();
			}
		if( iPeriodic )
		        {
		        delete iPeriodic;
		        iPeriodic = NULL;
		        }				    
		if( iLocation->WaitingGPSData() )
		    {
		    isSatelliteInfoLaunch = ETrue;
		    if(KNumOfSearchIcon != iPopupTextures.Count())
		    	CreatePopUpVisualL( );
			HBufC* noteText = StringLoader::LoadLC( R_BLID_STARTUP_NAVITEXT_SEARCHING_GPS_SATELLITES, CEikonEnv::Static() );	
			iPopUpMsg->SetTextL( *noteText );
            iDisplayIcon = 1;
		    iPeriodic = CPeriodic::NewL(0); // neutral priority 
            iPeriodic->Start( TTimeIntervalMicroSeconds32( KTimerInterval ),
                              TTimeIntervalMicroSeconds32( KTimerInterval ),
                              TCallBack(Tick, this));
            UpdateVisualImage();   
			CleanupStack::PopAndDestroy(noteText); //noteText
		    }
        else
            {
            isSatelliteInfoLaunch = EFalse;
		    HBufC* noteText = StringLoader::LoadLC( R_BLID_ERROR_NO_GPS_DEVICE, CEikonEnv::Static() );	
			iPopUpMsg->SetTextL( *noteText );
	    	iPopImage->SetImage(TAlfImage( *iPopupImgTexture ) );
			CleanupStack::PopAndDestroy(noteText); //noteText        	
				if(!isDeviceUnavailableDisplayed)
				{
					CHWRMLight* aHWRMLight = CHWRMLight::NewL();
					isDeviceUnavailableDisplayed = ETrue;
					if(aHWRMLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightOn)
					{
						User::ResetInactivityTime();
						aHWRMLight->LightOnL(CHWRMLight::EPrimaryDisplay);
						aHWRMLight->ReleaseLight(CHWRMLight::EPrimaryDisplay);
            }		    
		        	delete aHWRMLight;	
				}
            }	       	
		}	   	
		
	if( iLocation->HasTripStarted() ||
		iLocation->IsTripCleared() ||
		iLocation->IsTripReset() )
		{
	    SetTRipMeterValueL();
	    SetOdoMeterValueL();
	    SetTripTimeL();
	    SetTripSpeed();
		}
    // logic for rotating the dial
    if( iLocation->HasTripStarted() && 
    	!iLocation->IsTripCleared() && 
        !iLocation->IsTripReset() && 
        iLocation->IsGPSDataAvailable() )
	    {
	    // Calculate Current Speed
	    if ( iSettingsModel->UnitOfMeasurement() == EUnitsMetric )
		    {
	    	iCurrSpeed = ( iLocation->GetTripMaxSpeed() ) * KSpeedConvertion;
		    }
		else
			{
			iCurrSpeed = ( iLocation->GetTripMaxSpeed() ) * KSpeedConvertion;
			iCurrSpeed *= KKilometreToMiles;
			}		    
		// Check if scale has to be changed
		if( iCurrSpeed > 40 && iMaxScale == 40 )
			{
			ChangeSpeedometerScaleL();			
			}
		if( iCurrSpeed <= 40 && iMaxScale == 200 )
			{
			ChangeSpeedometerScaleL();			
			}
		// Calculate Next Angle	
		iNextAngle = -( ( 240.0 / iMaxScale ) * iCurrSpeed ) + 30;
		// Rotate dial  	
		RotateSpeedDialL( iPrevAngle, iNextAngle );
		// Set prev angle
		iPrevAngle = iNextAngle;
				
	    }
    }

// ---------------------------------------------------------
// CHtkTripMeterControl::Tick()
// ---------------------------------------------------------
//
TInt CHtkTripMeterControl::Tick(TAny* aObject)
    {
    ((CHtkTripMeterControl*)aObject)->UpdateVisualImage(); // cast, and call non-static function

    return KErrNone;
    }

// ---------------------------------------------------------
// CHtkTripMeterControl::UpdateVisualImage
// ---------------------------------------------------------
//
void CHtkTripMeterControl::UpdateVisualImage()
	{
	if( isSatelliteInfoLaunch )
		{
	    if(KNumOfSearchIcon > iDisplayIcon)
	        {
	        iDisplayIcon++;
	        }
	    else
	        {
	        iDisplayIcon = 1;
	        } 
	    //iPopImage->SetSize( animationRect.Size() );
	    iPopImage->SetImage( TAlfImage( *iPopupTextures[iDisplayIcon-1] ) );//N  
		}
	}
	
// ---------------------------------------------------------
// CHtkTripMeterControl::DisplayInfoPopUpMsgL
// ---------------------------------------------------------
//
void CHtkTripMeterControl::DisplayInfoPopUpMsgL()
	{
	if( isSatelliteInfoLaunch )
		{
		iView.LaunchSatelliteInfoDlgL();
		}
	else
		{
		LaunchInfoQueryDlgL();	
		}						
	}
	
// ---------------------------------------------------------
// CHtkTripMeterControl::LaunchDeviceNotAvlNoteL
// ---------------------------------------------------------
//
void CHtkTripMeterControl::LaunchDeviceNotAvlNoteL( )
    {
	TInt typefaceStyleId = iEnv.TextStyleManager().CreatePlatformTextStyleL(EAknHighestLogicalFont, EAlfTextStyleNormal); 
    CAlfTextStyle* style1 = iEnv.TextStyleManager().TextStyle( typefaceStyleId );

    iPopUpLayout = CAlfAnchorLayout::AddNewL( *this );
    
    iPopupDeck = CAlfDeckLayout::AddNewL( *this, iPopUpLayout );
    
    iPopUpLayout->SetAnchor(EAlfAnchorTopLeft, 0, 
        EAlfAnchorOriginHCenter, EAlfAnchorOriginTop,
        EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint( -0.05, 0.05));
    iPopUpLayout->SetAnchor(EAlfAnchorBottomRight, 0, 
        EAlfAnchorOriginHCenter, EAlfAnchorOriginTop,
        EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint( 0.4, 0.38));

	iPopupDeck->EnableBrushesL();

    TAlfXYMetric edgeoffset(TAlfMetric( 0.01, EAlfUnitNormalized ));
    iFrameBrush = CAlfFrameBrush::NewL(iEnv, KAknsIIDQsnFrPopup, edgeoffset);
    iFrameBrush->SetFrameRectsL(TRect(12,12,167,167), TRect(11,11,170,170));
    iFrameBrush->SetEdgeOffset(edgeoffset);
    iPopupDeck->Brushes()->AppendL( iFrameBrush, EAlfHasOwnership );
	
    iPopUpAnchor = CAlfAnchorLayout::AddNewL( *this, iPopupDeck );
    
	iPopUpAnchor->EnableBrushesL();
    
    iPopUpMsg = CAlfTextVisual::AddNewL( *this, iPopUpAnchor );
    iPopImage = CAlfImageVisual::AddNewL( *this, iPopUpAnchor );
    style1->SetTextSizeInPixels(20, ETrue );
    style1->SetBold( EFalse );
    
	TRgb rgb;
	AknsUtils::GetCachedColor ( AknsUtils::SkinInstance(),
								rgb,
								KAknsIIDQsnTextColors,
								EAknsCIQsnTextColorsCG1 
							   );
	
	iPopUpMsg->SetColor( rgb );
	iPopUpMsg->SetTextL( KText1 );
	iPopUpMsg->SetTextStyle( style1->Id() );
	iPopUpMsg->SetPadding(TAlfBoxMetric(TAlfMetric(.1, EAlfUnitNormalized)));
	iPopUpMsg->SetWrapping( CAlfTextVisual::ELineWrapBreak );
	iPopUpMsg->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
	
    iPopUpAnchor->SetAnchor(EAlfAnchorTopLeft, 0, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint( 0.02, 0.02));
    iPopUpAnchor->SetAnchor(EAlfAnchorBottomRight, 0, 
        EAlfAnchorOriginHCenter, EAlfAnchorOriginTop,
        EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint( 0.47, 0.3));

    iPopUpAnchor->SetAnchor(EAlfAnchorTopLeft, 1, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint( 0.28, 0.4 ));
    iPopUpAnchor->SetAnchor(EAlfAnchorBottomRight, 1, 
        EAlfAnchorOriginHCenter, EAlfAnchorOriginBottom,
        EAlfAnchorMetricRelativeToSize, EAlfAnchorMetricRelativeToSize,
        TAlfTimedPoint( 0.22, -0.16 ) );

    iPopUpLayout->SetOpacity( 0.0 );        
    }
    
// ---------------------------------------------------------
// CHtkTripMeterControl::DisplaySatelliteSignalStrength
// ---------------------------------------------------------
//
void CHtkTripMeterControl::DisplaySatelliteSignalStrength( TInt aSatCount ) 
	{
	TFileName imagepathname;
	imagepathname.Zero();
	iImageVisual->SetImage(TAlfImage(*iSignalTextures[aSatCount]));
	}
	
// ---------------------------------------------------------
// CHtkTripMeterControl::LaunchInfoQueryDlgL
// ---------------------------------------------------------
//
void CHtkTripMeterControl::LaunchInfoQueryDlgL()
	{
    CAknNoteDialog* dlg;
    dlg = new (ELeave) CAknNoteDialog(CAknNoteDialog::EConfirmationTone,CAknNoteDialog::ENoTimeout);
    TInt result = dlg->ExecuteLD( R_INFO_POPUP_MSG_QUERY );
    if( result == EAknSoftkeyExit )
	    {
	    iView.ExitMainApplicationL( EEikCmdExit );
	    }
	dlg = NULL;	    
	}
		
// ---------------------------------------------------------
// CHtkTripMeterControl::HasTripStarted
// ---------------------------------------------------------
//
TBool CHtkTripMeterControl::HasTripStarted()
    {
    return iLocation->HasTripStarted();
    }

// ---------------------------------------------------------
// CHtkTripMeterControl::WasGPSDataLost
// ---------------------------------------------------------
//
TBool CHtkTripMeterControl::WasGPSDataLost(void)
    {
    return iLocation->WasGPSDataLost();
    }
        
// ---------------------------------------------------------
// CHtkTripMeterControl::SetGPSDataAvailability
// ---------------------------------------------------------
//
void CHtkTripMeterControl::SetGPSDataAvailability(TBool aAvailability)
    {
    iLocation->SetGPSDataAvailability(aAvailability);
    }

// ---------------------------------------------------------
// CHtkTripMeterControl::ProcessCommandL
// ---------------------------------------------------------
//
void CHtkTripMeterControl::ProcessCommandL(TInt aCommand)
    {
    ReChangeTripFocus();
	switch(aCommand)
		{
		case EBlidCmdTripStart:
			{
			/**
			 * Start the calculations for the current trip
			 */
			iLocation->SetTripStartL();
			break;
			}
        case EBlidCmdTripStop:
        	{
        	/**
			* Stop the calculations for the current trip
			*/        	
        	iLocation->SetTripStop();
        	break;
			}
        case EBlidCmdTripResume:
        	{
        	/**
			* Resume the calculations for the current trip after stopping
			*/			
			iLocation->SetTripResumeL();
			break;
			}
        case EBlidCmdTripReset:
        	{
        	/**
			* Reset the items only for the current trip
			*/
        	iLocation->ResetTrip();
        	break;
			}
        case EBlidCmdTripClear:
        	{
         	/**
			* Reset all the items in the trip when it is stopped
			*/
			RotateSpeedDialL( iPrevAngle, 30 );    
			iLocation->SetTripClear();
	        break;
			}
		default:
			{
			break;
			}
		}
		UpdateL();
    }
    

// ----------------------------------------------------
// CHtkTripMeterControl::CreateNaviPaneL
// ----------------------------------------------------
//
void CHtkTripMeterControl::CreateNaviPaneL( TBool aDefaultNaviPane )
    {    
    CEikStatusPane* sp = StatusPane();
    // Fetch pointer to the default navi pane control
    CAknNavigationControlContainer* naviPane = 
        static_cast<CAknNavigationControlContainer*>(sp->ControlL( 
        TUid::Uid(EEikStatusPaneUidNavi)));
 
    if( iDecoratedTabGroup )
	    {
	    delete iDecoratedTabGroup;
	    iDecoratedTabGroup = NULL;	
	    }
   
    if ( aDefaultNaviPane ) // dummy tab
        {        
        naviPane->PushDefaultL(EFalse);
        }
    else
        {              
        iDecoratedTabGroup = naviPane->CreateTabGroupL();
        iDecoratedTabGroup->SetControlType( CAknNavigationDecorator::ETabGroup );
        CAknTabGroup* tabGroup = 
        static_cast< CAknTabGroup* > ( iDecoratedTabGroup->DecoratedControl() );        
        AddTabsL( *tabGroup );        
        }
    }


// ----------------------------------------------------
// CHtkTripMeterControl::GetNiviDecorator
// ----------------------------------------------------
//
CAknNavigationDecorator* CHtkTripMeterControl::GetNiviDecorator( )
	{
	return iDecoratedTabGroup;	
	}
	
	    
// ----------------------------------------------------
// CHtkTripMeterControl::ActivateTabL
// ----------------------------------------------------
//
void CHtkTripMeterControl::ActivateTabL( 
                         CAknNavigationDecorator& aNaviDecorator,
                         TInt aTabId )
    {
    CEikStatusPane* sp = StatusPane();    
    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>(
        sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( 
        aNaviDecorator.DecoratedControl() );
    tabGroup->SetActiveTabById( aTabId );
    naviPane->PushL( aNaviDecorator );
    }


// ----------------------------------------------------
// CHtkTripMeterControl::StatusPane
// ----------------------------------------------------
//
CEikStatusPane* CHtkTripMeterControl::StatusPane()
    {
    return CEikonEnv::Static()->AppUiFactory()->StatusPane();
    }


// ----------------------------------------------------
// CHtkTripMeterControl::MakeTitleL
// ----------------------------------------------------
//
void CHtkTripMeterControl::MakeTitleL( TInt aResourceText )
    {
    HBufC* buf = StringLoader::LoadLC(aResourceText);       
    CAknTitlePane* title = static_cast<CAknTitlePane*>( StatusPane()->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    CleanupStack::Pop(); //buf
    title->SetText(buf); // Takes ownership of buf
    }


// ----------------------------------------------------
// CHtkTripMeterControl::AddTabsL
// ----------------------------------------------------
//
void CHtkTripMeterControl::AddTabsL(CAknTabGroup& aTabGroup)
    {
    aTabGroup.SetTabFixedWidthL(EAknTabWidthWithTwoTabs);

    MAknsSkinInstance* skin = AknsUtils::SkinInstance(); 
    HBufC* filename = BlidUtils::GetBitmapFilenameLC();        
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
        
    //Add tab icon 1
    AknsUtils::CreateIconLC( skin,
                            KAknsIIDQgnPropBlidNavigationTab3,
                            bitmap,
                            bitmapMask,
                            *filename,
                            EMbmBlidQgn_prop_blid_navigation_tab3,
                            EMbmBlidQgn_prop_blid_navigation_tab3_mask );             
    aTabGroup.AddTabL( EBlidNavigationView, bitmap, bitmapMask );
    CleanupStack::Pop(2); //bitmap, bitmapmask

    //Add tab icon 2
    AknsUtils::CreateIconLC( skin,
                            KAknsIIDQgnPropBlidTripTab3,
                            bitmap,
                            bitmapMask,
                            *filename,
                            EMbmBlidQgn_prop_blid_trip_tab3,
                            EMbmBlidQgn_prop_blid_trip_tab3_mask );             
    aTabGroup.AddTabL( EBlidTripMeterView, bitmap, bitmapMask );
    CleanupStack::Pop(2); //bitmap, bitmapmask

    
    
    CleanupStack::PopAndDestroy(); //filename
    }

// ----------------------------------------------------
// CHtkTripMeterControl::OfferEventL
// ----------------------------------------------------
//
TBool CHtkTripMeterControl::OfferEventL(const TAlfEvent& aEvent)
	{
    if(aEvent.IsKeyEvent() && aEvent.Code() == EEventKey)
        {
        CEikStatusPane* sp = StatusPane();
	    // Fetch pointer to the default navi pane control
	    CAknNavigationControlContainer* naviPane = 
	        static_cast<CAknNavigationControlContainer*>(sp->ControlL( 
	        TUid::Uid(EEikStatusPaneUidNavi)));
	    CAknNavigationDecorator* naviDecorator = naviPane->Top();

	    // Check if navigation pane controls exists, because decorator does 
	    // not exist in every cases. E.g. after contact fetch in "new rule" 
	    // command operation naviDecorator is null. 
	    if ( naviDecorator && naviDecorator->ControlType() == 
	        CAknNavigationDecorator::ETabGroup )
	        {
	        if ( naviDecorator->DecoratedControl()->OfferKeyEventL( 
	            aEvent.KeyEvent(), aEvent.Code() ) == EKeyWasConsumed )
	            {
	            CAknTabGroup* tabGroup = static_cast< CAknTabGroup* > ( naviDecorator->DecoratedControl() );
	            TInt tabIndex = tabGroup->ActiveTabIndex();
				iView.ActivateCorrectViewL( tabIndex );
	            return ETrue;
	            }
	        }    
		}
	else if( aEvent.IsPointerEvent() )
		{
		if ( aEvent.PointerDown() )
			{
			iIsDownPressed = ETrue;
			return ETrue;
			}
		if ( aEvent.PointerUp() && iIsDownPressed )
			{	
			if( iLocation->IsGPSDataAvailable() )
				{
				iView.LaunchTripContextMenuL();
				return ETrue;				
				}
			else
				{
				if( aEvent.Visual() == iPopUpAnchor ||
					aEvent.Visual() == iPopUpMsg ||
					aEvent.Visual() == iPopImage )					  
					{
					if( isSatelliteInfoLaunch )
						{
						iView.LaunchSatelliteInfoDlgL();
						return ETrue;
						}
					else
						{
						LaunchInfoQueryDlgL();	
						return ETrue;						
						}						
					}
				}
			return EFalse;
			}
		}
	return EFalse;
	}
	

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::RotateSpeedDialL()
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::RotateSpeedDialL( int aThetaPrevious, int aThetaFinal )
    {
	iTripDialVisual->EnableTransformationL(ETrue);
	CAlfTransformation& aXf = iTripDialVisual->Transformation();
	aXf.LoadIdentity();
	TAlfTimedValue angle;
	angle.SetValueNow( aThetaPrevious );
	angle.SetTarget( aThetaFinal, 8000 );
	aXf.Rotate(angle); 
    }

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::SetTRipMeterValueL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::SetTRipMeterValueL( )
    {
	// set the tripmeter distance
	TBuf<128> tripmeterString;	
	TripmeterFormatL( tripmeterString );
	iTripMeter->SetTextL( *iTripDistance );
    }
    
// ----------------------------------------------------------------------------
// CHtkTripMeterControl::SetOdoMeterValueL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::SetOdoMeterValueL( )
    {
	// set the odometer distance
	TBuf<128> odometerString;	
	OdometerFormat( odometerString );
	iOdoMeter->SetTextL( *iOdometerDistance );
    }

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::SetTripTimeL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::SetTripTimeL( )
    {
	// set the odometer distance
	TBuf<128> odometerString;	
	TotalTimeFormat( odometerString );
	iTriptime->SetTextL( odometerString );
    }

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::SetTripSpeed
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::SetTripSpeed( )
    {
    // Nothing to do now
    }

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::FormatOdometerDistance
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::FormatOdometerDistance( TReal aLastodometerValue )    
	{
    TInt result(0);        
    if ( BlidUtils::TRealToTInt(aLastodometerValue, result) != KErrNone )
        {
        iOdometerDistance->Des().Copy( *iDataBlank );
        return;
        }
    aLastodometerValue = result;        
    TReal temp;
                    
    // metric or imperial
    if ( iSettingsModel->UnitOfMeasurement() == EUnitsMetric )
        {

        if ( aLastodometerValue >= KKilometerToMeters )
            {
            TReal distanceInKm = aLastodometerValue / KKilometerToMeters;
            Math::Round(temp, distanceInKm, 1);
            distanceInKm = temp;
            TBuf<KBlidLBItemMaxLen> temp;        
            temp.Format( KDistanceFormat, Abs(distanceInKm) );
            iOdometerDistance->Des().Copy( temp );
            }
    	else
        	{
            if ( BlidUtils::TRealToTInt(aLastodometerValue, result) != KErrNone )
            	{
                iOdometerDistance->Des().Copy( *iDataBlank );
            	}
            else
            	{
            	Math::Mod(temp, result, 10);        	
		        if( temp >= 5 )
		        	{
		            result += (10 - temp);
		            }
		        else
		        	{
		            result -= temp; 
		        	}
		        	
		        if ( result >= KKilometerToMeters )
	            	{
	            	aLastodometerValue = result;
	            	TReal distanceInKm = aLastodometerValue / KKilometerToMeters;
	                Math::Round(temp, distanceInKm, 1);
	                distanceInKm = temp;
	                TBuf<KBlidLBItemMaxLen> temp;        
	                temp.Format( KDistanceFormat, Abs(distanceInKm) );
	                iOdometerDistance->Des().Copy( temp );
	            	}
	            else
	            	{
	            	iOdometerDistance->Des().Zero();
	                iOdometerDistance->Des().AppendNum( result );
	            	}				
            	}
        	}
        }
    else
    	{
    	TReal32 distanceImperial = aLastodometerValue*KMetreToFoot;
            
    	if ( distanceImperial >= KMileToFeets ) 
        	{
        	distanceImperial /= KMileToFeets;
        	Math::Round(temp, distanceImperial, 1);
        	distanceImperial = temp;
        	TBuf<KBlidLBItemMaxLen> tempstring;
            tempstring.Format( KDistanceFormat, Abs(distanceImperial) );
            iOdometerDistance->Des().Copy( tempstring );
        	}
    	else
    		{                
            if ( BlidUtils::TRealToTInt(
                     distanceImperial, result) != KErrNone )
            	{
                iOdometerDistance->Des().Copy( *iDataBlank );
                }
            else
            	{
            	Math::Mod(temp, result, 10);        	
	            if( temp >= 5 )
	                {
	                result += (10 - temp);
	                }
	            else
	                {
	                result -= temp; 
	                }
	                
	            if ( result >= KMileToFeets )
	            	{
	            	distanceImperial = result;
	            	distanceImperial /= KMileToFeets;
	            	Math::Round(temp, distanceImperial, 1);
	            	distanceImperial = temp;
	            	TBuf<KBlidLBItemMaxLen> temp;
	                temp.Format( KDistanceFormat, Abs(distanceImperial) );
	                iOdometerDistance->Des().Copy( temp );
	            	}
	            else
	            	{
	            	iOdometerDistance->Des().Zero();
	                iOdometerDistance->Des().AppendNum( result );
	            	}                	
                }
            }
    	}
        		
	}

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::OdometerFormat
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::OdometerFormat( TDes& aOdometerString )
    {
    aOdometerString.Zero();
   
    if ( iLocation->IsTotalDistanceTravelled() &&
    	 iLocation->IsGPSDataAvailable() )
        {
        TReal32 totalDistance;
        iLocation->GetTotalDistance( totalDistance );
        totalDistance = totalDistance + iLastodometerValue;
        iOdoValueToStore = totalDistance;
        TInt result(0);        
        if ( BlidUtils::TRealToTInt(totalDistance, result) != KErrNone )
            {
            aOdometerString = *iDataBlank;
            iOdometerDistance->Des().Copy( aOdometerString );
            return;
            }
        totalDistance = result;        
        TReal temp;
                        
        // metric or imperial
        if ( iSettingsModel->UnitOfMeasurement() == EUnitsMetric )
            {
 
            if ( totalDistance >= KKilometerToMeters )
                {
                TReal distanceInKm = totalDistance / KKilometerToMeters;
                Math::Round(temp, distanceInKm, 1);
                distanceInKm = temp;
                TBuf<KBlidLBItemMaxLen> temp;        
                temp.Format( KDistanceFormat, Abs(distanceInKm) );
                iOdometerDistance->Des().Copy( temp );
                }
        	else
            	{
                if ( BlidUtils::TRealToTInt(totalDistance, result) != KErrNone )
                	{
                    aOdometerString = *iDataBlank;
                    iOdometerDistance->Des().Copy( aOdometerString );
                	}
                else
                	{
                	Math::Mod(temp, result, 10);        	
			        if( temp >= 5 )
			        	{
			            result += (10 - temp);
			            }
			        else
			        	{
			            result -= temp; 
			        	}
			        	
			        if ( result >= KKilometerToMeters )
		            	{
		            	totalDistance = result;
		            	TReal distanceInKm = totalDistance / KKilometerToMeters;
		                Math::Round(temp, distanceInKm, 1);
		                distanceInKm = temp;
		                TBuf<KBlidLBItemMaxLen> temp;        
		                temp.Format( KDistanceFormat, Abs(distanceInKm) );
		                iOdometerDistance->Des().Copy( temp );
		            	}
		            else
		            	{
		            	iOdometerDistance->Des().Zero();
		                iOdometerDistance->Des().AppendNum( result );
		            	}				
                	}
            	}
            }
        else
        	{
        	TReal32 distanceImperial = totalDistance*KMetreToFoot;
                
        	if ( distanceImperial >= KMileToFeets ) 
            	{
            	distanceImperial /= KMileToFeets;
            	Math::Round(temp, distanceImperial, 1);
            	distanceImperial = temp;
            	TBuf<KBlidLBItemMaxLen> tempstring;
                tempstring.Format( KDistanceFormat, Abs(distanceImperial) );
                iOdometerDistance->Des().Copy( tempstring );
            	}
        	else
        		{                
                if ( BlidUtils::TRealToTInt(
                         distanceImperial, result) != KErrNone )
                	{
                    aOdometerString = *iDataBlank;
                    iOdometerDistance->Des().Copy( aOdometerString );
                    }
                else
                	{
                	Math::Mod(temp, result, 10);        	
		            if( temp >= 5 )
		                {
		                result += (10 - temp);
		                }
		            else
		                {
		                result -= temp; 
		                }
		                
		            if ( result >= KMileToFeets )
		            	{
		            	distanceImperial = result;
		            	distanceImperial /= KMileToFeets;
		            	Math::Round(temp, distanceImperial, 1);
		            	distanceImperial = temp;
		            	TBuf<KBlidLBItemMaxLen> temp;
		                temp.Format( KDistanceFormat, Abs(distanceImperial) );
		                iOdometerDistance->Des().Copy( temp );
		            	}
		            else
		            	{
		            	iOdometerDistance->Des().Zero();
		                iOdometerDistance->Des().AppendNum( result );
		            	}                	
                    }
                }
        	}
        }
    }

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::TotalTimeFormat
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::TotalTimeFormat( TDes& aTotalTimeString )
    {
    TInt error(0);
    TTimeIntervalSeconds timeInterval;
    TTime totaltime;
    iLocation->GetTotalTime(timeInterval);
    TBuf<KBlidLBItemMaxLen> totaltimeString;
    if ( BlidUtils::FormatTimeToString( totaltimeString, (TReal)(timeInterval.Int()))
         != KErrNone )
        {
        return;
        }
    totaltime.Set( totaltimeString );
    TRAP( error, totaltime.FormatL( 
          aTotalTimeString , *iDurationFormatLong ));
          
    if(error!=KErrNone)
	    {
	    CEikonEnv::Static()->HandleError( error );
	    aTotalTimeString .Zero();
	    }
    }

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::TripmeterFormatL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::TripmeterFormatL( TDes& aTripmeterString )
    {
    aTripmeterString.Zero();
    TInt result(0);
    TReal temp;    
  
	if ( iLocation->IsTripDistanceTravelled()  &&
    	 iLocation->IsGPSDataAvailable() )
	    {
	    TReal32 distanceTravelled;
	    iLocation->GetTripDistance(distanceTravelled);        
	    // metric or imperial
	    if ( iSettingsModel->UnitOfMeasurement() == EUnitsMetric )
	        {
	        iSpeedFormatVisual->SetTextL( *iSpeedFormatKmph );
	        if ( BlidUtils::TRealToTInt(distanceTravelled, result) != KErrNone )
	        	{
	            aTripmeterString = *iDataBlank;
	            iTripDistance->Des().Copy( aTripmeterString );
	            }
	        else
	        	{            	
	        	distanceTravelled = result;
	                
	            if ( distanceTravelled >= KKilometerToMeters )
	            	{
	            	TReal distanceInKm = distanceTravelled / KKilometerToMeters;
	            	Math::Round(temp, distanceInKm, 1);
	            	distanceInKm = temp;
	                TBuf<KBlidLBItemMaxLen> tempstring;        
	                tempstring.Format( KDistanceFormat, Abs(distanceInKm) );
	            	iTripDistance->Des().Copy( tempstring );
	               	}
	            else
	            	{	            	
	            	Math::Mod(temp, distanceTravelled, 10);        	
			        if( temp >= 5 )
			        	{
			            distanceTravelled += (10 - temp);
			            }
			        else
			        	{
			            distanceTravelled -= temp; 
			        	}
			        
			        if(distanceTravelled >= KKilometerToMeters )
			        	{
			        	TReal distanceInKm = distanceTravelled / KKilometerToMeters;
		            	Math::Round(temp, distanceInKm, 1);
	                	distanceInKm = temp;
		                TBuf<KBlidLBItemMaxLen> tempstring;   
		                tempstring.Format( KDistanceFormat, Abs(distanceInKm) );
		                iTripDistance->Des().Copy( tempstring );
			        	}
			        else
			        	{
		                iTripDistance->Des().Zero();
		                iTripDistance->Des().AppendNum( distanceTravelled );
			        	}
	            	}
	            }
	        }
	    else
	    	{
	    	iSpeedFormatVisual->SetTextL( *iSpeedFormatMph );
	        TReal32 distanceImperial = distanceTravelled*KMetreToFoot; 
	        
	        if ( distanceImperial >= KMileToFeets ) 
	        	{
	        	distanceImperial /= KMileToFeets;
	        	Math::Round(temp, distanceImperial, 1);
	        	distanceImperial = temp;
	        	TBuf<KBlidLBItemMaxLen> temp;
	            temp.Format( KDistanceFormat, Abs(distanceImperial) );
                iTripDistance->Des().Copy( temp );
	        	}
	        else
	        	{
	        	if ( BlidUtils::TRealToTInt(
	                 distanceImperial, result) != KErrNone )
	            	{
	                aTripmeterString = *iDataBlank;
	                iTripDistance->Des().Copy( aTripmeterString );
	                }
	            else
	            	{
	            	Math::Mod(temp, distanceImperial, 10);        	
		            if( temp >= 5 )
		                {
		                distanceImperial += (10 - temp);
		                }
		            else
		                {
		                distanceImperial -= temp; 
		                }
		                
		            if ( distanceImperial >= KMileToFeets )
		            	{
		            	distanceImperial /= KMileToFeets;
		            	Math::Round(temp, distanceImperial, 1);
		            	distanceImperial = temp;
		            	TBuf<KBlidLBItemMaxLen> temp;
		                temp.Format( KDistanceFormat, Abs(distanceImperial) );
		                iTripDistance->Des().Copy( temp );
		            	}
		            else
		            	{
		                iTripDistance->Des().Zero();
		                iTripDistance->Des().AppendNum( distanceImperial );
		            	}
	                }
	        	}            
	        }
   		}    
   }

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::SaveOdometerValue
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::SaveOdometerValue( )
	{
	if( iRepository )
		{
	    iRepository->Set(KBlidOdometerSettingsValue, iOdoValueToStore);		
		}
	}
	
// ----------------------------------------------------------------------------
// CHtkTripMeterControl::ResetOdometerDistanceL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::ResetOdometerDistanceL( )
	{
	if( iOdometerDistance )
		{
		iOdometerDistance->Des().Zero();
		iOdometerDistance->Des().AppendNum( 0 );
		iOdoMeter->SetTextL( *iOdometerDistance );			
		}
	iLocation->ResetOdometerValue();		
	iOdoValueToStore = 0.0;
	iLastodometerValue = 0.0;	
	}

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::SaveOdometerValue
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::ChangeTripFocus( TBool aSTripDimmed )
	{
	iSTripDimmed = aSTripDimmed;
	if( iTripLayout )
		{
		TAlfTimedValue opecity;
		opecity.SetTarget( 0.3, 0 );
		iTripLayout->SetOpacity( opecity );				
		}
	}
	
// ----------------------------------------------------------------------------
// CHtkTripMeterControl::SaveOdometerValue
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::ReChangeTripFocus( )
	{
	iSTripDimmed = EFalse;
	if( iTripLayout )
		{
		TAlfTimedValue opecity;
		opecity.SetTarget( 1.0, 0 );
		iTripLayout->SetOpacity( opecity );				
		}
	}

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::UpdateLayoutL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::UpdateLayoutL( )
	{
	// Call all the root label setanchor and adjust the new LAyout rectangle.
	CalculateRects();	
	SetAnchors();
	}

// ----------------------------------------------------------------------------
// CHtkTripMeterControl::UpdateLayoutL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::CalculateRects( )
	{
	TAknLayoutRect mainPane;
	TAknLayoutRect mainBlid2Pane;
	TAknLayoutRect layoutWindow;
	TAknLayoutText textLayout;
	TRect mainRect;
	TRect mainBlidRect;
	TRect tripMeterRect;
	TRect valueRect;
	TInt variety;
	TInt mainvariety;

	if( Layout_Meta_Data::IsLandscapeOrientation() )
	    {
	    variety = 1;
	    mainvariety = 4;
	    }
	 else
	 	{
	 	variety = 0;
	    mainvariety = 0;
	 	}
	// 	
    mainPane.LayoutRect( iView.ApplicationRect(),
    							AknLayoutScalable_Apps::main_pane(mainvariety).LayoutLine() );
    							
	mainRect = mainPane.Rect(); 
	
    mainBlid2Pane.LayoutRect( mainRect,
    							AknLayoutScalable_Apps::main_blid2_pane().LayoutLine() );
    							
	mainBlidRect = mainBlid2Pane.Rect(); 							
    																				   
    layoutWindow.LayoutRect( mainBlidRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane( 0 ).LayoutLine() );
    							
	tripMeterRect =  layoutWindow.Rect();   							
	// Since the Hitchcock does draw at main pane, we take the reference to
	// Tripmeter(main) pane's.
	tripMeterRect.Move(tripMeterRect.iTl.iX,-tripMeterRect.iTl.iY);  							
	 	
	// Bg rect
    layoutWindow.LayoutRect( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_g1( variety ).LayoutLine() );
    							
	iBigTripRect =  layoutWindow.Rect();
	
	// Trip dial rect
    layoutWindow.LayoutRect( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_g2( variety ).LayoutLine() );
    							
	iTripDialRect =  layoutWindow.Rect();

	// Since the LAF Rect for Dial is 90 degree wrt image, we calculate the rect for 90 degree rotation
	TAlfRealPoint topPoint = iTripDialRect.iTl;
	TAlfRealPoint bottomPoint = iTripDialRect.iBr;
	TReal32 topX 		= topPoint.iX;
	TReal32 topY  		= topPoint.iY;
	TReal32 bottomX 	= bottomPoint.iX;
	TReal32 bottomY  	= bottomPoint.iY;
	TReal32 wt			= bottomX - topX;
	TReal32 ht			= bottomY - topY;	

	TReal32 tX 		= topX+(wt/2)-(ht/2);
	TReal32 tY  	= topY+(ht/2)-(wt/2);
	TReal32 bX		= topX+(wt/2)+(ht/2);
	TReal32 bY		= topY+(ht/2)+(wt/2);

	iTripDialRect.SetRect( tX, tY, bX, bY );
	
	// Trip deck image rect
    layoutWindow.LayoutRect( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_g3( variety ).LayoutLine() );
    							
	iTripDeckRect =  layoutWindow.Rect();

	// Odo meter image rect
    layoutWindow.LayoutRect( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_g4( variety ).LayoutLine() );
    							
	iOdoDeckRect =  layoutWindow.Rect();

	// Trip meter text rect
    textLayout.LayoutText( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_t1( variety ).LayoutLine() );
    							
	iTripMeterRect =  textLayout.TextRect();
	
	// Odo meter text rect
    textLayout.LayoutText( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_t2( variety ).LayoutLine() );
    							
	iOdoMeterRect =  textLayout.TextRect();
	
	// Time text rect
    textLayout.LayoutText( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_t3( variety ).LayoutLine() );
    							
	iTripTimeRect =  textLayout.TextRect();
	
	// Speed Format text rect
    textLayout.LayoutText( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_t3( variety ).LayoutLine() );
    							
	iSpeedFormatRect =  textLayout.TextRect();
	// since we donot have the LAF for Speed Format, we calculate it based on Dial Layout
	iSpeedFormatRect.SetRect( topX, topY+ht/4, bottomX, topY+ht/3 );

	// Clock image rect
    layoutWindow.LayoutRect( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_g5( variety ).LayoutLine() );
    							
	iClockImageRect =  layoutWindow.Rect();

	// Speeds Value rect
    layoutWindow.LayoutRect( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane( variety ).LayoutLine() );
    							
	valueRect =  layoutWindow.Rect();
	
	// Speed Value 1 rect
    textLayout.LayoutText( valueRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane_t1( 0 ).LayoutLine() );
    							
	iValueRect1 =  textLayout.TextRect();
	
	// Speed Value 2 rect
    textLayout.LayoutText( valueRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane_t2( 0 ).LayoutLine() );
    							
	iValueRect2 =  textLayout.TextRect();
	
	// Speed Value 3 rect
    textLayout.LayoutText( valueRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane_t3( 0 ).LayoutLine() );
    							
	iValueRect3 =  textLayout.TextRect();
	
	// Speed Value 4 rect
    textLayout.LayoutText( valueRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane_t4( 0 ).LayoutLine() );
    							
	iValueRect4 =  textLayout.TextRect();
	
	// Speed Value 5 rect
    textLayout.LayoutText( valueRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane_t5( 0 ).LayoutLine() );
    							
	iValueRect5 =  textLayout.TextRect();
	
	// Speed Value 6 rect
    textLayout.LayoutText( valueRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane_t6( 0 ).LayoutLine() );
    							
	iValueRect6 =  textLayout.TextRect();
	
	// Speed Value 7 rect
    textLayout.LayoutText( valueRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane_t7( 0 ).LayoutLine() );
    							
	iValueRect7 =  textLayout.TextRect();
	
	// Speed Value 8 rect
    textLayout.LayoutText( valueRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane_t8( 0 ).LayoutLine() );
    							
	iValueRect8 =  textLayout.TextRect();
	
	// Speed Value 9 rect
    textLayout.LayoutText( valueRect, 
    							AknLayoutScalable_Apps::blid2_tripm_values_pane_t9( 0 ).LayoutLine() );
    							
	iValueRect9 =  textLayout.TextRect();
	
	// Signal Strength
    layoutWindow.LayoutRect( tripMeterRect, 
    							AknLayoutScalable_Apps::blid2_tripm_pane_g6( variety ).LayoutLine() );
    							
	iSignalStrengthRect =  layoutWindow.Rect();
	
	
	}


// ----------------------------------------------------------------------------
// CHtkTripMeterControl::SetAnchors
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::SetAnchors( )
	{	
	// Anchor for Big trip visual
	TAlfRealPoint topPoint( iBigTripRect.iTl );
	TAlfRealPoint bottomPoint( iBigTripRect.iBr );
	
    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 0, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 0, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
	       
	// Anchor for trip box image visual
	topPoint =  iTripDeckRect.iTl;
	bottomPoint = iTripDeckRect.iBr;
	
    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 1, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 1, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for tripmeter text visual
	topPoint =  iTripMeterRect.iTl;
	bottomPoint = iTripMeterRect.iBr;
    
    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 2, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 2, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for odometer box image visual
	topPoint =  iOdoDeckRect.iTl;
	bottomPoint = iOdoDeckRect.iBr;
	
    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 3, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 3, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for odometer text visual
	topPoint =  iOdoMeterRect.iTl;
	bottomPoint = iOdoMeterRect.iBr;
	
    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 4, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 4, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for Speed Format visual
	topPoint =  iSpeedFormatRect.iTl;
	bottomPoint = iSpeedFormatRect.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 5, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 5, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );

    // Anchor for Speed format
	topPoint =  iTripDialRect.iTl;
	bottomPoint = iTripDialRect.iBr;
	
    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 6, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 6, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );
        
	// Anchor for clock image visual
	topPoint =  iClockImageRect.iTl;
	bottomPoint = iClockImageRect.iBr;
	
    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 7, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 7, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for trip time text visual
	topPoint =  iTripTimeRect.iTl;
	bottomPoint = iTripTimeRect.iBr;
	   
    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 8, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 8, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );

    // Anchors for iMileVisuals (8 to 16)
        
	// Anchor for Mile Value 1
	topPoint =  iValueRect1.iTl;
	bottomPoint = iValueRect1.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 9, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 9, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for Mile Value 2
	topPoint =  iValueRect2.iTl;
	bottomPoint = iValueRect2.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 10, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 10, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for Mile Value 3
	topPoint =  iValueRect3.iTl;
	bottomPoint = iValueRect3.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 11, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 11, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for Mile Value 4
	topPoint =  iValueRect4.iTl;
	bottomPoint = iValueRect4.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 12, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 12, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for Mile Value 5
	topPoint =  iValueRect5.iTl;
	bottomPoint = iValueRect5.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 13, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 13, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for Mile Value 6
	topPoint =  iValueRect6.iTl;
	bottomPoint = iValueRect6.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 14, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 14, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for Mile Value 7
	topPoint =  iValueRect7.iTl;
	bottomPoint = iValueRect7.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 15, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 15, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for Mile Value 8
	topPoint =  iValueRect8.iTl;
	bottomPoint = iValueRect8.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 16, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 16, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
        
	// Anchor for Mile Value 9
	topPoint =  iValueRect9.iTl;
	bottomPoint = iValueRect9.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 17, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 17, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );
    
    // Anchor for Satellite Status image visual   (18) 
	topPoint =  iSignalStrengthRect.iTl;
	bottomPoint = iSignalStrengthRect.iBr;

    iTripLayout->SetAnchor( EAlfAnchorTopLeft, 18,   
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,  
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iTripLayout->SetAnchor( EAlfAnchorBottomRight, 18, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );

	}
	
// ----------------------------------------------------------------------------
// CHtkTripMeterControl::CreateSignalTexturesL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::CreateSignalTexturesL( )
	{
	MAlfBitmapProvider* provider = NULL;	
	
    // then create the bitmap provider
    iView.ImageLoaderUtils()->SetSize( iSignalStrengthRect.Size() ); 
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSatSignal01, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_sat_signal_01, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_sat_signal_01_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture1 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault );
	iSignalTextures.Append( &thumbTexture1 );// At 0
	

    // then create the bitmap provider
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSatSignal02, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_sat_signal_02, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_sat_signal_02_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture2 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault ); 	

	iSignalTextures.Append( &thumbTexture2 );// At 1			                                                                  
    // then create the bitmap provider
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSatSignal03, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_sat_signal_03, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_sat_signal_03_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture3 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault );

	iSignalTextures.Append( &thumbTexture3 );// At 2  
	
    // then create the bitmap provider
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSatSignal04, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_sat_signal_04, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_sat_signal_04_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture4 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault );

	iSignalTextures.Append( &thumbTexture4 );// At 3  
	
    // then create the bitmap provider
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSatSignal05, // svg icon skin constant id defiened in Aknsconstant.h file
    												 			iView.ImagePathName(),        // mif file name with path.
    												 			EMbmBlidQgn_graf_blid_sat_signal_05, // bitmap id defiened in mbg file
    												 			EMbmBlidQgn_graf_blid_sat_signal_05_mask ); // bitmap mask id defiened in mbg file
    												 
	    												 
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture5 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
                                                                provider,
                                                                EAlfTextureFlagDefault );

	iSignalTextures.Append( &thumbTexture5 );// At 4
	}
	
// ----------------------------------------------------------------------------
// CHtkTripMeterControl::CreatePopupImgTextureL
// ----------------------------------------------------------------------------
//
void CHtkTripMeterControl::CreatePopupImgTextureL( )
	{
	MAlfBitmapProvider* provider = NULL;			
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidUnavail, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_unavail, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_unavail_mask ); // bitmap mask id defiened in mbg file
	
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
		                                                                
	iPopupImgTexture = &thumbTexture;
	}	
		
//End Of File

