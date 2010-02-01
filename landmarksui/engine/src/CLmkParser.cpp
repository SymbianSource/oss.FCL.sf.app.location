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
* Description:   This file contains methods which interact with Landmarks Framework
*                parser which does the job of parsing landmarks in the received
*                landmark package
*
*/







// INCLUDE FILES

#include "EPos_CPosLandmarkParser.h"
#include "EPos_CPosLandmarkDatabase.h"
#include "EPos_CPosLandmark.h"
#include "landmarks.hrh"
#include "LmkFileUtils.h"
#include "CLmkAOOperation.h"
#include "CLmkDbUtils.h"
#include "CLmkParser.h"
#include "CLmkParseAllWrapper.h"
#include <apgcli.h>
#include <lmkerrors.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

#if defined(_DEBUG)
_LIT( KPanicMsg, "CLmkParser" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif

_LIT( KLmkTempDataFile, "c:\\system\\Temp\\LmkTemp.lmx" );

}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkParser::CLmkParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkParser::CLmkParser(
	CPosLandmarkDatabase& aDb,
	MLmkAOOperationObserver& aObserver,
	TBool aProgressNote )
	: iDb( aDb ),
	  iObserver( &aObserver ),
	  iProgressNote( aProgressNote )
	{
	}

// ---------------------------------------------------------
// CLmkParser::NewL
// ---------------------------------------------------------
//
EXPORT_C CLmkParser* CLmkParser::NewL(
	const TDesC& aFile,
	CPosLandmarkDatabase& aDb,
	MLmkAOOperationObserver& aObserver,
	TBool aProgressNote )
	{
	CLmkParser* self =
        new ( ELeave ) CLmkParser( aDb, aObserver, aProgressNote );
	CleanupStack::PushL( self );
	self->ConstructL( aFile );
	CleanupStack::Pop(); //self
    return self;
	}

// ---------------------------------------------------------
// CLmkParser::NewL
// ---------------------------------------------------------
//
EXPORT_C CLmkParser* CLmkParser::NewL(
	RFile& aFile,
	CPosLandmarkDatabase& aDb,
	MLmkAOOperationObserver& aObserver,
	TBool aProgressNote )
	{
	CLmkParser* self =
        new ( ELeave ) CLmkParser( aDb, aObserver, aProgressNote );
	CleanupStack::PushL( self );
	self->ConstructL( aFile );
	CleanupStack::Pop(); //self
	return self;
	}


// -----------------------------------------------------------------------------
// CLmkParser::ConstructL()
// -----------------------------------------------------------------------------
//
void CLmkParser::ConstructL( const TDesC& aFile )
	{

	iDbUtils = CLmkDbUtils::NewL( iDb, *this, iProgressNote );
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile file;
	LmkFileUtils::DeleteFileL(KLmkTempDataFile);
	User::LeaveIfError(file.Create(fs, KLmkTempDataFile, EFileShareAny));
	CleanupClosePushL(file);

	RFile fileHandle;
	User::LeaveIfError(fileHandle.Open(fs, aFile, EFileShareAny));
	CleanupClosePushL(fileHandle);

	TDataType mimeType;
	User::LeaveIfError(ResolveMimeTypeL(fileHandle,mimeType));
	iParser = CPosLandmarkParser::NewL(  mimeType.Des8());

	HBufC8* tempBuf = HBufC8::NewLC(5000);  // to manage for 5 Landmarks at the most
	TPtr8 ptr = tempBuf->Des();
	User::LeaveIfError(fileHandle.Read( ptr ));
	User::LeaveIfError(file.Write( ptr ));

	CleanupStack::PopAndDestroy(); // File
	CleanupStack::PopAndDestroy(1); //tempBuf

	iParser->SetInputFileL( KLmkTempDataFile);
	iParseWrapper = CLmkParseAllWrapper::NewL( *iParser, *this, iDb );
	iFileName = HBufC::NewL(256);
	*iFileName = KLmkTempDataFile;
	iFileHandleFlag = EFalse;

    CleanupStack::PopAndDestroy(); // File Handle
	CleanupStack::PopAndDestroy(); // fs
	}

