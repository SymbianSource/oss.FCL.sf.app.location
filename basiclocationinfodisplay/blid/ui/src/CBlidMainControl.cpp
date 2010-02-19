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
#include <aknmessagequerydialog.h> //CAknMessageQueryDialog
#include <StringLoader.h>
#include <aknconsts.h>
#include <Blid.rsg>
#include <akntitle.h> 
#include <aknnavide.h>
#include <eikspane.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <blid.mbg>

// HTK includes
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
#include <alf/alfimageloaderutil.h>
#include <alf/alfimagevisual.h>

#include "Blid.hrh"

// USER INCLUDES
#include "CBlidMainControl.h"
#include "CBlidEng.h"
#include "CBlidDocument.h"
#include "MBlidLocation.h"
#include "CBlidBaseView.h"
#include "CBlidBaseContainer.h"
#include "bliduiconsts.h"
#include "BlidNotes.h"
#include "CBlidLocSettingLauncher.h"
#include "Debug.h"

TInt CBlidMainControl::iPSYTimeoutCount = 0;

TInt MessageQueryCallBack(TAny* aPtr);

const TInt KTimerValue = 200000;

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CBlidMainControl::NewL()
// Two phased constructor
// ---------------------------------------------------------
//
CBlidMainControl* CBlidMainControl::NewL(CAlfEnv& aEnv, const TRect& aRect,
        CBlidBaseView& aView)
    {
    CBlidMainControl* self = new (ELeave) CBlidMainControl(aEnv, aView);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
//CBlidMainControl::CBlidMainControl()
// First phase constructor, can not leave
// ---------------------------------------------------------
//
CBlidMainControl::CBlidMainControl(CAlfEnv& aEnv, CBlidBaseView& aView) :
    CAlfControl(), iEnv(aEnv), iView(aView)
    {
    iOnlineMode = EFalse;
    }

// ---------------------------------------------------------
// CBlidMainControl::ConstructL()
// Second phase constructor, may leave
// ---------------------------------------------------------
//
void CBlidMainControl::ConstructL(const TRect& /*aRect*/)
    {
    CAlfControl::ConstructL(iEnv);

    iMainLayout = CAlfAnchorLayout::AddNewL(*this);
    iSearchTextVisual = CAlfTextVisual::AddNewL(*this, iMainLayout);
    iAnimationVisual = CAlfImageVisual::AddNewL(*this, iMainLayout);

    iDisplayIcon = 1;
    iPeriodic = CPeriodic::NewL(0); // neutral priority 
    if (!iPeriodic->IsActive())
        {
        iPeriodic->Start(TTimeIntervalMicroSeconds32(KTimerValue),
                TTimeIntervalMicroSeconds32(KTimerValue), TCallBack(Tick,
                        this));
        }

    UpdateDisplayLayoutL();
    CreateSearchingTextL();
    CreateAnimationL();
    UpdateAnimation();

    iLauncher = NULL;
    isSettingsLaunched = EFalse;

    //start requesting
    CBlidEng* engine = iView.BlidDocument()->Engine();
    iLocation = engine->LocationModel();
    }

// ---------------------------------------------------------
// CBlidMainControl::Tick()
// Timer callback function, to update image in animation
// ---------------------------------------------------------
//
TInt CBlidMainControl::Tick(TAny* aObject)
    {
    ((CBlidMainControl*) aObject)->UpdateAnimation(); // cast, and call non-static function
    return KErrNone;
    }

// ---------------------------------------------------------
// CBlidMainControl::CreateSearchingTextL()
// Function to create search text visual
// ---------------------------------------------------------
//
void CBlidMainControl::CreateSearchingTextL()
    {
    /* Layout Hierarchy
     main_blid2_pane -> blid2_search_pane -> blid2_search_pane_t1
     */

    // Get the text font

    TInt typefaceStyleId = iEnv.TextStyleManager().CreatePlatformTextStyleL(
            EAknLogicalFontPrimarySmallFont, EAlfTextStyleNormal);
    CAlfTextStyle* style1 =
            iEnv.TextStyleManager().TextStyle(typefaceStyleId);
    style1->SetBold(ETrue);
    // Set the text font size in pixel
    style1->SetTextPaneHeightInPixels(26, ETrue);

    TRgb rgb;
    // Fetch the skin color 
    AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), rgb,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG1);

    iSearchTextVisual->SetColor(rgb);
    HBufC* noteText = StringLoader::LoadLC(R_BLID_NOTE_SEARCHING_ANIMATION,
            CEikonEnv::Static());
    iSearchTextVisual->SetTextL(*noteText);
    iSearchTextVisual->SetTextStyle(style1->Id());
    iSearchTextVisual->SetAlign(EAlfAlignHCenter, EAlfAlignVCenter);
    CleanupStack::PopAndDestroy(noteText); //noteText

    }

