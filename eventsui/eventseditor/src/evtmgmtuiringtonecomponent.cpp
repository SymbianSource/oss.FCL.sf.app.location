/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   'Set Assigntone' component for retrieving ringtone.
*
*/

#include <evtmgmteditorui.rsg>
#include <StringLoader.h>
#include <CFLDFileListContainer.h>  // Audio File List
#include <DRMHelper.h>              // DRM
#include <eikenv.h>
#include <aknlistquerydialog.h>
#include <pathinfo.h>
#include <driveinfo.h>
#include <f32file.h>
#include <NewFileServiceClient.h>   // Voice Recorder
#include <AiwGenericParam.h>    
#include <AknIconArray.h>       // Icon Array
#include <evteditor.mbg>
#include <avkon.mbg>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <data_caging_path_literals.hrh>
#include <gulicon.h>            // Gul Icon
#include <eikapp.h>
#include <eikappui.h>
#include <CalendarDomainCRKeys.h>
#include <centralrepository.h>
#include <AknUtils.h>
#include <collate.h>

#include "evtmgmteditorui.hrh"
#include "evtmgmtuiringtonecomponent.h"
#include "evtdebug.h"
#include "evteditorconsts.h"

const TInt KSetToneGranularity = 2;

// ---------------------------------------------------------------------------
// CEvtEditor ::NewL()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiRingtoneComponent* CEvtMgmtUiRingtoneComponent::NewL(const TPtrC aPrevRingtone)
    {
    CEvtMgmtUiRingtoneComponent * self = CEvtMgmtUiRingtoneComponent::NewLC(aPrevRingtone);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtEditor ::NewLC()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiRingtoneComponent* CEvtMgmtUiRingtoneComponent::NewLC(const TPtrC aPrevRingtone)
    {
    CEvtMgmtUiRingtoneComponent * self = new (ELeave) CEvtMgmtUiRingtoneComponent();
    CleanupStack::PushL(self);
    self->ConstructL( aPrevRingtone );
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiDescComponent ::~CEvtMgmtUiDescComponent()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiRingtoneComponent::~CEvtMgmtUiRingtoneComponent()
    {    
    delete iRingtone;
    iRingtone = NULL;
    
    if( iRecorderClient ) 
        {
        delete iRecorderClient;        
        iRecorderClient = NULL;
        }
        
    if( iSelectRingtoneDlg )
        {
        delete iSelectRingtoneDlg;        
        iSelectRingtoneDlg = NULL;
        }  
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent ::ConstructL()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiRingtoneComponent ::ConstructL( const TPtrC aPrevRingtone )
    {
    iRingtone = aPrevRingtone.AllocL();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent ::CEvtMgmtUiRingtoneComponent()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiRingtoneComponent::CEvtMgmtUiRingtoneComponent()
    {    
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::Ringtone()
// It is used to retrieve the selected ringtone's absolute path.
// @ret absolute path of the selected ringtone
// -----------------------------------------------------------------------------
TPtrC CEvtMgmtUiRingtoneComponent::Ringtone() const
    {
    // if ringtone, returns ringtone else empty string
    if( iRingtone )
        return *iRingtone;
    else
        return KNullDesC();
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::DefaultRingtoneL()
// It is used to retrieve the path of the Default Tone.
// @ret absolute path of the default ringtone
// -----------------------------------------------------------------------------
TPtrC CEvtMgmtUiRingtoneComponent::DefaultRingtoneL()
    {
    TFileName toneFileName;
    CRepository* centralRepository = CRepository::NewLC(KCRUidCalendar);
    User::LeaveIfError(centralRepository->Get(KCalendarSoundFile, toneFileName));
    CleanupStack::PopAndDestroy(centralRepository);
    if( iRingtone )
        {
        delete iRingtone;
        iRingtone = NULL;
        }
    iRingtone = toneFileName.AllocL();   
    
    // if ringtone, returns ringtone else empty string
    if( iRingtone )
        return *iRingtone;
    else
        return KNullDesC();
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::DisplayAudioPopupDlgL()
// It is used to display 'Set tone' popup dialog and to retrieve ringtone.
// @ret error if there is any error
//      KErrNone if it is successful  
// -----------------------------------------------------------------------------
TInt CEvtMgmtUiRingtoneComponent::DisplayAudioTonePopupDlgL()
    {    
    TInt response = KErrNone;   
    
    //display 'set tone' popup dialog   
    TInt index = DisplayPopupDlgL();
   
    switch(index)
        {        
        case EEvtMgmtUiAssignedToneTone: // if 'select from exising' is selected
            {
            response = DisplayRingtoneSelectionDlgL();
            break;
            }
        case EvtMgmtUiAssignedToneRecordNew: //if 'record new' is selected
            {
            response = RecordNewToneL();
            break;
            }
        default:
            {
            response = KErrCancel;
            Debug(_L("DisplayAudioTonePopupDlgL panic"));
            break;
            }
        }    
     return response;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::DisplayRingtoneSelectionDlgL()
// t is used display  system's 'select tone' dialog.
// @ret error while displaying 'select tone' dialog and 
//      retrieving ringtone from it. 
//      KErrNone if it is successful
// -----------------------------------------------------------------------------
TInt CEvtMgmtUiRingtoneComponent::DisplayRingtoneSelectionDlgL()
    {
    TInt response = KErrNone;
    
    // Create instance of FileList using new FileList API
    if(!iSelectRingtoneDlg)
        iSelectRingtoneDlg = CFLDFileListContainer::NewL();

    iSelectRingtoneDlg->SetAutomatedType( CDRMHelper::EAutomatedTypeRingingTone );

    // Do not show videos, use new FileList API
    iSelectRingtoneDlg->AddExclusiveMediaTypeL( ECLFMediaTypeVideo );
    
    HBufC* strOff = StringLoader::LoadLC( R_EVTUI_RINGTONESELECT_OFF );
    
    iSelectRingtoneDlg->InsertEndNullItemL( *strOff );
    TFileName ringtone( *iRingtone );
    
    //launch selection dialog
    if( iSelectRingtoneDlg->LaunchL( ringtone ) ) //if ringtone is selected
        {        
        delete iRingtone;
        iRingtone = NULL;
        iRingtone = ringtone.AllocL();   
        }
    else // if dialog is cancelled
        response = KErrCancel;
    
    CleanupStack::PopAndDestroy(1 ); // strOff
    delete iSelectRingtoneDlg;
    iSelectRingtoneDlg = NULL;
    
    //return error code
    return response;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::DisplayPopupDlgL()
// It is used to display generic popup dialog.
// @ret index of item selected if OK is pressed
//       KErrCancel if Cancel is pressed 
// -----------------------------------------------------------------------------
TInt CEvtMgmtUiRingtoneComponent::DisplayPopupDlgL()
    {
    TInt index = 0;
    const TInt KToneIconIndex = 0;
    const TInt KRecordNewIconIndex = 1;
    const TInt KIconTextLength = 1;
    const TInt KAssignToneGranularity =2;   
    _LIT(KTab, "\t");  
    HBufC* dummy = NULL;
    
    //title text
    HBufC* title  = StringLoader::LoadLC( R_EVTUI_CREATENEW_FORM_ASSIGNTONEPOPUP_TITLE );
     
    //Tone
    dummy = StringLoader::LoadLC(R_EVTUI_CREATENEW_FORM_ASSIGNTONEPOPUPOPTIONS_TONE);
    HBufC* tone = HBufC::NewL( KIconTextLength + KTab().Length() + 
                       dummy->Length() + KTab().Length( )+ KTab().Length() );
    TPtr ptrDummy = tone->Des();
    ptrDummy.Num(KToneIconIndex);
    ptrDummy.Append(KTab);
    ptrDummy.Append(*dummy);
    ptrDummy.Append(KTab);
    ptrDummy.Append(KTab);
    CleanupStack::PopAndDestroy(dummy);
    CleanupStack::PushL(tone);
    
    // Record new
    dummy = StringLoader::LoadLC(R_EVTUI_CREATENEW_FORM_ASSIGNTONEPOPUPOPTIONS_RECORDNEW);
    HBufC* recordNew = HBufC::NewL( KIconTextLength + KTab().Length() + 
                      dummy->Length() + KTab().Length( )+ KTab().Length() );
    ptrDummy.Set( recordNew->Des() );
    ptrDummy.Num(KRecordNewIconIndex);
    ptrDummy.Append(KTab);
    ptrDummy.Append(*dummy);
    ptrDummy.Append(KTab);
    ptrDummy.Append(KTab);
    CleanupStack::PopAndDestroy(dummy);
    CleanupStack::PushL(recordNew);
    
    //array
    CDesCArrayFlat* array = new(ELeave)CDesCArrayFlat( KAssignToneGranularity );
    CleanupStack::PushL(array);
    array->AppendL(*tone);
    array->AppendL(*recordNew);
    
    //launch dialog
    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &index );
    dlg->PrepareLC( R_EVTMGMTUI_LIST_QUERY );  
    UpdateIconsL( *dlg );
    dlg->SetItemTextArray( array ); 
    dlg->SetOwnershipType( ELbmOwnsItemArray );  
    dlg->QueryHeading()->SetTextL( *title );
    if ( !dlg->RunLD() )     // if dialog is cancelled  
       index = KErrCancel;   
    
    CleanupStack::Pop(array); //ownership was transferred to dialog
    
    CleanupStack::PopAndDestroy( 3 ); //recordNew, tone, title
    return index;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::RecordNewToneL()
// It is used to record new ringtone.
// @ret KErrNone if recording is succesful.
//      error code if it is failed.
// -----------------------------------------------------------------------------
TInt CEvtMgmtUiRingtoneComponent::RecordNewToneL()
    {
    TInt err = KErrNone;
    
    TFileName fileName;
    err = FileNameForRecordingL( fileName );
    if(err!=KErrNone)
        {
        Debug( _L("Error while generating filename for recording =%d"), err );
        return err;
        }
    
    Debug( _L("Recording filename = ") );
    Debug( fileName );
    
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);
    fs.ShareProtected(); // The file server session must be in this mode

    
    // Record Audio Clip    
    if(!iRecorderClient)
        iRecorderClient = NewFileServiceFactory::NewClientL();  
       
    //create empty audio file
    RFile file;
    file.Replace( fs, fileName, EFileShareAny|EFileWrite  );
    CleanupClosePushL(file);
    
 
    // launch recorder application to record tone.
    TBool response = EFalse;
    CAiwGenericParamList* params = CAiwGenericParamList::NewLC();
    TAiwVariant variant;
    variant.Set( _L("file" ) );
    TAiwGenericParam param( EGenericParamURL, variant );
    params->AppendL( param );
    TRAP(err, response = iRecorderClient->NewFileL( file, params, ENewFileServiceAudio ) );   
    Debug( _L( "Error while recording =%d, response =%d" ), err, response );
    CleanupStack::PopAndDestroy( 2 ); // params, file
    delete iRecorderClient;
    iRecorderClient = NULL;
   //if recording is successful, set ringtone.
   if( response && err == KErrNone)
       {
       delete iRingtone;
       iRingtone = NULL;
       iRingtone = fileName.AllocL();
       }
   
   //if recording is cancelled, set err to KErrCancel
   if( !response )
       err = KErrCancel;       
   
   //if there is error in recording or recording is cancelled, delete the file.
   if( err!=KErrNone )
       fs.Delete( fileName );
   
   CleanupStack::PopAndDestroy(); //fs
   
   return err;
   }

// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::RecordNewToneL()
// It is used to generate filename for recording new file.
// @param[out] aFileName - it will contain generated file-name
// @ret KErrNone if recording is succesful.
//      error code if it is failed.
// -----------------------------------------------------------------------------
TInt CEvtMgmtUiRingtoneComponent:: FileNameForRecordingL( TDes& aFileName )
    { 
    PathInfo::GetFullPath(aFileName, EDriveC, PathInfo::ESoundsPath );
    
    HBufC* recStr = StringLoader::LoadLC( R_LOCEV_LIST_VOREC_CLIP_NAME );
    
    //Open sound directory
    RDir directory;
    TInt err = directory.Open(CEikonEnv::Static()->FsSession(), aFileName, KEntryAttNormal);       
    if( err!=KErrNone )
       {     
       return err;
       }   
    CleanupClosePushL(directory);
    
    //Retrieve files from directory
    TEntryArray fileList;    
    err = directory.Read(fileList);
    
    TInt maxValue = 0;
    
    if ( err == KErrNone || err == KErrEof )
       {
       err = KErrNone;
       //Check folder's files and find last recent audio file    
       for (TInt i = 0; i < fileList.Count(); ++i)
           {
           TEntry fileEntry = fileList[i];
           
           TCollationMethod cm = *Mem::CollationMethodByIndex( 0 ); // default collation method
           cm.iFlags |= TCollationMethod::EIgnoreNone;
           
           //Check that file is valid audiofile                
           if (!fileEntry.IsReadOnly() && 
               !fileEntry.IsHidden() &&
               !fileEntry.IsSystem() && 
               !fileEntry.IsDir() &&
               TDesC::EIsPrefix == fileEntry.iName.HasPrefixC(*recStr, 0, &cm) &&
               fileEntry.iName.Find(*recStr) != KErrNotFound)
               {
               //Find out running number
               TParse parser;
               parser.Set( fileEntry.iName, NULL, NULL );
               TFileName fileName = parser.Name();
               fileName.Delete(0,recStr->Length());
               fileName.Trim();

               //Convert digits from any digit format to european digit format. 
               AknTextUtils::ConvertDigitsTo( fileName, EDigitTypeWestern );
                               
               TInt multiplier = 1;
               TInt count = 0;
               for (TInt i = fileName.Length() - 1; i != -1;  --i)
                   {
                   TChar numberchar = fileName[i];
                   //Check if the character is digit
                   if( numberchar.IsDigit() )
                       {
                       TInt number = numberchar.GetNumericValue();
                       
                       count += number * multiplier;
                       multiplier = multiplier * 10;
                       }
                   else
                       {
                       break;
                       }
                   }
               
               //Save current number of audio files
               if (count > maxValue)
                   {
                   maxValue = count;
                   }
               }
           }
       }
    
    //Create a text containing the count to localize
    TFileName num;
    num.Zero();
    num.AppendNum(maxValue+1);

    //Converts between arabic-indic digits and european digits 
    //based on existing language setting.
    AknTextUtils::LanguageSpecificNumberConversion( num );
    
    //set recording filename
    aFileName.Append(*recStr);
    aFileName.Append(num);    
    aFileName.Append(KRecorderFileEnd);
    
    CleanupStack::PopAndDestroy(2); // dir, recStr
    
    return err;           
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::UpdateIconsL
// It update icons
// -----------------------------------------------------------------------------
void CEvtMgmtUiRingtoneComponent::UpdateIconsL( CAknListQueryDialog& aDialog )
    {
    Debug(_L("start CEvtMgmtUiPlaceComponent::UpdateIconsL()"));
    
    CAknIconArray* icons = new( ELeave ) CAknIconArray( KSetToneGranularity );
    CleanupStack::PushL( icons );

    TFileName *iconFile = IconFileNameLC();

    AddIconL( *icons, 
              *iconFile,
              KAknsIIDQgnPropFmgrFileSound, 
              EMbmEvteditorQgn_prop_fmgr_file_sound, EMbmEvteditorQgn_prop_fmgr_file_sound_mask);
    
    AddIconL( *icons, 
             *iconFile,
             KAknsIIDQgnPropFmgrFileVoicerec, 
             EMbmEvteditorQgn_prop_fmgr_file_voicerec, EMbmEvteditorQgn_prop_fmgr_file_voicerec_mask);

    CleanupStack::PopAndDestroy(iconFile);

    //set new icons to array
    aDialog.SetIconArrayL( icons );  
    CleanupStack::Pop(icons);
    
    Debug(_L("end CEvtMgmtUiPlaceComponent::UpdateIconsL()"));
        
    }
    
// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::AddIconL
// Append icon to icon array. CAknIconArray owns icons so it is 
// easier to it leave safely.
// @param aIcons icon array
// @param aIconFileWithPath icon file name with path
// @param aSkinID skin id of the icon
// @param aIconGraphicsIndex picture index
// @param aIconGraphicsMaskIndex mask index
// -----------------------------------------------------------------------------
void CEvtMgmtUiRingtoneComponent::AddIconL( CAknIconArray&  aIcons,
                                           const TDesC&    aIconFileWithPath,
                                           TAknsItemID     aSkinID,
                                           TInt            aIconGraphicsIndex,
                                           TInt            aIconGraphicsMaskIndex )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    EVTUIDEBUG( "AknsUtils::SkinInstance" );
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
    TRAPD( err, AknsUtils::CreateIconL( skin,
                            aSkinID,
                            bitmap,
                            bitmapMask,
                            aIconFileWithPath,
                            aIconGraphicsIndex,
                            aIconGraphicsMaskIndex ) );

	
    EVTUIDEBUG1( "AknsUtils::CreateIconL - %d", err );
    if(err != KErrNone)
	    {
	    TRAP( err, AknIconUtils::CreateIconL( bitmap,
	                            bitmapMask,
	                            aIconFileWithPath,
	                            aIconGraphicsIndex,
	                            aIconGraphicsMaskIndex ) );
    	EVTUIDEBUG1( "AknIconUtils::CreateIconL - %d", err );
	    }
    User::LeaveIfError( err );
    
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( bitmapMask );
    
    CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );
    
    // Remove the Bitmap and the Bitmap Icon from the Clean up stack since 
    // the ownership is taken over by the icon
    CleanupStack::Pop( bitmapMask );
    CleanupStack::Pop( bitmap ); 
    
    CleanupStack::PushL( icon );
    aIcons.AppendL( icon );
    CleanupStack::Pop( icon ); // icon
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiRingtoneComponent::IconFileNameLC
// Gets Events Management UI icon file path and name. Ownership
// is transferred, item is left in the cleanup stack.
// @return TFileName* The full name of the Events UI icon file.
// -----------------------------------------------------------------------------
TFileName* CEvtMgmtUiRingtoneComponent::IconFileNameLC() const
    {
    TFileName* fileName = new(ELeave) TFileName;
    CleanupStack::PushL( fileName );
    
    TFileName* tmpName = new(ELeave) TFileName;
    CleanupStack::PushL( tmpName );
    
    tmpName->Append( KDC_BITMAP_DIR );
    tmpName->Append( KEvtMgmtUiIconFileName );

    CEikAppUi *appUi = ( CEikAppUi *)( CEikonEnv::Static()->AppUi());
    TFileName appDrive = appUi->Application()->AppFullName();
    
    TParse parse;
    User::LeaveIfError( parse.Set( appDrive, NULL, NULL ) );
    User::LeaveIfError( parse.Set( parse.Drive(), tmpName, NULL ) );
    
    fileName->Copy(parse.FullName());

    CleanupStack::PopAndDestroy( tmpName );

    return fileName;
    }  
//end of file
