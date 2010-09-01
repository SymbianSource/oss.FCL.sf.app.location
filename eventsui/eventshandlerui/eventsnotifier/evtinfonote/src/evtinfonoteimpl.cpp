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
* Description:  Info Note implementation
*
*/


// System Includes
#include <evtinfonote.rsg>
#include <eikbtgpc.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <avkon.mbg>
#include <evtinfonote.mbg>
#include <eikimage.h>
#include <StringLoader.h>
#include <aknlists.h> 
#include <aknPopup.h> 
#include <badesca.h> 
#include <data_caging_path_literals.hrh>
#include <AknsConstants.h> 		// Skin Ids
#include <AknIconUtils.h>
#include <AknsSkinInstance.h>

// User Includes
#include "evtinfonoteimpl.h"
#include "evtinfonoteparams.h"
#include "evtinfonoteconsts.hrh"
#include "evtkeylockhandler.h"

// Constant Defintions
_LIT( KEvtDefaultIconFileName, "evtinfonote.mif" );
static const TInt KTimetoPlay = 60;
static const TInt KTimetoSnooze = 60;

// Context menu Array
 	const TInt KListGranularity = 5;
static const TInt KEvtOpenEvent = 0;
static const TInt KEvtViewDesc = 1;
static const TInt KEvtNone = 2;

#include "evtdebug.h"

//
// ------------------------- Member Function definition ----------------------

// ---------------------------------------------------------------------------
// CEvtInfoNoteImpl::CEvtInfoNoteImpl()
// ---------------------------------------------------------------------------
//
CEvtInfoNoteImpl::CEvtInfoNoteImpl( CEvtInfoNoteInputParam*      aInputParam )
    :CAknQueryDialog( CAknQueryDialog::ENoTone ),
    iInputParam( aInputParam )
    {
    }