// ---------------------------------------------------------
// CBlidMainControl::CreateAnimationL()
// Creates image visuals for animation
// ---------------------------------------------------------
//
void CBlidMainControl::CreateAnimationL()
    {
    /* Layout Hierarchy
     main_blid2_pane -> blid2_search_pane -> blid2_search_pane_g1
     */

    // code to load svg icon
    MAlfBitmapProvider* provider = NULL;

    // first creste the image loader utils and then call SetSize.	
    iView.ImageLoaderUtils()->SetSize(iAnimationRect.Size());

    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL(
            KAknsIIDQgnGrafBlidStartup, // svg icon skin constant id defiened in Aknsconstant.h file
            iView.ImagePathName(), // mif file name with path.
            EMbmBlidQgn_graf_blid_startup_01, // bitmap id defiened in mbg file
            EMbmBlidQgn_graf_blid_startup_01_mask); // bitmap mask id defiened in mbg file
    // now using that bitmap provider create the texture.
    CAlfTexture & texture1 = iEnv.TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault);
    iTextures.Append(&texture1);

    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL(
            KAknsIIDQgnGrafBlidStartup, // svg icon skin constant id defiened in Aknsconstant.h file
            iView.ImagePathName(), // mif file name with path.
            EMbmBlidQgn_graf_blid_startup_02, // bitmap id defiened in mbg file
            EMbmBlidQgn_graf_blid_startup_02_mask); // bitmap mask id defiened in mbg file
    // now using that bitmap provider create the texture.
    CAlfTexture & texture2 = iEnv.TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault);
    iTextures.Append(&texture2);

    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL(
            KAknsIIDQgnGrafBlidStartup, // svg icon skin constant id defiened in Aknsconstant.h file
            iView.ImagePathName(), // mif file name with path.
            EMbmBlidQgn_graf_blid_startup_03, // bitmap id defiened in mbg file
            EMbmBlidQgn_graf_blid_startup_03_mask); // bitmap mask id defiened in mbg file
    // now using that bitmap provider create the texture.
    CAlfTexture & texture3 = iEnv.TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault);
    iTextures.Append(&texture3);

    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL(
            KAknsIIDQgnGrafBlidStartup, // svg icon skin constant id defiened in Aknsconstant.h file
            iView.ImagePathName(), // mif file name with path.
            EMbmBlidQgn_graf_blid_startup_04, // bitmap id defiened in mbg file
            EMbmBlidQgn_graf_blid_startup_04_mask); // bitmap mask id defiened in mbg file
    // now using that bitmap provider create the texture.
    CAlfTexture & texture4 = iEnv.TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault);
    iTextures.Append(&texture4);

    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL(
            KAknsIIDQgnGrafBlidStartup, // svg icon skin constant id defiened in Aknsconstant.h file
            iView.ImagePathName(), // mif file name with path.
            EMbmBlidQgn_graf_blid_startup_05, // bitmap id defiened in mbg file
            EMbmBlidQgn_graf_blid_startup_05_mask); // bitmap mask id defiened in mbg file
    // now using that bitmap provider create the texture.
    CAlfTexture & texture5 = iEnv.TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault);
    iTextures.Append(&texture5);

    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL(
            KAknsIIDQgnGrafBlidStartup, // svg icon skin constant id defiened in Aknsconstant.h file
            iView.ImagePathName(), // mif file name with path.
            EMbmBlidQgn_graf_blid_startup_06, // bitmap id defiened in mbg file
            EMbmBlidQgn_graf_blid_startup_06_mask); // bitmap mask id defiened in mbg file
    // now using that bitmap provider create the texture.
    CAlfTexture & texture6 = iEnv.TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault);
    iTextures.Append(&texture6);

    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL(
            KAknsIIDQgnGrafBlidStartup, // svg icon skin constant id defiened in Aknsconstant.h file
            iView.ImagePathName(), // mif file name with path.
            EMbmBlidQgn_graf_blid_startup_07, // bitmap id defiened in mbg file
            EMbmBlidQgn_graf_blid_startup_07_mask); // bitmap mask id defiened in mbg file
    // now using that bitmap provider create the texture.
    CAlfTexture & texture7 = iEnv.TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault);
    iTextures.Append(&texture7);

    // then create the bitmap provider  
    provider = iView.ImageLoaderUtils()->CreateImageLoaderL(
            KAknsIIDQgnGrafBlidStartup, // svg icon skin constant id defiened in Aknsconstant.h file
            iView.ImagePathName(), // mif file name with path.
            EMbmBlidQgn_graf_blid_startup_08, // bitmap id defiened in mbg file
            EMbmBlidQgn_graf_blid_startup_08_mask); // bitmap mask id defiened in mbg file
    // now using that bitmap provider create the texture.
    CAlfTexture & texture8 = iEnv.TextureManager().CreateTextureL(
            KAlfAutoGeneratedTextureId, provider, EAlfTextureFlagDefault);
    iTextures.Append(&texture8);

    }

