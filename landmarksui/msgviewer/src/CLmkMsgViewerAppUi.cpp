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
* Description:    LandmarksUi Content File -    Provides LM UI class methods.
*
*/







// INCLUDE FILES
#include "CLmkMsgViewerAppUi.h"
#include "CLmkMsgViewerAppView.h"
#include "landmarks.hrh"
#include "CLmkMsgViewerDocument.h"
#include "CLmkParser.h"
#include "CLmkSender.h"
#include "CLmkPackageEditorImpl.h"
#include "CLmkDlgPackageSelectorImpl.h"
#include "LmkConsts.h"
#include "CLmkEditorDlg.h"
#include <AknsUtils.h>
#include <AknWaitDialog.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <LmkMsgViewer.rsg>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include "LmkNotes.h"
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkMsgViewerAppUi");

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif

// Constant for periodic timer
const TInt KOneMicroSec = 1;

// ================= LOCAL FUNCTIONS =======================

//-----------------------------------------------------------------------
// ErrorNoteL
// ----------------------------------------------------------------------------
//
void ErrorNoteL(const TInt aResourceId)
    {
    HBufC* noteText = CEikonEnv::Static()->AllocReadResourceLC(aResourceId);

    CAknErrorNote* dialog =
        new( ELeave ) CAknErrorNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
    }

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CLmkMsgViewerAppUi::ConstructL
// ----------------------------------------------------------
//
void CLmkMsgViewerAppUi::ConstructL()
    {

    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible);

    iAppView = CLmkMsgViewerAppView::NewL( ClientRect() );

    // Start an idle timer to start the dialog synchronously.
    iIdleTimer = CPeriodic::NewL(CActive::EPriorityIdle);
    iIdleTimer->Start(KOneMicroSec, KOneMicroSec,
        TCallBack(CallBackFuncL,this));
    }

// ----------------------------------------------------
// CLmkMsgViewerAppUi::~CLmkMsgViewerAppUi
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CLmkMsgViewerAppUi::~CLmkMsgViewerAppUi()
    {
    if ( iIdleTimer )
        {
        delete iIdleTimer;
        iIdleTimer = NULL;
        }

    TInt err = KErrNone;
    TRAP(err, CloseWaitNoteL());

    delete iAppView;
    delete iLmkSender;
    delete iParser;
    delete iDb;

    if( iDoorObserver )
        {
        iDoorObserver->NotifyExit( MApaEmbeddedDocObserver::EEmpty );
        }
    }

// ----------------------------------------------------
// CLmkMsgViewerAppUi::HandleCommandL
// ----------------------------------------------------
//
void CLmkMsgViewerAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        // these all are same: save & exit
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EAknCmdExit:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ----------------------------------------------------
// CLmkMsgViewerAppUi::LmkSender
// ----------------------------------------------------
//
CLmkSender& CLmkMsgViewerAppUi::LmkSender() const
    {
    __ASSERT_DEBUG( iLmkSender, Panic( KLmkPanicNullMember ) );
    return *iLmkSender;
    }


// ----------------------------------------------------------------------------
// CLmkMsgViewerAppUi::OpenFileL
// This is called by framework when application is already open in background
// and user opens other file
// New file to been shown is passed via aFileName.
// ----------------------------------------------------------------------------
//
void CLmkMsgViewerAppUi::OpenFileL( const TDesC& aFileName )
	{
	// File changed.

    // TODO: What to do with the possibly existing package?
    // Should we just leave or do something else?
    if ( iParser )
        {
        User::Leave( KErrAlreadyExists );
        }
    else
        {
        Document()->OpenFileL( EFalse, aFileName, iEikonEnv->FsSession() );
        }
	}


// ----------------------------------------------------------------------------
// CLmkMsgViewerAppUi::OpenFileL
// This is called by framework when application is already open in background
// and user opens other file
// New file to been shown is passed via aFileName.
// ----------------------------------------------------------------------------
//
void CLmkMsgViewerAppUi::OpenFileL(RFile& aFileHandle )
	{
	// File changed.
    if ( iParser )
        {
        User::Leave( KErrAlreadyExists );
        }
    else
        {
        CFileStore* aFileStore = NULL;
        static_cast<CLmkMsgViewerDocument*>(Document())->OpenFileL(aFileStore, aFileHandle);
        }
	}

