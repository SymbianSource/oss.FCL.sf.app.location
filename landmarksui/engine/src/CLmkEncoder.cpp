/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file implements the methods which wrap functionality related
*                to landmark encoding into a simpler interface.
*
*/







// INCLUDE FILES

#include <e32base.h>
#include <e32std.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <lmkui.rsg>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLandmarkEncoder.h>
#include "LmkConsts.h"
#include "Debug.h"
#include "LmkFileUtils.h"
#include "CLmkAOOperation.h"
#include "CLmkEncoder.h"
#include "CLmkEncodeUnsavedLandmarksOp.h"
#include <lmkerrors.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT8( KLandmarkMimeType, "application/vnd.nokia.landmarkcollection+xml" );
// We wrap two async. tasks (export and write to file) into one
// using estimated 0.8/0.2 progress distribution:
const TReal32 KLmkExportMaxProgress( TReal32( 0.8 ) );

#if defined(_DEBUG)
_LIT( KPanicMsg, "CLmkEncoder" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkEncoder::CLmkEncoder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkEncoder::CLmkEncoder(
	CPosLandmarkDatabase& aDb,
	MLmkAOOperationObserver& aObserver,
	TBool aProgressNote )
	: iDb( aDb ),
	iObserver( aObserver ),
	iProgressNote( aProgressNote )
	{
	}

// -----------------------------------------------------------------------------
// CLmkEncoder::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkEncoder* CLmkEncoder::NewL(
	CPosLandmarkDatabase& aDb,
	MLmkAOOperationObserver& aObserver,
	TBool aProgressNote )
	{
	CLmkEncoder* self =
	new ( ELeave ) CLmkEncoder( aDb, aObserver, aProgressNote );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); //self
	return self;
	}

// -----------------------------------------------------------------------------
// CLmkEncoder::ConstructL()
// -----------------------------------------------------------------------------
//
void CLmkEncoder::ConstructL()
	{
	}

// -----------------------------------------------------------------------------
// CLmkEncoder::~CLmkEncoder()
// -----------------------------------------------------------------------------
//
CLmkEncoder::~CLmkEncoder()
	{
	delete iAOOperation;
	delete iEncoder;
	}

// -----------------------------------------------------------------------------
// CLmkEncoder::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkEncoder::HandleOperationL(
	TOperationTypes aType,
	TReal32 aProgress,
	TInt aStatus )
	{
	DEBUG2( CLmkEncoder::HandleOperationL entered: progress=%d status=%d,
	aProgress, aStatus );

	__ASSERT_DEBUG( aType == EEncode, Panic( KLmkPanicUnknownOperation ) );

	if ( aStatus != KPosLmOperationNotComplete )
		{
		__ASSERT_DEBUG( iAOOperation, Panic( KLmkPanicNullMember ) );

		delete iAOOperation;
		iAOOperation = NULL;

		if ( iExporting && aStatus == KErrNone )
			{ // start second part of asynchronous task
			FinalizeEncodingL();
			aStatus = KPosLmOperationNotComplete;
			}
		}

	// Modify progress information because we have wrapped two tasks into one:
	if ( iExporting && aStatus == KPosLmOperationNotComplete )
		{
		aProgress = aProgress * KLmkExportMaxProgress;
		}
	else if ( !iExporting && aStatus == KPosLmOperationNotComplete )
		{ // Progress of writing to file is between KLmkExportMaxProgress...1
		aProgress = KLmkExportMaxProgress +
		aProgress * ( 1 - KLmkExportMaxProgress );
		}

	if( iProgressNote || aStatus != KPosLmOperationNotComplete )
		{ // Caller either requested to receive all events, or we
		// have completed entirely and inform that to the caller.
		iObserver.HandleOperationL( aType, aProgress, aStatus );
		}
}

// -----------------------------------------------------------------------------
// CLmkEncoder::SetPackageNameL
// -----------------------------------------------------------------------------
//
//EXPORT_C void CLmkEncoder::SetPackageNameL( const TDesC& aName )
void CLmkEncoder::SetPackageNameL( const TDesC& aName )
	{
	iEncoder->AddCollectionDataL( EPosLmCollDataCollectionName, aName );
	}