// ---------------------------------------------------------
// CBlidMainControl::UpdateAnimation
// updates the animation image
// ---------------------------------------------------------
//
void CBlidMainControl::UpdateAnimation()
    {
    if (KNumOfStartUpIcon > iDisplayIcon)
        {
        iDisplayIcon++;
        }
    else
        {
        iDisplayIcon = 1;
        }
    iAnimationVisual->SetImage(TAlfImage(*iTextures[iDisplayIcon - 1]));//N   
    }

// ----------------------------------------------------
// CBlidMainControl::~CBlidMainControl
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CBlidMainControl::~CBlidMainControl()
    {
    iTextures.Reset();
    iTextures.Close();
    delete iLauncher;
    delete iHeadPaneText;
    delete iMsgQueryText;
    delete iLinkText;
    delete iMsgQText;
    delete iPeriodic;
    }

// ---------------------------------------------------------
// CBlidMainControl::UpdateL
// Updates the view after getting update from location model
// ---------------------------------------------------------
//
void CBlidMainControl::UpdateL()
    {
    TInt result = 0;
    DEBUG1(" CBlidMainControl::UpdateL iErrorCode ::%d", iErrorCode );

    if (!iView.IsSatViewActive() && iOnlineMode)
        {

        if (iView.IsForeGroundApp())
            {
            iView.ActivateSatelliteViewL();
            }
        }
    TInt retVal = iLocation->CheckGPSAvailability();
    // Check GPS device availability
    DEBUG1(" CBlidMainControl::CheckGPSAvailability code ::%d", retVal );
    switch (iErrorCode)
        {
        case KErrTimedOut:
            {
            if (retVal == 1)
                {
                /* 
                 GPS device connected but position information
                 available but not of good quality
                 */
                if (iView.IsForeGroundApp())
                    {
                    iView.ActivateSatelliteViewL();
                    }
                break;
                }
            case KErrNotFound:
            // No module (PSY) selected or invalid PSY
                {
                iOnlineMode = EFalse;
                // Notify not module selected
                //            if (iPSYTimeoutCount == 0)
                //                {
                if (iView.IsForeGroundApp())
                    {
                    iLocation->StopRequesting();
                    TCallBack callback(MessageQueryCallBack, this);
                    result
                            = DispMsgQueryWithLinkL(R_BLID_NOPSY_ENABLED,
                                    R_BLID_ERROR_NO_PSY,
                                    R_BLID_SELECT_POSITIONING_METHOD, ETrue,
                                    callback);
                    if (result == EBlidSoftkeyRetry)
                        {
                        iLocation->StartRequesting();
                        }
                    else if (result == EAknSoftkeyOk)
                        {
                        iView.ExitMainApplicationL(EEikCmdExit);
                        }
                    }
                break;
                }
            case KPositionPartialUpdate:
                {
                if (iView.IsForeGroundApp())
                    {
                    iView.ActivateSatelliteViewL();
                    }
                break;
                }
            case KErrArgument:
            // The positioning module is unable to support the requested type
            default:
                {
                iOnlineMode = EFalse;

                if (retVal == 0)
                    {
                    if (iLocation->IsSatCapablePsyAvailable())
                        {
                        iLocation->StopRequesting();
                        iOnlineMode = EFalse;
                        TCallBack callback(MessageQueryCallBack, this);
                        result = DispMsgQueryWithLinkL(R_BLID_NOGPS_FOUND,
                                R_BLID_NOGPS_FOUND_TEXT,
                                R_BLID_SELECT_POSITIONING_METHOD, ETrue,
                                callback);
                        if (result == EBlidSoftkeyRetry)
                            {
                            iLocation->StartRequesting();
                            }
                        else if (result == EAknSoftkeyOk)
                            {
                            iView.ExitMainApplicationL(EEikCmdExit);
                            }
                        }
                    // no GPS device note
                    // GPS un-available
                    else if (iView.IsForeGroundApp())
                        {
                        iLocation->StopRequesting();
                        TCallBack callback(MessageQueryCallBack, this);
                        DispMsgQueryWithLinkL(R_BLID_NOGPS_FOUND,
                                R_BLID_NOGPS_AVAILABLE_TEXT,
                                R_BLID_SELECT_POSITIONING_METHOD, EFalse,
                                callback);
                        iView.ExitMainApplicationL(EEikCmdExit);
                        }
                    else
                        {
                        BlidNotes::iGPSUnavailableDisplayed = ETrue;
                        }
                    CBlidBaseContainer::SetGPSAvailability(EFalse);
                    }
                break;

                }
            }
        }
    }