// ----------------------------------------------------------------------------
// CLmkMsgViewerAppUi::ProcessCommandParametersL
// This method must return true or documents OpenFileL() get wrong file name.
// ----------------------------------------------------------------------------
//
TBool CLmkMsgViewerAppUi::ProcessCommandParametersL(
    TApaCommand /*aCommand*/,
	TFileName& /*aDocumentName*/,
	const TDesC8& /*aTail*/ )
    {
    // Never call CEikAppUi::ProcessCommandParametersL(..) because
    // it is so harmful to overwrite and destroy aDocumentName.

	// Return ETrue so that CLmkMsgViewerAppUi::OpenFileL() gets
    // the right document name.
    return ETrue;
    }


// ----------------------------------------------------
// CLmkMsgViewerAppUi::HandleOperationL
// ----------------------------------------------------
//
void CLmkMsgViewerAppUi::HandleOperationL(
    TOperationTypes aType,
    TReal32 /*aProgress*/,
    TInt aStatus )
    {
    __ASSERT_DEBUG( ( aType == EInitialize ||
                      aType == EParse ) &&
                    aStatus != KPosLmOperationNotComplete,
                    Panic( KLmkPanicAlreadyActive ) );

    TInt err = KErrNone;
    if (aType == EInitialize)
        {
        TRAP(err, HandleDbInitFinishedL(aStatus));
        }
    else // (aType == EParse)
        {
        TRAP(err, HandleParseFinishedL(aStatus));
        }

    if (err)
        {
        CloseWaitNoteL(); // when wait note closes, application exits
        }
    }

// ----------------------------------------------------
// CLmkMsgViewerAppUi::HandleDbInitFinishedL
// ----------------------------------------------------
//
void CLmkMsgViewerAppUi::HandleDbInitFinishedL( TInt aStatus )
    {
    if (aStatus != KErrNone)
        {
        CCoeEnv::Static()->HandleError(aStatus);
        User::Leave(aStatus);
        }
    iParser->StartParsingL();
    }

// ----------------------------------------------------
// CLmkMsgViewerAppUi::HandleParseFinishedL
// ----------------------------------------------------
//
void CLmkMsgViewerAppUi::HandleParseFinishedL( TInt aStatus )
    {
    if (aStatus != KErrNone)
        {
        ErrorNoteL(R_LMK_BADPACKAGE_ERROR);
        User::Leave(aStatus);
        }
    // now everything's ready for viewing
    iDataReadyForView = ETrue;
    CloseWaitNoteL(); // when wait note closes, view dialog opens
    }

void CLmkMsgViewerAppUi::CloseWaitNoteL()
    {
    if (iWaitDialog)
        {
        iWaitDialog->ProcessFinishedL();
        iWaitDialog = NULL;
        }
    }

// ----------------------------------------------------
// CLmkMsgViewerAppUi::ExecuteLmViewerL
// ----------------------------------------------------
//
void CLmkMsgViewerAppUi::ExecuteLmViewerL()
    {
    __ASSERT_DEBUG( iParser->NumOfParsedLandmarks() == 1,
                    Panic( KLmkPanicOutOfRange ) );

    CLmkPackageEditorImpl* editorImpl =
        CLmkPackageEditorImpl::NewL( *iDb, *iParser, *iLmkSender, 0 );

    // Parser observing responsibility is given to the lm viewer,
    // application is not interested in it anymore:
    iParser->ChangeObserver( *editorImpl );
    editorImpl->ExecuteLD();
    }

// ----------------------------------------------------
// CLmkMsgViewerAppUi::ExecutePkgViewerL
// ----------------------------------------------------
//
void CLmkMsgViewerAppUi::ExecutePkgViewerL()
    {
    __ASSERT_DEBUG( iParser->NumOfParsedLandmarks() > 1,
                    Panic( KLmkPanicOutOfRange ) );

    CLmkDlgPackageSelectorImpl* pkgViewer =
        CLmkDlgPackageSelectorImpl::NewL( *iDb, *iParser, *iLmkSender );
    CleanupStack::PushL( pkgViewer );

    // Parser observing responsibility is given to the package viewer,
    // application is not interested in it anymore:
    iParser->ChangeObserver( *pkgViewer );

    CArrayFix<TInt>* selectedItems =
        new( ELeave ) CArrayFixFlat<TInt>( KLmkMultiSelectionGranularity );
    CleanupStack::PushL( selectedItems );

    pkgViewer->ExecuteL( *selectedItems,  iParser->PackageName());
    CleanupStack::PopAndDestroy( selectedItems );
    CleanupStack::PopAndDestroy( pkgViewer );
    }

// ----------------------------------------------------
// CLmkMsgViewerAppUi::OpenL
// ----------------------------------------------------
//
void CLmkMsgViewerAppUi::OpenL( const TDesC& aFilename )
    {
    iParser = CLmkParser::NewL( aFilename, *iDb, *this, EFalse );
    DoOpenL();
    Exit();
    }

