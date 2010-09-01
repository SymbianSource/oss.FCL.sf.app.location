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


// SYSTEM INCLUDES
#include <coemain.h>
#include <akncontext.h> 
#include <aknnavide.h> 
#include <eikspane.h> 
#include <barsread.h> 
#include <AknIconArray.h>
#include <gulicon.h>
#include <StringLoader.h> 
#include <akntitle.h> 
#include <avkon.hrh>
#include <AknsUtils.h>
#include <akntabgrp.h>
#include <blid.mbg>
#include <blid.rsg>
#include <avkon.hrh>
#include <eikenv.h>
#include <aknsoundsystem.h>
#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>
#include <EPos_CPosLandmark.h>
#include <aknnotedialog.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknUtils.h>
#include <AknsConstants.h>
// for HTK
#include <alf/alfenv.h>
#include <alf/alfanchorlayout.h>
#include <alf/alftextvisual.h>
#include <alf/alftextstyle.h>
#include <alf/alfborderbrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfevent.h>
#include <alf/alftransformation.h>
#include <alf/alfimagevisual.h>
#include <alf/alftexture.h>
#include <alf/alfroster.h>
#include <alf/alfcurvepath.h>
#include <alf/alfcurvepathlayout.h>
#include <alf/alfimagebrush.h>
#include <alf/alfdecklayout.h>
#include <alf/alfutil.h>
#include <alf/alfframebrush.h>
#include <alf/alfimageloaderutil.h>
// USER INCLUDES
#include "CHtkNavigationControl.h"
#include "MBlidSettings.h"
#include "Blid.hlp.hrh"
#include "Blid.hrh"
#include "CBlidAppUi.h"
#include "MBlidLocation.h"
#include "MBlidRouter.h"
#include "CBlidEng.h"
#include "bliduiconsts.h"
#include "Blidutils.h"
#include "TNamedCoordinate.h"
#include "CBlidArrivedTonePayerUtility.h"
#include "Debug.h"

_LIT( KSpeedFormat, "%.1f" );
_LIT( KText1,  " " );
_LIT( KFlagVisual, " Flag");
_LIT( KAccVisual, " Accuracy");

const TInt KTimerDuration = 200000;

TBool CHtkNavigationControl::iHasToPlayArrivedTone = EFalse;
TBool CHtkNavigationControl::iHasArrived = EFalse;
TBool CHtkNavigationControl::iInArrivedZone = EFalse;

// FORWARD DECLARATION
TInt TimerCallBackFunction(TAny* aPtr);


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CHtkNavigationControl::NewL()
// ---------------------------------------------------------
//
CHtkNavigationControl* CHtkNavigationControl::NewL(CAlfEnv& aEnv,
						                           MObjectProvider* aParent,
								                   const TRect& aRect,
						                           MBlidLocation& aLocation,
						                           MBlidRouter& aRouter,
						                           CBlidEng& aEngine,
						                           CBlidBaseView& aView)
    {
    CHtkNavigationControl* self = CHtkNavigationControl::NewLC(aEnv,
    														   aParent,
    														   aRect,
    														   aLocation,
    														   aRouter,
    														   aEngine,
    														   aView);
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------
// CHtkNavigationControl::NewLC()
// ---------------------------------------------------------
//
CHtkNavigationControl* CHtkNavigationControl::NewLC(CAlfEnv& aEnv,
						                           MObjectProvider* /*aParent*/,
								                   const TRect& aRect,
						                           MBlidLocation& aLocation,
						                           MBlidRouter& aRouter,
						                           CBlidEng& aEngine,
						                           CBlidBaseView& aView)
    {
    CHtkNavigationControl* self = new (ELeave) CHtkNavigationControl(aEnv,
    														 		 aLocation,
    														 		 aView,
    														 		 aEngine );
    CleanupStack::PushL(self);
    self->ConstructL(aEnv,aRect,aRouter,aEngine);
    return self;
    }

// ---------------------------------------------------------
//
 //CHtkNavigationControl::CHtkNavigationControl()
// ---------------------------------------------------------
//
CHtkNavigationControl::CHtkNavigationControl(CAlfEnv& aEnv,
                                             MBlidLocation& aLocation,
                                             CBlidBaseView& aView,
                                             CBlidEng& aEngine ) 
    : CAlfControl( ),iLocationModel( aLocation ), iView(aView), iEngine(aEngine),iEnv(aEnv)

    {
    
    }


// ---------------------------------------------------------
// CHtkNavigationControl::ConstructL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::ConstructL( CAlfEnv& aEnv,
                                        const TRect& /*aRect*/,
                                        MBlidRouter& aRouter,
                                        CBlidEng& aEngine )
    {
    
    CAlfControl::ConstructL( aEnv );
    
    // Create a single text visual. The pointer to the visual is stored
    // as a member variable for later access.
	CreateNaviPaneL( EFalse );
    ActivateTabL( *iDecoratedTabGroup, EBlidNavigationView );
    
	iRouter = &aRouter;
	iSettings = aEngine.SettingsModel();
	CreateApplicationStringsL( );

	iImagePath = HBufC::NewL( iEnv.TextureManager().ImagePath().Length() + 1 );
	iImagePath->Des().Copy( iEnv.TextureManager().ImagePath() );
    iLayout = CAlfAnchorLayout::AddNewL( *this );

	AknsUtils::GetCachedColor ( AknsUtils::SkinInstance(),
								iVisualColor,
								KAknsIIDQsnTextColors,
								EAknsCIQsnTextColorsCG1 
								);
	
	TInt typefaceStyle1Id = iEnv.TextStyleManager().CreatePlatformTextStyleL(EAknHighestLogicalFont, EAlfTextStyleNormal); 
	TInt typefaceStyle2Id = iEnv.TextStyleManager().CreatePlatformTextStyleL(EAknHighestLogicalFont, EAlfTextStyleNormal); 
    CAlfTextStyle* style1 = iEnv.TextStyleManager().TextStyle( typefaceStyle1Id );
    CAlfTextStyle* style2 = iEnv.TextStyleManager().TextStyle( typefaceStyle2Id );

	CalculateRects();
	
	CreateAnimationL();
	CreateNavigationVisualsL( );// image
	CreateNavigationTextVisualsL( style1 );
	CreateDistanceVisualL( style1 );
	CreateSpeedVisualL( style2 );
	CreateIndicatorVisualL();
	DrawCurvesL();
	CreateSignalTexturesL();
	CreatePopupImgTextureL();
	CreateAccTextureL();
	LaunchDeviceNotAvlNoteL( );

	SetAnchors();	
	isDeviceUnavailableDisplayed = EFalse;	
	iIsDownPressed = EFalse;	
    }

// ---------------------------------------------------------
// CHtkNavigationControl::CreateAnimationL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::CreateAnimationL( )
	{
	// code to load svg icon
	MAlfBitmapProvider* provider = NULL;
	
	// first creste the image loader utils and then call SetSize.
    iView.ImageLoaderUtils()->SetSize( TAlfRealSize( 210, 210 ) );
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidFlag, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_flag_01, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_flag_01_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture1 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture1);
    												 
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidFlag, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_flag_02, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_flag_02_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture2 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture2);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidFlag, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_flag_03, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_flag_03_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture3 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture3);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidFlag, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_flag_04, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_flag_04_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture4 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture4);
    
    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidFlag, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_flag_05, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_flag_05_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & texture5 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
    iTextures.Append(&texture5);

	}
	
// ----------------------------------------------------
// CHtkNavigationControl::CreateGlobeVisualL
// ----------------------------------------------------
//
void CHtkNavigationControl::CreatePopUpVisualL( )
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
// CHtkNavigationControl::DrawCurvesL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::DrawCurvesL()
	{
	MAlfBitmapProvider* provider = NULL;
	// Create curvepath layout.
	iCurveLayout = CAlfCurvePathLayout::AddNewL( *this, iCurveDecLyt );
	CAlfCurvePath& path = iCurveLayout->CurvePath();

	iCurveLayout->SetBaseUnit( TAlfMetric( 1, EAlfUnitNormalized ) );
	TAlfRealPoint centre( 0.5, 0.5 );
	TAlfRealSize  radious( 0.47, 0.37 );
	path.AppendArcL( centre, radious, 0, 360, 1 );
	path.EnableLoop();

	iNdecLayout = CAlfDeckLayout::AddNewL( *this, iCurveLayout );
	
	iActualNdeck = CAlfDeckLayout::AddNewL( *this, iNdecLayout );

    iActualNdeck->SetSize( TAlfRealSize( 32, 32 ) );

	// now map the image visual to curve path and move.
	TAlfTimedPoint pos1;
	pos1.SetMappingFunctionIdentifier( iCurveLayout->MappingFunctionIdentifier() );
	pos1.SetTarget( 0.75, 0 );
	iNdecLayout->SetPos( pos1 );

	// create the image visual to set on curve path
	//MAlfBitmapProvider* provider = NULL;
	iNorthVisual = CAlfImageVisual::AddNewL(*this, iActualNdeck );
	
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidNorth, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_north, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_north_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture1 = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
                                                                      
	iNorthVisual->SetImage(TAlfImage( thumbTexture1 ) );//N
	
    iNdecLayout->EnableScrollingL( ETrue );
    // scroll offset should be half of the 
    // size of decklayout.
	iNdecLayout->SetScrollOffset( TAlfTimedPoint(  14 , 14  ) );
	}


