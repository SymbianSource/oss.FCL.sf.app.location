/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility class for loading Application Icons
*
*/


// SYSTEM INCLUDES 
#include <AknsUtils.h>
#include <gulicon.h>
#include <fbs.h>
#include <data_caging_path_literals.hrh>
#include <s32mem.h>
#include <SVGEngineInterfaceImpl.h>
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayout2def.h>
#include <lcicons.mbg>

// USER INCLUDES
#include "lciconloader.h"
#include "lcipcparams.h"


// CONSTANT DEFINITION
_LIT( KLcDefaultIconFileName, "lcicons.mif" );

// ========================= MEMBER FUNCTIONS ================================
	
// ---------------------------------------------------------------------------
// EXPORT_C CGulIcon*	LcIconLoader::LoadApplicationIconL
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* LcIconLoader::LoadApplicationIconL( const TUid	aAppUid )
    {
    CGulIcon* icon = NULL;
    TRAPD( error, icon = LoadIconfromUIDL( aAppUid ));
    if( error )
        {
        icon = LoadDefaultIconL();
        }
        
    return icon;
    }

// ---------------------------------------------------------------------------
// EXPORT_C CGulIcon* LcIconLoader::LoadMifFileIconL
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* LcIconLoader::LoadMifFileIconL(
                          const TDesC&				aAppIconFile,
                                TInt				aFrameNo )		
    {
    CGulIcon* icon = NULL;
    TRAPD( error, icon = LoadIconFromMifFileL( aAppIconFile, aFrameNo ));
    if( error )
        {
        icon = LoadDefaultIconL();
        }
    return icon;    
    }
		
// ---------------------------------------------------------------------------
// EXPORT_C CGulIcon* LcIconLoader::LoadDefaultIconL
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* LcIconLoader::LoadDefaultIconL()
    {
    TFileName dllFileName;    
    Dll::FileName( dllFileName );
	
	TParse parse;
	User::LeaveIfError( parse.Set( KLcDefaultIconFileName, &KDC_APP_BITMAP_DIR, &dllFileName ));
    TFileName fileName( parse.FullName());

    CGulIcon* icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(),
        KAknsIIDQgnPropSetDeviceSub,
        fileName,
        EMbmLciconsQgn_menu_am_sis,
        EMbmLciconsQgn_menu_am_sis_mask );
        
    return icon;  
    }
	
// ---------------------------------------------------------------------------
// CGulIcon* LcIconLoader::LoadIconFromFileL
// ---------------------------------------------------------------------------
//
CGulIcon* LcIconLoader::LoadIconfromUIDL( const TUid	aAppUid )
    {
    // Create the Icon and the mask for the Application using AknUtils  
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    AknsUtils::CreateAppIconLC( AknsUtils::SkinInstance(),
                                aAppUid, 
                                EAknsAppIconTypeList, 
                                bitmap, 
                                mask );
    
    User::LeaveIfNull( bitmap );
    User::LeaveIfNull( mask );
        
    // If the bitmap and the mask exists then make it into a GulIcon
    // which can then be retured.    
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    
    // Not specifying the order because we dont know the order in which they
    // would be created. Though its generally, the bitmap first and then the
    // mask, we should be speculating.
    CleanupStack::Pop( 2 );
    return icon; 
    }
	
// ---------------------------------------------------------------------------
// CGulIcon* LcIconLoader::LoadIconFromMifFileL
// ---------------------------------------------------------------------------
//
CGulIcon* LcIconLoader::LoadIconFromMifFileL( const TDesC&  aAppIconFile,
                                                  TInt       aFrameNo )
    {
    // Create the Icon and the mask for the Application using AknUtils  
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
   
    AknIconUtils::CreateIconLC( bitmap, mask, aAppIconFile, aFrameNo, aFrameNo + 1 );
    
    User::LeaveIfNull( bitmap );
    User::LeaveIfNull( mask );
        
    // If the bitmap and the mask exists then make it into a GulIcon
    // which can then be retured.    
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    
    // Not specifying the order because we dont know the order in which they
    // would be created. Though its generally, the bitmap first and then the
    // mask, we should be speculating.
    CleanupStack::Pop( 2 );
    return icon;  
    }

