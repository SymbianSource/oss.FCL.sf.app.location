/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides Blid main view class methods.
*
*/

// INCLUDE FILES
#include <aknlists.h>
#include <AknQueryDialog.h> 
#include <StringLoader.h>
#include <eikmenub.h> // CEikMenuBar
#include <blid.rsg>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmItemIterator.h>
#include <e32std.h>
#include <CLmkEditorDlg.h>
#include <TLmkItemIdDbCombiInfo.h>
#include <alf/alfimageloaderutil.h>

#include "CBlidBaseView.h"
#include "CBlidAppUi.h"
#include "CBlidBaseContainer.h"
#include "MBlidRouter.h"
#include "MBlidSettings.h"
#include "MBlidLocation.h"
#include "CBlidEng.h"
#include "CBlidDocument.h"
#include "Blid.hrh"
#include "bliduiconsts.h"
#include "CBlidCurrentPositionDlg.h"

#include <locationvariationdomaincrkeys.h>
#include <centralrepository.h>

enum TPanicCode
    {
    KBlidNullPointer = 1
    };

// Start with the assumption that PSY is not connected
TInt CBlidBaseView::iPSYTimeoutCount = 0;
TBool CBlidBaseView::iOnline = EFalse;

const TInt KMiddleSKId = 3;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBlidBaseView::CBlidBaseView
// First phase constructor, should not leave
// ---------------------------------------------------------
//
CBlidBaseView::CBlidBaseView( MKeyProcessor& aKeyProcessor )
        : iKeyProcessor(aKeyProcessor)
    {
    StringLoader::Load( iManualWaypointName, R_BLID_DESTINATION_COORDINATES, iCoeEnv );
    }

// ---------------------------------------------------------
// CBlidBaseView::BaseConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CBlidBaseView::BaseConstructL( TInt aResId )
    {
    FeatureManager::InitializeLibL();
    CAknView::BaseConstructL( aResId ); 

    iEngine = static_cast<CBlidDocument*>(AppUi()->Document())->Engine();
    
    if( !iEngine->LocationModel() )
	    {
    	iEngine->CreateModelL( CBlidEng::EBlidLocation );	    	
	    }
    iLocation = iEngine->LocationModel();
    __ASSERT_DEBUG( iLocation, User::Panic(_L("CBlidOwnPosView"),
        KBlidNullPointer));

    iRouter = iEngine->RouterModel(); 
    
    iImageLoaderUtil = new (ELeave) CAlfImageLoaderUtil();
    
	iConFileName.Zero();
	iConFileName.Append( _L("z") );
	iConFileName.Append( KBlidSystemIconPath );

	iIsSettingsViewActive = EFalse;   
    
    __ASSERT_DEBUG( iRouter, User::Panic(_L("CBlidOwnPosView"),
        KBlidNullPointer));
    }

// ---------------------------------------------------------
// CBlidBaseView::~CLocBaseView()
// Destructor, frees allocated resources
// ---------------------------------------------------------
//
CBlidBaseView::~CBlidBaseView()
    {
    FeatureManager::UnInitializeLib();
    delete iImageLoaderUtil;
    }