// -----------------------------------------------------------------------------
// CLmkEncoder::StartEncodingL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEncoder::StartEncodingL(
	const TDesC& aFileName,
	const RArray<TPosLmItemId>& aLandmarkIdArray )
	{
	DEBUG( CLmkEncoder::StartEncodingL entered );
	LeaveIfInUseL();
    PrepareEncoderL(aFileName);
	if (aLandmarkIdArray.Count() > 1) // Only if multiple Landmarks Are selected
		{
	    // Set the package name here
		SetPackageNameL(aFileName);
		}

	CPosLmOperation* operation =
	iDb.ExportLandmarksL( *iEncoder,
	                      aLandmarkIdArray,
	                      CPosLandmarkDatabase::EIncludeCategories );
	CleanupStack::PushL( operation );
	iAOOperation = CLmkAOOperation::NewL( operation,
	                                  *this,
	                                  EEncode,
	                                  iProgressNote );
	CleanupStack::Pop( operation ); // ownership transferred

	iExporting = ETrue; // exporting part of encoding is being done

	DEBUG( CLmkEncoder::StartEncodingL: starting operation );
	iAOOperation->StartOperation();
	}

// -----------------------------------------------------------------------------
// CLmkEncoder::CancelEncodeOperationL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEncoder::CancelEncodeOperationL()
	{
	if ( iAOOperation )
		{
		iAOOperation->Cancel();
		delete iAOOperation;
		iAOOperation = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CLmkEncoder::FinalizeEncodingL
// -----------------------------------------------------------------------------
//
void CLmkEncoder::FinalizeEncodingL()
    {
    DEBUG( CLmkEncoder::FinalizeEncodingL entered );

    LeaveIfInUseL();

    CPosLmOperation* operation = iEncoder->FinalizeEncodingL();
    CleanupStack::PushL( operation );
    iAOOperation = CLmkAOOperation::NewL( operation,
                                          *this,
                                          EEncode,
                                          iProgressNote );
    CleanupStack::Pop( operation ); // ownership transferred

    iExporting = EFalse; // write to file -part of encoding is being done

    DEBUG( CLmkEncoder::FinalizeEncodingL: starting operation );
    iAOOperation->StartOperation();
    }

// -----------------------------------------------------------------------------
// CLmkEncoder::LeaveIfInUseL
// -----------------------------------------------------------------------------
//
void CLmkEncoder::LeaveIfInUseL()
    {
    if ( iAOOperation )
        {
        User::Leave( KErrInUse );
        }
    }

// -----------------------------------------------------------------------------
// CLmkEncoder::PrepareEncoderL
// -----------------------------------------------------------------------------
//
void CLmkEncoder::PrepareEncoderL(const TDesC& aFileName)
    {
    // Full path name (Private path c:\\Provate\\UID3 + pacakge File name)
    HBufC* nameBuf = HBufC::NewLC( 2 * KLmkPackageNameMaxLen +  KLmkEncodedDataFileExtension().Length() );

	// For getting private path
	TPtr ptr = 	nameBuf->Des();
	RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    fs.SessionPath(ptr);
    fs.MkDirAll(ptr);
    CleanupStack::PopAndDestroy(); // fs

	ptr.Append(aFileName);
	ptr.Append(KLmkEncodedDataFileExtension);
	TInt err = KErrNone;

	if ( !iEncoder )
		{
		DEBUG( CLmkEncoder::StartEncodingL: creating encoder );
		TRAP(err, iEncoder = CPosLandmarkEncoder::NewL( KLandmarkMimeType ));
		DEBUG( CLmkEncoder::StartEncodingL: encoder created);
		}
    if(err != KErrNone)
		{
		LmkFileUtils::DeleteFileL( ptr ); // Delete old file if it exists
	    CleanupStack::PopAndDestroy(nameBuf); //nameBuf
	    User::LeaveIfError( err );
		}
    LmkFileUtils::DeleteFileL( ptr ); // Delete old file if it exists
	iEncoder->SetOutputFileL( ptr );
    CleanupStack::PopAndDestroy(nameBuf); //nameBuf
    }

// -----------------------------------------------------------------------------
// CLmkEncoder::StartEncodingL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEncoder::StartEncodingL(
	const TDesC& aFileName,
	const RPointerArray<CLmkLandmark>& aLandmarks )
	{
	DEBUG( CLmkEncoder::StartEncodingL entered );
	LeaveIfInUseL();
    PrepareEncoderL(aFileName);
	if (aLandmarks.Count() > 1) // Only if multiple Landmarks Are selected
		{
	    // Set the package name here
		SetPackageNameL(aFileName);
		}

    CPosLmOperation* operation =
            CLmkEncodeUnsavedLandmarksOp::NewL( *iEncoder,
                                                aLandmarks,
                                                    CPosLandmarkDatabase::EIncludeCategories);
	CleanupStack::PushL( operation );
	iAOOperation = CLmkAOOperation::NewL( operation,
	                                  *this,
	                                  EEncode,
	                                  iProgressNote );
	CleanupStack::Pop( operation ); // ownership transferred

	iExporting = ETrue; // exporting part of encoding is being done

	DEBUG( CLmkEncoder::StartEncodingL: starting operation );
	iAOOperation->StartOperation();
	}
//  End of File