// ---------------------------------------------------------
// CHtkNavigationControl::CreateIndicatorVisualL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::CreateIndicatorVisualL()
	{
    iNavigationRing = CAlfImageVisual::AddNewL(*this, iLayout ); // ordinal 6
    iNavigationRing->SetScaleMode( CAlfImageVisual::EScaleCover );
    iNavigationRing->SetPadding(TAlfBoxMetric(TAlfMetric( 0, EAlfUnitNormalized ) ) );

	// Place the curve path circle ring
    MAlfBitmapProvider* provider = NULL;  
                              
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidRing, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_ring, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_ring_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
	iNavigationRing->SetImage(TAlfImage( thumbTexture ) );

	//Start
	iAccVisual = CAlfImageVisual::AddNewL( *this, iLayout ); // ordinal 7

	iCurveDecLyt = CAlfDeckLayout::AddNewL( *this, iLayout); // ordinal 8
	
	iIndicatorVisual = CAlfImageVisual::AddNewL( *this, iLayout ); // ordinal 9
	//End

    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidCompass, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_compass, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_compass_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTextureind = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
     	                                                                  	
	iIndicatorVisual->SetImage(TAlfImage(thumbTextureind));//indicator  	
	iIndicatorVisual->SetOpacity(0.8);
	}


// ---------------------------------------------------------
// CHtkNavigationControl::CreateNavigationVisualsL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::CreateNavigationVisualsL( )
	{
	iImageVisual = CAlfImageVisual::AddNewL(*this, iLayout); // ordinal 0
	}
	

// ---------------------------------------------------------
// CHtkNavigationControl::CreateNavigationTextVisualsL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::CreateNavigationTextVisualsL( CAlfTextStyle* aStyle1 )	
	{
    aStyle1->SetBold( ETrue );
    aStyle1->SetItalic( ETrue );

	iLabel = CAlfTextVisual::AddNewL(*this, iLayout); // ordinal 1
	
	iLabel->EnableBrushesL();

	aStyle1->SetTextSizeInPixels( 22, ETrue );
	iLabel->SetColor( iVisualColor );
	iLabel->SetTextL( KText1 );
	iLabel->SetTextStyle( aStyle1->Id() );
	iLabel->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
	iLabel->SetPadding(TAlfBoxMetric(TAlfMetric( 0.01, EAlfUnitNormalized ) ) );
	iLabel->SetAlign( EAlfAlignHCenter, EAlfAlignVTop );
	}

// ---------------------------------------------------------
// CHtkNavigationControl::CreateDistanceVisualL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::CreateDistanceVisualL( CAlfTextStyle* aStyle1 )	
	{
    aStyle1->SetBold( ETrue );
    aStyle1->SetItalic( ETrue );
    
	iDistanceLabel = CAlfTextVisual::AddNewL( *this, iLayout ); // ordinal 2
	
	iDistanceLabel->EnableBrushesL();

	iDistanceLabel->SetColor( iVisualColor );

	aStyle1->SetTextSizeInPixels( 22, ETrue );

	iDistanceLabel->SetTextL( KText1 );
	iDistanceLabel->SetTextStyle( aStyle1->Id() );
	iDistanceLabel->SetAlign( EAlfAlignHCenter, EAlfAlignVTop );
	}

// ---------------------------------------------------------
// CHtkNavigationControl::CreateSpeedVisualL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::CreateSpeedVisualL( CAlfTextStyle* aStyle1 )	
	{
	CAlfImageVisual* speedmeterVisual = CAlfImageVisual::AddNewL(*this, iLayout ); // ordinal 3

	// Place the speed
    MAlfBitmapProvider* provider = NULL;  
    
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidSpwatch, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_spwatch, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_spwatch_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
	                                                                  
	speedmeterVisual->SetImage(TAlfImage( thumbTexture ) );// Speed image

	iSpeedLabel = CAlfTextVisual::AddNewL(*this, iLayout ); // ordinal 4

	aStyle1->SetTextSizeInPixels( 13, ETrue );	
	aStyle1->SetItalic( ETrue );
	
	iSpeedLabel->SetColor( KRgbWhite );
    iSpeedLabel->SetClipping( EFalse );
	iSpeedLabel->SetTextL( KText1 );
	iSpeedLabel->SetTextStyle( aStyle1->Id() );
	iSpeedLabel->SetPadding(TAlfBoxMetric(TAlfMetric(.1, EAlfUnitNormalized)));
	iSpeedLabel->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
	
	iSpeedFormatLabel = CAlfTextVisual::AddNewL(*this, iLayout ); // ordinal 5

	aStyle1->SetTextSizeInPixels( 13, ETrue );	
	aStyle1->SetItalic( ETrue );
	
	iSpeedFormatLabel->SetColor( KRgbWhite );
    iSpeedFormatLabel->SetClipping( EFalse );
	iSpeedFormatLabel->SetTextL( KText1 );
	iSpeedFormatLabel->SetTextStyle( aStyle1->Id() );
	iSpeedFormatLabel->SetPadding(TAlfBoxMetric(TAlfMetric(.1, EAlfUnitNormalized)));
	iSpeedFormatLabel->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
		
	}

// ---------------------------------------------------------
// CHtkNavigationControl::SetFlagVisualL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::SetFlagVisualL()
	{
	if( !iLocationModel.IsGPSDataAvailable() )
		{
		return;			
		}
	
	// Check wheather landmark is set or not.
	
   if( !iRouter->Waypoint() && !iRouter->Landmark() )
	   {
	   iIsDestinationSet = EFalse;
	   return;	
	   }
	
	
	idecLayout = CAlfDeckLayout::AddNewL( *this, iCurveLayout );
	
	iFlagdeck = CAlfDeckLayout::AddNewL( *this, idecLayout );

    iFlagdeck->SetSize(TAlfRealSize( 30, 30 ));

    TInt error = KErrNone;
    // calculate angle of bearing    
    const TPosition& position = iPositionInfo;
    TReal heading = iLocationModel.GetHeading();
    TReal32 bearing(0);
    	    
    TBool isWaypointSet = iRouter->IsWaypointSet();
    TNamedCoordinate* waypoint = NULL;
    CPosLandmark* landmark = NULL;
    
    if( isWaypointSet )
	    {
	    waypoint = iRouter->Waypoint() ;	
	    }
    else
	    {
	    landmark = iRouter->Landmark();	
	    }
       
    if ( !Math::IsNaN(heading) )
        {	    	
    	iHeading = heading;
    	}	    

    if( isWaypointSet && waypoint )
        {
        error = position.BearingTo( TCoordinate( 
    							                      waypoint->Latitude(),
    							                      waypoint->Longitude(),
    							                      waypoint->Altitude() ), 
    							                      bearing );
        }
    else if( landmark )
    	{
    	TLocality location;
    	landmark->GetPosition(location);
    	error = position.BearingTo( TCoordinate( 
    		                                          location.Latitude(),
    	 		                                      location.Longitude(),
                         		                      location.Altitude() ), 
                         		                      bearing );
    	}

    //calculate bearing and heading
    TInt resultBearing(0);
    TInt resultHeading(0);
	TReal32 lpointFlag( 0 );	
	
    error = BlidUtils::TRealToTInt(bearing, resultBearing);
    if( error == KErrNone)
	    {
	    error = BlidUtils::TRealToTInt(iHeading, resultHeading);
	    if( error == KErrNone)
    	    {
			// for flag movement					    
			TReal32 lpoint = resultBearing - resultHeading;
			if( lpoint > 0 )
				{
				lpointFlag = lpoint / 360;	
				}
			else
				{
				lpointFlag = ( 360 + lpoint ) / 360;	
				}
    	    }
	    }
	
	// now map the image visual to curve path and move.
	TAlfTimedPoint pos1;
	pos1.SetMappingFunctionIdentifier( iCurveLayout->MappingFunctionIdentifier() );
	pos1.SetTarget( lpointFlag, 0 );
	idecLayout->SetPos( pos1 );

	// create the image visual to set on curve path
	iFlagVisual = CAlfImageVisual::AddNewL(*this, iFlagdeck );
    iDisplayIcon = 1;
    //iFlagVisual->SetSize( animationRect.Size() );
    iFlagVisual->SetImage( TAlfImage( *iTextures[iDisplayIcon-1] ) );//N   
	iFlagVisual->SetScaleMode( CAlfImageVisual::EScaleCover );
	iFlagVisual->SetTagL( iFlagText->Des() );	
	iFlagVisual->SetOpacity( 0.8 );

    idecLayout->EnableScrollingL( ETrue );
	idecLayout->SetScrollOffset( TAlfTimedPoint(  17, 17 ));
	}