// ---------------------------------------------------------
// CBlidBaseView::ProcessKeyEventL
// Processes the key event
// ---------------------------------------------------------
//
TBool CBlidBaseView::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                      TEventCode aType )
    {
    return iKeyProcessor.ProcessKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CBlidBaseView::TabObserver()
// Returns the tab observer
// ---------------------------------------------------------
//
MAknTabObserver* CBlidBaseView::TabObserver()
    {
    return iKeyProcessor.TabObserver();
    }

// ---------------------------------------------------------
// CBlidBaseView::DynInitMenuPaneL
// Called to initialize the menu pane at run time
// ---------------------------------------------------------
//
void CBlidBaseView::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    #ifdef _DEBUG
    RDebug::Print(_L("CBlidCompassView::DynInitMenuPaneL() called"));
    #endif
    switch( aResourceId )
        { 
        case R_BLID_VIEW_MENU:
        {
            if ( !(iLocation->IsGPSDataAvailable() && 
                   iLocation->SatelliteCapability()) )
                {                
                aMenuPane->SetItemDimmed( EBlidCmdSatelliteStatus, ETrue );                
                }
             if ( (!iRouter->Count()) || FeatureManager::FeatureSupported( KFeatureIdLandmarks ) )
                {
                aMenuPane->SetItemDimmed( EBlidCmdSavedWaypoints, ETrue );
                }
                             
            // hide the save position.
            if(!iLocation->IsOnline())
                {
                aMenuPane->SetItemDimmed( EBlidCmdCurrentLocation, ETrue );
                }
            
                
            
        }
                //-fallthrough
        case R_BLID_VIEW_OK_MENU:        
            {      
            if ( !iRouter->IsAnyPointSet() )
                {
                aMenuPane->SetItemDimmed( EBlidCmdStopNavigation, ETrue );
                }                
            break;
            }
        case R_BLID_MAIN_MENU:
            {
            CBlidBaseView::HandleHelpFeatureL( aMenuPane );
            break;
            }
            
        case R_BLID_SET_DESTINATION_MENU:
			{
			if ( iRouter->Count() <= 0 )
				{
				aMenuPane->SetItemDimmed( EBlidCmdWaypoint, ETrue );
				}                
			else
				{
				aMenuPane->SetItemDimmed( EBlidCmdWaypoint, EFalse );	
				}
				
			// Features manager checks for waypoint/landmarks variation
			if ( FeatureManager::FeatureSupported( KFeatureIdLandmarks ) )
				{
				aMenuPane->SetItemDimmed( EBlidCmdWaypoint, ETrue );				
				}
			else
				{
				aMenuPane->SetItemDimmed( EBlidCmdLmk, ETrue );
				}
			
			if ( FeatureManager::FeatureSupported( KFeatureIdLandmarks ) )
			    {
    			if( IsLmAvailableL() )
    				{
    				aMenuPane->SetItemDimmed( EBlidCmdLmk, EFalse );
    				}
    			else
    				{
    				aMenuPane->SetItemDimmed( EBlidCmdLmk, ETrue );
    				}		
    			}
    			break;
    	
			} 
        case R_BLID_SAVE_CURRENT_LOCATION_MENU:
        {
        	TInt settingsValue = 0;
            CRepository* repository = CRepository::NewL( TUid::Uid( KCRUidLocLocalVariation) );
            CleanupStack::PushL( repository );
            User::LeaveIfError( repository->Get( KLocHideCoordinates,
                                                      settingsValue ));  
            CleanupStack::PopAndDestroy( repository );
            
            if ( ELocCoordinatesHidden == settingsValue )
            {
            	 aMenuPane->SetItemDimmed( EBlidCmdShowDetails, ETrue );
            }
            else
            {
             	 aMenuPane->SetItemDimmed( EBlidCmdShowDetails, EFalse );
            }
            
            break;
        } 
        default:
            {
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CBlidBaseView::NotifyL
// Called by location model to notify about updates in location
// ----------------------------------------------------------------------------
//
void CBlidBaseView::NotifyL( const TBool /*aOnlineMode*/ )
    {  
    if( iPopupDialog )
	    {
		iPopupDialog->ChangeDlgSoftKey();	    	
	    }      
    // GPS available    
    CBlidBaseContainer::SetGPSAvailability(ETrue);
    }

// ----------------------------------------------------------------------------
// CBlidBaseView::NotifyError
// Called by location model to notify about error in location
// ----------------------------------------------------------------------------
//
void CBlidBaseView::NotifyErrorL( TInt /*aErrorCode*/ )
    {
    if( iPopupDialog )
	    {
		iPopupDialog->ChangeDlgSoftKey();	    	
	    }
    }

// ---------------------------------------------------------
// CBlidBaseView::HandleHelpFeatureL
// Handles the Help option command
// ---------------------------------------------------------
//
void CBlidBaseView::HandleHelpFeatureL( CEikMenuPane* aMenuPane ) const
    {
    //implementation for req 406-917. Hide position view based on feature variation flag
    TInt helpSettingValue = ELocHelpNotHidden;
    CRepository* repository = CRepository::NewL( TUid::Uid( KCRUidLocLocalVariation) );
    CleanupStack::PushL( repository );
     User::LeaveIfError( repository->Get( KLocHideHelp,
                                              helpSettingValue ));  
    CleanupStack::PopAndDestroy( repository );
	
	  
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) && !helpSettingValue)
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp, EFalse );
        }
    else
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
        }
    }