// ----------------------------------------------------
// CBlidMainControl::MakeTitleL
// Sets the title to string represented by resource
// ----------------------------------------------------
//
void CBlidMainControl::MakeTitleL(TInt aResourceText)
    {
    HBufC* buffer = StringLoader::LoadLC(aResourceText);
    CAknTitlePane* title =
            static_cast<CAknTitlePane*> (StatusPane()-> ControlL(TUid::Uid(
                    EEikStatusPaneUidTitle)));
    CleanupStack::Pop(); //buffer
    title->SetText(buffer); // Takes ownership of buf
    }

// ----------------------------------------------------
// CBlidMainControl::StatusPane
// Returns the status pane
// ----------------------------------------------------
//
CEikStatusPane* CBlidMainControl::StatusPane() const
    {
    return CEikonEnv::Static()->AppUiFactory()->StatusPane();
    }

// ----------------------------------------------------
// CBlidMainControl::SetErrorCode
// Sets the error code
// ----------------------------------------------------
//
void CBlidMainControl::SetErrorCode(const TInt aErrorCode)
    {
    iErrorCode = aErrorCode;
    }

// ----------------------------------------------------
// CBlidMainControl::SetOnlineMode
// Sets the online mode
// ----------------------------------------------------
//
void CBlidMainControl::SetOnlineMode(const TBool aOnlineMode)
    {
    iOnlineMode = aOnlineMode;
    }