// ---------------------------------------------------------
// CHtkNavigationControl::SetAccRacyVisualL()
// ---------------------------------------------------------
//
void CHtkNavigationControl::SetAccRacyVisualL()
	{
	if( !iLocationModel.IsGPSDataAvailable() )
		{
		return;			
		}
	// Check wheather landmark or waypoint is set or not.
	if( !iRouter->Waypoint() && !iRouter->Landmark() )
	   {
	   iIsDestinationSet = EFalse;
	   return;	
	   }
	
	iAccVisual->SetImage(TAlfImage( *iAccTexture ) );//accuracy
	iAccVisual->SetScaleMode( CAlfImageVisual::EScaleCover );
	iAccVisual->SetTagL( iAccText->Des() );	
	iAccVisual->SetOpacity( 1.0 );
	iAccVisual->SetScale( 0.8 );
	
	// Find all the original size of acuracy ring, Flag, navigation ring
	iLayout->ChildSize( 6, iCurveSize );
	TAlfTimedPoint lAccpoint = iAccVisual->Size();
	TAlfTimedPoint lFagpoint = iFlagVisual->Size();
	iFlagSize = lFagpoint.IntValueNow().AsSize();
	iAccuracyringcurveSize = lAccpoint.IntValueNow().AsSize();
	
	}

// ---------------------------------------------------------
// CHtkNavigationControl::~CHtkNavigationControl()
// ---------------------------------------------------------
//
CHtkNavigationControl::~CHtkNavigationControl()
    {
    delete iPeriodic;
    iPeriodic = NULL;
    iTextures.Reset();
    iTextures.Close();
    iPopupTextures.Reset();
    iPopupTextures.Close();
	delete iDecoratedTabGroup;

	delete iDistanceFormatKm;
	delete iDistanceFormatM;
	delete iDistanceFormatMiles;
	delete iDistanceFormatFt;
	delete iDurationFormatLong;
	delete iBlidSpeed;
	delete iSpeedFormatKmph;
	delete iSpeedFormatMph;
	delete iDataBeingCalculated;
	delete iDataBlank;
	delete iFlagText;
	delete iAccText;
    delete iTimer;
    delete iImagePath;
	delete iToneUtility;
	iSignalTextures.Reset();
    iSignalTextures.Close();
    delete iMenu;
    }


// ----------------------------------------------------
// CHtkNavigationControl::CreateNaviPaneL
// ----------------------------------------------------
//
void CHtkNavigationControl::CreateNaviPaneL( TBool aDefaultNaviPane )
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
// CHtkNavigationControl::GetNiviDecorator
// ----------------------------------------------------
//
CAknNavigationDecorator* CHtkNavigationControl::GetNiviDecorator( )
	{
	return iDecoratedTabGroup;	
	}