// ----------------------------------------------------------------------------
// CBlidBaseView::InsertWaypointL
// Insers a waypoint
// ----------------------------------------------------------------------------
//
TBool CBlidBaseView::InsertWaypointL()
    {
    TBuf<KBlidWaypointNameMaxLen> textData;
    StringLoader::Load( textData, R_BLID_WAYPOINT_NAME, iCoeEnv );
    TInt errorCode(KErrNone);	
    TBool returnValue(ETrue);
    TInt removedIndex;
    
    do
        {
        if(errorCode != KErrNone)
        	{
        	return EFalse;
        	}
        	
        CAknTextQueryDialog* dlg = new(ELeave) CAknTextQueryDialog(textData,
                                                    CAknQueryDialog::ENoTone);
        
        if (!dlg->ExecuteLD(R_BLID_WAYPOINT_QUERY))
            {
            return EFalse;           
            }
            
        }while( CheckIfExistL( textData, errorCode, removedIndex ) );
        
		if(errorCode != KErrNone)
				{
				return EFalse;
				}

		/// if KMaxNumberOfWaypoints or more than that is already present
		/// display note.
		if ( iRouter->Count() >= KMaxNumberOfWaypoints )
		    {
		    static_cast<CBlidAppUi*>(AppUi()
		        )->ShowDialogL( BlidNotes::EBlidOverWriteWaypointNote );            
		    return EFalse;            
		    }    

		TNamedCoordinate* waypoint = new(ELeave)TNamedCoordinate();
		CleanupStack::PushL( waypoint );
		waypoint->SetName( textData );
		TPosition position = iLocation->GetCurrentPosition();
		waypoint->SetCoordinate( position.Latitude(),
		                         position.Longitude(),
		                         position.Altitude() );
		position.SetHorizontalAccuracy(position.HorizontalAccuracy());
		waypoint->SetAccuracy(position.HorizontalAccuracy());
		//iRouter takes waypoint's ownership
		TInt retVal = iRouter->AppendL( waypoint, ETrue);
		if(retVal == KErrDiskFull)
				{
				delete waypoint;
				BlidNotes::OutOfMemoryNoteL();
				returnValue = EFalse;
				}
		CleanupStack::Pop(); //waypoint

		return returnValue;
		}

// ----------------------------------------------------------------------------
// CBlidBaseView::SelectWaypoint
// Select a waypoint
// ----------------------------------------------------------------------------
//
TInt CBlidBaseView::SelectWaypointL()
    {
    TInt index(KErrCancel);   
    TInt currentIndex(0);     
    CEikFormattedCellListBox* listBox = 
        new(ELeave)CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( listBox );    
    
    HBufC* name = NULL;
    TInt count( iRouter->Count() );
    TNamedCoordinate* waypoint = iRouter->Waypoint();

    CDesCArray* array = new(ELeave)CDesCArrayFlat(KMaxNumberOfWaypoints);
    CleanupStack::PushL( array );

    TInt waypointAdditionCount = 0;
	// fill up the array with waypoint's names from the engine.
	for ( TInt i = 0; i < count; i++ )
        {
        name = iRouter->At(i).Name();
        if ( name )
            {
            CleanupStack::PushL( name );
		    array->AppendL( *name );
		    waypointAdditionCount ++;
		    CleanupStack::PopAndDestroy(name); //name
            }
        else
        	{
        	currentIndex = i;
        	}
        }


    CAknPopupList* popupList = CAknPopupList::NewL( listBox,
        R_AVKON_SOFTKEYS_OK_CANCEL,
        AknPopupLayouts::EPopupSNotePopupWindow);
    CleanupStack::PushL(popupList);

    HBufC* title = iCoeEnv->AllocReadResourceLC( R_BLID_QUERY_WAYPOINT );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy(title); //title    
    title = NULL;
    
    listBox->ConstructL( popupList, 
        EAknListBoxSelectionList | EAknListBoxLoopScrolling);
    
    listBox->CreateScrollBarFrameL(ETrue); 
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
                                CEikScrollBarFrame::EAuto, 
                                CEikScrollBarFrame::EAuto);
    
    //listbox's model take ownership of array
    listBox->Model()->SetItemTextArray(array);
    
    CleanupStack::Pop(popupList); // popupList
    CleanupStack::Pop(array); //array 
    TBool isEmpty = array->Count() == 0;
        
    if ( popupList->ExecuteLD()  ) 
        {
        index = listBox->CurrentItemIndex();
        }
    CleanupStack::PopAndDestroy(listBox); //listBox
    
    if(isEmpty)
    	{
    	return KErrCancel;
    	}
    
    if(waypoint && waypointAdditionCount != iRouter->Count())   
    	{
    	if(index >= currentIndex)
    		{
    		index += 1;    				
    		}        
    	}
    if ( index != KErrCancel )
    	{
    	TNamedCoordinate waypoint = iRouter->At(index);
    	if(waypoint.Accuracy() >= KBlidPoorAccuracy)
    		{
    		HBufC* noteText = StringLoader::LoadLC( R_BLID_CONF_DEST_INACCURATE , iCoeEnv );	        		
    		CAknQueryDialog* dlg = CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
    		if(dlg->ExecuteLD(R_BLID_CONFIRMATION_QUERY, *noteText))
    			{
    			CleanupStack::PopAndDestroy( noteText );
    			return index;
    			}
    		else
    			{
    			CleanupStack::PopAndDestroy( noteText );
    			return KErrCancel;
    			}
    		}
    	}
    return index;
    }