// ----------------------------------------------------
// CLmkMsgViewerAppUi::OpenL
// ----------------------------------------------------
//
void CLmkMsgViewerAppUi::OpenL(RFile& aFile)
    {
    iParser = CLmkParser::NewL( aFile, *iDb, *this, EFalse );
    DoOpenL();
    Exit();
    }

void CLmkMsgViewerAppUi::DoOpenL()
    {
    // We need to initialize db before we start parsing:
    iParser->InitializeDbL();

    iWaitDialog = new( ELeave ) CAknWaitDialog( NULL, ETrue );
    if (!iWaitDialog->ExecuteLD( R_LMK_MSGVIEWER_WAIT ))
        {
        //making iWaitDialog null, since framework destroys waitnote
        //dialog on pressing cancel.

        iWaitDialog = NULL;
        }

    if( iDataReadyForView )
        {
        if ( iParser->NumOfParsedLandmarks() == 1 )
            {
            ExecuteLmViewerL();
            }
        else if ( iParser->NumOfParsedLandmarks() > 1 )
            {
            ExecutePkgViewerL();
            }
        else
            {
            // exit
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkMsgViewerAppUi::CallBackFuncL()
// Used to callback by the idle timer in order to launch
// the dialog
// -----------------------------------------------------------------------------
//
TInt CLmkMsgViewerAppUi::CallBackFuncL(TAny *aPtr)
    {
    CLmkMsgViewerAppUi* appUi =
        static_cast< CLmkMsgViewerAppUi* >( aPtr );

    if ( appUi )
        {
        return ( appUi->LaunchDialogL() );
        }
    else
        {
        // To indicate that the timer is not
        // called again, return false to the timer
        // RunL
        return ( EFalse );
        }
    }


// -----------------------------------------------------------------------------
// CLmkMsgViewerAppUi::LaunchDialogL()
// This function accesses the document and launches the
// content.
// -----------------------------------------------------------------------------
//
TBool CLmkMsgViewerAppUi::LaunchDialogL()
	{
	if(!iIsDbOpen)
		{
		// Try to open DB
		TRAPD( err, iDb = CPosLandmarkDatabase::OpenL() );

    	if( KErrNone != err )
    		{
    		// Cancel the periodic timer
        	if ( iIdleTimer->IsActive() )
            	{
            	iIdleTimer->Cancel();
            	}
            // Display general system error note
		    HBufC* noteText = StringLoader::LoadL( R_LMK_ERR_EIKON_GENERAL, iEikonEnv );
		    CleanupStack::PushL( noteText );
	    	CAknErrorNote* dialog = new(ELeave)CAknErrorNote( ETrue );
		    dialog->SetTone( CAknNoteDialog::EErrorTone );
		    dialog->ExecuteLD( *noteText );
	   		CleanupStack::PopAndDestroy( noteText ); //noteText
	   		// exit LmkMsgViewer app
		    Exit();
		    }
    	    // DB was opened successfully
    	    iIsDbOpen = ETrue;
    		iLmkSender = CLmkSender::NewL( *iDb );
    	}
	CLmkMsgViewerDocument* document =
	    static_cast<CLmkMsgViewerDocument*>(iDocument);

	// Check if file-handle in document is valid
	if (document->IsFileHandleValid())
		{
		// Obtain the file-handle
		RFile& fileHandle = document->GetLandmarkContentFileHandle();

        // Cancel the periodic timer
        if ( iIdleTimer->IsActive() )
            {
            iIdleTimer->Cancel();
            }
		// Launch the content
		TRAPD(err,OpenL(fileHandle));
		if(err != KErrNone)
			{
			if( err == KErrNotSupported)
				{
				LmkNotes::InformationNoteL( iEikonEnv, R_LMK_BADPACKAGE_ERROR );
				}
			Exit();
			}
		// Delete and reset the idle-timer
        delete iIdleTimer;
        iIdleTimer = NULL;

		// Return False to indicate that
		// timer need not callback further.
		return EFalse;
		}
	else
		{
		// Return True to indicate that the
		// timer needs to callback this function
		// again, as the file-handle is not valid
		// yet.
		return ETrue;
		}
	}

// -----------------------------------------------------------------------------
// CLmkMsgViewerAppUi::HandleResourceChangeL()
// -----------------------------------------------------------------------------
//
void CLmkMsgViewerAppUi::HandleResourceChangeL(TInt aType)
    {
     CAknAppUi::HandleResourceChangeL( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iAppView->SetRect(ClientRect());
        }
    }
// End of File