// -----------------------------------------------------------------------------
// CLmkParser::ConstructL()
// -----------------------------------------------------------------------------
//
void CLmkParser::ConstructL(RFile& aFile )
	{
	iDbUtils = CLmkDbUtils::NewL( iDb, *this, iProgressNote );

	TDataType mimeType;
	User::LeaveIfError(ResolveMimeTypeL(aFile,mimeType));
	iParser = CPosLandmarkParser::NewL(  mimeType.Des8());
	iParser->SetInputFileHandleL(aFile);
	iParseWrapper = CLmkParseAllWrapper::NewL( *iParser, *this, iDb );

	iFileHandle = &aFile;
	iFileHandleFlag = ETrue;
	}

// -----------------------------------------------------------------------------
// CLmkParser::~CLmkParser()
// -----------------------------------------------------------------------------
//
CLmkParser::~CLmkParser()
	{
	delete iAOOperation;
	delete iParser;
	delete iParseWrapper;
	delete iDbUtils;
	delete iFileName;
	iFileHandle = NULL;
	}

// -----------------------------------------------------------------------------
// CLmkParser::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkParser::HandleOperationL(
TOperationTypes aType,
TReal32 aProgress,
TInt aStatus )
	{
	// Initialize operation is not done with iAOOperation so in that case
	// we don't delete iAOOperation.
	if ( aType != EInitialize && aStatus != KPosLmOperationNotComplete )
		{
		/* Parsing is done with iAOOperation when wrapper isn't used anymore.
		 * Remove the following if-statement when removing wrapper
		 */
		if( aType != EParse )
			{
			__ASSERT_DEBUG( iAOOperation, Panic( KLmkPanicNullMember ) );

			delete iAOOperation;
			iAOOperation = NULL;
			}
		}
	__ASSERT_DEBUG( iObserver, Panic( KLmkPanicNullMember ) );
	iObserver->HandleOperationL( aType, aProgress, aStatus );
	}

// -----------------------------------------------------------------------------
// CLmkParser::FileName
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CLmkParser::FileName()
	{
	TInt error = KErrNone;
	if( !iFileName )
		{
		TRAP( error,iFileName = HBufC::NewL(256));
		}
	if( !error )
		{
		TPtr ptr = iFileName->Des();
		iFileHandle->Name( ptr );
		return *iFileName;
		}
	return TPtrC();
	}

// -----------------------------------------------------------------------------
// CLmkParser::FileHandle
// -----------------------------------------------------------------------------
//
EXPORT_C RFile& CLmkParser::FileHandle()
	{
	return *iFileHandle;
	}

// -----------------------------------------------------------------------------
// CLmkParser::IsFileHandle
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CLmkParser::IsFileHandle()
    {
    return iFileHandleFlag;
    }

// -----------------------------------------------------------------------------
// CLmkParser::InitializeDbL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkParser::InitializeDbL()
	{
	iDbUtils->InitializeDbL();
	}

// -----------------------------------------------------------------------------
// CLmkParser::StartParsingL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkParser::StartParsingL()
	{
	LeaveIfInUseL();
	iParseWrapper->ParseAllL();
	}

// -----------------------------------------------------------------------------
// CLmkParser::NumOfParsedLandmarks
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkParser::NumOfParsedLandmarks()
	{
	return iParseWrapper->NumOfLandmarks();
	}

// -----------------------------------------------------------------------------
// CLmkParser::PackageName
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CLmkParser::PackageName()
	{
	_LIT( KDot, "." );
	TPtrC ptr = iParser->CollectionData( EPosLmCollDataCollectionName );
	if( ptr.Length() == 0)
		{
		//get the position of '.' character
		TInt pos = FileName().Find(KDot);
		if( pos != KErrNotFound)
			{
			//get the left most string starting from '.' char
			return FileName().Left( pos );
			}
		else
			{
			return FileName();
			}
		}
	return ptr;
	}

// -----------------------------------------------------------------------------
// CLmkParser::LandmarkLC
// -----------------------------------------------------------------------------
//
EXPORT_C CPosLandmark* CLmkParser::LandmarkLC( TInt aIndex )
	{
	return iParseWrapper->LandmarkLC( aIndex );
	}