// ---------------------------------------------------------------------------
// CEvtInfoNoteImpl::~CEvtInfoNoteImpl()
// ---------------------------------------------------------------------------
//
CEvtInfoNoteImpl::~CEvtInfoNoteImpl()
    {
    // Delete the tone player
    delete iTonePlayer;

    // Delete the timer
    if( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        }
    
		// Update the Key Lock state before Close.
    if(iKeyLockHandler)
    		{
    		if(*iExitFlag == EEvtCloseInfoNote || *iExitFlag == EEvtCmdSnoozeEvent )
						iKeyLockHandler->UpdateStateWithNote();
				else
						iKeyLockHandler->UpdateStateWithoutNote();
    		}
    
    // Delete Key Lock Handler
    delete iKeyLockHandler;
    
    // Delete the Input parameter object
    delete iInputParam;          
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteImpl::ConstructL()
// ---------------------------------------------------------------------------
//    
void CEvtInfoNoteImpl::ConstructLC()
    {
    EVTUIDEBUG( "+ CEvtInfoNoteImpl::ConstructL" );
    
    CAknQueryDialog::PrepareLC( R_LOC_EVT_INFO_QUERY );
    
    // Prepare the Heading resource
    TPtrC debug( iInputParam->Subject() );
    EVTUIDEBUG1( "Notifier for - Subject = %S", &debug );
    
	// Set the Heading
    QueryHeading()->SetTextL( iInputParam->Subject());
    
    // Set the Query Text   
    SetPromptL( iInputParam->Description());
    
    // Load the Image fpr the query
    LoadImageL();
        
    iKeyLockHandler = CEvtKeyLockHandler::NewL( );
    
    // Check if the tone needs to be played. In case, it is true then
    // create the player and update the CBA
    if ( iInputParam->ShowSnooze() )
        {
        if ( iInputParam->ToneEnabled() )
            {
            // Create the tone player
            iTonePlayer = CEvtTonePlayer::NewL( *this );
            
            // Update the CBA - Snooze context silence
            ButtonGroupContainer().SetCommandSetL( R_LOC_EVT_INFONOTE_SNOOZE_TONE_CBA );            
            }
        else
            {
            // Update the CBA - Snooze context close
            ButtonGroupContainer().SetCommandSetL( R_LOC_EVT_INFONOTE_SNOOZE_CBA );     
            }
        }
        else
        {
        if ( iInputParam->ToneEnabled() )
            {
            // Create the tone player
            iTonePlayer = CEvtTonePlayer::NewL( *this );
            
            // Update the CBA - open context silence
            ButtonGroupContainer().SetCommandSetL( R_LOC_EVT_INFONOTE_OPEN_TONE_CBA );            
            }
        }
    EVTUIDEBUG( "- CEvtInfoNoteImpl::ConstructL" )     		           		             
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteImpl::NewL()
// ---------------------------------------------------------------------------
//     
CEvtInfoNoteImpl* CEvtInfoNoteImpl::NewLC( CEvtInfoNoteInputParam*      aInputParam )
    {
    CEvtInfoNoteImpl* self = new ( ELeave ) CEvtInfoNoteImpl( aInputParam );
    CleanupStack::PushL( self );
    self->ConstructLC();
    CleanupStack::Pop( self );
    return self;    
    }

// ---------------------------------------------------------------------------
// TInt CEvtInfoNoteImpl::RunLD()
// ---------------------------------------------------------------------------
//  
TInt CEvtInfoNoteImpl::RunLD(TEvtInfoNoteCmds* aFlag)
    {
    // Call RunLD
    iExitFlag = aFlag;
    return RunLD();
    } 

// ---------------------------------------------------------------------------
// TInt CEvtInfoNoteImpl::RunLD()
// ---------------------------------------------------------------------------
//  
TInt CEvtInfoNoteImpl::RunLD()
    {
    // Maintain the Key Lock State and Enable if it is already not enabled
    if ( iKeyLockHandler )
        {
        iKeyLockHandler->MaintainState();
        }
    
    TInt timeToPlay = KTimetoPlay;
    if ( iInputParam->ShowSnooze() )
        {
		// Start the Timer if the Event has to be snoozed.
        if( !iTimer )
            {
            iTimer = CPeriodic::NewL( 0 ); // neutral priority
            }
        iTimer->Start( TTimeIntervalMicroSeconds32( KTimetoSnooze * 1000000 ),
                          TTimeIntervalMicroSeconds32( KTimetoSnooze * 1000000 ),
                          TCallBack(TickL, this));
        
        timeToPlay = 0;
        }
    
    // If tone player is there, then start tone playing
    if ( iTonePlayer )
        {
        iTonePlayer->PlayFileL( iInputParam->Tone(), iInputParam->ToneRepeat(), timeToPlay );
        }
        
    // Pass the function to the base class
    return CAknQueryDialog::RunLD();
    }                  
                         
// ---------------------------------------------------------------------------
// void CEvtInfoNoteImpl::OkToExitL()
// ---------------------------------------------------------------------------
//                                       
TBool CEvtInfoNoteImpl::OkToExitL( TInt aButtonId )
    {
    EVTUIDEBUG1( "+ CEvtInfoNoteImpl::OkToExitL - ButtonId = %d", aButtonId );
    switch(aButtonId)
        {
        case EEvtCmdSnoozeEvent: // when LSK Snooze is pressed
            {
	        *iExitFlag = EEvtCmdSnoozeEvent; 
	        return ETrue;
            }
        case EEvtCmdOpenEvent: // when LSK Open is pressed
            {
			// Disable the Key Lock irrespective of previously maintained state.
		    if ( iKeyLockHandler )
		        {
		        iKeyLockHandler->EnableKeyLock( EFalse );
		        }
	        *iExitFlag = EEvtCmdOpenEvent; 
	        return ETrue;
            }
        case EEvtCmdSilence: // when RSK Silence is pressed
            {
	        // If tone player is there, then start tone playing
	        if ( iTonePlayer )
	            {
	            iTonePlayer->StopPlayingL( EFalse );
	            } 
	        *iExitFlag = EEvtCmdSilence; 
	        return EFalse;
            }
        case EAknSoftkeyContextOptions: //when MSK is pressed
            {
 
	    	switch( GetMskOptionL() )
	    		{
	    		case KEvtOpenEvent:
	    			// Disable the Key Lock irrespective of previously maintained state.
				    if ( iKeyLockHandler )
				        {
				        iKeyLockHandler->EnableKeyLock( EFalse );
				        }
	    			*iExitFlag = EEvtCmdOpenEvent;
	    			 return ETrue;
	    		case KEvtViewDesc:
	    			// Disable the Key Lock irrespective of previously maintained state.
				    if ( iKeyLockHandler )
				        {
				        iKeyLockHandler->EnableKeyLock( EFalse );
				        }
	    			*iExitFlag = EEvtCmdShowDetails;
	    			 return ETrue;
	    		default:
	    			 return EFalse;
	    		}
            }
        case EEvtCloseInfoNote: // when RSK Silence is pressed
            {
		    *iExitFlag = EEvtCloseInfoNote;
	        return ETrue;
            }
        default: // close dialog
            {
            return ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteImpl::PostLayoutDynInitL()
// ---------------------------------------------------------------------------
//     
void CEvtInfoNoteImpl::PostLayoutDynInitL()
    {
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteImpl::TonePlayCompleteL()
// ---------------------------------------------------------------------------
//  
void CEvtInfoNoteImpl::TonePlayCompleteL( TBool aTimeout )
	{
		// Enable KeyLock on Timeout.
    if ( iKeyLockHandler && aTimeout )
        {
        iKeyLockHandler->EnableKeyLock( ETrue );
        iKeyLockHandler->UpdateStateWithoutNote();
        }
        
    // Update the CBA
    if ( iInputParam->ShowSnooze() )
        {
        ButtonGroupContainer().SetCommandSetL( R_LOC_EVT_INFONOTE_SNOOZE_CBA );
        }
    else
        {
        ButtonGroupContainer().SetCommandSetL( R_LOC_EVT_INFONOTE_OPEN_CBA );
        }
	}


// ---------------------------------------------------------------------------
// TInt CEvtInfoNoteImpl::GetMskOptionL()
// ---------------------------------------------------------------------------
//  
TInt CEvtInfoNoteImpl::GetMskOptionL()
	{
    EVTUIDEBUG( "+ CEvtInfoNoteImpl::GetMskOptionL()" );
	TInt selected(KEvtNone);
	
    CAknSinglePopupMenuStyleListBox* list = new(ELeave)CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);
 
    CAknPopupList* popupList = CAknPopupList::NewL(list, R_AVKON_SOFTKEYS_OK_CANCEL,
												 AknPopupLayouts::EMenuWindow);  
    CleanupStack::PushL(popupList);
 
    list->ConstructL(popupList, 0);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
														CEikScrollBarFrame::EAuto);
 	
    CDesCArrayFlat* itemArray = new(ELeave)CDesCArrayFlat( KListGranularity );
    CleanupStack::PushL(itemArray);
 
    // add the item popup item here
    HBufC* openevent = StringLoader::LoadLC( R_LOCEV_OPTIONS_OPEN_EVENT );
    itemArray->AppendL(*openevent);
    CleanupStack::PopAndDestroy(); // openevent
    
    if( iInputParam->Description().Length() != 0 )
    	{
	    HBufC* viewdesc = StringLoader::LoadLC( R_LOCEV_OPTIONS_VIEW_DESCRIPTION );
	    itemArray->AppendL(*viewdesc);
	    CleanupStack::PopAndDestroy(); // viewdesc
    	}
 
    CleanupStack::Pop();            //itemArray
    list->Model()->SetItemTextArray(itemArray);
    list->Model()->SetOwnershipType(ELbmOwnsItemArray);
  
    // Show popup list and then show return value.
    TInt popupOk = popupList->ExecuteLD();
    if (popupOk)
    {
    selected = list->CurrentItemIndex();
    }
	
    CleanupStack::Pop();            // popuplist
    CleanupStack::PopAndDestroy(); // list
    
    EVTUIDEBUG1( "- CEvtInfoNoteImpl::GetMskOptionL() - Selected = %d", selected );
    return selected;
	}
	
// ---------------------------------------------------------------------------
// void CEvtInfoNoteImpl::LoadImageL()
// ---------------------------------------------------------------------------
//     
void CEvtInfoNoteImpl::LoadImageL()
    {   
    EVTUIDEBUG( "+ CEvtInfoNoteImpl::LoadImageL" )
        
    // Obtain the text query control
    CAknQueryControl* control = QueryControl();
    if ( control )
        {
        TFileName dllFileName;    
        Dll::FileName( dllFileName );
        //dllFileName.Copy( KTempDrive );
    	
    	TParse parse;
    	User::LeaveIfError( parse.Set( KEvtDefaultIconFileName, &KDC_APP_BITMAP_DIR, &dllFileName ));
        TFileName fileName( parse.FullName() );
        

         //retrieve accuracy level       
        CEvtInfoNoteInputParam::TEvtTriggerAccuracy accuracy = iInputParam->TrigerringAccuracy();
        EVTUIDEBUG1( "Icon creation:Accuracy level = %d", accuracy )
        
	    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	
	    EVTUIDEBUG( "AknsUtils::SkinInstance" );
	    
	    CFbsBitmap* bitmap = NULL;
	    CFbsBitmap* bitmapMask = NULL;
	    TAknsItemID aSkinID;
	    TInt bmpID;
	    TInt bmpMaskID;
	    
	    switch( accuracy )
	    	{
	    	case CEvtInfoNoteInputParam::EEvtAccurateTrigger:
	    		aSkinID = KAknsIIDQgnNoteLocevAlarmAcc;
	    		bmpID = EMbmEvtinfonoteQgn_note_locev_alarm_acc;
	    		bmpMaskID = EMbmEvtinfonoteQgn_note_locev_alarm_acc_mask;
	    		break;
	    	case CEvtInfoNoteInputParam::EEvtInaccurateTrigger:
	    		aSkinID = KAknsIIDQgnNoteLocevAlarmInacc;
	    		bmpID = EMbmEvtinfonoteQgn_note_locev_alarm_inacc;
	    		bmpMaskID = EMbmEvtinfonoteQgn_note_locev_alarm_inacc_mask;
	    		break;
	    	case CEvtInfoNoteInputParam::EEvtHighlyInaccurateTrigger:
	    	default:
	    		aSkinID = KAknsIIDQgnNoteLocevAlarmInaccHeavy;
	    		bmpID = EMbmEvtinfonoteQgn_note_locev_alarm_inacc_heavy;
	    		bmpMaskID = EMbmEvtinfonoteQgn_note_locev_alarm_inacc_heavy_mask;
	    		break;
	    	}
	    
	    TRAPD( err, AknsUtils::CreateIconL( skin,
	                            aSkinID,
	                            bitmap,
	                            bitmapMask,
	                            fileName,
	                            bmpID,
	                            bmpMaskID ) );
	
		
	    EVTUIDEBUG1( "AknsUtils::CreateIconL - %d", err );
	    if(err != KErrNone)
		    {
		    TRAP( err, AknIconUtils::CreateIconL( bitmap,
		                            bitmapMask,
		                            fileName,
		                            bmpID,
		                            bmpMaskID ) );
	    	EVTUIDEBUG1( "AknIconUtils::CreateIconL - %d", err );
		    }
		    
    	User::LeaveIfError( err );
    	
        EVTUIDEBUG( "Bitmap validation" )
        
        User::LeaveIfNull( bitmap );
        
        EVTUIDEBUG( "Mask validation" )
        User::LeaveIfNull( bitmapMask );               

	    CleanupStack::PushL( bitmap );
	    CleanupStack::PushL( bitmapMask );

		CEikImage*  image = new ( ELeave ) CEikImage;
		CleanupStack::PushL( image );
		
		EVTUIDEBUG( "Settings image" )
		image->SetPicture( bitmap, bitmapMask );
		
		control->SetImageL( image );
    			        
        // Set the image			    		
	    // control->SetImageL( fileName, EMbmAvkonQgn_note_query, EMbmAvkonQgn_note_query_mask );
	    
		CleanupStack::Pop( 3 );	    
        }
        
    EVTUIDEBUG( "- CEvtInfoNoteImpl::LoadImageL" )      
    }

// ---------------------------------------------------------------------------
// CEvtInfoNoteImpl::TickL()
// ---------------------------------------------------------------------------
//
TInt CEvtInfoNoteImpl::TickL(TAny* aObject)
    {
    ((CEvtInfoNoteImpl*)aObject)->TryExitL(EEvtCmdSnoozeEvent);
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// void CEvtInfoNoteImpl::HandlePointerEventL()
// ---------------------------------------------------------------------------
//  
void CEvtInfoNoteImpl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    CEikDialog::HandlePointerEventL( aPointerEvent );      
        if(aPointerEvent.iType == TPointerEvent::EButton1Down )
           {
           TryExitL(EAknSoftkeyContextOptions);
           }
    }

