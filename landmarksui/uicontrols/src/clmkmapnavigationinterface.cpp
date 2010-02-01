/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Applications interface class to map and navigation use cases
*
*/


// INCLUDE FILES
#include <AiwGenericParam.h>
#include <epos_poslandmarkserialization.h>
#include <lbsposition.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <lmkui.rsg>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmOperation.h>
#include <EPos_CPosLandmark.h>
#include <EPos_Landmarks.h>
#include <e32math.h>
#include <AiwServiceHandler.h>
#include <AknQueryDialog.h>
#include <mnaiwservices.h>
#include <AknWaitDialog.h>
#include "clmkdbsearchutils.h"
#include "CLmkSender.h"
#include "CLmkEditorImpl.h"
#include "LmkNotes.h"
#include "clmkmapnavigationinterface.h"

// ================= LOCAL FUNCTIONS =======================
static void CleanupArray(TAny* aArray)
    {
    ( reinterpret_cast<RPointerArray<CPosLandmark>*>( aArray ) )->ResetAndDestroy();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::CLmkMapNavigationInterface
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkMapNavigationInterface::CLmkMapNavigationInterface( CPosLandmarkDatabase& aDb )
	: iDb(aDb)
	{
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkMapNavigationInterface::ConstructL()
	{
	iAiwServiceHandler = CAiwServiceHandler::NewL();
	iInList = CAiwGenericParamList::NewL();
    iOutList = CAiwGenericParamList::NewL();
	iSearchUtil = NULL;
	iCategoryName = NULL;
	iCategoryId = KPosLmNullItemId;
	iEnv = CEikonEnv::Static();
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::NewL
// Symbian 1st phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkMapNavigationInterface* CLmkMapNavigationInterface::NewL( CPosLandmarkDatabase& aDb)
	{
	CLmkMapNavigationInterface* self = static_cast<CLmkMapNavigationInterface*>(Dll::Tls());
	if(!self)
		{
		self = new (ELeave) CLmkMapNavigationInterface( aDb );
		CleanupStack::PushL( self );
		self->ConstructL();
		CleanupStack::Pop();//self
		Dll::SetTls(self);
		}
	self->iRefCnt++;
	return self;
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::~CLmkMapNavigationInterface
// C++ Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkMapNavigationInterface::~CLmkMapNavigationInterface()
	{
	Dll::SetTls( NULL );
	delete iAiwServiceHandler;
    delete iInList;
    delete iOutList;
    delete iCategoryName;
    delete iSearchUtil;
    iLandmarkArray.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::HandleNotifyL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkMapNavigationInterface::HandleNotifyL( TInt /*aCmdId*/,
														TInt /*aEventId*/,
													    CAiwGenericParamList& /*aEventParamList*/,
													   	const CAiwGenericParamList& /*aInParamList*/ )
	{
	if ( iOutList->Count() )
		{
		const TAiwGenericParam& param = ( *iOutList )[0];
        if ( EGenericParamLandmark == param.SemanticId() )
            {
            TPtrC8 landmarkData( param.Value().AsData() );
            CPosLandmark* lm = PosLandmarkSerialization::UnpackL( landmarkData );
            CleanupStack::PushL( lm );
            SaveLandmarkL( *lm );
            CleanupStack::PopAndDestroy( lm );
            }
        iOutList->Reset();
		}
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::DialogDismissedL
// -----------------------------------------------------------------------------
//
void CLmkMapNavigationInterface::DialogDismissedL( TInt /*aButtonId*/ )
	{
	CleanupStack::PushL( TCleanupItem( CleanupArray, &iLandmarkArray ) );
	ShowLandmarksOnMapL( iLandmarkArray, iCategoryCommand, iViewType );
	iLandmarkArray.ResetAndDestroy();
	CleanupStack::PopAndDestroy(); //iLandmarkArray
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::SaveLandmarkL
// -----------------------------------------------------------------------------
//
void CLmkMapNavigationInterface::SaveLandmarkL(CPosLandmark& aLandmark)
	{
	CPosLandmarkDatabase* db = CPosLandmarkDatabase::OpenL();
	CleanupStack::PushL( db );
	CLmkSender* sender = CLmkSender::NewL( *db );
	CleanupStack::PushL( sender );
	CPosLmOperation* operation = db->InitializeL();
	CleanupStack::PushL( operation );
	operation->ExecuteL();

	if( iCategoryId != KPosLmNullItemId )
		{
		aLandmark.AddCategoryL( iCategoryId );
		}

	CLmkEditorImpl *dlg = CLmkEditorImpl::NewL( *db,
											   *sender,
											   CLmkEditorDlg::ELmkAll,
											   CLmkEditorDlg::ELmkEditor,
                                               &aLandmark );
	if( dlg->ExecuteLD() )
	{
	if ( iObserver )
    	{
    	iObserver->HandleLmCreated(aLandmark.LandmarkId());
    	}
	}
	CleanupStack::PopAndDestroy(3);	//sender, operation and db
	iCategoryId = KPosLmNullItemId;
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::ShowLandmarksOnMapL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::ShowLandmarksOnMapL(RPointerArray<CPosLandmark> &aArray,
															  TInt aCommand,
															  EViewType aViewType)
	{
	CPosLandmark* lm;
	HBufC8* lmBuf;
	iViewType = aViewType;
	// Error checking
	if(!CheckAndDisplayIfLandmarksEmptyL(aArray))
		{
		return;
		}

	iInList->Reset();
	// Pack the landmark into generic parameter
	for(TInt index = 0; index < aArray.Count(); ++index)
		{
		lm = aArray[index];
		lmBuf =PosLandmarkSerialization::PackL( *lm );
		CleanupStack::PushL( lmBuf );
		TAiwGenericParam param( EGenericParamLandmark, TAiwVariant( *lmBuf ) );
    	iInList->AppendL( param );
    	TMnAiwCommonOptionsParam options;
		options.iRunChained = EFalse;
		TPckg<TMnAiwCommonOptionsParam> optionsPack( options );
		TAiwGenericParam optParam( EGenericParamMnCommonOptions, TAiwVariant( optionsPack ) );
		iInList->AppendL( optParam );
    	CleanupStack::PopAndDestroy( lmBuf );
		}
	// Send service command for "Show Landmark on Map"
	iAiwServiceHandler->ExecuteMenuCmdL( aCommand, *iInList, *iOutList, 0, NULL );
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::ShowCategoryOnMapL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::ShowCategoryOnMapL( TPosLmItemId aCategoryId,
															  TInt aCommand,
															  EViewType aViewType )
	{
	iViewType = aViewType;
	iCategoryCommand = aCommand;
	// This will get deleted in the callback
	if(iSearchUtil == NULL)
		{
		iSearchUtil = CLmkDbSearchUtils::NewL();
		}
	if(iCategoryName != NULL)
		{
		delete iCategoryName;
		iCategoryName = NULL;
		}
	iSearchUtil->GetLandmarksUnderCategoryL( this, aCategoryId );
	if( aCategoryId == KPosLmNullItemId )
		{
		iCategoryName = StringLoader::LoadL( R_LMK_LM_LIST_UNCATEGORISED, iEnv );
		}
	else
		{
		iCategoryName = iSearchUtil->GetCategoryNameL( aCategoryId );
		}
	iWaitNote = new (ELeave) CAknWaitDialog( NULL, ETrue );
	iWaitNote->SetCallback( this );
	iWaitNote->ExecuteLD( R_LMKUI_PROCESSING_WAIT_NOTE );
	iWaitNote = NULL;
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::NavigateToLandmarkL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::NavigateToLandmarkL( CPosLandmark* aLandmark, TInt aCommand )
	{
	HBufC8* lmBuf = PosLandmarkSerialization::PackL( *aLandmark );
    CleanupStack::PushL( lmBuf );
    TAiwGenericParam param( EGenericParamLandmark, TAiwVariant( *lmBuf ) );
    iInList->Reset();
    iInList->AppendL( param );
    TMnAiwCommonOptionsParam options;
	options.iRunChained = EFalse;
	TPckg<TMnAiwCommonOptionsParam> optionsPack( options );
	TAiwGenericParam optParam( EGenericParamMnCommonOptions, TAiwVariant( optionsPack ) );
	iInList->AppendL( optParam );
    CleanupStack::PopAndDestroy( lmBuf );
    iAiwServiceHandler->ExecuteMenuCmdL( aCommand, *iInList, *iOutList, 0, NULL );
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::GetLandmarkFromMapL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::GetLandmarkFromMapL( TInt aCommand )
	{
	// This string will be sent to the provider to display it in the map view
    HBufC* buf = StringLoader::LoadLC( R_LM_SELECT_FROM_MAP, iEnv );
	TAiwGenericParam param( EGenericParamRequestText, TAiwVariant( *buf ) );
    iInList->Reset();
    iInList->AppendL( param );
    iOutList->Reset();
	iAiwServiceHandler->ExecuteMenuCmdL( aCommand, *iInList, *iOutList, 0, this );
	CleanupStack::PopAndDestroy(); // buf
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::GetLandmarkFromMapForCategoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::GetLandmarkFromMapForCategoryL( TInt aCommand, TPosLmItemId aCategoryId )
	{
	iCategoryId = aCategoryId;
	GetLandmarkFromMapL( aCommand );
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::GetServiceCmdByMenuCmd
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkMapNavigationInterface::GetServiceCmdByMenuCmd( TInt aCommand )
	{
	return iAiwServiceHandler->ServiceCmdByMenuCmd( aCommand );
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::AttachAIWInterestL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::AttachAIWInterestL(TInt aMenuPane, TInt aInterest)
	{
	iAiwServiceHandler->AttachMenuL(aMenuPane, aInterest);
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::AttachAIWInterestL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CLmkMapNavigationInterface::HandleMenuOperationL( TInt aResourceId,
															     CEikMenuPane *aMenuPane,
															     TInt aBaseCommand )
	{
	if(iAiwServiceHandler->HandleSubmenuL( *aMenuPane ))
		{
		return ETrue;
		}

	// Checks whether it is aiw menu or not
	if ( iAiwServiceHandler->IsAiwMenu( aResourceId ) )
		{
		CAiwGenericParamList* params = CAiwGenericParamList::NewLC();

		// Adds the aiw plugin menus
		TInt err;
		TRAP( err, iAiwServiceHandler->InitializeMenuPaneL(
              *aMenuPane, aResourceId, aBaseCommand, *params ) );
        CleanupStack::PopAndDestroy(); // params
        return ETrue;
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::AttachMenuPaneL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::AttachMenuPaneL( CEikMenuPane *aMenuPane, TInt aResourceId, TInt aBaseCommand )
	{
	if(iAiwServiceHandler->IsAiwMenu( aResourceId ))
		{
		CAiwGenericParamList* params = CAiwGenericParamList::NewLC();
		TInt err = KErrNone;
        TRAP( err, iAiwServiceHandler->InitializeMenuPaneL(
                   *aMenuPane, aResourceId, aBaseCommand, *params ) );
        CleanupStack::PopAndDestroy( params );
		}
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::DetachInterestL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::DetachInterestL( TInt aMenuResourceId, TInt aInterestresourceId )
	{
	iAiwServiceHandler->DetachMenu( aMenuResourceId, aInterestresourceId );
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::HandleCategorySearchNotifyL
// -----------------------------------------------------------------------------
//
void CLmkMapNavigationInterface::HandleCategorySearchNotifyL( RArray<TPosLmItemId> aArray )
	{
	iLandmarkArray.ResetAndDestroy();
	for(TInt i = 0; i < aArray.Count(); ++i)
		{
		CPosLandmark* lmk = iSearchUtil->GetLandmarkForIdL( aArray[i] );
		iLandmarkArray.Append( lmk );
		}
	if(iWaitNote)
		{
		iWaitNote->ProcessFinishedL();
		}
	delete iSearchUtil;
	iSearchUtil = NULL;
	}
// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::HandleLmkSearchNotifyL
// -----------------------------------------------------------------------------
//
void CLmkMapNavigationInterface::HandleLmkSearchNotifyL()
	{
	// Do nothing
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::CheckAndDisplayIfLandmarksEmptyL
// -----------------------------------------------------------------------------
//
TBool CLmkMapNavigationInterface::CheckAndDisplayIfLandmarksEmptyL( RPointerArray<CPosLandmark> &aArray )
	{
	//Error cases when Landmark(s) is/are empty
	enum ErrorCases
		{
		EErrAllEmpty,        // All selected landmarks are empty
		EErrSomeEmpty,       // Some selected landmarks are empty
		EErrOneEmpty,        // Only one of the selected landmark is empty
		EErrOnlyOneAndEmpty, // Only one landmark is selected and that is empty
		EErrNone 			 // All landmarks have valid coordinate data
		};

	TInt count = aArray.Count();
	TInt err = EErrNone;
	CPosLandmark* lmk;
	TInt numberOfEmptyLmk = 0;
	TInt emptyIndex = 0;

	for(TInt index = 0; index < count; ++index)
		{
		lmk = aArray[index];
		if( IsLandmarkDataEmptyL(*lmk) )
			{
			err = EErrSomeEmpty;
			++numberOfEmptyLmk;
			emptyIndex = index;
			}
		}

	if(numberOfEmptyLmk == count)
		{
		/*
		 * When view type is by lmk view then we make the check for
		 * only one landmark.
		 */
		if( count == 1 && (EByLmkView == iViewType) )
			{
			err = EErrOnlyOneAndEmpty;
			}
		else
			{
			err = EErrAllEmpty;
			}
		}
	else if(err == EErrSomeEmpty && numberOfEmptyLmk == 1)
		{
		err = EErrOneEmpty;
		}

	switch(err)
		{
		case EErrAllEmpty:
			{
			if(EByLmkView == iViewType)
				{
				LmkNotes::AllLandmarksEmptyNoteL( iEnv );
				}
			else
				{
				LmkNotes::CategoryEmptyNoteL( iEnv, iCategoryName->Des());
				}
			return EFalse;
			}
		case EErrSomeEmpty:
			{
    		HBufC* noteText = NULL;

    		if(EByLmkView == iViewType)
    			{
    			noteText = StringLoader::LoadLC( R_LM_SHOW_QUERY_NO_COORDINATES_SEVERAL,
                                  	         	 numberOfEmptyLmk,
                                  	         	 iEnv );
    			}
    		else
				{
				CPtrC16Array* array = new (ELeave) CPtrC16Array(2);
        		array->Reset();
    			CleanupStack::PushL( array );
    			HBufC* ptr = iCategoryName->AllocLC();
    			array->AppendL( *ptr );

        		CArrayFix<TInt>* arrFix = new( ELeave ) CArrayFixFlat<TInt>(2);
        		CleanupStack::PushL( arrFix );
        		arrFix->AppendL(numberOfEmptyLmk);
				noteText = StringLoader::LoadLC( R_LM_SHOW_QUERY_CATEGORY_NO_COORDINATES_SEVERAL,
												 *array,
                                  	         	 *arrFix,
                                  	         	 iEnv );
                CleanupStack::Pop();// noteText
                CleanupStack::PopAndDestroy( 3 ); //ptr, array
                CleanupStack::PushL( noteText );
				}

			CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
			TInt retVal = dlg->ExecuteLD( R_EMPTY_LANDMARKS_DIALOG, *noteText );
			CleanupStack::PopAndDestroy( noteText );
			if(!retVal)
				{
				return EFalse;
				}
			break;
			}
		case EErrOneEmpty:
			{
    		lmk = aArray[emptyIndex];
    		TPtrC landmarkname;
    		lmk->GetLandmarkName( landmarkname );
    		HBufC* noteText = NULL;

    		if(EByLmkView == iViewType)
    			{
    			noteText = StringLoader::LoadLC( R_LM_SHOW_QUERY_NO_COORDINATES_SINGLE,
                                  	         	 landmarkname,
                                  	         	 iEnv );
    			}
    		else
				{
				CPtrC16Array* array = new (ELeave) CPtrC16Array(2);
				array->Reset();
				CleanupStack::PushL( array );
				HBufC* ptr = landmarkname.AllocLC();
				array->AppendL( *ptr );
				array->AppendL( *iCategoryName );

			    noteText = StringLoader::LoadLC( R_LM_SHOW_QUERY_CATEGORY_NO_COORDINATES_SINGLE,
												 *array,
			    								 iEnv );
				CleanupStack::Pop();// noteText
				CleanupStack::PopAndDestroy( 2 ); //ptr, array
				CleanupStack::PushL( noteText );
				}

    		CAknQueryDialog* dlg = CAknQueryDialog::NewL( CAknQueryDialog::ENoTone );
    		TInt retVal = dlg->ExecuteLD( R_EMPTY_LANDMARK_DIALOG, *noteText );
    		CleanupStack::PopAndDestroy( noteText ); //array, lmk, category and noteText
			if(!retVal)
				{
				return EFalse;
				}
			break;
			}
		case EErrOnlyOneAndEmpty:
			{
			lmk = aArray[emptyIndex];
    		TPtrC landmarkname;
    		lmk->GetLandmarkName( landmarkname );
			if( EByLmkView == iViewType )
				{
				// This error case happens only when in by Landmark view.
				LmkNotes::LandmarksEmptyNoteL( iEnv, landmarkname );
				}
			else
				{

				}
			return EFalse;
			}
		default:
			{
			break;
			}
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::IsLandmarkDataEmptyL
// -----------------------------------------------------------------------------
//
TBool CLmkMapNavigationInterface::IsLandmarkDataEmptyL( CPosLandmark& aLandmark )
	{
    // verify that destination landmark has coordinates
    TLocality loc;
    TBool isValid = (KErrNone == aLandmark.GetPosition( loc ) );
    isValid &= !Math::IsNaN( loc.Latitude() ) && !Math::IsNaN( loc.Longitude() );
    return isValid;
    
/*    if ( !isValid )
        {
        // verify that destination landmark has address info
        TPositionFieldId fieldId = aLandmark.FirstPositionFieldId();
        while ( fieldId != EPositionFieldNone )
            {
            if ( fieldId > EPositionFieldAddressCapabilitiesBegin &&
                 fieldId < EPositionFieldBuildingTelephone )
                {
                TPtrC field;
                aLandmark.GetPositionField( fieldId, field );
                if ( field.Length() )
                    {
                    isValid = ETrue;
                    break;
                    }
                }
            fieldId = aLandmark.NextPositionFieldId( fieldId );
            }
        }
    return !isValid;*/
	}

// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::SetObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::SetObserver(MLmkMapAndNavigationObserver* aObserver)
    {
    iObserver =   aObserver;
    }
// -----------------------------------------------------------------------------
// CLmkMapNavigationInterface::Release
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkMapNavigationInterface::Release()
	{
	iRefCnt--;
	if( iRefCnt == 0)
		{
		delete this;
		}
	}
// End of file