// ---------------------------------------------------------
// CBlidBaseView::CheckIfExist
// Checks if a waypoint exists
// ---------------------------------------------------------
//
TBool CBlidBaseView::CheckIfExistL( const TDes& aName, TInt& aErrorCode, TInt& aRemovedIndex )
    {
    TInt i(0);
    TInt count( iRouter->Count() );
    HBufC* name = NULL;
    TBool retVal;
	aRemovedIndex = KErrNotFound;

    for ( i = 0; i < count; i++ )
        {
        name = iRouter->At( i ).Name();
        if ( name && name->Compare( aName ) == 0 )
            {
            CleanupStack::PushL(name);
            HBufC* text;
            text = StringLoader::LoadLC(R_BLID_QUERY_WAYPOINT_NAME_EXISTS,
                                       *name,
                                       iCoeEnv);
                                    
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            CleanupStack::PushL(dlg);
            dlg->SetPromptL( *text ); 
            CleanupStack::Pop( dlg );           
            retVal = dlg->ExecuteLD(R_BLID_CONFIRMATION_QUERY);            
            
            if(retVal)
                {
                TInt retVal = iRouter->RemoveL(i);
				aRemovedIndex = i;
                if(retVal == KErrDiskFull)
		        	{
		        	aErrorCode = KErrDiskFull;
		        	BlidNotes::OutOfMemoryNoteL();
		        	}
                }
            CleanupStack::PopAndDestroy(2); //name and text
            return !retVal;
            }
        delete name;
        name = NULL;
        }
    return EFalse;
    }