// ----------------------------------------------------
// CHtkNavigationControl::ActivateTabL
// ----------------------------------------------------
//
void CHtkNavigationControl::ActivateTabL( 
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
// CHtkNavigationControl::StatusPane
// ----------------------------------------------------
//
CEikStatusPane* CHtkNavigationControl::StatusPane()
    {
    return CEikonEnv::Static()->AppUiFactory()->StatusPane();
    }


// ----------------------------------------------------
// CHtkNavigationControl::MakeTitleL
// ----------------------------------------------------
//
void CHtkNavigationControl::MakeTitleL( TInt aResourceText )
    {
    HBufC* buf = StringLoader::LoadLC(aResourceText);       
    CAknTitlePane* title = static_cast<CAknTitlePane*>( StatusPane()->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    CleanupStack::Pop(); //buf
    title->SetText(buf); // Takes ownership of buf
    }


// ----------------------------------------------------
// CHtkNavigationControl::AddTabsL
// ----------------------------------------------------
//
void CHtkNavigationControl::AddTabsL(CAknTabGroup& aTabGroup)
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
// CHtkNavigationControl::OfferEventL
// ----------------------------------------------------
//
TBool CHtkNavigationControl::OfferEventL(const TAlfEvent& aEvent)
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
		StopPlayerL( );	            
		}
 	if( aEvent.IsPointerEvent() )
		{
		TPointerEvent ptrEvent = aEvent.PointerEvent();
		TPoint tapPoint = ptrEvent.iPosition;
		if ( ptrEvent.iType == TPointerEvent::EButton1Down )
			{
			iIsDownPressed = ETrue;
			return ETrue;
			}
		if ( ptrEvent.iType == TPointerEvent::EButton1Up && iIsDownPressed)
		{
			if( iLocationModel.IsGPSDataAvailable() )
				{
				TAlfRealRect rect = iLabel->DisplayRectTarget();
				if( tapPoint.iY < rect.iTl.iY || tapPoint.iY > rect.iBr.iY )
					{
					iView.LaunchNavigationContextMenuL();	
					}
				else
					{
					LaunchStylusPopupMenuL( ptrEvent );
					}				
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


// ---------------------------------------------------------
// CHtkNavigationControl::LaunchInfoQueryDlgL
// ---------------------------------------------------------
//
void CHtkNavigationControl::LaunchInfoQueryDlgL()
	{
    CAknNoteDialog* dlg;
    dlg = new (ELeave) CAknNoteDialog(CAknNoteDialog::EConfirmationTone,CAknNoteDialog::ENoTimeout);
    TInt result = dlg->ExecuteLD( R_INFO_POPUP_MSG_QUERY );
    if( result == EAknSoftkeyExit )
	    {
	    iView.ExitMainApplicationL( EAknSoftkeyExit );
	    }
	dlg = NULL;	
	}

// ---------------------------------------------------------
// CHtkNavigationControl::UpdateL
// ---------------------------------------------------------
//
void CHtkNavigationControl::UpdateL()
    {
    if((iEngine.AppMode() == CBlidEng::EAppForeground) && 
       (iSettings->GetBacklightOption() == EBlidBackLightOnDuringNavigation) && !iView.IsSettingsViewActive() )
        {
        User::ResetInactivityTime();        
        }
	            
    iPositionInfo = iLocationModel.GetCurrentPosition();
    iCourse = iLocationModel.GetCourse();

	if( iLocationModel.IsGPSDataAvailable() )
        {
        isDeviceUnavailableDisplayed = EFalse;
        CalculateIfArrivedL();
        iLayout->SetOpacity( 1.0 );
        iPopUpLayout->SetOpacity( 0.0 );
        
        // If MSK visible then after getting fix softkey should change
		if( iView.CBAPtr() &&
			iMskInfoVisible  )
			{
			iView.CBAPtr()->SetCommandSetL( R_BLID_SOFTKEYS_OPTIONS_EXIT__CONTEXTOPTIONS );
			iView.CBAPtr()->DrawDeferred();
			iMskInfoVisible = EFalse;
			}			    
		TInt satCount = 0;
		iLocationModel.GetNumOfSatellitesL( satCount );
		DisplaySatelliteSignalStrength( satCount );
		CalculateDistanceL( iPositionInfo );
    	CalculateDurationL( iCourse );
    	// if destination is set then
    	// distance should be update.
		if( iIsDestinationSet )
			{
			if( !iRouter->Waypoint() && !iRouter->Landmark() )
			   {
			   iIsDestinationSet = EFalse;
			   return;	
			   }
					
			// Set the landmarks Name
			SetLandMarksNameL();
			// distance format calculation
			TBuf<KBlidLBItemMaxLen> distanceString;
			DistanceFormat( distanceString );
			// distance unit calculation
    	    TBuf<KBlidLBItemMaxLen> unitString;
    	    DistanceUnit( unitString );
    	    if(distanceString != *iDataBlank)
    	    	{
    	    	distanceString.Append( unitString );
    	    	}
			iDistanceLabel->SetTextL( distanceString );
			
			TBuf<KBlidLBItemMaxLen> speedString;
		    TBuf<KBlidLBItemMaxLen> speedFormatString;
		    if(iLocationModel.IsOnline())
		        {
		        SpeedFormat( speedString, speedFormatString );
		        iSpeedLabel->SetTextL( speedString );
		        iSpeedFormatLabel->SetTextL( speedFormatString );
		        }
		    else
		        {
		        speedString.Append(*iDataBlank);
		        speedFormatString.Append(*iDataBlank);
		        }
			PrepareDynamicMoveL();		        
			}
	   	}
	else
		{
		StopPlayerL();
        TFileName imagepathname;		
		DisplaySatelliteSignalStrength( 0 );
        iLayout->SetOpacity( 0.3 );
        iPopUpLayout->SetOpacity( 1.0 );
		
		// for MSK enabled phone add MSK        
		if( iView.CBAPtr() )
			{
			iView.CBAPtr()->SetCommandSetL( R_BLID_OPTION_INFO_EXIT );
			iView.CBAPtr()->DrawDeferred();
			iMskInfoVisible = ETrue;
			}			    
		if( iPeriodic )
	        {
	        delete iPeriodic;
	        iPeriodic = NULL;
	        }
		if( iLocationModel.WaitingGPSData() )
		    {
		    isSatelliteInfoLaunch = ETrue;
		    if(KNumOfSearchIcon != iPopupTextures.Count())
		    	CreatePopUpVisualL( );
			HBufC* noteText = StringLoader::LoadLC( R_BLID_STARTUP_NAVITEXT_SEARCHING_GPS_SATELLITES, CEikonEnv::Static() );	
			iPopUpMsg->SetTextL( *noteText );
            iDisplayIcon = 1;
		                
            iPeriodic = CPeriodic::NewL(0); // neutral priority 
            iPeriodic->Start( TTimeIntervalMicroSeconds32( KTimerDuration ),
                              TTimeIntervalMicroSeconds32( KTimerDuration ),
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
					if(aHWRMLight->LightStatus(CHWRMLight::EPrimaryDisplay) != CHWRMLight::ELightOn)
					{
						User::ResetInactivityTime();
						aHWRMLight->LightOnL(CHWRMLight::EPrimaryDisplay);
						aHWRMLight->ReleaseLight(CHWRMLight::EPrimaryDisplay);
						isDeviceUnavailableDisplayed = ETrue;
						
					}
					
		        	delete aHWRMLight;	
						
				}
            }		    
		}	   	
    
    if(iHasArrived && 
       (iEngine.AppMode() == CBlidEng::EAppForeground) &&
       iHasToPlayArrivedTone &&
       iLocationModel.IsGPSDataAvailable())
        {
        if(iAccVisual->Opacity().ValueNow() == 0.0)
		{
			iAccVisual->SetOpacity(1.0);
		}
			
		if(iFlagVisual->Opacity().ValueNow() == 0.0)
		{
			iFlagVisual->SetOpacity(0.8);
		}
		if( iFlagVisual )
			{
            iDisplayIcon = 1;
		    if( iPeriodic )
		        {
		        delete iPeriodic;
		        iPeriodic = NULL;
		        }            
            iPeriodic = CPeriodic::NewL(0); // neutral priority 
            iPeriodic->Start( TTimeIntervalMicroSeconds32( KTimerDuration ),
                              TTimeIntervalMicroSeconds32( KTimerDuration ),
                              TCallBack(Tick, this));
            UpdateVisualImage();
			}
        PlayArrivedToneL();
        }
    }

// ---------------------------------------------------------
// CHtkNavigationControl::DisplayInfoPopUpMsgL
// ---------------------------------------------------------
//
void CHtkNavigationControl::DisplayInfoPopUpMsgL()
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
// CHtkNavigationControl::LaunchDeviceNotAvlNoteL
// ---------------------------------------------------------
//
void CHtkNavigationControl::LaunchDeviceNotAvlNoteL( )
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
    
	iPopUpMsg->SetColor( iVisualColor );
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
// CHtkNavigationControl::DisplaySatelliteSignalStrength
// ---------------------------------------------------------
//
void CHtkNavigationControl::DisplaySatelliteSignalStrength( TInt aSatCount ) 
	{
    TFileName imagepathname;
	imagepathname.Zero();
	iImageVisual->SetImage(TAlfImage(*iSignalTextures[aSatCount]));                                                           
	}

// ---------------------------------------------------------
// CHtkNavigationControl::SetLandMarksNameL
// ---------------------------------------------------------
//
void CHtkNavigationControl::SetLandMarksNameL()   
	{
	TDesC* name = NULL;
	TBool isWaypointSet = iRouter->IsWaypointSet();
	TNamedCoordinate* waypoint = NULL;
	CPosLandmark* landmark = NULL;

	TPtrC landmarkname;
	if( isWaypointSet )
	   {
	   waypoint = iRouter->Waypoint() ;	
	   name = waypoint->Name();
	   }
	else
	   {
	   landmark = iRouter->Landmark();	
	   landmark->GetLandmarkName(landmarkname);
	   }

	HBufC* landmarksName = HBufC::NewLC( landmarkname.Length() + 20 );
	
	if( isWaypointSet )
	   {
	   landmarksName->Des().Copy( *name );
	   delete name;
	   }
	else
	   {
	   landmarksName->Des().Copy(landmarkname);
	   }
	iLabel->SetTextL( *landmarksName );
	CleanupStack::PopAndDestroy(landmarksName);
	}

// ---------------------------------------------------------
// CHtkNavigationControl::ClearNavigationInfoL
// ---------------------------------------------------------
//
void CHtkNavigationControl::ClearNavigationInfoL()
	{
	CAlfVisual* lvisual = NULL;
	if( iFlagdeck )
		{
		lvisual =  iFlagdeck->FindTag( iFlagText->Des() );
		if( lvisual )
			{
			lvisual->SetOpacity( 0.0 ); 
			}
		}

	if( iAccVisual )
		{
		lvisual =  iLayout->FindTag( iAccText->Des() );
		if( lvisual )
			{
			lvisual->SetOpacity( 0.0 );
			}
		}
	
	if( iCurveLayout && iIsDestinationSet )
		{
		iNavigationRing->SetScale( 1.0 );
		CAlfCurvePath& path = iCurveLayout->CurvePath();
		TAlfRealPoint centre( 0.5, 0.5 );
		TAlfRealSize  radious( 0.47, 0.37 );
		path.Reset();
		path.AppendArcL( centre, radious, 0, 360, 1 );
		path.EnableLoop();
		}
		
	iDistanceLabel->SetTextL( KText1 );
	iSpeedLabel->SetTextL( KText1 );
	iSpeedFormatLabel->SetTextL( KText1 );
	iLabel->SetTextL( KText1 );
	
	if( iNdecLayout && iIsDestinationSet )
		{
		TAlfTimedPoint pos1;
		pos1.SetTarget( 0.75, 0 );
		iNdecLayout->SetPos( pos1 );
		}
	
	// Playing should be stopped if user stop navigation
    if( iToneUtility )
	    {
		// change back the softkey to optionexit
	    if( iView.CBAPtr() )
		    {
		    iView.CBAPtr()->SetCommandSetL( R_BLID_SOFTKEYS_OPTIONS_EXIT__CONTEXTOPTIONS );
		    iView.CBAPtr()->DrawDeferred();
		    }			    
	    iToneUtility->CanclePayer();		    	
	    }

	iHasArrived = EFalse;
	iHasToPlayArrivedTone = ETrue;
	iTimeFirstArrived = EFalse;
	iTimerComplete = EFalse;
	iIsDestinationSet = EFalse;
	} 

// ---------------------------------------------------------
// CHtkNavigationControl::SetUpdateOptionL
// ---------------------------------------------------------
//
void CHtkNavigationControl::SetUpdateOptionL( TBool aIsDestSet )
	{
	iIsFirstDistance = 0;
	iIsDestinationSet = aIsDestSet;
	iLocationModel.SetTripStartL();
	}

// ---------------------------------------------------------
// CHtkNavigationControl::PlayArrivedToneL
// ---------------------------------------------------------
//
void CHtkNavigationControl::PlayArrivedToneL()    
    {
    if( !iToneUtility )
    	{
    	iToneUtility = CBlidArrivedTonePlayerUtility::NewL();
    	}
    TBuf<KMaxFileName> lFileName;
    lFileName.Copy( iSettings->GetArrivedToneName() );
    iToneUtility->PlayToneL( lFileName );
    iHasToPlayArrivedTone = EFalse;
    if( iView.CBAPtr() )
	    {
	    iView.CBAPtr()->SetCommandSetL( R_BLID_SOFTKEYS_OPTIONS_STOP );
	    iView.CBAPtr()->DrawDeferred();
	    }
    }
    
// ---------------------------------------------------------
// CHtkNavigationControl::CalculateIfArrivedL
// ---------------------------------------------------------
//
void CHtkNavigationControl::CalculateIfArrivedL()
	{
	if(iHasArrived)
		{
		if( iTimeFirstArrived )
		    {
		    if( iTimer )
		        {
		        delete iTimer;
		        iTimer = NULL;
		        }
		    iTimer = CPeriodic::NewL( EPriorityHigh );
		    if( iTimer )
		        {
    		    TCallBack callback( TimerCallBackFunction, this );
    		    iTimer->Start( TTimeIntervalMicroSeconds32( KMinArrivalTime ),
    	                       TTimeIntervalMicroSeconds32( 80000000 ), 
                               callback );
                iTimeFirstArrived = EFalse;                           
		        }
			
			TReal32 flagArrivedScale = ( iCurveSize.iWidth ) / ( ( iFlagSize.iWidth / 2 ) + ( iAccuracyringcurveSize.iWidth / 2 ) );
			TReal32 accArrivedScale = ( iCurveSize.iWidth / 2 ) / ( ( iFlagSize.iWidth / 2 ) + ( iAccuracyringcurveSize.iWidth / 2 ) );
			iAccVisual->SetScale( accArrivedScale );
			//iFlagVisual->SetScale( flagArrivedScale );
			iNavigationRing->SetScale( 0.25 );
			// change the curve path size once arrived.
			if( iCurveLayout )
				{
				TAlfRealPoint centre( 0.5, 0.4 );
				TAlfRealSize  radious( 0, 0 );
				iCurveLayout->CurvePath().Reset();
				iCurveLayout->CurvePath().AppendArcL( centre, radious, 0, 360, 1 );
				iCurveLayout->CurvePath().EnableLoop();
				if(iFlagdeck) 
					iFlagdeck->SetPos( iCurveLayout->Pos() );
				}				
				
			if(iInArrivedZone)
				{
				iFlagVisual->SetScale( 4 * flagArrivedScale );
				}
			isStopUpdate = ETrue;				
		    }
		else if( iTimerComplete )
		    {
		    isStopUpdate = EFalse;
		    if( iToneUtility )
			    {
			    iToneUtility->CanclePayer();		    	
			    }
			// change back the softkey to optionexit
		    if( iView.CBAPtr() )
			    {
			    iView.CBAPtr()->SetCommandSetL( R_BLID_SOFTKEYS_OPTIONS_EXIT__CONTEXTOPTIONS );
			    iView.CBAPtr()->DrawDeferred();	
			    }			    
			if( !IsInArrivedZone() )
				{
				iHasArrived = EFalse;
				}			
		    }
		else if( !IsInArrivedZone() )
			{
		    StopPlayerL();			    			
			iHasArrived = EFalse;	
			iMskInfoVisible = ETrue;	
			}
		}
	else
		{
		if(iEngine.HasArrivedToDestination())
			{
			iHasArrived = ETrue;
			iHasToPlayArrivedTone = ETrue;
			iTimeFirstArrived = ETrue;
			iTimerComplete = EFalse;
			}
		}    
	}

// ---------------------------------------------------------
// CHtkNavigationControl::Tick()
// ---------------------------------------------------------
//
TInt CHtkNavigationControl::Tick(TAny* aObject)
    {
    ((CHtkNavigationControl*)aObject)->UpdateVisualImage(); // cast, and call non-static function

    return KErrNone;
    }

// ---------------------------------------------------------
// CHtkNavigationControl::UpdateVisualImage
// ---------------------------------------------------------
//
void CHtkNavigationControl::UpdateVisualImage()
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
	else if(!iTimerComplete && isStopUpdate)
		{
	    if(KNumOfFlagIcon > iDisplayIcon)
	        {
	        iDisplayIcon++;
	        }
	    else
	        {
	        iDisplayIcon = 1;
	        } 
	    //iFlagVisual->SetSize( animationRect.Size() );
	    iFlagVisual->SetImage( TAlfImage( *iTextures[iDisplayIcon-1] ) );//N  
		}
	}
	
// ---------------------------------------------------------
// CHtkNavigationControl::ChangeFlagVisualImageL
// ---------------------------------------------------------
//
void CHtkNavigationControl::ChangeFlagVisualImageL()
	{
    if( iPeriodic )
        {
        delete iPeriodic;
        iPeriodic = NULL;
        }
	iDisplayIcon = 1;
    //iFlagVisual->SetSize( animationRect.Size() );
    iFlagVisual->SetImage( TAlfImage( *iTextures[iDisplayIcon-1] ) );//N  
	}

// ---------------------------------------------------------
// CHtkNavigationControl::IsInArrivedZone
// ---------------------------------------------------------
//
TBool CHtkNavigationControl::IsInArrivedZone()
	{
	if(iRouter->IsAnyPointSet())
		{
		TPosition& currentPosition = iLocationModel.GetCurrentPosition();
		TReal32 distanceToDestination(0);
		
		if(iRouter->IsWaypointSet())
	        {
	        TNamedCoordinate* destination = iRouter->Waypoint();
	        if(destination->Distance(currentPosition, distanceToDestination) != KErrNone)
	        	{
	        	return EFalse;
	        	}
	        }
	    else
	        {
	        TPosition destination;
	        CPosLandmark* landmark = iRouter->Landmark();	        
	        landmark->GetPosition(destination);
	        if(currentPosition.Distance(destination, distanceToDestination) != KErrNone)
	        	{
	        	return EFalse;
	        	}
	        }	    
        
	    if(  distanceToDestination < iSettings->GetArrivedDistance() && 
	        iHasArrived )
	    	{
	    	return ETrue;
	    	}
		}	
    return EFalse;		
	}

// ---------------------------------------------------------
// CHtkNavigationControl::HasArrivedDestination
// ---------------------------------------------------------
//
TBool CHtkNavigationControl::HasArrivedDestination()
	{
	return iHasArrived;
	}

// ----------------------------------------------------------------------------
// CHtkNavigationControl::CreateApplicationStringsL
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::CreateApplicationStringsL( )
	{
    iDistanceFormatKm = CEikonEnv::Static()->AllocReadResourceL( 
                                R_BLID_NAV_DISTANCE_KM );
    iDistanceFormatM = CEikonEnv::Static()->AllocReadResourceL( 
                                R_BLID_NAV_DISTANCE_M );

    iDistanceFormatMiles = CEikonEnv::Static()->AllocReadResourceL( 
                                R_BLID_NAV_DISTANCE_MI );

    iDistanceFormatFt = CEikonEnv::Static()->AllocReadResourceL( 
                                R_BLID_NAV_DISTANCE_FT );

    iDurationFormatLong = CEikonEnv::Static()->AllocReadResourceL( 
                 R_QTN_TIME_DURAT_LONG_WITH_ZERO );
    
    iBlidSpeed = StringLoader::LoadL( R_BLID_NAVIGATION_SPEED, CEikonEnv::Static());
    
   
    iSpeedFormatKmph = CEikonEnv::Static()->AllocReadResourceL( R_BLID_METRIC_SPEED_FORMAT );
    iSpeedFormatMph = CEikonEnv::Static()->AllocReadResourceL( R_BLID_IMPERIAL_SPEED_FORMAT );
    iDataBeingCalculated = StringLoader::LoadL( R_BLID_WAITING_GPS_LIST, CEikonEnv::Static() );
    iDataBlank = StringLoader::LoadL( R_BLID_BLANK, CEikonEnv::Static() );
    
	iFlagText = HBufC8::NewL( 8 );
	iAccText = HBufC8::NewL( 15 );
	iFlagText->Des().Copy( KFlagVisual );
    iAccText->Des().Copy( KAccVisual );
	}


// ---------------------------------------------------------
// CHtkNavigationControl::CalculateDistance
// ---------------------------------------------------------
//
void CHtkNavigationControl::CalculateDistanceL( TPosition aPosition )
    {
    // If destination is not set don't do anything
    if( iRouter->IsAnyPointSet() )
	    {
	    TNamedCoordinate* waypoint = NULL;
	    CPosLandmark* landmark = NULL;
	    
	    if( iRouter->IsWaypointSet() )
		    {
		    waypoint = iRouter->Waypoint() ;	
		    }
	    else
		    {
		    landmark = iRouter->Landmark();	
		    }
	    
	    iDistance = 0;
	    if ( iRouter->IsAnyPointSet() && waypoint )
	        {
	        User::LeaveIfError( aPosition.Distance( TCoordinate( 
	                                     waypoint->Latitude(),
	                                     waypoint->Longitude(),
	                                     waypoint->Altitude() ),iDistance) );
	        }
	    else if( landmark )
	    	{
	    	TLocality location;
		    landmark->GetPosition(location);
	        User::LeaveIfError( aPosition.Distance( TCoordinate( 
	                                     location.Latitude(),
		 		                         location.Longitude(),
	                     		         location.Altitude() ),iDistance) );
	        }
	    }
    }

// ---------------------------------------------------------
// CHtkNavigationControl::CalculateDurationL
// ---------------------------------------------------------
//
void CHtkNavigationControl::CalculateDurationL( TCourse /*aCourse*/ )
    {
    TCourse course = iLocationModel.GetCourse();
    iSpeed = course.Speed();    
         
    iDurationInSeconds = 0;

    if ( !Math::IsNaN(iSpeed) )
        {
        User::LeaveIfError( Math::Round( iSpeed, iSpeed, KDecimals3 ) );
         iIsSpeed = ETrue;
        }
    else
        {
        iIsSpeed = EFalse;
        iSpeed = 0;
        }

    if ( iSpeed != 0 && iDistance != 0)
        {
        iDurationInSeconds = (iDistance/iSpeed);
        }
    
    TBuf<KBlidLBItemMaxLen> durationString;
    if ( BlidUtils::FormatStringToTime( durationString, iDurationInSeconds ) 
         != KErrNone )
        {
        return;
        }

    if ( iDurationInSeconds < KDayToSeconds )
        {
        iDuration.Set( durationString );
        }
    else
        {
        User::LeaveIfError( 
            BlidUtils::SecondsToDays( iDurationInSeconds, iDurationInDays ) ); 
        }
    }



// ---------------------------------------------------------
// CHtkNavigationControl::PrepareDynamicMoveL
// ---------------------------------------------------------
//
void CHtkNavigationControl::PrepareDynamicMoveL( )
	{
	if( iLocationModel.IsGPSDataAvailable() )
	    {
	    if( isStopUpdate )
		    {
			return;		    	
		    }

	    if( iHasArrived )
		    {
			return;		    	
		    }
		    		    
	    TInt error = KErrNone;
        // calculate angle of bearing    
        const TPosition& position = iPositionInfo;
        TReal heading = iLocationModel.GetHeading();
        TReal32 bearing(0);
        	    
        TBool isWaypointSet = iRouter->IsWaypointSet();
        TNamedCoordinate* waypoint = NULL;
        CPosLandmark* landmark = NULL;
        
        if( isWaypointSet )
    	    {
    	    waypoint = iRouter->Waypoint() ;	
    	    }
        else
    	    {
    	    landmark = iRouter->Landmark();	
    	    }
           
        if ( !Math::IsNaN(heading) )
	        {	    	
	    	iHeading = heading;
	    	}	    

        if( isWaypointSet && waypoint )
	        {
	        error = position.BearingTo( TCoordinate( 
        							                      waypoint->Latitude(),
        							                      waypoint->Longitude(),
        							                      waypoint->Altitude() ), 
        							                      bearing );
	        }
	    else if( landmark )
	    	{
	    	TLocality location;
	    	landmark->GetPosition(location);
	    	error = position.BearingTo( TCoordinate( 
        		                                          location.Latitude(),
        	 		                                      location.Longitude(),
                             		                      location.Altitude() ), 
                             		                      bearing );
	    	}
    
	    //calculate bearing and heading
	    TInt resultBearing(0);
	    TInt resultHeading(0);
	    TReal32 lDistance;
	    TReal degreeN( 0 );

	    error = BlidUtils::TRealToTInt(bearing, resultBearing);
	    if( error == KErrNone)
    	    {
		    error = BlidUtils::TRealToTInt(iHeading, resultHeading);
		    if( error == KErrNone)
	    	    {
			    degreeN = resultHeading;

				TReal32 NavigationPaneRadius = iCurveSize.iWidth / 2;
				TReal32 accuracyRingRadius = iAccuracyringcurveSize.iWidth / 2;
				TReal32 flagRadious = iFlagSize.iWidth / 2;
				
			    if( isWaypointSet )
				    {
				    position.Distance(TCoordinate(waypoint->Latitude(),waypoint->Longitude()), lDistance);
				    }	    	
			    else
			    	{
			    	TLocality location;
			    	landmark->GetPosition(location);
			    	position.Distance(TCoordinate(location.Latitude(),location.Longitude()), lDistance);
			    	}
			        
				if( iIsFirstDistance == 0 )
					{
					iFirstDistance = lDistance;	
					iIsFirstDistance = 1;					
					}
				
				TReal32 flagFactor = 0,ringFactor = 0, accFactor = 0, ringMap = 0, ringWidth = 0;
				TReal32 newRingFact = 0, newFlagFact = 0, newAccFact = 0, difference = 0;
				TReal32 flagAccFact = NavigationPaneRadius / 2;
					if(iAccVisual->Opacity().ValueNow() == 0.0)
					{
						
						iAccVisual->SetOpacity(1.0);
					}
						
					if(iFlagVisual->Opacity().ValueNow() == 0.0)
					{
						
						iFlagVisual->SetOpacity(0.8);
					}
				    if( lDistance < iFirstDistance && iFirstDistance > 0 )
					{
					ringFactor = NavigationPaneRadius / iFirstDistance;
					flagFactor = ( NavigationPaneRadius - flagRadious ) / iFirstDistance;
					accFactor = ( flagAccFact - accuracyRingRadius ) / iFirstDistance;

					difference = iFirstDistance - lDistance;
					newRingFact = lDistance * ringFactor;
					newFlagFact = flagRadious + flagFactor * difference;
					newAccFact = accuracyRingRadius + accFactor * difference;
					
					ringMap = (newRingFact * 0.37 ) / NavigationPaneRadius;
					ringWidth = (newRingFact * 0.47 ) / NavigationPaneRadius;
					if(	ringMap <= 0.37 &&  ringMap > 0 )					
						{
						TAlfRealPoint centre( 0.5, 0.5 );
						TAlfRealSize  radious( ringWidth,ringMap );
						Env().SetRefreshMode( EAlfRefreshModeManual );
						iCurveLayout->CurvePath().Reset();
						TRAP( error, iCurveLayout->CurvePath().AppendArcL( centre, radious, 0, 360, 1 ) )
						iCurveLayout->CurvePath().EnableLoop();
						Env().SetRefreshMode( EAlfRefreshModeAutomatic );
						}
					// scale flag and  accuracy ring
					iAccVisual->SetScale( TAlfTimedValue( newAccFact / accuracyRingRadius ) );
					iFlagVisual->SetScale( TAlfTimedValue( newFlagFact / flagRadious ) );
					iNavigationRing->SetScale( newRingFact / NavigationPaneRadius );
					}
				else
					{
					iFirstDistance = lDistance;
					Env().SetRefreshMode( EAlfRefreshModeManual );
					iCurveLayout->CurvePath().Reset();
					TRAP( error, iCurveLayout->CurvePath().AppendArcL( TAlfRealPoint( 0.5, 0.5 ),
														  TAlfRealPoint( 0.47, 0.37 ),
														  0,
														  360,
														  1 ) );
					iCurveLayout->CurvePath().EnableLoop();
					Env().SetRefreshMode( EAlfRefreshModeAutomatic );
					iAccVisual->SetScale( 0.8 );
					iFlagVisual->SetScale( 1.5 );
					iNavigationRing->SetScale( 1.0 );
					}					

				// for N movement					    
			    TReal32 lpointN = ( 360 - degreeN ) / 360;
			    // As start from top position
			    lpointN -= 0.25;
					
				// for flag movement					    
				TReal32 lpoint = resultBearing - resultHeading;
				TReal32 lpointFlag( 0 );
				if( lpoint > 0 )
					{
					lpointFlag = lpoint / 360;	
					}
				else
					{
					lpointFlag = ( 360 + lpoint ) / 360;	
					}
				// As start from top position	
				lpointFlag -= 0.25;
				 
			    TAlfPointCommand moveCommandN( 
			        iNdecLayout, 
			        EAlfPointCommandVisualSetPos,
			        TAlfRealPoint( lpointN , lpointN ),
			        0 );
				moveCommandN.ExecuteL( iEnv );		        

			    // move it again later on
			    TAlfPointCommand moveCommand( 
			        idecLayout, 
			        EAlfPointCommandVisualSetPos,
			        TAlfRealPoint( lpointFlag , lpointFlag ),
			        0 );
			    moveCommand.ExecuteL( iEnv ); 
	    	    }
    	    }
    	}
	}

// ---------------------------------------------------------
// CHtkNavigationControl::DistanceFormat
// ---------------------------------------------------------
//
void CHtkNavigationControl::DistanceFormat( TDes& aDistanceString )
    {
    //max distance is 20.000km, so aDistanceString is allways 
    //long enough
    TBuf<KBlidLBItemMaxLen> temp;    
    TInt result;
	if( !iRouter->IsAnyPointSet() )
    	{
    	// 
    	// Destination not set
    	//
    	aDistanceString = *iDataBlank;
    	return;
    	}
    	
    if ( iSettings->UnitOfMeasurement() == EUnitsMetric )
        {
        if ( iDistance >= KKilometerToMeters )
            {
            TReal distanceInKm = iDistance / KKilometerToMeters;
            if(distanceInKm >1 && distanceInKm <100)
                {
                BlidUtils::DistanceFormat( aDistanceString, distanceInKm,ETrue );            
                }
            else
                {
                BlidUtils::DistanceFormat( aDistanceString, distanceInKm, EFalse );                            
                }                
            }
        else
            {
            if ( BlidUtils::TRealToTInt(iDistance, result) != KErrNone )
                {
                result = 0;   
                }                        
            BlidUtils::DistanceFormat( aDistanceString, result, EFalse ); 	
            }
        }
    else
        {
        TReal distanceImperial = iDistance*KMetreToFoot;
        if ( distanceImperial >= KMileToFeets ) 
            {
            distanceImperial /= KMileToFeets;
            if(distanceImperial >10 && distanceImperial <100)
                {
                BlidUtils::DistanceFormat( aDistanceString, distanceImperial,ETrue );
                }
            else
                {
                BlidUtils::DistanceFormat( aDistanceString, distanceImperial, EFalse );                            
                }                
            }
        else
            {    
            if ( BlidUtils::TRealToTInt(distanceImperial, result) != KErrNone )
                {
                 result = 0;   
                }
            BlidUtils::DistanceFormat( aDistanceString, result, EFalse );
            }
        }
    }
    
// ---------------------------------------------------------
// CHtkNavigationControl::DistanceUnit
// ---------------------------------------------------------
//
void CHtkNavigationControl::DistanceUnit( TDes& aUnitString )
    {
    //max distance is 20.000km, so aDistanceString is allways 
    //long enough
    TInt result;

    if ( iSettings->UnitOfMeasurement() == EUnitsMetric )
        {
        if ( iDistance >= KKilometerToMeters )
            {
                         
            aUnitString.Append(*iDistanceFormatKm);            
            }
        else
            {
            if ( BlidUtils::TRealToTInt(iDistance, result) != KErrNone )
                {
                 result = 0;   
                }
            
            aUnitString.Append(*iDistanceFormatM);           
            }
        }
    else
        {
        TReal distanceImperial = iDistance*KMetreToFoot;
        if ( distanceImperial >= KMileToFeets ) 
            {
            distanceImperial /= KMileToFeets;
            aUnitString.Append(*iDistanceFormatMiles); 
            }
        else
            {    
            if ( BlidUtils::TRealToTInt(distanceImperial, result) != KErrNone )
                {
                 result = 0;   
                }
            aUnitString.Append(*iDistanceFormatFt);
                       
            }
        }
    }

// ----------------------------------------------------------------------------
// CHtkNavigationControl::SpeedFormat
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::SpeedFormat( TDes& aSpeedString, TDes& aSpeedFormatString)
    {
    if(!iRouter->IsAnyPointSet())
    	{
    	// 
    	// Destination not set
    	//
    	aSpeedString = *iDataBlank;
    	aSpeedFormatString = *iDataBlank;
    	return;
    	}
    
    if ( iIsSpeed )
        {
        TReal speedInKmph = iSpeed * KSpeedConvertion;
        //
        //If speed is less than 1km or mile per hour,then no data
        // is displayed
        //
        if(speedInKmph < 1)
        	{
        	aSpeedString = *iDataBlank;
        	aSpeedFormatString = *iDataBlank;
        	return;
        	}
        TBuf<KBlidLBItemMaxLen> temp;        
        //HBufC* formatter = NULL;
        if ( iSettings->UnitOfMeasurement() == EUnitsMetric )
            {
            temp.Format( KSpeedFormat, speedInKmph );
            //formatter = iSpeedFormatKmph;
            aSpeedString.Copy(temp);
            aSpeedFormatString.Copy(*iSpeedFormatKmph);
            }
        else
            {
            speedInKmph *= KKilometreToMiles;
            temp.Format( KSpeedFormat, speedInKmph );
            //formatter = iSpeedFormatMph;  
            aSpeedString.Copy(temp);
            aSpeedFormatString.Copy(*iSpeedFormatMph);           
            }
        //StringLoader::Format( aSpeedString, *formatter, 0, temp );  
        }
    else
        {    
     	aSpeedString = *iDataBeingCalculated;
        }
    }

// ----------------------------------------------------------------------------
// CHtkNavigationControl::StopPlayerL
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::StopPlayerL(  )
	{
    if( iToneUtility && isStopUpdate )
	    {
	    iToneUtility->CanclePayer();
	    ChangeFlagVisualImageL();
	    isStopUpdate = EFalse;
		// change back the softkey to optionexit
	    if( iView.CBAPtr() )
		    {
		    iView.CBAPtr()->SetCommandSetL( R_BLID_SOFTKEYS_OPTIONS_EXIT__CONTEXTOPTIONS );
		    iView.CBAPtr()->DrawDeferred();
		    }			    
	    }	    
	}


// ----------------------------------------------------------------------------
// CHtkNavigationControl::NotifyControlVisibility
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::NotifyControlVisibility( TBool aIsVisible, 
                              						 CAlfDisplay& /*aDisplay*/ )
	{
	if( !aIsVisible )
		{
		TRAP_IGNORE(StopPlayerL());	
		}
	}

// ----------------------------------------------------------------------------
// CHtkNavigationControl::UpdateLayoutL
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::UpdateLayoutL( )
	{
	// Call all the root label setanchor and adjust the new LAyout rectangle.
	CalculateRects();	
	SetAnchors();
	}

// ----------------------------------------------------------------------------
// CHtkNavigationControl::UpdateLayoutL
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::CalculateRects( )
	{
	TAknLayoutRect mainPane;
	TAknLayoutRect mainBlid2Pane;
	TAknLayoutRect layoutWindow;
	TAknLayoutText textLayout;
	TRect mainRect;
	TRect mainBlidRect;
	TRect navigationRect;
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
	
	/////////////// calculate signal strength  rect /////////////////////////

	/*TRect mainPaneRect;
	AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );*/
    mainPane.LayoutRect( iView.ApplicationRect(),
    							AknLayoutScalable_Apps::main_pane(mainvariety).LayoutLine() );
    							
	mainRect = mainPane.Rect(); 
	
    mainBlid2Pane.LayoutRect( mainRect,
    							AknLayoutScalable_Apps::main_blid2_pane().LayoutLine() );
    							
	mainBlidRect = mainBlid2Pane.Rect(); 							
    																				   
    layoutWindow.LayoutRect( mainBlidRect, 
    							AknLayoutScalable_Apps::blid2_navig_pane( 0 ).LayoutLine() );
    							
	navigationRect =  layoutWindow.Rect();   							

	// Since the Hitchcock does draw at main pane, we take the reference to
	// Navigation(main) pane's.
	navigationRect.Move(navigationRect.iTl.iX,-navigationRect.iTl.iY);  
    							
	/////////////// calculate Navigation  rect /////////////////////////
	layoutWindow.LayoutRect( navigationRect, 
    							AknLayoutScalable_Apps::blid2_navig_pane_g1( variety ).LayoutLine() );
	
	iSignalStrengthRect = layoutWindow.Rect();
	
	/////////////// calculate Diastance  rect /////////////////////////
	textLayout.LayoutText( navigationRect, 
    							AknLayoutScalable_Apps::blid2_navig_pane_t2( variety ).LayoutLine() );
    							
	iDistanceRect = textLayout.TextRect();
		    								
	/////////////// calculate Landmark Name rect /////////////////////////
	textLayout.LayoutText( navigationRect, 
    							AknLayoutScalable_Apps::blid2_navig_pane_t1( variety ).LayoutLine() );
	iLmkNameRect = textLayout.TextRect();
	
	/////////////// calculate Speed pane rects /////////////////////////	
	layoutWindow.LayoutRect( navigationRect, 
    							AknLayoutScalable_Apps::blid2_speed_pane( variety ).LayoutLine() );
	
	
	TRect speedPaneRect = layoutWindow.Rect();
	
	layoutWindow.LayoutRect( speedPaneRect, 
    							AknLayoutScalable_Apps::blid2_speed_pane_g1( 0 ).LayoutLine() );
    
    // Bg image	Rect						
	iSpeedBgRect = 	layoutWindow.Rect();
	
	textLayout.LayoutText( speedPaneRect, 
    							AknLayoutScalable_Apps::blid2_speed_pane_t2( 0 ).LayoutLine() );
	
	// Speed Rect
	iSpeedRect = textLayout.TextRect();
	
	textLayout.LayoutText( speedPaneRect, 
    							AknLayoutScalable_Apps::blid2_speed_pane_t1( 0 ).LayoutLine() );
	
	// Format Rect
	iSpdFormatRect = textLayout.TextRect();
	iSpdFormatRect.Move( 0, -6);	// For Text alignment
	
	
	
	/////////////// calculate Navigation ring pane rect /////////////////////////
	layoutWindow.LayoutRect( navigationRect, 
    							AknLayoutScalable_Apps::blid2_navig_ring_pane( variety ).LayoutLine() );
	
	
	TRect navRingPaneRect = layoutWindow.Rect();
	
	layoutWindow.LayoutRect( navRingPaneRect, 
    							AknLayoutScalable_Apps::blid2_navig_ring_pane_g1( 0 ).LayoutLine() );
	
	
	iNavRingBgRect = layoutWindow.Rect();
	
	/////////////// calculate North rect /////////////////////////	
	layoutWindow.LayoutRect( navRingPaneRect, 
    							AknLayoutScalable_Apps::blid2_navig_ring_pane_g2( 0 ).LayoutLine() );
	
	iNorthRect = layoutWindow.Rect();							

	/////////////// calculate Direction Ind rect /////////////////////////	
	layoutWindow.LayoutRect( navRingPaneRect, 
    							AknLayoutScalable_Apps::blid2_navig_ring_pane_g3( 0 ).LayoutLine() );
    							
	iDirectionIndRect = layoutWindow.Rect();
	iDirectionIndRect.Move( 0, -15);	// For Proper alignment
    							
	
	/////////////// calculate Accuracy Ring rect /////////////////////////	
	layoutWindow.LayoutRect( navRingPaneRect, 
    							AknLayoutScalable_Apps::blid2_navig_ring_pane_g4( 0 ).LayoutLine() );
	
	
	iAccRingRect = layoutWindow.Rect();
	/////////////// calculate Flag  rect /////////////////////////		
	layoutWindow.LayoutRect( navRingPaneRect, 
    							AknLayoutScalable_Apps::blid2_navig_ring_pane_g5( 0 ).LayoutLine() );
	
	iFlagRect = layoutWindow.Rect();
	
	}


// ----------------------------------------------------------------------------
// CHtkNavigationControl::SetAnchors
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::SetAnchors( )
	{
	// Anchor for Signal strength image
	TAlfRealPoint topPoint( iSignalStrengthRect.iTl );
	TAlfRealPoint bottomPoint( iSignalStrengthRect.iBr );

    iLayout->SetAnchor( EAlfAnchorTopLeft, 0, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY  ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 0, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY  ) );

    // Anchor for Name
	topPoint = iLmkNameRect.iTl;
    bottomPoint = iLmkNameRect.iBr;
    
    iLayout->SetAnchor( EAlfAnchorTopLeft, 1, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 1, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );
        
    // Anchor for Distance
    topPoint = iDistanceRect.iTl;
    bottomPoint = iDistanceRect.iBr;
    
    iLayout->SetAnchor( EAlfAnchorTopLeft, 2, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 2, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );
        
    // Anchor for Speed Pane Bg Image
    topPoint = iSpeedBgRect.iTl;
    bottomPoint = iSpeedBgRect.iBr;
    
    iLayout->SetAnchor( EAlfAnchorTopLeft, 3, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 3, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );
        
    // Anchor for Speed 
    topPoint = iSpeedRect.iTl;
    bottomPoint = iSpeedRect.iBr;
    
    iLayout->SetAnchor( EAlfAnchorTopLeft, 4, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 4, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );
    
    // Anchor for Speed Format
    topPoint = iSpdFormatRect.iTl;
    bottomPoint = iSpdFormatRect.iBr;
    
    iLayout->SetAnchor( EAlfAnchorTopLeft, 5, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 5, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );
        

    // Anchor for Nav Ring Pane
    topPoint = iNavRingBgRect.iTl;
    bottomPoint = iNavRingBgRect.iBr;
    
    iLayout->SetAnchor( EAlfAnchorTopLeft, 6, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 6, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );

    // Anchor for Acc Ring Pane
    topPoint = iAccRingRect.iTl;
    bottomPoint = iAccRingRect.iBr;
    
    iLayout->SetAnchor( EAlfAnchorTopLeft, 7, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 7, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );
        
	//Anchor for Visuals inside curve ring (iCurveDecLyt)
    topPoint = iNavRingBgRect.iTl;
    bottomPoint = iNavRingBgRect.iBr;
        
    iLayout->SetAnchor( EAlfAnchorTopLeft, 8, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 8, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );

    // Anchor for Direction Ind Pane
    topPoint = iDirectionIndRect.iTl;
    bottomPoint = iDirectionIndRect.iBr;
    
    iLayout->SetAnchor( EAlfAnchorTopLeft, 9, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( topPoint.iX, topPoint.iY ) );
    iLayout->SetAnchor( EAlfAnchorBottomRight, 9, 
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop,
        EAlfAnchorMetricAbsolute, EAlfAnchorMetricAbsolute,
        TAlfTimedPoint( bottomPoint.iX, bottomPoint.iY ) );
        
	}
                              						 
// ----------------------------------------------------------------------------
// TimerCallBackFunction
// ----------------------------------------------------------------------------
//
TInt TimerCallBackFunction(TAny* aPtr)
    {
    CHtkNavigationControl* ptr = static_cast<CHtkNavigationControl*>(aPtr);
    if( ptr )
        {
        if( ptr->HasArrivedDestination() )
            {
            TRAP_IGNORE(ptr->ChangeFlagVisualImageL());
            ptr->iTimerComplete = ETrue;
            }
        }
    return 0;
    }

// ----------------------------------------------------------------------------
// CHtkNavigationControl::CreateSignalTexturesL
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::CreateSignalTexturesL( )
	{
	MAlfBitmapProvider* provider = NULL;	
    // then create the bitmap provider
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
// CHtkNavigationControl::CreatePopupImgTextureL
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::CreatePopupImgTextureL( )
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
	
// ----------------------------------------------------------------------------
// CHtkNavigationControl::CreateAccTextureL
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::CreateAccTextureL( )
	{
	MAlfBitmapProvider* provider = NULL;
	
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL( KAknsIIDQgnGrafBlidDetCircle, // svg icon skin constant id defiened in Aknsconstant.h file
    												 iView.ImagePathName(),        // mif file name with path.
    												 EMbmBlidQgn_graf_blid_det_circle, // bitmap id defiened in mbg file
    												 EMbmBlidQgn_graf_blid_det_circle_mask ); // bitmap mask id defiened in mbg file
	// now using that bitmap provider create the texture.
	CAlfTexture & thumbTexture = iEnv.TextureManager().CreateTextureL( KAlfAutoGeneratedTextureId,
	                                                                provider,
	                                                                EAlfTextureFlagDefault ); 
		                                                                  
	iAccTexture = &thumbTexture;
	}		

// ----------------------------------------------------------------------------
// CHtkNavigationControl::LaunchStylusPopupMenuL
// ----------------------------------------------------------------------------
//
void CHtkNavigationControl::LaunchStylusPopupMenuL( TPointerEvent aPointerEvent )
	{
	if( !iMenu )
		{
		iMenu = CAknStylusPopUpMenu::NewL( &iView, aPointerEvent.iParentPosition, NULL );
	    TInt resourceReaderId = R_BLID_DESTINATION_STYLUS_MENU;
	    TResourceReader reader;
	    CCoeEnv* env = iView.CoeEnv(); 
	    env->CreateResourceReaderLC( reader , resourceReaderId );
	    iMenu->ConstructFromResourceL( reader );
	    CleanupStack::PopAndDestroy(); // reader
		}
		
   if(iView.IsLmFeatureAvailable())
        {
        iMenu->SetItemDimmed(EBlidCmdWaypoint,ETrue);
        if(!iIsDestinationSet)
            {
            iMenu->SetItemDimmed(EBlidCmdStopNavigation,ETrue);
            if(!iView.IsLmAvailableL())
                {
                iMenu->SetItemDimmed(EBlidCmdLmk,ETrue);
                }
            else
                {
                iMenu->SetItemDimmed(EBlidCmdLmk,EFalse);
                }                        
            }
        else
            {
            iMenu->SetItemDimmed(EBlidCmdStopNavigation,EFalse);
            if(!iView.IsLmAvailableL())
                {
                iMenu->SetItemDimmed(EBlidCmdLmk,ETrue);
                }                    
            else
                {
                iMenu->SetItemDimmed(EBlidCmdLmk,EFalse);
                }                        
            }                    
        }
    else
        {
        iMenu->SetItemDimmed(EBlidCmdLmk,ETrue);
        if(!iIsDestinationSet)
            {
            iMenu->SetItemDimmed(EBlidCmdStopNavigation,ETrue);
            if(!iView.IsWpAvailable())
                {
                iMenu->SetItemDimmed(EBlidCmdWaypoint,ETrue);
                }
            else
                {
                iMenu->SetItemDimmed(EBlidCmdWaypoint,EFalse);
                }                        
            }
        else
            {
            iMenu->SetItemDimmed(EBlidCmdStopNavigation,EFalse);
            if(!iView.IsWpAvailable())
                {
                iMenu->SetItemDimmed(EBlidCmdWaypoint,ETrue);
                }
            else
                {
                iMenu->SetItemDimmed(EBlidCmdWaypoint,EFalse);
                }                        
            }                                    
        }		
    iMenu->SetPosition( aPointerEvent.iPosition );
    iMenu->ShowMenu();	
    }
    
//End of File
