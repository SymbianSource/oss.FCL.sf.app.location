/*
 * Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:    LandmarksUi Sender class implementation
 *
 */

// INCLUDE FILES

#include <e32std.h>
#include <StringLoader.h>
#include <f32file.h>

#include <AknWaitDialog.h>
#include <coemain.h>
#include <eikmenup.h>
#include <AknQueryDialog.h>
#include <sendui.h>
#include <SenduiMtmUids.h>
#include <CMessageData.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLmCategoryManager.h>
#include <lmkui.rsg>
#include "LmkConsts.h"
#include "CLmkEncoder.h"
#include "landmarks.hrh"
#include "CLmkUiUtils.h"
#include "LmkFileUtils.h"
#include "Debug.h"
#include "CLmkSender.h"
#include "CLmkFields.h"
#include "LmkConsts.h"
#include "CLmkDbUtils.h"
#include <CSendingServiceInfo.h>
#include <lmkerrors.h>

// CONSTANTS
//for the sendui query
const static TInt KBluetooth = 0x10009ed5;
const static TInt KInfrared = 0x100053A4;
const static TInt KEmail = 0x10001028;
const static TInt KMessaging = 0x102072D6;

const TInt KNumOfReplaceChars(25);

/// Unnamed namespace for local definitions
namespace
    {
    const TInt KLmkMtmFilterGranularity(8);
#if defined(_DEBUG)
    _LIT( KPanicMsg, "CLmkSender" );

    void Panic(TPanicCode aReason)
        {
        User::Panic(KPanicMsg, aReason);
        }
#endif
    } // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkSender::CLmkSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkSender::CLmkSender(CPosLandmarkDatabase& aDb) :
    iLastOperationResult(KErrNone), iDb(aDb)
    {
    }

// ----------------------------------------------------
// CLmkSender::ConstructL
// ----------------------------------------------------
//
void CLmkSender::ConstructL(CPosLandmarkDatabase& aDb)
    {
    iSendUi = CSendUi::NewL();
    iEncoder = CLmkEncoder::NewL(aDb, *this, ETrue);
    }