// ----------------------------------------------------
// CBlidBaseView::LaunchLandmarksDialogL
// Launch the landmarks dialog
// ----------------------------------------------------
//
TInt CBlidBaseView::LaunchLandmarksDialogL()
    {
    CPosLandmarkDatabase* lDb = NULL;
    TPosLmItemId selectedItem;
    iLandmarkInfo = CLmkLandmarkSelectorDlg::NewL();    
    iLandmarkInfo->SetMopParent(this);
    TLmkItemIdDbCombiInfo* selected = new (ELeave) TLmkItemIdDbCombiInfo();
    CleanupStack::PushL(selected);
    TInt retVal = iLandmarkInfo->ExecuteLD( *selected );
    if( retVal != KErrNone )
	    {	       	
	    selectedItem = selected->GetItemId();
	    lDb = selected->GetLmDb();
	    if(lDb)
	    	{
	    	CleanupStack::PushL( lDb );
	    	CPosLandmark *landmarkInfo = lDb->ReadLandmarkLC(selectedItem);
	        CleanupStack::PushL( landmarkInfo );
	        CPosLandmark* landMark = CPosLandmark::NewL(*landmarkInfo);
	        CleanupStack::Pop( landmarkInfo );
	        TLocality lmkPosition;
	        TInt error = landMark->GetPosition(lmkPosition);
	        if( Math::IsNaN(lmkPosition.Latitude()) || 
	        	Math::IsNaN(lmkPosition.Longitude()) )
	        	{
	        	TPtrC landmarkname;
	        	landMark->GetLandmarkName(landmarkname);
                // Fix done for TSW error EPZO-75PBW8
                HBufC* lmkName = NULL;
                lmkName = HBufC::NewL( landmarkname.Length() );
                lmkName->Des().Copy( landmarkname );
                TPtr lmkActualName( lmkName->Des() );
                TBuf<1> charsToRemove;
                charsToRemove.Append(TChar(TInt(CEditableText::EParagraphDelimiter)));
                AknTextUtils::ReplaceCharacters( lmkActualName, charsToRemove, TChar(TInt(CEditableText::ESpace)));
	        	BlidNotes::EmptyLandmarkNoteL( *lmkName );
	        	delete lmkName;
                // end of error fix.
	        	delete landMark;
	        	retVal = 0;
	        	}
	        else
	        	{
	            TLocality lmkPosition;
	        	landMark->GetPosition(lmkPosition);
	        	TReal accuracy = lmkPosition.HorizontalAccuracy();
	        	if(accuracy >= KBlidPoorAccuracy)
	        		{	        		
	        		HBufC* noteText = StringLoader::LoadLC( R_BLID_CONF_DEST_INACCURATE , iCoeEnv );	        		
	        		CAknQueryDialog* dlg = CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
	        		if(dlg->ExecuteLD(R_BLID_CONFIRMATION_QUERY, *noteText))
	        			{
	        			iRouter->SetLandmark(landMark);	        			
	        			}
	        		else
	        			{
	        			delete landMark;
	        			}
	        		CleanupStack::PopAndDestroy(noteText);
	        		}
	        	else
	        		{
	        		iRouter->SetLandmark(landMark);
	        		}	        	

	        	}
	    	CleanupStack::PopAndDestroy(1); // ReadLandmarkLC
	    	landMark = NULL;
	    	landmarkInfo = NULL;
	    	CleanupStack::PopAndDestroy( lDb );
	    	}
	    }
	CleanupStack::Pop(1); //selectedItem
	iLandmarkInfo = NULL;
	delete selected;
	return retVal;	
    }

// ----------------------------------------------------------------------------
// CBlidBaseView::SaveCurrentPositionAsLandmarkL
// Called to save the current location as a landmark in landmark database
// ----------------------------------------------------------------------------
//
void CBlidBaseView::SaveCurrentPositionAsLandmarkL()
    {
    CLmkEditorDlg::TLmkEditorParams editParams; 
	editParams.iAttributes = CLmkEditorDlg::ELmkAll;
	editParams.iEditorMode = CLmkEditorDlg::ELmkEditor;
	
	TPosition position = iLocation->GetCurrentPosition();
    
    CPosLandmark* landmark = CPosLandmark::NewLC();
    TLocality location;
    location.SetCoordinate(position.Latitude(),
    					   position.Longitude(),
    					   position.Altitude());
    location.SetHorizontalAccuracy(position.HorizontalAccuracy());
    location.SetVerticalAccuracy(position.VerticalAccuracy());
    
    landmark->SetPositionL(location);
    CPosLandmarkDatabase* db = CPosLandmarkDatabase::OpenL();
    CleanupStack::PushL(db);
    
    CPosLmOperation* operation = db->InitializeL();
	CleanupStack::PushL(operation);
	operation->ExecuteL();	
    CLmkEditorDlg *dlg = CLmkEditorDlg::NewL(*db, 
                                             *landmark, 
                                             editParams);
	dlg->ExecuteLD();
	CleanupStack::PopAndDestroy(3); // landmark, operation and db
    }

// ----------------------------------------------------------------------------
// CBlidBaseView::LaunchManualWaypointDialogL
// Launches the manual waypoint dialog
// ----------------------------------------------------------------------------
//    
TInt CBlidBaseView::LaunchManualWaypointDialogL()
    {    
    TInt retVal;    
    TCoordinate coord(0,0);
    TLocality loc(coord,0);    
    TTime time;
    time.HomeTime();
    TPosition pos(loc, time); 

    CAknMultiLineDataQueryDialog* dlg = CAknMultiLineDataQueryDialog::NewL(pos);
    retVal = dlg->ExecuteLD(R_BLID_MANUAL_LOCATION_QUERY_DIALOG);
    if( retVal )
	    {	    
	    iRouter->SetManualWaypoint(pos.Latitude(), pos.Longitude(), iManualWaypointName);	
	    }
    return retVal;
    }