// -----------------------------------------------------------------------------
// CLmkParser::LmkLandmarkLC
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkLandmark* CLmkParser::LmkLandmarkLC( TInt aIndex )
	{
	return iParseWrapper->LmkLandmarkLC(aIndex);
	}
// -----------------------------------------------------------------------------
// CLmkParser::CategoryNameLC
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkLandMarkCategoriesName* CLmkParser::CategoryNameLC( TInt aIndex )
    {
    /* THIS FUNCTIONALITY WILL BE USED WHEN WRAPPER IS REMOVED:
    return iParser->LandmarkLC( aIndex );
    */
    return iParseWrapper->CategoryNameLC( aIndex );
    }

//
// -----------------------------------------------------------------------------
// CLmkParser::ChangeObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkParser::ChangeObserver(
	MLmkAOOperationObserver& aNewObserver )
	{
	iObserver = &aNewObserver;
	}

// -----------------------------------------------------------------------------
// CLmkParser::StartImportingL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkParser::StartImportingL(TBool aIncludeNewCategories)
	{
	LeaveIfInUseL();

	CPosLandmarkDatabase::TTransferOptions options;

    if ( aIncludeNewCategories )
	    {
	    options = CPosLandmarkDatabase::EIncludeCategories;
	    }
	else
		{
		options =  (CPosLandmarkDatabase::EIncludeCategories) |
		           (CPosLandmarkDatabase::ESupressCategoryCreation);
		}

	CPosLmOperation* operation =
	    iDb.ImportLandmarksL( *iParser, options );
	CleanupStack::PushL( operation );
	iAOOperation = CLmkAOOperation::NewL( operation,
	                                      *this,
	                                      EImport,
	                                      iProgressNote );
	CleanupStack::Pop( operation ); // ownership transferred

	iAOOperation->StartOperation();
	}

// -----------------------------------------------------------------------------
// CLmkParser::StartImportingL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkParser::StartImportingL(
	const RArray<TUint>& aSelected, TBool aIncludeNewCategories)
	{
	LeaveIfInUseL();

    CPosLandmarkDatabase::TTransferOptions options;

    if ( aIncludeNewCategories )
	    {
	    options = CPosLandmarkDatabase::EIncludeCategories;
	    }
	else
		{
		options = (CPosLandmarkDatabase::EIncludeCategories) | (CPosLandmarkDatabase::ESupressCategoryCreation);
		}

	CPosLmOperation* operation =
	    iDb.ImportLandmarksL( *iParser, aSelected, options );
	CleanupStack::PushL( operation );
	iAOOperation = CLmkAOOperation::NewL( operation,
	                                      *this,
	                                      EImport,
	                                      iProgressNote );
	CleanupStack::Pop( operation ); // ownership transferred

	iAOOperation->StartOperation();
	}

// -----------------------------------------------------------------------------
// CLmkParser::CancelImporting
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkParser::CancelImporting()
	{
	if ( iAOOperation )
		{
		iAOOperation->Cancel();
		delete iAOOperation;
		iAOOperation = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CLmkParser::LeaveIfInUseL
// -----------------------------------------------------------------------------
//
void CLmkParser::LeaveIfInUseL()
	{
	if ( iAOOperation )
		{
		User::Leave( KErrInUse );
		}
	}

// -----------------------------------------------------------------------------
// CLmkParser::ResolveMimeTypeL
// -----------------------------------------------------------------------------
//
TInt CLmkParser::ResolveMimeTypeL( RFile& aFile, TDataType& aDataType ) const
	{
	TInt result(KErrNotFound);
	RApaLsSession ls ;
	TDataRecognitionResult dataType;
	TInt err( ls.Connect() );
	if ( err == KErrNone )
		{
		CleanupClosePushL( ls );
		err = ls.RecognizeData(aFile, dataType);
		if(err == KErrNone &&
		   (dataType.iConfidence == CApaDataRecognizerType::EProbable
		    || dataType.iConfidence == CApaDataRecognizerType::ECertain
		    || dataType.iConfidence == CApaDataRecognizerType::EPossible))
			{
			aDataType = dataType.iDataType;
			result = KErrNone;
			}
		CleanupStack::PopAndDestroy(&ls);
		}
	return result;
	}
//  End of File
