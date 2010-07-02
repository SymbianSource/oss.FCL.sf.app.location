/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -
*
*/


// INCLUDE FILES

// Added for testing
#include <eikcapc.h>
#include <eikimage.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <akncontext.h>
#include <aknappui.h>
#include <AknIconUtils.h>
#include <EPos_CPosLandmark.h>
#include <lmkerrors.h>

#include "CLmkUiUtils.h"
#include "MLmkEditorUiBuilder.h"
#include "MLmkFieldData.h"
#include "CLmkFieldData.h"
#include "CLmkFields.h"
#include "LmkConsts.h"
#include "landmarks.hrh"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
    const TInt KPosUrlColon (3);
 
    _LIT( KPanicMsg, "CLmkUiUtils" );

    void Panic( TPanicCode aReason )
        {
        User::Panic( KPanicMsg, aReason );
        } 
    } // namespace

//============================ MEMBER FUNCTIONS ===============================
//----------------------------------------------------------------------------
// CLmkUiUtils::NewL
// ----------------------------------------------------------------------------
//
CLmkUiUtils* CLmkUiUtils::NewL()
    {
    CLmkUiUtils* self = new( ELeave ) CLmkUiUtils();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkUiUtils::CLmkUiUtils
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkUiUtils::CLmkUiUtils()
    {
    }

// ----------------------------------------------------------------------------
// CLmkUiUtils::ConstructL
// ----------------------------------------------------------------------------
//
void CLmkUiUtils::ConstructL()
    {
    iNaviPane = static_cast<CAknNavigationControlContainer*>(
            StatusPane()->ControlL( TUid::Uid(EEikStatusPaneUidNavi) ) );

    }

// -----------------------------------------------------------------------------
// CLmkUiUtils::~CLmkUiUtils()
// -----------------------------------------------------------------------------
//
CLmkUiUtils::~CLmkUiUtils()
    {
    CEikonEnv* env = CEikonEnv::Static();
    env->DeleteResourceFile( iResourceOffset );

    if ( iNaviLabel )
        {
        if (iNaviPane)
	        {
	        iNaviPane->Pop( iNaviLabel );
	        }
        delete iNaviLabel;     
        iNaviLabel = NULL;
        } 
    
    delete iOldImage;
    delete iStoredTitlePaneText;

    //If RestoreOldNaviPaneL is not called, it
    //must do atleast now
    if ( iStoredDecorator )
        {
        TRAPD( error, RestoreOldNaviPaneL() );
        if ( error != KErrNone )
            {
            env->HandleError( error );
            }
        }
    delete iStoredDecorator;
    }

// ----------------------------------------------------------------------------
// CLmkUiUtils::StoreTitlePaneL
// ----------------------------------------------------------------------------
//
void CLmkUiUtils::StoreTitlePaneL()
	{
    CEikStatusPane* statusPane = StatusPane();
    if ( statusPane && statusPane->PaneCapabilities(
        TUid::Uid(EEikStatusPaneUidTitle) ).IsPresent() )
        {
        iTitlePane = static_cast<CAknTitlePane*>( statusPane->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

        if ( iTitlePane->Text() )
            {
            iStoredTitlePaneText = iTitlePane->Text()->AllocL();
            }
        }
	}

// ----------------------------------------------------------------------------
// CLmkUiUtils::ChangeTitlePaneL
// ----------------------------------------------------------------------------
//
EXPORT_C void CLmkUiUtils::ChangeTitlePaneL( HBufC* aTitle )
	{
    CEikStatusPane* statusPane = StatusPane();
    if ( statusPane && statusPane->PaneCapabilities(
        TUid::Uid(EEikStatusPaneUidTitle) ).IsPresent() )
        {
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>( statusPane->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        titlePane->SetText( aTitle ); // Takes ownership of aTitle
        }
	}

// ----------------------------------------------------------------------------
// CLmkUiUtils::ChangeTitlePaneL
// ----------------------------------------------------------------------------
//
EXPORT_C void CLmkUiUtils::ChangeTitlePaneL( TInt aResourceText )
    {
    if ( aResourceText != KZeroResourceId )
        {
        HBufC* buf = StringLoader::LoadLC( aResourceText );
        ChangeTitlePaneL( buf );
        CleanupStack::Pop(); // buf, ownership transferred
        }
    }

// ----------------------------------------------------------------------------
// CLmkUiUtils::GetFileWithCorrectDriveL
// ----------------------------------------------------------------------------
//
EXPORT_C void CLmkUiUtils::GetFileWithCorrectDriveL( TFileName& aFileName )
    {
    TFileName* dllDrive = new( ELeave ) TFileName;
    CleanupStack::PushL( dllDrive );
    TParse* parse = new( ELeave ) TParse;
    CleanupStack::PushL( parse );

    Dll::FileName( *dllDrive );
	User::LeaveIfError( parse->Set( *dllDrive, NULL, NULL ) );
    *dllDrive = parse->Drive(); // contains drive, e.g. "c:"
    User::LeaveIfError( parse->Set( *dllDrive, &aFileName, NULL ) );
	aFileName = parse->FullName();

    CleanupStack::PopAndDestroy( 2 ); // parse, dlldrive
    }

// ----------------------------------------------------
// CLmkUiUtils::AvkonIconFileL
// ----------------------------------------------------
//
TFileName* CLmkUiUtils::AvkonIconFileLC()
    {
    TFileName* iconFile = new(ELeave) TFileName(AknIconUtils::AvkonIconFileName());
    CleanupStack::PushL(iconFile);
    //return AknIconUtils::AvkonIconFileName(); //fix
    return iconFile;
    }

// ----------------------------------------------------
// CLmkUiUtils::LmkUiIconFileL
// ----------------------------------------------------
//
TFileName* CLmkUiUtils::LmkUiIconFileLC()
    {
    TFileName* file = new(ELeave) TFileName( KLmkSystemMbmPath );
    CleanupStack::PushL(file);
    file->Append( KLmkUiIconFileName ); //always safe
    GetFileWithCorrectDriveL( *file );
    return file;
    }

// ----------------------------------------------------------------------------
// CLmkUiUtils::RestoreOldTitlePane
// ----------------------------------------------------------------------------
//
void CLmkUiUtils::RestoreOldTitlePane()
    {
    if ( iTitlePane && iStoredTitlePaneText )
        {
        // iTitlePane takes ownership of iStoredTitlePaneText
        iTitlePane->SetText( iStoredTitlePaneText );
        iStoredTitlePaneText = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CLmkUiUtils::ReadLmkUiResourceFileL
// ----------------------------------------------------------------------------
//
void CLmkUiUtils::ReadLmkUiResourceFileL()
	{
	CEikonEnv* env = CEikonEnv::Static();
    TFileName* file = new( ELeave ) TFileName;
    CleanupStack::PushL( file );

    file->Append( KLmkTmp_RESOURCE_FILES_DIR ); // always safe
    file->Append( KLmkUiRscFileName ); // always safe
    GetFileWithCorrectDriveL( *file ) ;
    BaflUtils::NearestLanguageFile( env->FsSession(), *file );
    iResourceOffset = env->AddResourceFileL( *file );

    CleanupStack::PopAndDestroy( file );
	}

// ----------------------------------------------------
// CLmkUiUtils::CreateDefaultNaviPaneL
// ----------------------------------------------------
//
void CLmkUiUtils::CreateDefaultNaviPaneL()
    {
    iNaviPane->PushDefaultL();
    }

// ----------------------------------------------------
// CLmkUiUtils::CreateNaviLabelL
// ----------------------------------------------------
//
void CLmkUiUtils::CreateNaviLabelL( const TDesC& aLabel )
    {
    __ASSERT_ALWAYS( iNaviPane, Panic( KLmkPanicNullPointer ) );
    if ( iNaviLabel )
        {
        iNaviPane->Pop( iNaviLabel );
        delete iNaviLabel;
        iNaviLabel = NULL;
        }

    iNaviLabel = iNaviPane->CreateNavigationLabelL( aLabel );
    iNaviPane->PushL( *iNaviLabel ); // activate navi label in navi pane
    }

// ----------------------------------------------------
// CLmkUiUtils::StoreNaviPaneL
// ----------------------------------------------------
//
void CLmkUiUtils::StoreNaviPaneL()
    {
    iStoredDecorator = iNaviPane->Top();
    }

// ----------------------------------------------------
// CLmkUiUtils::RestoreOldNaviPaneL
// ----------------------------------------------------
//
void CLmkUiUtils::RestoreOldNaviPaneL()
    {
    if ( iStoredDecorator && iNaviPane )
        {
        iNaviPane->PushL( *iStoredDecorator );
        iStoredDecorator = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CLmkUiUtils::SwapNewContextIcon
// -----------------------------------------------------------------------------
//
void CLmkUiUtils::SwapNewContextIconL( CEikImage* aNewImage )
    {
    SetContextPaneL();
    if ( iContextPane )
        {
        iOldImage = iContextPane->SwapPicture( aNewImage );
        }
    }

// -----------------------------------------------------------------------------
// CLmkUiUtils::SetOldContextIcon
// -----------------------------------------------------------------------------
//
void CLmkUiUtils::SetOldContextIcon()
    {
    if ( iContextPane && iOldImage && iOldImage->Bitmap())
        {
        iContextPane->SetPicture( iOldImage );
        iOldImage = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CLmkUiUtils::UpdateContextIconL
// -----------------------------------------------------------------------------
//
void CLmkUiUtils::UpdateContextIconL( CEikImage* aNewImage )
    {
    SetContextPaneL();
    if ( iContextPane )
        {
        iContextPane->SetPicture( aNewImage );
        }
    }

// ----------------------------------------------------------------------------
// CLmkUiUtils::StatusPane
// ----------------------------------------------------------------------------
//
CEikStatusPane* CLmkUiUtils::StatusPane()
    {
    return CEikonEnv::Static()->AppUiFactory()->StatusPane();
    }

// -----------------------------------------------------------------------------
// CLmkUiUtils::SetContextPane
// -----------------------------------------------------------------------------
//
void CLmkUiUtils::SetContextPaneL()
    {
    iContextPane = static_cast<CAknContextPane*>(
                StatusPane()->ControlL( TUid::Uid(
                    EEikStatusPaneUidContext ) ) );
    }


// ----------------------------------------------------------------------------
// CLmkUiUtils::ChangeLmNameLabelIconL()
// ----------------------------------------------------------------------------
//
EXPORT_C void CLmkUiUtils::ChangeLmNameLabelIconL (
              MLmkEditorUiBuilder& aUiBuilder,
              MLmkFieldData& nameField)
	{
	if ( nameField.FieldType() == EName )
		{
		CEikImage* image = new(ELeave) CEikImage();
		CleanupStack::PushL( image );
		//TFileName* defaultIconFile =  CLmkUiUtils::LmkUiIconFileLC();
		// No need to add iconFile to cleanupstack since it is a member variable from CLmkFieldData class
		HBufC* iconFile = nameField.IconPath();
		TInt err = KErrNotFound;
		CFbsBitmap* bitmap = NULL;
		CFbsBitmap* mask = NULL;

		if (iconFile) // landmark has a defined icon
			{
			if ( nameField.IconId() != KErrNotFound )
				{
				TRAP(err,AknIconUtils::CreateIconL( bitmap, mask,
									   *iconFile, nameField.IconId(),
									   nameField.IconId()+1 ););
				}

			image->SetPictureOwnedExternally(EFalse);
			image->SetPicture(bitmap, mask);
			image->CopyControlContextFrom(aUiBuilder.LineControl( nameField.UniqueFieldIdentity() ));
			image->SetContainerWindowL(*aUiBuilder.LineControl( nameField.UniqueFieldIdentity() ));
			image->SetNonFocusing();
			image->SetBrushStyle(CGraphicsContext::ENullBrush);
			delete aUiBuilder.LineControl(nameField.UniqueFieldIdentity())->iBitmap;
			aUiBuilder.LineControl(nameField.UniqueFieldIdentity())->iBitmap = image;
			aUiBuilder.LineControl(nameField.UniqueFieldIdentity())->iBitmap = image;

			TRect rect = aUiBuilder.LineControl(nameField.UniqueFieldIdentity())->Rect();
			rect.Resize(-1, -1);
			aUiBuilder.LineControl(nameField.UniqueFieldIdentity())->SetRect(rect);
			rect.Resize(1, 1);
			aUiBuilder.LineControl(nameField.UniqueFieldIdentity())->SetRect(rect);
			}
		CleanupStack::Pop(image);
		}
	}

// ----------------------------------------------------------------------------
// CLmkUiUtils::FindIfWebAddrFieldIsEmpty()
// ----------------------------------------------------------------------------
//
TBool CLmkUiUtils::FindIfWebAddrFieldIsEmpty (const TDes& aUrl)
	{
	TBool result = EFalse;
	TInt totalLength = aUrl.Length();
	TInt forSlashPos = aUrl.Find(KProtocol);
	if (((forSlashPos + KPosUrlColon) > 0 &&
		(forSlashPos + KPosUrlColon) >= totalLength ))
		{
		result = ETrue;
		}
    return result;
	}
//  End of File