// ----------------------------------------------------------------------------
// CBlidBaseView::Update
// ----------------------------------------------------------------------------
//    
void CBlidBaseView::Update(CBlidBaseContainer* aContainer)
    {
    aContainer->SetRect(ClientRect());
    aContainer->DrawNow();
    }

// ----------------------------------------------------------------------------
// CBlidBaseView::CBAPtr
// Returns the current CBA
// ----------------------------------------------------------------------------
//    
CEikButtonGroupContainer* CBlidBaseView::CBAPtr() const
    {
    return (Cba());
    }
    
// ----------------------------------------------------------------------------
// CBlidBaseView::SetMiddleSoftKeyLabelL
// Sets the Middle Soft Key label.
// ----------------------------------------------------------------------------
//    
void CBlidBaseView::SetMiddleSoftKeyLabelL(
    const TInt aResourceId,
    const TInt aCommandId )   
    {
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        HBufC* middleSKText = StringLoader::LoadLC( aResourceId );
        cbaGroup->AddCommandToStackL(KMiddleSKId, 
                                     aCommandId, 
                                     *middleSKText );
        CleanupStack::PopAndDestroy( middleSKText );
        }
    }

// ----------------------------------------------------------------------------
// CBlidBaseView::RemoveMiddleSoftKeyLabel
// Remove the Middle Soft Key label.
// ----------------------------------------------------------------------------
//    
void CBlidBaseView::RemoveMiddleSoftKeyLabel( const TInt aCommandId )   
    {
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        cbaGroup->RemoveCommandFromStack(KMiddleSKId, 
                                     aCommandId );
        }
    }
    
// ----------------------------------------------------------------------------
// CBlidBaseView::SetMSKDimmed
// Sets the Middle Soft Key as not visible.
// ----------------------------------------------------------------------------
//     
void CBlidBaseView::SetMSKNotVisible()    
    {
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        cbaGroup->MakeCommandVisible(EAknSoftkeyContextOptions, EFalse);
        iMSKVisible = EFalse;
        }
    }
    
// ----------------------------------------------------------------------------
// CBlidBaseView::SetMSKVisible
// Sets the Middle Soft Key as visible.
// ----------------------------------------------------------------------------
//     
void CBlidBaseView::SetMSKVisible()    
    {
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        cbaGroup->MakeCommandVisible(EAknSoftkeyContextOptions, ETrue);
        iMSKVisible = ETrue;
        }
    }   

// ----------------------------------------------------------------------------
// CBlidBaseView::UpdateMskStateL
// Update Middle Soft Key label/Icon.
// ----------------------------------------------------------------------------
// 
void CBlidBaseView::UpdateMskStateL()    
    {
    if (!iLocation->IsGPSDataAvailable())        
        {
        SetMSKNotVisible();
        }
    else
        {
        if(!iMSKVisible)
            {
            SetMSKVisible(); 
            MenuBar()->SetContextMenuTitleResourceId(
                                                     R_BLID_OWN_POS_VIEW_OK_MENUBAR);               
            }
        }
    }

#ifdef RD_SCALABLE_UI_V2    
// ---------------------------------------------------------------------------
// CBlidBaseView::HandleListBoxEventL
// Handles listbox item selection
// ---------------------------------------------------------------------------
//
void CBlidBaseView::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            HandleListBoxSelectionL();
            break;
        default:
           break;
        }
    }  
    
#endif //RD_SCALABLE_UI_V2      
// ---------------------------------------------------------------------------
// CBlidBaseView::IsLmFeatureAvailable
// Checks if Landmarks feature i available
// ---------------------------------------------------------------------------
//
TBool CBlidBaseView::IsLmFeatureAvailable()    
    {
    return FeatureManager::FeatureSupported( KFeatureIdLandmarks );
    } 
    
