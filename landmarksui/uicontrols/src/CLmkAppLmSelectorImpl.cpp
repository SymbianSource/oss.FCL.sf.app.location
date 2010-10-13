/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the selector implementation for landmarks by
*               name view
*
*/







// INCLUDE FILES
#include <AiwServiceHandler.h>      // AIW
#include <AiwCommon.hrh>            // AIW
#include <lbsfields.h>
#include <aknsfld.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <EPos_CPosLmCategoryCriteria.h>
#include <EPos_CPosLmCategoryManager.h>
#include <AknWaitDialog.h>
#include <lmkui.rsg>
#include <Landmarks.rsg>
#include <CLmkEditorDlg.h> // from system include!
#include "clmkdbsearchutils.h"
#include "LmkConsts.h"
#include "landmarks.hrh"
#include "CLmkDbUtils.h"
#include "CLmkFields.h"
#include "CLmkUiUtils.h"
#include "CLmkEditorImpl.h"
#include "LmkNotes.h"
#include "CLmkSender.h"
#include "LmkListProviderFactory.h"
#include "CLmkLmItemListProvider.h"
#include "CLmkDlgCategorySelectorImpl.h"
#include "CLmkLmItemListMemento.h"
#include "clmkgotourlcmd.h"
#include "CLmkAppLmSelectorImpl.h"
#include "MLmkMapAndNavigationObServer.h"
#include "CLmkAOOperation.h"
#include "MLmkMskObserver.h"
#include "CLmkLocSettingLauncher.h"
#include <lmkerrors.h>



// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
//Below three flags are used only for category remove operation.
//This one is used, when the remove operation starts
const TInt KRemoveOperationStarted(2);
//This one is used when, remove operation ends.
const TInt KRemoveOperationCompleted(4);
//This one is used, when the remove operation ends & the list box model
//also is updated, after getting an event from database.
const TInt KRremoveListBoxModUpdated(8);
#if defined(_DEBUG)
_LIT( KPanicMsg, "CLmkAppLmSelectorImpl" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace

_LIT(KWildCard, "*");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::CLmkAppLmSelectorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkAppLmSelectorImpl::CLmkAppLmSelectorImpl(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TBool aFindBox )
    : CLmkAppLmItemSelectorImpl( aDb, aSender, aFindBox ),iIsDlgOpen(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::ConstructL()
    {
    CLmkAppLmItemSelectorImpl::BaseConstructL();
    // Added for rel 3.1
    iLmkCallCmd = CLmkCallCmd::NewL();
    iDbSearchUtils = NULL;
    // launching browser
    iLmkGoToURLCmd = CLmkGoToURLCmd::NewL();
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::ConstructL(
    const MLmkListMemento& aCategoryMemento )
	{
	// Listing contents of current category is implemented using category
	// view's memento:
	const CLmkLmItemListMemento& categoryMemento =
	static_cast<const CLmkLmItemListMemento&>( aCategoryMemento );
	TPosLmItemId categoryId = categoryMemento.MemorizedItemId();

	if (categoryId == KPosLmNullItemId) // 'uncategorised' category
		{
		// set criteria for searchnig 'Uncategorised' landmarks
		iCategoryName = StringLoader::LoadL(
		    R_LMK_LM_LIST_UNCATEGORISED, const_cast<CEikonEnv *>( iEnv ));
		}
	else
		{
		// Construction must fail if category has been deleted:
		CPosLmCategoryManager* mgr = CPosLmCategoryManager::NewL( iDb );
		CleanupStack::PushL( mgr );
		// Leaves with KErrNotFound if category doesn't exist
		CPosLandmarkCategory* category = mgr->ReadCategoryLC( categoryId );
		TPtrC categoryName;
		User::LeaveIfError(category->GetCategoryName(categoryName));
		iCategoryName = categoryName.AllocL();
		CleanupStack::PopAndDestroy( 2 ); // category, mgr
		}

	iCriteria = CPosLmCategoryCriteria::NewLC();
	CleanupStack::Pop(); // iCriteria
	iCriteria->SetCategoryItemId( categoryId );
    iMarkedCatCountAdded = 0;
	// Call "normal" ConstructL():
	ConstructL();
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkAppLmSelectorImpl* CLmkAppLmSelectorImpl::NewL(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TBool aFindBox )
    {
    CLmkAppLmSelectorImpl* self =
        new( ELeave ) CLmkAppLmSelectorImpl( aDb, aSender, aFindBox );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkAppLmSelectorImpl* CLmkAppLmSelectorImpl::NewL(
    CPosLandmarkDatabase& aDb,
    const MLmkListMemento& aCategoryMemento,
    CLmkSender& aSender,
    TBool aFindBox )
    {
    CLmkAppLmSelectorImpl* self =
        new( ELeave ) CLmkAppLmSelectorImpl( aDb, aSender, aFindBox );

    CleanupStack::PushL( self );
    self->ConstructL( aCategoryMemento );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::~CLmkAppLmSelectorImpl
// -----------------------------------------------------------------------------
//
CLmkAppLmSelectorImpl::~CLmkAppLmSelectorImpl()
    {
    delete iCriteria;
    delete iCategoryName;
    delete iLmkCallCmd;
    delete iDbSearchUtils;
    delete iLmkGoToURLCmd;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::CategoryNameL
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CLmkAppLmSelectorImpl::CategoryNameL()
    {
    return *iCategoryName;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::SetupListProviderL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::SetupListProviderL()
    {
    if ( !iListProvider )
        {
        if ( iCriteria )
            {
            iListProvider =
                LmkListProviderFactory::CreateProviderL( iDb, *iCriteria );
            }
        else
            {
            iListProvider =
                LmkListProviderFactory::CreateProviderL( iDb,
                                                         ELandmarkSelector );
            }
        iListProvider->AddObserverL( *this );
        iListProvider->InitializeL();
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::ListProvider
// -----------------------------------------------------------------------------
//
const CLmkListProviderBase& CLmkAppLmSelectorImpl::ListProvider() const
    {
    __ASSERT_DEBUG( iListProvider, Panic( KLmkPanicNullMember ) );
    return *iListProvider;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::HandleCategorySearchNotifyL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::HandleCategorySearchNotifyL(RArray<TPosLmItemId> /*aArray*/)
	{
	// This search is not used
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::HandleLmkSearchNotifyL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::HandleLmkSearchNotifyL()
	{
	iWaitNote->ProcessFinishedL();
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::ProcessCommandL( TInt aCommandId )
    {
    __ASSERT_DEBUG( iListProvider, Panic( KLmkPanicNullMember ) );
    switch ( aCommandId )
        {
        case ELmkCmdEditLm:
            {
            if( iIsDlgOpen )
            	break;
            	
          	EditLandmarkCmdL(CLmkEditorDlg::ELmkEditor);
  					iSearchBox->ResetL();

            break;
            }
        case ELmkCmdOpenLm:
            {
            if( iIsDlgOpen )
            	break;

            EditLandmarkCmdL(CLmkEditorDlg::ELmkViewer);
    		iSearchBox->ResetL();

    	#ifdef RD_SCALABLE_UI_V2
    		iSearchBox->DrawDeferred();
        #endif// RD_SCALABLE_UI_V2
       		iListProvider->StartSearchingLandmarksL(KWildCard, EFalse);
            break;
            }
        case ELmkCmdGoToUrl:
            {
			HBufC* lmkUrl = HBufC::NewL( KMaxBufferLen );
			CleanupStack::PushL( lmkUrl );
			if (GetPositionFieldForSelectedLmL(
							ELmkPositionFieldWebAddress,
							*lmkUrl ) == KErrNone)
				{
				if ( lmkUrl->Length() > 0 )
					{
					// Get the URL of the LM and launch browser
					iLmkGoToURLCmd->LaunchBrowserL( *lmkUrl );
					}
				}
			CleanupStack::PopAndDestroy( lmkUrl );
			break;
            }
        case ELmkCmdCall:
	        {
			// Create AIW interest
	        ExecuteAIWCallCmdL( aCommandId );
	        break;
	        }
        case ELmkCmdDeleteLm:
            {
            DeleteLandmarksCmdL();
            break;
            }
		case ELmkCmdChangeIcon:
			{
            ChangeIconCmdL();
			break;
			}
        case ELmkCmdAddToCat:
            {
            AddToCatCmdL();
            break;
            }
        case ELmkCmdSendVia1:
        case ELmkCmdSendVia2:
        case ELmkCmdSendVia3:
        case ELmkCmdSendVia4:
        case ELmkCmdSendVia5:
        case ELmkCmdSendVia6:
        case ELmkCmdSendVia7:
        case ELmkCmdSendVia8:
        case ELmkCmdSendVia9:
        case ELmkCmdSendVia10:
        case ELmkCmdSend:
            {
            RArray<TPosLmItemId>& markedIds = MarkedItemIdsL();
            if(markedIds.Count() == 0)
            	{
            	CPosLandmark* landmark = iDb.ReadLandmarkLC(SelectedIdL());
            	iSender.SendSingleLandmarkL(*landmark);
            	CleanupStack::PopAndDestroy();//landmark
            	}
            else
            	{
            	//Send multiple landmarks
           		TPtrC ptr;
	        	RPointerArray<CLmkLandmark> lmkArray;
	        	TRAPD( sendErr, iSender.PrepareMultipleLandmarkForSendL(lmkArray,markedIds) );
	        	if(sendErr == KErrNone)
	        		{
	        		TRAP( sendErr, iSender.SendLandmarksL(aCommandId,ptr,lmkArray) );
	        		}
	        	lmkArray.ResetAndDestroy();
	        	if(sendErr == KErrNoMemory)
	        		{
	        		User::LeaveNoMemory();
	        		}
            	}
            break;
            }
        case EAknCmdMark:
        case EAknCmdUnmark:
        case EAknMarkAll:
        case EAknUnmarkAll:
	        {
	        if(iMskObserver)
				{
				iMskObserver->UpdateMskContainerL();
				}
			break;
	        }
		case ELmkCmdLaunchPositionSettings:
			{
			LaunchPosSettingsL();
			break;
			}
        case ELmkCmdFindBoxSearchAfresh:
        case ELmkCmdFindBoxSearchInPreviousResults:
        	{
			TInt searchTextLength = iSearchBox->TextLength();
		    const TInt KExtraChars = 2; // 2 chars wildcards
		    HBufC* filterBuf = HBufC::NewLC(searchTextLength + KExtraChars);
		    TPtr filter = filterBuf->Des();
		    iSearchBox->GetSearchText(filter);
	        filter.Append(KWildCard);
	        iListProvider->StartSearchingLandmarksL(filter, EFalse);
            CleanupStack::PopAndDestroy( filterBuf );

	        break;
        	}
        default:
            {
            CLmkAppLmItemSelectorImpl::ProcessCommandL( aCommandId );
            break;
            }
        }
    }

EXPORT_C void CLmkAppLmSelectorImpl::StartSearchingLandmarksL(
								const TDesC& /*aSearchPattern*/,
    							TBool /*aSearchOnlyInPreviousMatches*/)
	{
	iListProvider->InitializeL();
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::HandleOperationL(
                                       TOperationTypes aType,
                                       TReal32 /*aProgress*/,
                                       TInt aStatus )
    {
    TBool deleteNote = EFalse;
    switch (aType)
        {
         case EDeleteLandmarks:
             {
             if (aStatus != KPosLmOperationNotComplete)
                 {
                 deleteNote = ETrue;
                 }
             }
         break;
         case EAddToCategory:
             {
             if (iMarkedCatCountAdded ==1)
                 {
                 deleteNote = ETrue;
                 }
              else if (iMarkedCatCountAdded > 1)
                 {
                 --iMarkedCatCountAdded;
                 }
             }
         break;
        case ERemoveCategory:
        	{
        	if (iOperation && aStatus != KPosLmOperationNotComplete)
	        	{
		        if((iCatRemoveFlag & KRremoveListBoxModUpdated) &&
		           (iCatRemoveFlag & KRemoveOperationStarted )&& iWaitNote)
					{
					//Since the operation is completed and listbox model
					//is updated first, wait note can be closed now
					CloseWaitNoteL();
			        iListBox->SetCurrentItemIndex(0);
			        //nullify all the flags
			        iCatRemoveFlag &= ~KRemoveOperationCompleted;
			        iCatRemoveFlag &= ~KRremoveListBoxModUpdated;
			        iCatRemoveFlag &= ~KRemoveOperationStarted;
					}
				else
					{
					//operation completed but cannot close the wait dialog
					//since, the listbox model is not updated.Now , it will
					//be closed in HandleEventListReadyL function.Set the operation
					//completion flag
					iCatRemoveFlag |= KRemoveOperationCompleted;
					}
				}
    	   	}
         break;
         default:
         break;
        }

    if (deleteNote && iWaitNote)
        {
        iWaitNote->ProcessFinishedL();
        iWaitNote = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::ChangeIconL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::ChangeIconL(
    TPosLmItemId aId,
    TInt aIconIndex,
    TInt aMaskIndex)
    {
    TFileName* iconFile = CLmkUiUtils::LmkUiIconFileLC();
    iDbUtils->ChangeIconL( iDb, aId, ELmkItemTypeLandmark,
                           *iconFile, aIconIndex, aMaskIndex);
    CleanupStack::PopAndDestroy();//iconFile
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::ChangeIconsL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::ChangeIconsL(
    RArray<TPosLmItemId>& aIdArray,
    TInt aIconIndex,
    TInt aMaskIndex)
    {
    TFileName* iconFile = CLmkUiUtils::LmkUiIconFileLC();
    iDbUtils->ChangeIconsL( aIdArray, ELmkItemTypeLandmark,
                            *iconFile, aIconIndex, aMaskIndex);
	CleanupStack::PopAndDestroy();//iconFile
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::InitialLandmarkL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::InitialLandmarkL( CPosLandmark* aLandmark )
    {
    if ( iCriteria )
        {// category is a valid category in the LM database (not 'uncategorised')
        if (iCriteria->CategoryItemId() != KPosLmNullItemId)
	        {
	        aLandmark->AddCategoryL( iCriteria->CategoryItemId() );
	        }

        }
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::EditLandmarkCmdL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::EditLandmarkCmdL(TLmkEditorMode aEditMode)
    {
    TLmkEditorAttributes attributeFlags( CLmkEditorDlg::ELmkAll );
    TChar DriveName = 'c';
    TInt BytesToWrite = 0;

    DiskSpaceBelowCriticalLevelL(BytesToWrite, DriveName);
    iNewLmkItemId = SelectedIdL();
    if(iDlg)
    	{
    	iDlg = NULL;
    	}
    iDlg = CLmkEditorImpl::NewL( iDb,
                                                iSender,
                                                attributeFlags,
                                                aEditMode,
                                                iNewLmkItemId );
    iDlg->ExecuteLD();
    iDlg = NULL;

#ifdef RD_SCALABLE_UI_V2
   TInt listProviderCount = LmItemListProvider().ItemCount();
   TInt listBoxItemCount = iListBox->Model()->NumberOfItems();
   TInt currentItemIndex = LmItemListProvider().SelectedIndex(iNewLmkItemId);// iListBox->CurrentItemIndex();
   if (listProviderCount != listBoxItemCount)
       {
       iDeletionHelper.StoreListInformation( *iListBox, EFalse );
       }
   iDeletionHelper.UpdateListIfDeleteHappenedL();
   if( currentItemIndex >= 0 && currentItemIndex < iListBox->Model()->NumberOfItems() )
       {
       iListBox->SetCurrentItemIndex(currentItemIndex);
       }   
#endif // RD_SCALABLE_UI_V2
   TRAPD(err,CPosLandmark* landmark = iDb.ReadLandmarkLC(iNewLmkItemId);CleanupStack::PopAndDestroy( landmark ));
   if (err == KErrNotFound)
       {
       LmItemListProvider().RemoveItem( iNewLmkItemId );
       }      
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::GetSelectedLandmarksL
// -----------------------------------------------------------------------------
//
TInt CLmkAppLmSelectorImpl::GetSelectedLandmarksL(RPointerArray<CPosLandmark> &aArray)
	{
	if( ! iListBox->Model()->NumberOfItems() )
		{
		return KErrNotFound;
		}

	RArray<TPosLmItemId>& markedItemIds = MarkedItemIdsL();
    TInt markedCount = markedItemIds.Count();
    if ( markedCount == 0 )
    	{
    	TPosLmItemId itemId = SelectedIdL();
    	CPosLandmark *lmk = iDb.ReadLandmarkLC(itemId);
    	aArray.Append(lmk);
    	CleanupStack::Pop(); // ReadLandmarkLC
    	return KErrNone;
    	}
    else
    	{
    	if(markedCount == 1)
    		{
    		// Donot request if only one landmark
    		TPosLmItemId itemId = markedItemIds[0];
    		CPosLandmark *lmk = iDb.ReadLandmarkLC(itemId);
    		aArray.Append(lmk);
    		CleanupStack::Pop(); // ReadLandmarkLC
    		return KErrNone;
    		}
    	RArray<TPosLmItemId> itemIds;
    	for(TInt index = 0;index < markedCount; index++)
    		{
    		TPosLmItemId itemId = markedItemIds[index];
    		itemIds.Append(itemId);
    		}
    	if(iDbSearchUtils == NULL)
    		{
    		iDbSearchUtils = CLmkDbSearchUtils::NewL();
    		}
    	iDbSearchUtils->GetLandmarksL(this, itemIds ,&aArray);
    	iWaitNote = new (ELeave) CAknWaitDialog(NULL, ETrue);
		iWaitNote->ExecuteLD(R_LMKUI_PROCESSING_WAIT_NOTE);
		iWaitNote = NULL;
		return KErrNone;
    	}
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::IsLandmarkDataEmptyL
// -----------------------------------------------------------------------------
//
TBool CLmkAppLmSelectorImpl::IsLandmarkDataEmptyL(CPosLandmark* aLandmark)
	{
	// verify that destination landmark has coordinates
    TLocality loc;
    TBool isValid = (KErrNone == aLandmark->GetPosition( loc ) );
    isValid &= !Math::IsNaN( loc.Latitude() ) && !Math::IsNaN( loc.Longitude() );
    return !isValid;
    
/*
    if ( !isValid )
        {
        // verify that destination landmark has address info
        TPositionFieldId fieldId = aLandmark->FirstPositionFieldId();
        while ( fieldId != EPositionFieldNone )
            {
            if ( fieldId > EPositionFieldAddressCapabilitiesBegin &&
                 fieldId < EPositionFieldBuildingTelephone)
                {
                TPtrC field;
                aLandmark->GetPositionField( fieldId, field );
                if ( field.Length() )
                    {
                    isValid = ETrue;
                    break;
                    }
                }
            fieldId = aLandmark->NextPositionFieldId( fieldId );
            }
        }
    return !isValid;
*/
	}


// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::DeleteLandmarksCmdL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::DeleteLandmarksCmdL()
    {

    if( ! iListBox->Model()->NumberOfItems() )
    	return;

    RArray<TPosLmItemId>& markedItemIds = MarkedItemIdsL();
    TInt markedCount = markedItemIds.Count();
    if ( markedCount == 0 )
        {
        if ( LmkNotes::LandmarkConfirmationQueryL( iEnv, 1 ) )
            {
            iDeletionHelper.StoreListInformation( *iListBox, EFalse );
            TPosLmItemId selectedId = SelectedIdL();
            LmItemListProvider().RemoveItem( selectedId );
            iDbUtils->DeleteLandmarkL( selectedId, iDb );
            }

        }
    else
        {
        if ( LmkNotes::LandmarkConfirmationQueryL( iEnv, markedCount ) )
            {
			StoreIdTobeFocussedAfterDeletion( markedItemIds );
            iDeletionHelper.StoreListInformation( *iListBox, ETrue );

            if( markedCount == 1 )
            	{
            	LmItemListProvider().RemoveItem( markedItemIds[0] );
            	iDbUtils->DeleteLandmarkL( markedItemIds[0], iDb );
            	return;
            	}
			// The 10 value is set as standard to remove the multiple landmarks and new procedure for operation
            if( markedCount > 10)
            	{
            	LmItemListProvider().RemoveItemsL(markedItemIds,
            								MLmkAOOperationObserver::EDeleteLandmarks,
            								KPosLmNullItemId);
            	}
        	else
        		{
        		iDbUtils->DeleteLandmarksL( markedItemIds );

	            // wait note
	            iWaitNote = new (ELeave) CAknWaitDialog( NULL, ETrue );
	            if ( !iWaitNote->ExecuteLD( R_LMK_DELETING_WAIT_NOTE ) )
	                {
	                //making iWaitNote null, since framework destroys waitnote
	        		//dialog on pressing cancel.
	        		iWaitNote = NULL;
	                iDbUtils->CancelLmOperation();
	                }
        		}
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::AddToCatL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::AddToCatCmdL()
    {
    CLmkDlgCategorySelectorImpl* dlg =
        CLmkDlgCategorySelectorImpl::NewL( iDb, ETrue, ETrue );
    CleanupStack::PushL( dlg );
    dlg->SetEmptyDlgLabel( R_LMK_EMPTY_NO_CATEGORIES ,R_LMK_GUIDE_CREATE_LANDMARK );
    RArray<TPosLmItemId> categories;
    CleanupClosePushL( categories );
    if ( dlg->ExecuteL( categories ) )
        {
        RArray<TPosLmItemId>& markedLmIds = MarkedItemIdsL();
        TInt markedLmCount = markedLmIds.Count();
        TInt markedCatCount = categories.Count();
        iMarkedCatCountAdded = markedCatCount;
        if ( markedCatCount > 0 )
            {
            LmItemListProvider().SetOperationCmd( ELmkCmdAddToCat );
            LmItemListProvider().SetSelectedItemsCount( markedCatCount );

            if( markedLmCount == 0 )
            	markedLmIds.Append( SelectedIdL() );

            iDbUtils->AddLmsToCategoriesL( markedLmIds, categories );

            // wait note
            iWaitNote = new (ELeave) CAknWaitDialog( NULL, ETrue );
            if ( !iWaitNote->ExecuteLD( R_LMK_PROCESSING_WAIT_NOTE ) )
                {
                //making iWaitNote null, since framework destroys waitnote
        		//dialog on pressing cancel.
	        	LmItemListProvider().SetSelectedItemsCount( 0 );
        		iWaitNote = NULL;
                iDbUtils->CancelLmOperation();
                }
            iListBox->ClearSelection();
	        }

		if( iMskObserver )
			{
			iMskObserver->UpdateMskContainerL();
			}
        }
    CleanupStack::PopAndDestroy( 2 ); // categories, dlg
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::HandleEventListReadyL
// -----------------------------------------------------------------------------
//
TBool CLmkAppLmSelectorImpl::HandleEventListReadyL()
    {
    // Possibly user-initiated delete finished --> if yes, update the list:
    TBool retVal;
	if( iNewListBoxItemAdded )
		{
		CAknFilteredTextListBoxModel* model = static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );
        CAknListBoxFilterItems* filter = static_cast<CAknListBoxFilterItems*>( model->Filter() );
        filter->HandleItemArrayChangeL();

	    TInt listboxIndex = LmItemListProvider().SelectedIndex(iNewLmkItemId);

	    /* When the user tries to create a new landmark in category content view (catId = n)
	     * Chooses to deselect that category from multiple category selector
	     * The landmark gets added to either 'uncategorised' category or any other
	     * category than the current category -->n
	     */

	    if ( listboxIndex == KLmkInValidIndex )
		    {
		    retVal = EFalse;
		    return retVal;
		    }

	    if ( listboxIndex != KErrNotFound &&
	         listboxIndex < iListBox->Model()->NumberOfItems() )
	        {
			iListBox->SetCurrentItemIndex(listboxIndex);
	        }
        UpdateMarkIndexAfterAdditionL( listboxIndex );
		iListBox->DrawDeferred();
		iNewListBoxItemAdded = EFalse;
		retVal = ETrue;
		}
    else  // Either item is updated/deleted
		{
		/*Check if the database item count and list item count are same or not
		*If there is a change, update the list box
		*(The deletion of an item due to updates in landmark category info
		*via category in selection via landmark editor or via 'Addtocategory'
		*command from the Landmark App views)
		*/
		if(iDlg)
		{
		TRAP_IGNORE(iDlg->UpdateViewDlgL());
		}
		TInt listProviderCount = LmItemListProvider().ItemCount();
		TInt listBoxItemCount = iListBox->Model()->NumberOfItems();
		if (listProviderCount != listBoxItemCount)
			{
			iDeletionHelper.StoreListInformation( *iListBox, EFalse );
			}

		retVal = iDeletionHelper.UpdateListIfDeleteHappenedL();

		if( retVal)
			{
			SetFocusToItemAfterDeletionOfMarkedItems();
			}
		//
		if( (iCatRemoveFlag & KRemoveOperationStarted) )
			{
			if((iCatRemoveFlag & KRemoveOperationCompleted) && iWaitNote)
				{
				CloseWaitNoteL();
		        iListBox->SetCurrentItemIndex(0);
		        iCatRemoveFlag &= ~KRremoveListBoxModUpdated;
		        iCatRemoveFlag &= ~KRemoveOperationCompleted;
		        iCatRemoveFlag &= ~KRemoveOperationStarted;
				}
			else
				{
				//operation completed but cannot close the wait dialog
				//since, the operation is not yet completed , it will
				//be closed in HandleOperationL function.
				iCatRemoveFlag |= KRremoveListBoxModUpdated;
				}
			}
		}
    return retVal;
    }

// ----------------------------------------------------
// CLmkAppLmSelectorImpl::AttachToAIWMenuL()
// ----------------------------------------------------
//
void CLmkAppLmSelectorImpl::AttachToAIWMenuL(
		TInt aMenuResourceId,
		TInt aInterestResourceId)
	{
	iLmkCallCmd->AttachMenuPaneL( aMenuResourceId, aInterestResourceId);
	}

// ----------------------------------------------------
// CLmkAppLmSelectorImpl::InitializeMenuPaneL()
// ----------------------------------------------------
//
void CLmkAppLmSelectorImpl::InitializeMenuPaneL(
		CEikMenuPane& aMenuPane,
		TInt aMenuResourceId)
	{
	// Initialize the AIW menu service
	iLmkCallCmd->InitializeMenuPaneL(
				aMenuPane,
				aMenuResourceId,
				ELmkCmdLast // Must not overlap with the other menu ids!
				);
	}
// ----------------------------------------------------
// CLmkAppLmSelectorImpl::AttachInterestL()
// ----------------------------------------------------
//
void CLmkAppLmSelectorImpl::AttachInterestL (
		TInt aInterestResourceId)
	{
	iLmkCallCmd->AttachInterestL(aInterestResourceId);
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::ExecuteAIWCallCmdL()
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::ExecuteAIWCallCmdL(
		 TInt aCommandId )
	{
	HBufC* lmkPhoneNum = HBufC:: NewL( KMaxPhoneNumberFieldLen );
	CleanupStack::PushL(lmkPhoneNum);

	if ( (GetPositionFieldForSelectedLmL(
							ELmkPositionFieldPhoneNumber,
							*lmkPhoneNum ) == KErrNone) && (lmkPhoneNum->Des().Length() > 0) )
		{
		iLmkCallCmd->ExecuteAIWCallCmdL( aCommandId, *lmkPhoneNum );
		}
	else
		{
		// Info note here with landmark name
		TPosLmItemId lmId = SelectedIdL();
		CPosLandmark* landmark = iDb.ReadLandmarkLC(lmId);
		// Get name
		TPtrC lmkNamePtr;
		if ( landmark->GetLandmarkName ( lmkNamePtr ) == KErrNone )
			{
			LmkNotes::InformationNotewithTextL( iEnv,
			                 R_LMK_NOTE_NO_NUMBER_TO_NAME, lmkNamePtr);
			}
		CleanupStack::PopAndDestroy( landmark );
		}
	CleanupStack::PopAndDestroy(lmkPhoneNum);
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::GetPositionFieldForSelectedLmL()
// -----------------------------------------------------------------------------
//
TInt CLmkAppLmSelectorImpl::GetPositionFieldForSelectedLmL(
	                        TPositionFieldId  aFieldId,
	                        HBufC& aFieldValue )
	{
	TPosLmItemId lmId = SelectedIdL();
	TPtrC tmpPtr;

	CPosLandmark* landmark = iDb.ReadLandmarkLC(lmId);
	TInt retVal (-1);
	if ( landmark->IsPositionFieldAvailable( aFieldId ) )
		{
		retVal = landmark->GetPositionField( aFieldId, tmpPtr );
		}
	if ( aFieldValue.Des().MaxLength() >= tmpPtr.Length() )
    	{
        aFieldValue.Des().Copy( tmpPtr );
    	}
	CleanupStack::PopAndDestroy( landmark );
	return retVal;
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::IsThisLandmarkFieldDefined
// -----------------------------------------------------------------------------
//
TBool CLmkAppLmSelectorImpl::IsLmWithThisFieldExistL ( TLmkAppCmdId aCmdId )
    {

    TBool lmkFieldExist = EFalse;
    TPositionFieldId lmkPostionField (0);

    if ( aCmdId == ELmkCmdCall )
        {
        lmkPostionField = ELmkPositionFieldPhoneNumber;
        }
    if ( aCmdId == ELmkCmdGoToUrl )
        {
        lmkPostionField = ELmkPositionFieldWebAddress;
        }

    HBufC* lmkField =  HBufC::NewL( KMaxBufferLen );
    CleanupStack::PushL( lmkField );

    if ( (GetPositionFieldForSelectedLmL(
							lmkPostionField,
							*lmkField ) == KErrNone) &&
							(lmkField->Des().Length() > 0) )
        {
        lmkFieldExist = ETrue;
        }
    CleanupStack::PopAndDestroy( lmkField );
    return lmkFieldExist;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::HandleLmCreated
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::HandleLmCreated(TPosLmItemId aNewLmkItemId)
    {
    iNewLmkItemId = aNewLmkItemId;
    iNewListBoxItemAdded = ETrue;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::RemoveLandmarksFromCategory
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkAppLmSelectorImpl::RemoveLandmarksFromCategoryL()
	{
	if(iCriteria)
		{
		TPosLmItemId catId = iCriteria->CategoryItemId();
		RArray<TPosLmItemId>& itemArray = MarkedItemIdsL();
        LmItemListProvider().SetOperationCmd( ERemoveFromCat );
		if (itemArray.Count() == 0)
			{
			TPosLmItemId id = SelectedIdL();
			itemArray.AppendL(id);
			}
			// The 10 value is set as standard to remove the multiple landmarks and new procedure for operation
		if( itemArray.Count() > 10)
			{
        	LmItemListProvider().RemoveItemsL(itemArray,
        								MLmkAOOperationObserver::ERemoveCategory,
        								catId);
			return;
			}
		else
			{
			CPosLmCategoryManager* mgr = CPosLmCategoryManager::NewL( iDb );
			CleanupStack::PushL(mgr);
			CPosLmOperation* operation = mgr->RemoveCategoryFromLandmarksL(catId,itemArray);
			CleanupStack::PopAndDestroy();//mgr
			if (iOperation)
				{
				delete iOperation;
				iOperation = NULL;
				}
			iOperation = CLmkAOOperation::NewL(operation,
								*this,MLmkAOOperationObserver::ERemoveCategory,ETrue);
			iOperation->StartOperation();
			}
		}
	iCatRemoveFlag |= KRemoveOperationStarted;
	iWaitNote = new (ELeave) CAknWaitDialog(NULL, ETrue);
	if(!iWaitNote->ExecuteLD(R_LMK_PROCESSING_WAIT_NOTE))
		{
		if (iOperation)
			{
			delete iOperation;
			iOperation = NULL;
			}
		iWaitNote = NULL;
		}
	iCatRemoveFlag &= ~KRemoveOperationStarted;
	}


// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::IsCriteriaUnCategorizedCat
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CLmkAppLmSelectorImpl::IsCriteriaUnCategorizedCat()
	{
	TBool result = EFalse;
	if (iCriteria && iCriteria->CategoryItemId() == KPosLmNullItemId)
		{
		result = ETrue;
		}
	return result;
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::ServiceCmdByMenuCmd
// -----------------------------------------------------------------------------
//
TInt CLmkAppLmSelectorImpl::ServiceCmdByMenuCmd( TInt aMenuCmd )
	{
	return iLmkCallCmd->ServiceCmdByMenuCmd( aMenuCmd );
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::CloseWaitNoteL
// -----------------------------------------------------------------------------
//
void CLmkAppLmSelectorImpl::CloseWaitNoteL()
	{
	iWaitNote->ProcessFinishedL();
    iWaitNote = NULL;
	if (iOperation)
		{
		delete iOperation;
		iOperation = NULL;
		}
	}

//  End of File