// -----------------------------------------------------------------------------
// CLmkSender::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkSender* CLmkSender::NewL(CPosLandmarkDatabase& aDb)
    {
    CLmkSender* self = new (ELeave) CLmkSender(aDb);
    CleanupStack::PushL(self);
    self->ConstructL(aDb);
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CLmkSender::~CLmkSender
// ----------------------------------------------------
//
CLmkSender::~CLmkSender()
    {
    if (iWaitNote)
        {
        TRAPD( err, iWaitNote->ProcessFinishedL() );
        if (err != KErrNone)
            {
            delete iWaitNote;
            iWaitNote = NULL;
            }
        }
    delete iEncoder;
    delete iSendUi;
    }

// ---------------------------------------------------------
// CLmkSender::DisplaySendMenuL
// ---------------------------------------------------------
//
EXPORT_C void CLmkSender::DisplaySendMenuL(CEikMenuPane& aMenuPane,
        TInt aVisibleCount, TInt aMenuTextResource) const
    {
    TInt pos(0);
    aMenuPane.ItemAndPos(ELmkCmdSendDummy, pos);
    // Set dummy command dimmed anyway. It was only needed because
    // SendUI needs to know the right position:
    aMenuPane.SetItemDimmed(ELmkCmdSendDummy, ETrue);

    // Now if there are items visible then let SendUi check whether
    // option can be shown:
    if (aVisibleCount > 0)
        {
        TSendingCapabilities capabilities(0, 0,
                TSendingCapabilities::EAllServices);
        iSendUi->AddSendMenuItemL(aMenuPane, pos, ELmkCmdSend, capabilities);

        // Set menu item text to desired value since previous operation
        // overrides it with SendUI default value.
        if (aMenuTextResource != KZeroResourceId)
            {
            aMenuPane.SetItemTextL(ELmkCmdSend, aMenuTextResource);
            }
        }
    }

// ---------------------------------------------------------
// CLmkSender::DisplaySendCascadeMenuL
// ---------------------------------------------------------
//
EXPORT_C void CLmkSender::DisplaySendCascadeMenuL(CEikMenuPane& /*aMenuPane*/) const
    {
    // Construct the MTM filter
    CArrayFixFlat<TUid>* mtmFilter = new (ELeave) CArrayFixFlat<TUid> (
            KLmkMtmFilterGranularity);
    CleanupStack::PushL(mtmFilter);
    mtmFilter->AppendL(KSenduiMtmSmsUid); // hide SMS
    mtmFilter->AppendL(KSenduiMtmFaxUid); // hide fax
    CleanupStack::PopAndDestroy(); // mtmFilter
    }

// ----------------------------------------------------
// CLmkSender::CreateAndSendL
// ----------------------------------------------------
//
void CLmkSender::CreateAndSendL(TInt aCommandId,
        const RArray<TPosLmItemId>& aMarkedIds, TPosLmItemId aSelectedId)
    {
    DEBUG( CLmkSender::CreateAndSendL entered );
    HBufC* filePath;
    PrepareSendFilePathL(filePath);
    TPtr ptr = filePath->Des();
    TInt markedCount = aMarkedIds.Count();
    if (markedCount == 0 || markedCount == 1)
        {
        // One item only marked or unmarked
        RArray<TPosLmItemId> lmIdArray(1);
        CleanupClosePushL(lmIdArray);
        User::LeaveIfError(lmIdArray.Append(aSelectedId));
        CPosLandmark* landmark = iDb.ReadLandmarkLC(aSelectedId);
        HBufC* webUrl = HBufC::NewL(KMaxBufferLen);
        CleanupStack::PushL(webUrl);
        TPtrC urlDes = webUrl->Des();
        landmark->GetPositionField(ELmkPositionFieldWebAddress, urlDes);
        if (urlDes.Length() > KMaxUrlFieldLenWithDefaultProtocol)
            {
            TInt pos = urlDes.Find(KProtocol);
            if (pos == KErrNotFound)
                {
                webUrl = webUrl->ReAllocL(KHttp().Length() + urlDes.Length());
                TPtr url = webUrl->Des();
                webUrl->Des().Copy(urlDes);
                webUrl->Des().Insert(0, KHttp);
                landmark->SetPositionFieldL(ELmkPositionFieldWebAddress,
                        webUrl->Des());
                }
            }
        CleanupStack::PopAndDestroy();//webUrl
        TPtrC landmarkName;
        TInt retVal(0);
        retVal = landmark->GetLandmarkName(landmarkName);
        if (retVal == KErrNone)
            {
            iEncoder->StartEncodingL(landmarkName, lmIdArray);
            ptr.Append(landmarkName);
            }
        else
            {
            iEncoder->StartEncodingL(KLmkEncodedDataFileName, lmIdArray);
            ptr.Append(KLmkEncodedDataFileName);
            }
        SendL(ptr, aCommandId);
        CleanupStack::PopAndDestroy(2); //landmark, lmIdArray
        }
    else
        {
        HBufC* pckgName;
        TPtrC currentPckgName;//not really used, only for passing to the func.
        if (QueryNewPackageNameLC(currentPckgName, pckgName))
            {
            TPtr name = pckgName->Des();
            iEncoder->StartEncodingL(name, aMarkedIds);
            ptr.Append(name);
            SendL(ptr, aCommandId);
            }
        CleanupStack::PopAndDestroy();
        }
    CleanupStack::PopAndDestroy(filePath); //filePath       
    }
// ----------------------------------------------------
// CLmkSender::HandleOperationL
// ----------------------------------------------------
//
void CLmkSender::HandleOperationL(TOperationTypes /*aType*/,
        TReal32 /*aProgress*/, TInt aStatus)
    {
    if (aStatus != KPosLmOperationNotComplete)
        {
        __ASSERT_DEBUG( iWaitNote, Panic( KLmkPanicNullMember ) );
        iWaitNote->ProcessFinishedL();
        iWaitNote = NULL;
        iLastOperationResult = aStatus;
        }
    }

// ----------------------------------------------------
// CLmkSender::SendLandmarksL
// ----------------------------------------------------
//
void CLmkSender::SendLandmarksL(TInt aCommandId, const TDesC& aPackageName,
        const RPointerArray<CLmkLandmark>& aLandmarks)
    {
    DEBUG( CLmkSender::SendMultipleLandmarkL entered );
    TInt lmkCount = aLandmarks.Count();
    __ASSERT_DEBUG(lmkCount > 0, Panic( KLmkPanicNullMember ) );
    if (lmkCount == 1)
        {
        SendSingleLandmarkL(aCommandId, aLandmarks);
        }
    else
        {
        SendLandmarksPackageL(aCommandId, aPackageName, aLandmarks);
        }
    }
	
// ----------------------------------------------------
// CLmkSender::SendSingleLandmarkL
// ----------------------------------------------------
//
void CLmkSender::SendSingleLandmarkL(const CPosLandmark& alandmark)
    {
    CLmkLandmark* lmkLandmark;
    LmkLandmarkFromPosLandmarkL(lmkLandmark, alandmark);
    SendSingleLandmarkL(ELmkCmdSend, *lmkLandmark);
    CleanupStack::PopAndDestroy();//landMarkLmk
    }
	
// ----------------------------------------------------
// CLmkSender::SendSingleLandmarkL()
// ----------------------------------------------------
//
void CLmkSender::SendSingleLandmarkL(TInt aCommandId,
        const CLmkLandmark& alandmark)
    {
    TPtrC ptr;
    RPointerArray<CLmkLandmark> landmark;
    landmark.Append(&alandmark);
    SendLandmarksL(aCommandId, ptr, landmark);
    landmark.Close();
    }
	
// ----------------------------------------------------
// CLmkSender::SendSingleLandmarkL()
// ----------------------------------------------------
//
void CLmkSender::SendSingleLandmarkL(TInt aCommandId, const RPointerArray<
        CLmkLandmark>& aLandmarks)
    {
    HBufC* filePath;
    PrepareSendFilePathL(filePath);
    TPtr ptr = filePath->Des();
    CLmkLandmark* lmk = aLandmarks[0];
    CPosLandmark* landmark = lmk->PosLandmark();
    CLmkDbUtils::AddDefaultProtocolL(landmark);
    TPtrC landmarkName;
    TInt retVal(KErrNone);
    retVal = landmark->GetLandmarkName(landmarkName);

    if (retVal == KErrNone && LmkFileUtils::IsValidPathL(landmarkName))
        {
        TRAP(retVal,iEncoder->StartEncodingL(landmarkName,aLandmarks));
        if (retVal == KErrNone)
            {
            ptr.Append(landmarkName);
            }
        }
    else
        {
        retVal = KErrBadName;
        }

    if (retVal != KErrNone)
        {
        TPtr lmName = ReplaceSpecialCharactersL(landmarkName);
        TRAP(retVal,iEncoder->StartEncodingL(lmName,aLandmarks));
        if (retVal == KErrNone)
            {
            ptr.Append(lmName);
            }
        else
            {
            iEncoder->StartEncodingL(KLmkEncodedDataFileName, aLandmarks);
            ptr.Append(KLmkEncodedDataFileName);
            }
        }
    SendL(ptr, aCommandId);
    CleanupStack::PopAndDestroy(filePath); //filePath
    }
	
// ----------------------------------------------------
// CLmkSender::SendLandmarksPackageL
// ----------------------------------------------------
//
void CLmkSender::SendLandmarksPackageL(TInt aCommandId,
        const TDesC& aPackageName,
        const RPointerArray<CLmkLandmark>& aLandmarks)
    {
    HBufC* pckgName;
    HBufC* filePath;
    PrepareSendFilePathL(filePath);
    TPtr ptr = filePath->Des();
    if (QueryNewPackageNameLC(aPackageName, pckgName))
        {
        TPtr desName = pckgName->Des();
        iEncoder->StartEncodingL(desName, aLandmarks);
        ptr.Append(desName);
        SendL(ptr, aCommandId);
        }
    CleanupStack::PopAndDestroy(2);
    }
	
// ----------------------------------------------------
// CLmkSender::SendL()
// ----------------------------------------------------
//
void CLmkSender::SendL(TDes& aFilePath, TInt aCommandId)
    {
    aFilePath.Append(KLmkEncodedDataFileExtension);
    iWaitNote = new (ELeave) CAknWaitDialog(NULL, ETrue);
    if (iWaitNote->ExecuteLD(R_LMK_ENCODING_WAIT_NOTE))
        {
        if (iLastOperationResult == KErrNone)
            {
            RFs fs;
            User::LeaveIfError(fs.Connect());
            CleanupClosePushL(fs);
            User::LeaveIfError(fs.ShareProtected());
            RFile fileHandle;
            User::LeaveIfError(fileHandle.Open(fs, aFilePath, EFileShareAny));
            CleanupClosePushL(fileHandle);
            TRAPD( err, SendFileL( aCommandId, fileHandle, ETrue));
            if (err != KErrNone)
                {
                CleanupStack::PopAndDestroy(); // fileHandle
                LmkFileUtils::DeleteFileL(aFilePath);
                CleanupStack::PopAndDestroy(); // fs
                User::LeaveIfError(err);
                }
            CleanupStack::PopAndDestroy(); // fileHandle
            LmkFileUtils::DeleteFileL(aFilePath);
            CleanupStack::PopAndDestroy(); // fs
            }
        else
            { // error situation
            CCoeEnv::Static()->HandleError(iLastOperationResult);
            }
        }
    else
        {
        //making iWaitNote null, since framework destroys waitnote
        //dialog on pressing cancel.
        iWaitNote = NULL;
        iEncoder->CancelEncodeOperationL();
        }
    }
	
// ----------------------------------------------------
// CLmkSender::SendFileL
// ----------------------------------------------------
//
void CLmkSender::SendFileL(TInt /*aCommandId*/, const TDesC& aFile,
        TBool aDeleteSentFile)
    {
    DEBUG( CLmkSender::SendFileL entered );

    // one attachment only:
    CMessageData * messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    messageData->AppendAttachmentL( aFile );
    DEBUG( CLmkSender::SendFileL: calling SendUI );
    LoadFilteredSendListQueryL(messageData);
    DEBUG( CLmkSender::SendFileL: SendUI returned );

    // Delete the file since it is no more needed
    if( aDeleteSentFile )
        {
        LmkFileUtils::DeleteFileL( aFile );
        }
    CleanupStack::PopAndDestroy( messageData );
    }

// ----------------------------------------------------
// CLmkSender::SendFileL -- FileHandle version
// ----------------------------------------------------
//
void CLmkSender::SendFileL(TInt /*aCommandId*/, RFile& aFile, TBool /*aDeleteSentFile*/)
    {
    DEBUG( CLmkSender::SendFileL entered );

    // one attachment only:
    CMessageData * messageData = CMessageData::NewL();
    CleanupStack::PushL(messageData);
    messageData->AppendAttachmentHandleL(aFile);
    DEBUG( CLmkSender::SendFileL: calling SendUI );
    LoadFilteredSendListQueryL(messageData);
    CleanupStack::PopAndDestroy(); //messageData
    DEBUG( CLmkSender::SendFileL: SendUI returned );
    }
	
// ----------------------------------------------------
// CLmkSender::QueryPackageName
// ----------------------------------------------------
//
TBool CLmkSender::QueryNewPackageNameLC(const TDesC& aCurrentPckgName,
        HBufC*& aNewPackageName)
    {
    TBool retVal = EFalse;
    HBufC* pkgName = StringLoader::LoadL(R_LMK_DEFAULT_PACKAGE_NAME,
            CEikonEnv::Static());

    if (aCurrentPckgName.Length() > pkgName->Des().Length())
        {
        delete pkgName;
        pkgName = NULL;
        pkgName = HBufC::NewL(aCurrentPckgName.Length());
        pkgName->Des().Copy(aCurrentPckgName);
        }

    TPtr desName = pkgName->Des();

    CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog(desName,
            CAknQueryDialog::ENoTone);
    if (dlg->ExecuteLD(R_LMK_PACKAGE_NAME_QUERY))
        {
        retVal = ETrue;
        if (!LmkFileUtils::IsValidPathL(desName))
            {
            delete pkgName;
            pkgName = NULL;

            TInt len = KLmkEncodedDataFileName().Length();
            aNewPackageName = HBufC::NewLC(len);
            aNewPackageName->Des().Copy(KLmkEncodedDataFileName);
            return retVal;
            }
        }
    aNewPackageName = HBufC::NewLC(pkgName->Des().Length());
    aNewPackageName->Des().Copy(pkgName->Des());

    delete pkgName;
    pkgName = NULL;

    return retVal;
    }

// ----------------------------------------------------
// CLmkSender::LmkLandmarkFromPosLandmarkL()
// ----------------------------------------------------
//
void CLmkSender::LmkLandmarkFromPosLandmarkL(CLmkLandmark*& aLmkLandmark,
        const CPosLandmark& aPosLandmark)
    {
    RArray<TPosLmItemId> categoryIdArray;
    RPointerArray<CPosLandmarkCategory> categories;
    aPosLandmark.GetCategoriesL(categoryIdArray);

    CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL(iDb);
    CleanupStack::PushL(categoryMgr);
    TInt catCount = categoryIdArray.Count();
    for (TInt i = 0; i < catCount; i++)
        {
        TPosLmItemId catId = categoryIdArray[i];
        CPosLandmarkCategory* category = categoryMgr->ReadCategoryLC(catId);
        categories.Append(category);
        CleanupStack::Pop();
        }
    categoryIdArray.Close();
    CleanupStack::PopAndDestroy();//categoryMgr
    aLmkLandmark = CLmkLandmark::NewL(&aPosLandmark, categories);
    CleanupStack::PushL(aLmkLandmark);
    categories.ResetAndDestroy();
    }
	
// ----------------------------------------------------
// CLmkSender::PrepareSendFilePathL()
// ----------------------------------------------------
//
void CLmkSender::PrepareSendFilePathL(HBufC*& aFilePath)
    {
    // Full path name (private path c:\\private\\UID3 + pacakge File name + File bname Extension)
    aFilePath = HBufC::NewLC(2 * KLmkPackageNameMaxLen
            + KLmkEncodedDataFileExtension().Length());
    TPtr ptr = aFilePath->Des();

    // Create the private path(c:\\private\\UID3) for Landmarks UI application
    TFileName fileName;
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    fs.SessionPath(fileName);
    fs.MkDirAll(fileName);
    aFilePath->Des().Append(fileName);
    CleanupStack::PopAndDestroy(); // fs
    }
	
// ----------------------------------------------------
// CLmkSender::PrepareMultipleLandmarkForSendL()
// ----------------------------------------------------
//
void CLmkSender::PrepareMultipleLandmarkForSendL(
        RPointerArray<CLmkLandmark>& aLmkLandmark,
        const RArray<TPosLmItemId>& aMarkedIds)
    {
    /*
     This function prepares an array of CLmkLandmark from given
     landmark ids.
     */
    for (TInt i = 0; i < aMarkedIds.Count(); i++)
        {
        CPosLandmark* landmark = iDb.ReadLandmarkLC(aMarkedIds[i]);
        CLmkLandmark* lmk;
        LmkLandmarkFromPosLandmarkL(lmk, *landmark);
        CleanupStack::Pop(lmk);
        User::LeaveIfError(aLmkLandmark.Append(lmk));
        CleanupStack::PopAndDestroy();//landmark
        }
    }

// ----------------------------------------------------
// CLmkSender::LoadFilteredSendListQueryL()
// ----------------------------------------------------
//
void CLmkSender::LoadFilteredSendListQueryL(const CMessageData* aMessageData)
    {
    /**
     * Currently we need to show only four services in the sendui query
     * 1.via messaging
     * 2.via bluetooth
     * 3.via infrared
     * 4.via email
     */
    RPointerArray<CSendingServiceInfo> serviceList;
    CleanupClosePushL(serviceList);
    //load all the available services
    iSendUi->AvailableServicesL(serviceList);
    TInt count = serviceList.Count();
    CArrayFixFlat<TUid>* serviceFilter = new (ELeave) CArrayFixFlat<TUid> (
            count);
    CleanupStack::PushL(serviceFilter);
    for (TInt i = 0; i < count; i++)
        {
        CSendingServiceInfo* info = serviceList[i];
        TPtrC ptr = info->ServiceName();
        CleanupStack::PushL(info);
        TUid id = info->ServiceId();
        //check if this service is not one of the required list
        if (id.iUid != KBluetooth && id.iUid != KInfrared && id.iUid
                != KEmail && id.iUid != KMessaging)
            {
            //unwanted serivce list for filtering.
            serviceFilter->AppendL(info->ServiceId());
            }
        CleanupStack::PopAndDestroy();//info
        }
    TSendingCapabilities capabilities(0, 0,
            TSendingCapabilities::ESupportsAttachments);
    iSendUi->ShowQueryAndSendL(aMessageData, capabilities, serviceFilter);
    CleanupStack::PopAndDestroy(2);//serviceList,serviceFilter
    }

// ----------------------------------------------------
// CLmkSender::ReplaceSpecialCharactersL
// ----------------------------------------------------
//
TPtr CLmkSender::ReplaceSpecialCharactersL(TPtrC aText)
    {
    TBuf<KNumOfReplaceChars> replaceChars;
    replaceChars.Zero();

    // special chars from Unicode/Character reference
    replaceChars.Append(0x0022); // """
    replaceChars.Append(0x002A); // "*"
    replaceChars.Append(0x002F); // "/"

    replaceChars.Append(0x003A); // ":"
    replaceChars.Append(0x003C); // "<"
    replaceChars.Append(0x003E); // ">"
    replaceChars.Append(0x003F); // "?"
    replaceChars.Append(0x005C); // "\"
    replaceChars.Append(0x007C); // "|"

    TInt nameLength = KLmkEncodedDataFileName.iTypeLength;
    if (nameLength < aText.Length())
        {
        nameLength = aText.Length();
        }
    HBufC* buffer = HBufC::NewLC(nameLength);
    TPtr des1 = buffer->Des();
    des1.Copy(aText);
    AknTextUtils::ReplaceCharacters(des1, replaceChars, TChar(' '));

    TInt spaceCount = 0;
    for (TInt i = 0; i < des1.Length(); i++)
        {
        if (des1[i] == TChar(' '))
            spaceCount++;
        }

    if (spaceCount == des1.Length())
        {
        des1.Copy(KLmkEncodedDataFileName);
        }

    CleanupStack::Pop(buffer);
    return des1;
    }

//  End of File