// ---------------------------------------------------------------------------
// CBlidBaseView::IsLmAvailableL
// Checks if there are any landmarks in the default database
// ---------------------------------------------------------------------------
//
TBool CBlidBaseView::IsLmAvailableL()    
    {
    TBool lResult = ETrue;
	// Check if there are any landmarks present. If not the
	// dim the landmark item from the set destination menu pane
	CPosLandmarkDatabase* lmkDb;
	lmkDb = CPosLandmarkDatabase::OpenL();			
	CleanupStack::PushL(lmkDb);
	// Initialize Landmarks data base if required			
	CPosLmOperation* operation = lmkDb->InitializeL();
	CleanupStack::PushL(operation);			
	operation->ExecuteL();
	CPosLmItemIterator* iterator = lmkDb->LandmarkIteratorL();
	CleanupStack::PushL(iterator);
	
	if( iterator->NumOfItemsL() == 0 )
        {
        lResult = EFalse;        
        }
    else
        {
        lResult = ETrue;
        }
    CleanupStack::PopAndDestroy(3); //lmkDb, iterator and operation                
    return lResult;        
    } 
    
// ---------------------------------------------------------------------------
// CBlidBaseView::IsWpAvailable
// Checks if waypoints are available
// ---------------------------------------------------------------------------
//
TBool CBlidBaseView::IsWpAvailable()    
    {
    if ( iRouter->Count() <= 0 )
        {
        return EFalse;        
        }
    return ETrue;
    }         
       

// ----------------------------------------------------------------------------
// CBlidBaseView::ActivateCorrectViewL
// Activates the view
// ----------------------------------------------------------------------------
//
void CBlidBaseView::ActivateCorrectViewL( TInt aIndex )
	{
    switch( aIndex )
        {
        case EBlidNavigationViewId:
            {
            AppUi()->ActivateLocalViewL( TUid::Uid( EBlidNavigationView ) );
            break;
            }
        case EBlidTripMeterViewId:
            {
            AppUi()->ActivateLocalViewL( TUid::Uid( EBlidTripMeterView ) );
            break;
            }
        default:
            break;
        }
	}


// ----------------------------------------------------------------------------
// CBlidBaseView::ShowCurrentPositionInfoL
// Launches the current position dialog
// ----------------------------------------------------------------------------
//
void CBlidBaseView::ShowCurrentPositionInfoL()
	{
	iPopupDialog = CBlidCurrentPositionDlg::NewL( *this, iLocation, iEngine );
	iPopupDialog->ExecuteLD();
	iPopupDialog = NULL;
	}

// ----------------------------------------------------------------------------
// CBlidBaseView::SaveCurrentPositionL
// Saves current position as landmark or as waypoint
// ----------------------------------------------------------------------------
//
void CBlidBaseView::SaveCurrentPositionL()
	{
	if ( !FeatureManager::FeatureSupported( KFeatureIdLandmarks ) )
		{
		if ( InsertWaypointL() )
            {
            static_cast<CBlidDocument*>(AppUi()->Document())->SaveL();
            }
		}
	else
		{
		SaveCurrentPositionAsLandmarkL();
		}
	}

// ----------------------------------------------------
// CBlidBaseView::BlidDocument
// Returns the Location document object.
// ----------------------------------------------------
//
CBlidDocument* CBlidBaseView::BlidDocument()
    {
    // Explicit cast: the document must always be of type CBlidDocument
    return static_cast<CBlidDocument*>(AppUi()->Document());
    }

// ----------------------------------------------------------------------------
// CBlidBaseView::ActivateSatelliteViewL
// Activates the Satellite view
// ----------------------------------------------------------------------------
//
void CBlidBaseView::ActivateSatelliteViewL( )
	{
	AppUi()->ActivateLocalViewL( TUid::Uid( EBlidMainSatelliteView ) );
	}
	
// ----------------------------------------------------------------------------
// CBlidBaseView::IsSatViewActive
// Checks if Satellite view is active
// ----------------------------------------------------------------------------
//
TBool CBlidBaseView::IsSatViewActive( )
	{
	return (static_cast<CBlidAppUi*>(AppUi()))->IsSatelliteViewActive();
	}

// ----------------------------------------------------------------------------
// CBlidBaseView::IsForeGroundApp
// Checks if Blid app is in foreground
// ----------------------------------------------------------------------------
//
TBool CBlidBaseView::IsForeGroundApp( )
	{
	return ( static_cast<CBlidAppUi*>(AppUi())->IsForeground() );
	}
	