// ----------------------------------------------------
// CBlidMainControl::DispMsgQueryWithLinkL()
// Displays the message query with a link
// ----------------------------------------------------
//
TInt CBlidMainControl::DispMsgQueryWithLinkL(TInt aHeadingText,
        TInt aMsgQueryText, TInt aLinkText, TBool aLinkShow,
        TCallBack aCallBack)
    {
    CEikonEnv* env = CEikonEnv::Static();
    if (!iHeadPaneText)
        {
        iHeadPaneText = env->AllocReadResourceL(aHeadingText);
        }

    if (!iMsgQueryText)
        {
        iMsgQueryText = env->AllocReadResourceL(aMsgQueryText);
        }

    if (!iLinkText)
        {
        iLinkText = env->AllocReadResourceL(aLinkText);
        }

    if (!iMsgQText)
        {
        iMsgQText = HBufC::NewL(iMsgQueryText->Length() + KNewLine().Length()
                + KOpeningLinkTag().Length() + iLinkText->Length()
                + KClosingLinkTag().Length());
        }

    iMsgQText->Des().Copy(*iMsgQueryText);
    iMsgQText->Des().Append(KNewLine);
    if (aLinkShow)
        {
        iMsgQText->Des().Append(KOpeningLinkTag);
        iMsgQText->Des().Append(*iLinkText);
        iMsgQText->Des().Append(KClosingLinkTag);
        }
    else
        {
        iMsgQText->Des().Append(_L( " "));
        }
    //////////////////////////////////////////////////////
    CAknMessageQueryDialog* msgDlg = CAknMessageQueryDialog::NewL(*iMsgQText);

    // if we remove this if part it will give CONE8 panic
    if (iDialog)
        {
        delete iDialog;
        iDialog = NULL;
        }

    iDialog = msgDlg;

    msgDlg->PrepareLC(R_BLID_MESSAGE_QUERY_DIALOG); // Pushed dialog is popped inside RunLD

    msgDlg->Heading()->SetTextL(*iHeadPaneText);

    msgDlg->SetLink(aCallBack);

    msgDlg->ButtonGroupContainer().SetCommandSetL(
            R_BLID_INFOPOPUP_SOFTKEYS_RETRY__EXIT);
    isDialogLaunched = ETrue;
    TInt retval = msgDlg->RunLD();
    if (isDialogLaunched)
        {
        msgDlg = NULL;
        iDialog = NULL;
        isDialogLaunched = EFalse;
        return retval;
        }
    return -1;
    }

// ----------------------------------------------------
// CBlidMainControl::CreateNaviPaneL
// Creates navigation pane
// ----------------------------------------------------
//
void CBlidMainControl::CreateNaviPaneL()
    {
    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    // Fetch pointer to the default navi pane control
    CAknNavigationControlContainer* naviPane =
            static_cast<CAknNavigationControlContainer*> (sp->ControlL(
                    TUid::Uid(EEikStatusPaneUidNavi)));

    naviPane->PushDefaultL(EFalse);
    }

// ----------------------------------------------------
// CBlidMainControl::LaunchPositioningSettingsViewL
// Launches positioning settings view
// ----------------------------------------------------
//
void CBlidMainControl::LaunchPositioningSettingsViewL()
    {
    if (!iLauncher)
        {
        iLauncher = CBlidLocSettingsLauncher::NewL();
        }
    iLauncher->SetControl(this);
    isSettingsLaunched = ETrue;
    iLauncher->LaunchL();
    }

// ----------------------------------------------------
// CBlidMainControl::DeleteDialogResource
// Seletes the dialog resources
// ----------------------------------------------------
//
void CBlidMainControl::DeleteDialogResource()
    {
    if (isSettingsLaunched)
        {
        delete iHeadPaneText;
        iHeadPaneText = NULL;

        delete iMsgQueryText;
        iMsgQueryText = NULL;

        delete iLinkText;
        iLinkText = NULL;

        delete iMsgQText;
        iMsgQText = NULL;
        }
    }

