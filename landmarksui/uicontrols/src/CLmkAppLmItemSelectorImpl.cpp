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
* Description:  This file contains methods of selector's base class
*
*/






// INCLUDE FILES
#include <aknsfld.h>
#include <lmkui.rsg>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <sysutil.h>
#include "landmarks.hrh"
#include "LmkConsts.h"
#include "LmkNotes.h"
#include "CLmkDbUtils.h"
#include "CLmkLocationService.h"
#include <CLmkEditorDlg.h> // TLmkEditorAttributes, TLmkEditorMode
#include "CLmkLmItemListProvider.h"
#include "CLmkLmItemListMemento.h"
#include "CLmkEditorImpl.h"
#include "CLmkAppLmItemSelectorImpl.h"
#include "CLmkLocSettingLauncher.h"
#include "lmkicondialog.h"
#include "lmkwaitdlglauncher.h" // Wait dialog launcher
#include <lmkerrors.h>


const TInt KFatClusterSize = 2048;

#if defined(_DEBUG)

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkAppLmItemSelectorImpl" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace

#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::CLmkAppLmItemSelectorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkAppLmItemSelectorImpl::CLmkAppLmItemSelectorImpl(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TBool aFindBox )
    : CLmkAppSelectorImplBase( aDb, aFindBox ),
      iSender( aSender ),
      iNewListBoxItemAdded(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::BaseConstructL()
    {
    CLmkAppSelectorImplBase::BaseConstructL();
    iDbUtils = CLmkDbUtils::NewL( iDb, *this, ETrue );
    iWaitDialogLauncher = CLmkWaitDialogLauncher::NewL();
    iWaitDialogLauncher->SetObserver(this);
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::~CLmkAppLmItemSelectorImpl
// -----------------------------------------------------------------------------
//
CLmkAppLmItemSelectorImpl::~CLmkAppLmItemSelectorImpl()
    {
    delete iWaitDialogLauncher;
    delete iLocationService;
    delete iDbUtils;
    delete iLauncher;
    iMarkedItemIds.Close();
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::LmItemListProvider
// -----------------------------------------------------------------------------
//
CLmkLmItemListProvider& CLmkAppLmItemSelectorImpl::LmItemListProvider()
    {
    __ASSERT_DEBUG( iListProvider, Panic( KLmkPanicNullMember ) );
    return static_cast<CLmkLmItemListProvider&>( *iListProvider );
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::CurrentCategoryId
// -----------------------------------------------------------------------------
//
TInt CLmkAppLmItemSelectorImpl::CurrentCategoryId()
	{
	TInt catId = KPosLmNullItemId;
	TRAP_IGNORE(catId = SelectedIdL());
	return catId;
	}

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::ChangeIconCmdL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::ChangeIconCmdL()
    {

    TInt lSelectedIconIndex(0);
    CLmkIconMapDialog* dialog = new(ELeave) CLmkIconMapDialog( lSelectedIconIndex );
    if (dialog->ExecuteLD(R_LMK_ICON_TABLE_DIALOG) )
        {
        if(lSelectedIconIndex >= 0)
            {
            // icon file also contains masks, therefore 2*
            TInt iconIndex =  2*lSelectedIconIndex;
            TInt maskIndex =  2*lSelectedIconIndex;

            RArray<TPosLmItemId>& markedItemIds = MarkedItemIdsL();
		    RFs fs;
		    User::LeaveIfError(fs.Connect());
		    CleanupClosePushL( fs );

		    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, sizeof( CPosLandmark ), EDriveC ) )
		        {
		        CleanupStack::PopAndDestroy(); // fs
		        User::LeaveNoMemory();
		        }
			else
				{
            TInt markedCount = markedItemIds.Count();
            if ( markedCount )
                {
	            	LmItemListProvider().SetSelectedItemsCount(markedCount);
                ChangeIconsL( markedItemIds, iconIndex + EMbmLmkuiQgn_prop_lm_transport, maskIndex + EMbmLmkuiQgn_prop_lm_transport_mask); // subclass implements
                iListBox->ClearSelection();
                }
            else
                {
	            	LmItemListProvider().SetSelectedItemsCount(1);
                ChangeIconL( SelectedIdL(),(iconIndex + EMbmLmkuiQgn_prop_lm_transport),(maskIndex + EMbmLmkuiQgn_prop_lm_transport_mask)); // subclass implements
                }
				}
			CleanupStack::PopAndDestroy(); // fs
            }
        }
    lSelectedIconIndex = 0;

    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::MementoL
// -----------------------------------------------------------------------------
//
MLmkListMemento* CLmkAppLmItemSelectorImpl::MementoL()
    {
    CLmkLmItemListMemento* memento = CLmkLmItemListMemento::NewL();
    return memento;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::ProcessCommandL( TInt aCommandId )
    {
    switch( aCommandId )
        {
        case ELmkCmdNewLmCurrentLocation:
            {
            RequestCurrenLocationL();
            break;
            }
        case ELmkCmdNewLmEmpty:
            {
            NewLandmarkCmdL();
            break;
            }
        case ELmkCmdDeleteLm:
            {
            // This is left empty intentionally, if subclasses do not
            // implement handling of this command then deletion using
            // backspace key is not supported for that selector.
            break;
            }
        case ELmkCmdLaunchPositionSettings:
        	{
        	LaunchPosSettingsL();
			break;
        	}
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::NotifyL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::NotifyL( const TInt aStatus )
    {
    // Stop the running wait dialog
 	TInt buttonId = iWaitDialogLauncher->StopWaitDialogL();
 	if( EEikBidCancel != buttonId )
 	    {
	    switch( aStatus )
    		{
            case KErrNone:
            case KPositionPartialUpdate:
                {
                if(iLocationService)
                	{
                	TPosition position = iLocationService->CurrentPosition();
			    	CreateLmkFromCurrentLocationL( position );
                	}
                break;
                }
            default:
                {
                break;
                }
    		}
 	    }
    delete iLocationService;
    iLocationService = NULL;
	}

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::NotifyErrorL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::NotifyErrorL( TInt aErrorCode )
    {

    // Stop the running wait dialog
 	TInt buttonId = iWaitDialogLauncher->StopWaitDialogL();

    delete iLocationService;
    iLocationService = NULL;

 	if( EEikBidCancel != buttonId )
 	    {
	    switch( aErrorCode )
    		{
	        case KPositionQualityLoss:
    		case KErrTimedOut:
    		case KErrNotFound: // No PSY selected.
    		case KErrUnknown:
    		case KErrCancel:
    		case KErrArgument:
    		default:
    		    // The above errors are because of location request failures
		        // Ask user to retry location request
    			{
    			if ( LmkNotes::MessageQueryWithLinkL(iEnv,R_LMK_MSG_QUERY_HEADING_PANE_TEXT,
    												R_LMK_CONF_RETRY_LOCATION_REQUEST,R_LMK_REQUEST_FAIL_MORE_INFO,
    												MessageQueryCallBack) )
    				{
    				if( iLocationService )
    					{
    					delete iLocationService;
    					iLocationService = NULL;
    					}
    				RequestCurrenLocationL();
    				}
    			}
    		}
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::SelectedIdL
// -----------------------------------------------------------------------------
//
TPosLmItemId CLmkAppLmItemSelectorImpl::SelectedIdL()
    {
    __ASSERT_DEBUG( iListBox, Panic( KLmkPanicNullMember ) );
    TPosLmItemId selected( KPosLmNullItemId );
    TInt currentItemIndex = iListBox->CurrentItemIndex();
    if ( iFindBox )
        {
        CAknFilteredTextListBoxModel* model =
            static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );
        TInt filteredItemIndex =
            model->Filter()->FilteredItemIndex( currentItemIndex );
        LmItemListProvider().GetSelectedItemId( selected, filteredItemIndex );
        }
    else
        {
        LmItemListProvider().GetSelectedItemId( selected, currentItemIndex );
        }
    return selected;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::MarkedItemIdsL
// -----------------------------------------------------------------------------
//
RArray<TPosLmItemId>& CLmkAppLmItemSelectorImpl::MarkedItemIdsL()
    {
    __ASSERT_DEBUG( iListBox, Panic( KLmkPanicNullMember ) );
    const CArrayFix<TInt>* markedIndexes = NULL;
    if ( iFindBox )
        {
        CAknFilteredTextListBoxModel* model =
            static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );
        model->Filter()->UpdateSelectionIndexesL();
        markedIndexes = model->Filter()->SelectionIndexes();
        }
    else
        {
        markedIndexes = iListBox->SelectionIndexes();
        }
    if( markedIndexes->Count() == LmItemListProvider().ItemCount() )
    	{
    	LmItemListProvider().GetAllItemIdsL( iMarkedItemIds );
    	}
    else
    	{
		LmItemListProvider().GetSelectedItemIdsL( iMarkedItemIds, *markedIndexes );
    	}
    return iMarkedItemIds;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::NewLandmarkCmdL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::NewLandmarkCmdL()
    {
    CPosLandmark* landmark = CPosLandmark::NewL();
    CleanupStack::PushL( landmark );
    InitialLandmarkL( landmark );

    TInt BytesToWrite = CalculateLandmarkDisksize(*landmark);
    TChar DriveName = 'c';
    DiskSpaceBelowCriticalLevelL(BytesToWrite, DriveName);

    ExecuteNewLandmarkEditorL( *landmark );
    CleanupStack::PopAndDestroy( landmark );
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::DiskSpaceBelowCriticalLevelL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::DiskSpaceBelowCriticalLevelL(
    TInt aBytesToWrite, TChar aDrive)
    {
    RFs  iFs;   // File session
    User::LeaveIfError(iFs.Connect());

    // Resource value is in kilobytes, convert to bytes. (* 1024)
    TInt iCriticalDiskLevel = 120 * 1024;
    TVolumeInfo* volumeInfo = new(ELeave) TVolumeInfo;
    CleanupStack::PushL(volumeInfo);
    TInt drive;

    User::LeaveIfError(iFs.CharToDrive(aDrive, drive));
    User::LeaveIfError(iFs.Volume(*volumeInfo, drive));

    // RFs allocates bytes in steps of 2048 bytes. Calculate the actual
    // bytes to write according to this.
    TInt bytesInLastCluster = aBytesToWrite % KFatClusterSize;
    TInt paddedBytesToFillCluster = 0;
    if (bytesInLastCluster > 0)
        {
        paddedBytesToFillCluster = KFatClusterSize - bytesInLastCluster;
        }

    TInt64* bytesToWrite= new(ELeave) TInt64(aBytesToWrite + paddedBytesToFillCluster);
    CleanupStack::PushL(bytesToWrite);
    TInt64* freeAfterWrite = new(ELeave) TInt64((*volumeInfo).iFree - (*bytesToWrite));
	CleanupStack::PushL(freeAfterWrite);
    TInt64* limit = new(ELeave) TInt64(iCriticalDiskLevel);
    CleanupStack::PushL(limit);
    iFs.Close();
    if ((*freeAfterWrite) < (*limit))
        {
        User::Leave(KErrDiskFull);
        }
    CleanupStack::PopAndDestroy(4);//volumeInfo
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::CalculateLandmarkDisksize
// -----------------------------------------------------------------------------
//
TInt CLmkAppLmItemSelectorImpl::CalculateLandmarkDisksize(
    const CPosLandmark& aLandmark)
	{
	// Constant for estimating disk size
    const TReal bytesToDisk = 4.4;
    // A bare landmark takes 44 bytes on disk
    const TInt bareLm = 44;
    // Adding 73 bytes when landmark have an icon.
    const TInt iconIncluded = 73;

    // Method variables
    TInt err = KErrNone;
    TReal result = 0;

    // Bare landmark
    result += bareLm * bytesToDisk;

    // Estimate location
    TLocality loc;
    err = aLandmark.GetPosition(loc);
    if (!err)
        {
        if (!TRealX(loc.Latitude()).IsNaN())
            {
            // TReal64
            result += bytesToDisk * bytesToDisk * 4;
            }
        if (!TRealX(loc.Longitude()).IsNaN())
            {
            // TReal64
            result += bytesToDisk * bytesToDisk * 4;
            }
        if (!TRealX(loc.Altitude()).IsNaN())
            {
            // TReal32
            result += bytesToDisk * bytesToDisk * 2;
            }
        if (!TRealX(loc.HorizontalAccuracy()).IsNaN())
            {
            // TReal32
            result += bytesToDisk * bytesToDisk * 2;
            }
        if (!TRealX(loc.VerticalAccuracy()).IsNaN())
            {
            // TReal32
            result += bytesToDisk * bytesToDisk * 2;
            }
        }

    // Calculate landmark coverage radius
    TReal32 real;
    err = aLandmark.GetCoverageRadius(real);
    if (!err)
        {
        result += bytesToDisk * bytesToDisk * 2;
        }

    // Calculate landmark name
    TPtrC des;
    err = aLandmark.GetLandmarkName(des);
    if (!err)
        {
        result += des.Length() * bytesToDisk * 2;
        }

    // Calculate landmark description
    err = aLandmark.GetLandmarkDescription(des);
    if (!err)
        {
        result += des.Length() * bytesToDisk * 2;
        }

    // Icon
    // Constants for adding a icon
    TInt iconIndex;
    TInt maskIndex;
    err = aLandmark.GetIcon(des, iconIndex, maskIndex);
    if (!err)
        {
        result += iconIncluded;
        }

    // Calculating position fields
    TInt nrFields = aLandmark.NumOfAvailablePositionFields();
    TPositionFieldId fid = aLandmark.FirstPositionFieldId();
    for (TInt j = 0; j < nrFields; j++)
        {
        err = aLandmark.GetPositionField(fid, des);
        if (!err)
            {
            result += des.Length() * bytesToDisk;
            }

        fid = aLandmark.NextPositionFieldId(fid);
        }

    return static_cast<TInt> (result);
	}

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::ExecuteNewLandmarkEditorL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::ExecuteNewLandmarkEditorL(
    CPosLandmark& aLandmark )
    {
    TLmkEditorAttributes attributeFlags( CLmkEditorDlg::ELmkAll );
    TLmkEditorMode editorMode( CLmkEditorDlg::ELmkEditor );

    iNewLmkItemId = 0;

    if(iDlg)
    	{
    	delete iDlg;
    	iDlg = NULL;
    	}
    iDlg = CLmkEditorImpl::NewL( iDb,
                                                iSender,
                                                attributeFlags,
                                                editorMode,
                                                &iNewLmkItemId,
                                                &aLandmark );
    if(iDlg->ExecuteLD())
    	{
    	iNewListBoxItemAdded = ETrue;
    	}
    iDlg = NULL;
    TInt searchTextLength = iSearchBox->TextLength();
	iSearchBox->ResetL();
    if( !iNewLmkItemId && searchTextLength > 0 )
        {
        ProcessCommandL( ELmkCmdFindBoxSearchAfresh );
        }
	iSearchBox->DrawNow();
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::RequestCurrenLocationL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::RequestCurrenLocationL()
    {
    HBufC* srvName = StringLoader::LoadLC(
                      R_LMK_SERVICE_RULE_NAME, const_cast<CEikonEnv *>( iEnv ));

    iLocationService = CLmkLocationService::NewL(*srvName);
   	CleanupStack::PopAndDestroy(srvName); //srvName
    iLocationService->SetObserver( *this );

    iLocationService->LocationRequestL();

    TRAPD( error, iWaitDialogLauncher->StartWaitDialogL());
    if( error )
        {
        delete iLocationService;
        iLocationService = NULL;

        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::CreateLmkFromCurrentLocationL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::CreateLmkFromCurrentLocationL(
                                    const TPosition& aPosition)
    {
    CPosLandmark* landmark = CPosLandmark::NewL();
    CleanupStack::PushL( landmark );
    landmark->SetPositionL( aPosition );
    InitialLandmarkL( landmark );
    ExecuteNewLandmarkEditorL( *landmark );
    CleanupStack::PopAndDestroy( landmark );
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::UpdateMarkIndexAfterAdditionL
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::UpdateMarkIndexAfterAdditionL(
    TInt aIndexToBeSelected )
    {
    __ASSERT_DEBUG( iListBox, Panic( KLmkPanicNullMember ) );
	CListBoxView* lListBoxView = ( CListBoxView* )NULL;
	lListBoxView = iListBox->View();
	if( lListBoxView )
    	{
		//Fetching the array of marked item
		CArrayFix<TInt>* lActualSelectionArray = (CArrayFix<TInt>*)lListBoxView->SelectionIndexes();
    	if( lActualSelectionArray && (lActualSelectionArray->Count() > 0) )
        	{
			TInt lCnt(0);
    		TInt lSelectedIndexCnt = lActualSelectionArray->Count();
    		/*Sorting these array item in Accessding order
    		 *Because user may marked the item in any order
    		 */
    		TKeyArrayFix lKeyArrayFix(0,ECmpTInt);
    		lActualSelectionArray->Sort(lKeyArrayFix);

    		/*Declaring a local array.which will hold selected indexs.
    		 *this need because if we change any selection in the listbox
    		 *that will effect the original array.That may crash the logic
    		 */
			CArrayFix<TInt>* lLocalArray = (CArrayFix<TInt>*)NULL;
			lLocalArray = new( ELeave ) CArrayFixFlat<TInt>(iListBox->Model()->NumberOfItems());

			//Copying the indexs from the original array to the local array
			while( lSelectedIndexCnt > 0)
    			{
				lLocalArray->AppendL( lActualSelectionArray->At(lCnt) );
				lCnt++;
				lSelectedIndexCnt--;
    			}

			lSelectedIndexCnt = lLocalArray->Count();
			while( lSelectedIndexCnt > 0 )
    			{
				TInt lToSelectIndex(0);
				lToSelectIndex = lLocalArray->At( lSelectedIndexCnt - 1 );
				/*if the currently added item index is less than or equals to
				 *marked index then only change the selection
				 */
				if( aIndexToBeSelected <= lToSelectIndex )
    				{
		    		lListBoxView->DeselectItem( lToSelectIndex );
		    		lListBoxView->SelectItemL( lToSelectIndex + 1 );
    				}
				lSelectedIndexCnt--;
    			}

			delete lLocalArray;
			lLocalArray = ( CArrayFix<TInt>* )NULL;
        	}
    	}
	}

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::HandleScreenSizeChange
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::HandleScreenSizeChange()
    {
	}
// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::HandleLmCreated
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::HandleLmCreated(TPosLmItemId aNewLmkItemId)
    {
    iNewLmkItemId = aNewLmkItemId;
    iNewListBoxItemAdded = ETrue;
    }
// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::LaunchPosSettingsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkAppLmItemSelectorImpl::LaunchPosSettingsL()
	{
	if(!iLauncher)
        		{
        		iLauncher = CLmkLocSettingsLauncher::NewL();
        		}
        	iLauncher->LaunchL();
    }

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::StoreIdTobeFocussedAfterDeletion
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::StoreIdTobeFocussedAfterDeletion( RArray<TPosLmItemId>& markedItemIds )
	{
	TInt markedCount = markedItemIds.Count();
	TInt index = iListBox->CurrentItemIndex();
	TInt cnt = LmItemListProvider().ItemCount();
	TBool bItemSelected = iListBox->View()->ItemIsSelected(index);
	iFocusItemIdAfterDeletion = KPosLmNullItemId;
	if(bItemSelected)
		{
		for( TInt i=index+1;i<cnt;i++)
			{
			if(!iListBox->View()->ItemIsSelected(i))
				{
				index = i;
				break;
				}
			}
		}
	if( index < cnt)
		{
		LmItemListProvider().GetSelectedItemId(iFocusItemIdAfterDeletion,index);
		}
}
// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::SetFocusToItemAfterDeletionOfMarkedItems
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::SetFocusToItemAfterDeletionOfMarkedItems()
	{
	//check if the focus to be moved to the item after deleting the marked item.
	if( iFocusItemIdAfterDeletion != KPosLmNullItemId)
		{
		//get the new index position of this item, after deletion.
		TInt index = LmItemListProvider().SelectedIndex(iFocusItemIdAfterDeletion);
	    if ( index != KErrNotFound &&
	         index < iListBox->Model()->NumberOfItems() )
			{
			iListBox->SetCurrentItemIndex( index );
			}
		else
			{
			iListBox->SetCurrentItemIndex( 0 );
			}
		}
	iFocusItemIdAfterDeletion = KPosLmNullItemId;
	}
// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::HandleDialogDismissed
// -----------------------------------------------------------------------------
//
void CLmkAppLmItemSelectorImpl::HandleDialogDismissed( TInt aButtonId )
	{
	if( EEikBidCancel == aButtonId )
		{
		delete iLocationService;
    	iLocationService = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::MessageQueryCallBack()
// -----------------------------------------------------------------------------
//
TInt CLmkAppLmItemSelectorImpl::MessageQueryCallBack( TAny* /*aPtr*/ )
	{
	TInt result = KErrNone;
	CEikonEnv* env = CEikonEnv::Static();
	TRAP_IGNORE(LmkNotes::GenericMessageQueryL(env,R_LMK_REQUEST_FAIL_INFO_TEXT,
									R_LMK_MSG_QUERY_HEADING_PANE_TEXT));
	return result;
	}

//  End of File