// ----------------------------------------------------------------------------
// CBlidBaseView::ExitMainApplicationL
// Exit the application
// ----------------------------------------------------------------------------
//
void CBlidBaseView::ExitMainApplicationL( TInt aButtonId )
	{
	static_cast<CBlidAppUi*>(AppUi())->HandleCommandL( aButtonId );
	}
	
// ----------------------------------------------------------------------------
// CBlidBaseView::LaunchTripContextMenuL
// Launches the context menu for tripmeter view
// ----------------------------------------------------------------------------
//
void CBlidBaseView::LaunchTripContextMenuL()
	{
    CEikMenuBar* menubar = static_cast<CAknViewAppUi*>
        (iAvkonAppUi)->View(TUid::Uid( EBlidTripMeterView ))->MenuBar();
    if ( menubar )
        {
        menubar->SetContextMenuTitleResourceId(R_BLID_TRIP_METER_VIEW_OK_MENUBAR);
        menubar->TryDisplayContextMenuBarL();
        }
	}

// ----------------------------------------------------------------------------
// CBlidBaseView::LaunchNavigationContextMenuL
// Launches the context menu for Navigation view
// ----------------------------------------------------------------------------
//
void CBlidBaseView::LaunchNavigationContextMenuL()
	{
    CEikMenuBar* menubar = static_cast<CAknViewAppUi*>
        (iAvkonAppUi)->View(TUid::Uid( EBlidNavigationView ))->MenuBar();
        
    if ( menubar )
        {
        menubar->SetContextMenuTitleResourceId(R_BLID_VIEW_OK_MENUBAR);
        menubar->StopDisplayingMenuBar();
        menubar->TryDisplayContextMenuBarL();                         
        }
	}

// ----------------------------------------------------------------------------
// CBlidBaseView::LaunchSatelliteInfoDlgL
// Launches the Satellite info dialog
// ----------------------------------------------------------------------------
//
void CBlidBaseView::LaunchSatelliteInfoDlgL()
	{
	static_cast<CBlidAppUi*>(AppUi())->LaunchSatelliteInfoDlgL( );
	}
	
// ----------------------------------------------------------------------------
// CBlidBaseView::ActivateNavigationViewL
// Activates the navigation view
// ----------------------------------------------------------------------------
//
void CBlidBaseView::ActivateNavigationViewL( )
	{
	AppUi()->ActivateLocalViewL( TUid::Uid( EBlidNavigationView ) );
	}
	
// ----------------------------------------------------------------------------
// CBlidBaseView::ChangeViewSoftKeyL
// Changes the soft key
// ----------------------------------------------------------------------------
//
void CBlidBaseView::ChangeViewSoftKeyL( )
	{
	CBAPtr()->SetCommandSetL( R_BLID_SOFTKEYS_INFO_EXIT );
	CBAPtr()->DrawDeferred();
	}	

// ----------------------------------------------------------------------------
// CBlidBaseView::ApplicationRect
// Returns the application rect
// ----------------------------------------------------------------------------
//
TRect CBlidBaseView::ApplicationRect( ) const
	{
	return AppUi()->ApplicationRect();
	}	

// ----------------------------------------------------------------------------
// CBlidBaseView::ImageLoaderUtils
// Returns the image loader utils
// ----------------------------------------------------------------------------
//
CAlfImageLoaderUtil* CBlidBaseView::ImageLoaderUtils( ) const
	{
	return iImageLoaderUtil;
	}
	
// ----------------------------------------------------------------------------
// CBlidBaseView::ImagePathName
// Returns the image path
// ----------------------------------------------------------------------------
//
TFileName CBlidBaseView::ImagePathName( ) const
	{
	return iConFileName;
	}	

// ----------------------------------------------------------------------------
// CBlidBaseView::IsSettingsViewActive
// Checks if settings view is active
// ----------------------------------------------------------------------------
//
TBool CBlidBaseView::IsSettingsViewActive()
	{
	return iIsSettingsViewActive;
	}

// ----------------------------------------------------------------------------
// CBlidBaseView::CoeEnv
// Returns the control environment
// ----------------------------------------------------------------------------
//
CCoeEnv* CBlidBaseView::CoeEnv()
	{
	return iCoeEnv;
	}
// End of File