// ----------------------------------------------------
// CBlidMainControl::UpdateDisplayLayoutL
// Updates the display layout in response to a change in app resource
// ----------------------------------------------------
//
void CBlidMainControl::UpdateDisplayLayoutL()
    {
    /* Layout Hierarchy
     main_blid2_pane -> blid2_search_pane -> blid2_search_pane_t1
     
     main_blid2_pane -> blid2_search_pane -> blid2_search_pane_g1
     */

    TAknLayoutRect mainBlid2Pane;
    TAknLayoutRect blid2SearchPane;
    TAknLayoutRect blid2SearchPaneG1;
    TAknLayoutText blid2SearchPaneT1;
    //TRect animationRect;	
    TInt variety;

    if (Layout_Meta_Data::IsLandscapeOrientation())
        {
        variety = 1;
        }
    else
        {
        variety = 0;
        }
    /////////////// recalculate text rect /////////////////////////
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    mainBlid2Pane.LayoutRect(rect,
            AknLayoutScalable_Apps::main_blid2_pane().LayoutLine());

    blid2SearchPane.LayoutRect(mainBlid2Pane.Rect(),
            AknLayoutScalable_Apps::blid2_search_pane(variety).LayoutLine());

    blid2SearchPaneT1.LayoutText(
            blid2SearchPane.Rect(),
            AknLayoutScalable_Apps::blid2_search_pane_t1(variety).LayoutLine());

    /////////////// recalculate animation rect /////////////////////////
    blid2SearchPaneG1.LayoutRect(
            blid2SearchPane.Rect(),
            AknLayoutScalable_Apps::blid2_search_pane_g1(variety).LayoutLine());
    iAnimationRect = blid2SearchPaneG1.Rect();

    TAlfRealPoint topTextPoint(blid2SearchPaneT1.TextRect().iTl);
    TAlfRealPoint bottomTextPoint(blid2SearchPaneT1.TextRect().iBr);

    //Anchor for searching Text	
    iMainLayout->SetAnchor(EAlfAnchorTopLeft, 0, EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop, EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute, TAlfTimedPoint(topTextPoint.iX,
                    topTextPoint.iY));
    iMainLayout->SetAnchor(EAlfAnchorBottomRight, 0, EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop, EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute, TAlfTimedPoint(bottomTextPoint.iX,
                    bottomTextPoint.iY));

    TAlfRealPoint topAnimationPoint(iAnimationRect.iTl);
    TAlfRealPoint bottomAnimationPoint(iAnimationRect.iBr);

    //Anchor for animation	
    iMainLayout->SetAnchor(EAlfAnchorTopLeft, 1, EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop, EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute, TAlfTimedPoint(topAnimationPoint.iX,
                    topAnimationPoint.iY));
    iMainLayout->SetAnchor(EAlfAnchorBottomRight, 1, EAlfAnchorOriginLeft,
            EAlfAnchorOriginTop, EAlfAnchorMetricAbsolute,
            EAlfAnchorMetricAbsolute, TAlfTimedPoint(bottomAnimationPoint.iX,
                    bottomAnimationPoint.iY));

    }

// -----------------------------------------------------------------------------
// CBlidMainControl::MessageQueryCallBack()
// Called when link is clicked on message query dialog
// -----------------------------------------------------------------------------
//
TInt MessageQueryCallBack(TAny* aPtr)
    {
    CBlidMainControl* ptr = static_cast<CBlidMainControl*> (aPtr);
    if (ptr)
        {
        TRAP_IGNORE( ptr->LaunchPositioningSettingsViewL() );
        if (ptr->iDialog)
            {
            TRAP_IGNORE( ptr->iDialog->ProcessCommandL( 3001 ) );
            }
        }
    return KErrNone;
    }
//End Of File

