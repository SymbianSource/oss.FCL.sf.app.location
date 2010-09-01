/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  View class for Events Management UI.
*
*/

// SYSTEM INCLUDES
#include <evtmgmtui.rsg>
#include <hlplch.h>
#include <featmgr.h>
#include <eikmenup.h>
#include <eikbtgpc.h>
#include <StringLoader.h>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h> 
#include <eikserverapp.h>
#include <textresolver.h>
#include <csxhelp/loc.hlp.hrh>

// USER INCLUDES
#include "evtmgmtuiuid.hrh"
#include "evtmgmtuistartupview.h"
#include "evtmgmtuistartupcontainer.h"
#include "evtmgmtuiappui.h"
#include "evtmgmtui.hrh"
#include "evteventsdlg.h"
#include "evtdebug.h"
#include "evtmgmtuilocsettinglauncher.h"
#include "evtmgmtuimodel.h"
#include "evtmgmtuilbtadapter.h"
#include "evtmgmtuimapnavigationadapter.h"

// CONSTANT DEFINTIONS
const TUid	KEvtMgmtUiStartupViewId = TUid::Uid( 1 );
const TInt KMaxEvents = 50;
_LIT( KWaitCycleError, "Wait Cycle Error" );

// ----------------- Member funtions for CEvtMgmtUiStartupView class -----------------------

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::CEvtMgmtUiStartupView
// ---------------------------------------------------------------------------
//  
CEvtMgmtUiStartupView::CEvtMgmtUiStartupView()
    {
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::~CEvtMgmtUiStartupView
// ---------------------------------------------------------------------------
//  
CEvtMgmtUiStartupView::~CEvtMgmtUiStartupView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
    	delete iContainer; 
        }
    
    // Unregister the command handler as an observer
    if( iEngine )
	{
	iEngine->RemoveObserver( this );
       	delete iEngine;
	}
    
    delete iPosSetLauncher;
    
    if(iEvtIdArray)
        {
        iEvtIdArray->Close();
        delete iEvtIdArray;
        }
    
    delete iModel;
    FeatureManager::UnInitializeLib();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView* CEvtMgmtUiStartupView::NewL
// ---------------------------------------------------------------------------
//        
CEvtMgmtUiStartupView* CEvtMgmtUiStartupView::NewL()
    {
    CEvtMgmtUiStartupView* self = new ( ELeave ) CEvtMgmtUiStartupView();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::ConstructL
// ---------------------------------------------------------------------------
//  
void CEvtMgmtUiStartupView::ConstructL()
    {
    if(iEikonEnv->StartedAsServerApp())
        {
        iIsEmbeddedApp = ETrue;
        BaseConstructL( R_EVTMGMTUI_STARTUP_VIEW_EMBEDDED );
        }
    else
        {
        iIsEmbeddedApp = EFalse;
        BaseConstructL( R_EVTMGMTUI_STARTUP_VIEW );
        }
    // Create Log Directory.
    //CREATELOGDIR();
    
    // Feature manager required for Help feature check
    FeatureManager::InitializeLibL();
    
    iEngine = CEvtMgmtUiEngine::NewL();    
    iEngine->SetObserver(this);  
    
    iEngine->SyncDbsL();
    
    iModel = CEvtMgmtUiModel::NewL();
    iEngine->UpdateModelL(iModel->EventsArray(), CEvtMgmtUiEngine::EEvtFilterAll ); 
    iModel->SetStatusFilter( CEvtMgmtUiEngine::EEvtFilterAll );  
    iModel->UpdateEventsStatus( );
    }
			
// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::DoActivateL
// ---------------------------------------------------------------------------
//  	
void CEvtMgmtUiStartupView::DoActivateL( const TVwsViewId&  /* PrevViewId*/,
                                             TUid           /* aCustomMessageId*/,
                                       const TDesC8&        /* aCustomMessage */)
    {
    // Destroy the existing container if it exists
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer=NULL;
        }                       
    
    // Create new Container 
    iContainer = CEvtMgmtUiStartupContainer::NewL( ClientRect(), iModel, *this);
    iContainer->SetMopParent( this );
    AppUi()->AddToViewStackL( *this, iContainer );   	   
    if(iModel->TotalEvents())
        MenuBar()->SetMenuTitleResourceId ( R_EVTMGMTUI_STARTUP_MENUBAR_MARK );
    else
        MenuBar()->SetMenuTitleResourceId ( R_EVTMGMTUI_STARTUP_MENUBAR );
    }
    
// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::DoDeactivate
// ---------------------------------------------------------------------------
//           
void CEvtMgmtUiStartupView::DoDeactivate()
    {
    // Destroy Container
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// TUid CEvtMgmtUiStartupView::Id
// ---------------------------------------------------------------------------
//   
TUid CEvtMgmtUiStartupView::Id() const
    {
    return KEvtMgmtUiStartupViewId;
    }

// -----------------------------------------------------------------------------
//  CEvtMgmtUiStartupView::HandleShowOnMapCommandL
//  It will handle 'show on map' command.
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiStartupView::HandleShowOnMapCommandL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiStartupView::HandleShowOnMapCommandL()" );
    CEvtMgmtUiMapNavigationAdapter* map = CEvtMgmtUiMapNavigationAdapter::NewL();
    CleanupStack::PushL(map);
    TCoordinate loc;
    iEngine->RetrieveLocationL( iModel->EventId( iContainer->CurrentItemIndex() ), loc );        
    map->ShowOnMapL( loc );
    CleanupStack::PopAndDestroy(); //map
    EVTUIDEBUG("- CEvtMgmtUiStartupView::HandleShowOnMapCommandL()" );
    }

// -----------------------------------------------------------------------------
//  CEvtMgmtUiStartupView::HandleNavigateToPlaceCommandL
//  It will handle 'navigate to place' command.
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiStartupView::HandleNavigateToPlaceCommandL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiStartupView::HandleNavigateToPlaceCommandL()" );
    CEvtMgmtUiMapNavigationAdapter* map = CEvtMgmtUiMapNavigationAdapter::NewL();
    CleanupStack::PushL(map);    
    TCoordinate loc;
    iEngine->RetrieveLocationL( iModel->EventId( iContainer->CurrentItemIndex() ), loc );
    map->NavigateToPlaceL( loc );
    CleanupStack::PopAndDestroy(); //map
    EVTUIDEBUG("- CEvtMgmtUiStartupView::HandleNavigateToPlaceCommandL()" );
    }
// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::HandleCommandL
// ---------------------------------------------------------------------------
//       
void CEvtMgmtUiStartupView::HandleCommandL( TInt aCommand )   
    {
    EVTUIDEBUG1("+ CEvtMgmtUiStartupView::HandleCommandL = %d", aCommand);
    
    TInt currentItemIndex= iContainer->CurrentItemIndex();
    // This class only handles the commands specific to this view
    switch( aCommand )
    	{
    	case EEvtMgmtUiStartupViewOpen:
			{			
			if(currentItemIndex>0)
				{ 
                TCoeHelpContext context;
                context.iContext = KHLP_LOC_EVENTS_EDITOR();
                context.iMajor = TUid::Uid( KEvtMgmtUiUid );	
                
				CEvtEvent* evt = iEngine->EventL( EventIdOfCurrentItem() );	
				CleanupStack::PushL( evt );
				CEvtEventsDlg* form = CEvtEventsDlg::NewL(EFalse, *evt, *iEngine);  
				form->SetHelpContext( context );            
				form->ExecuteLD( ); 	
				CleanupStack::PopAndDestroy( evt );
				iContainer->MakeTitleL();
				}           
			break;
			} 
    	case EEvtMgmtUiStartupViewCreate:
			{
			currentItemIndex = KErrNotFound;
			if(iModel->TotalEvents() < KMaxEvents )
			    {
                TCoeHelpContext context;
                context.iContext = KHLP_LOC_EVENTS_EDITOR();
                context.iMajor = TUid::Uid( KEvtMgmtUiUid );	
                
				CEvtEvent* evt = iEngine->EventL( 0 );	
				CleanupStack::PushL( evt );
			    CEvtEventsDlg* form = CEvtEventsDlg::NewL(ETrue, *evt, *iEngine );                
				form->SetHelpContext( context );            
			    form->ExecuteLD( );
				CleanupStack::PopAndDestroy( evt );
				iContainer->MakeTitleL();
				if(iModel->TotalEvents())
			        MenuBar()->SetMenuTitleResourceId ( R_EVTMGMTUI_STARTUP_MENUBAR_MARK );
			    else
			        MenuBar()->SetMenuTitleResourceId ( R_EVTMGMTUI_STARTUP_MENUBAR );
			    }
			else
			    {
			    HBufC* errMsg = StringLoader::LoadLC( R_EVTUI_MAXEVENTS_ERROR, iCoeEnv );
                CAknInformationNote* errNote = new(ELeave) CAknInformationNote();
                errNote->ExecuteLD( *errMsg );
                CleanupStack::PopAndDestroy(errMsg);   
			    }
			break;
			}   
    	case EEvtEditorCmdDelete:
    	    {
    	    TInt cnt = iContainer->NumberOfMarkedItems();
    	    if( currentItemIndex != 0 || cnt )
    	        {
                HBufC* msg = NULL;
                if(cnt>1)
                    msg = StringLoader::LoadLC(R_LOCEV_CONFIRM_MULTIDELETE_EVENT, cnt, CEikonEnv::Static() );
                else
                    msg = StringLoader::LoadLC(R_LOCEV_CONFIRM_DELETE_EVENT, CEikonEnv::Static() );
                
                //if not deleted, do nothing
               if(DisplayConfirmationQueryDlgL(*msg))
                   { 	         
                    iWaitState = EWaitDlgLaunchToDelete;   
                    StartWaitDialogL();
                   }
                CleanupStack::PopAndDestroy(msg);
    	        }
            break;
            }   	
    	case EEvtEditorCmdShowAll:
    		{
    		iModel->SetCurrentItemId( iContainer->CurrentItemIndex() );
    		iModel->SetStatusFilter( CEvtMgmtUiEngine::EEvtFilterAll );
    		UpdateViewL(); 
    		break;
    		}
    	case EEvtEditorCmdShowDrafts:
    		{    		
    		iModel->SetCurrentItemId( iContainer->CurrentItemIndex() );
    		iModel->SetStatusFilter( CEvtMgmtUiEngine::EEvtFilterDraft );
    		UpdateViewL(); 
    		break;
    		}
    	case EEvtEditorCmdShowActive:
    		{ 
    		iModel->SetCurrentItemId( iContainer->CurrentItemIndex() );
    		iModel->SetStatusFilter( CEvtMgmtUiEngine::EEvtFilterActive );
    		UpdateViewL(); 
    		break;
    		}
        case EEvtEditorCmdShowCompleted:
            {           
            iModel->SetCurrentItemId( iContainer->CurrentItemIndex() );
            iModel->SetStatusFilter( CEvtMgmtUiEngine::EEvtFilterCompleted );
            UpdateViewL(); 
            break;
            }
    	case EEvtEditorCmdActivate:
    	    {    	    
    	    iEvtStatus = EActive;
    	    iWaitState = EWaitDlgLaunchToChangeStatus;   
    	    StartWaitDialogL();    	        	    
    	    break;
    	    }
	   case EEvtEditorCmdSaveDraft:
            {
            iEvtStatus = EDraft;  
            iWaitState = EWaitDlgLaunchToChangeStatus;   
            StartWaitDialogL();                    
            break;
            }
	   case EAknCmdMark:
	   case EAknSoftkeyMark:
	       {
	       if(currentItemIndex != KErrNotFound)
	           {
	           iContainer->HandleMarkCmdL(currentItemIndex, ETrue);
	           HandleCmdL( EListBoxItemFocused );
	           }
	       break;
	       }
	   case EAknCmdUnmark:
	   case EAknSoftkeyUnmark:
	       {
	       if(currentItemIndex != KErrNotFound)
	           {
    	       iContainer->HandleMarkCmdL(currentItemIndex, EFalse);
    	       HandleCmdL( EListBoxItemFocused );
	           }
	       break;
	       }
	   case EAknMarkAll:
	       {
	       iContainer->HandleMarkCmdL(KErrNotFound, ETrue);
	       HandleCmdL( EListBoxItemFocused );
	       break;
	       }
	   case EAknUnmarkAll:
	        {	                   
	        iContainer->HandleMarkCmdL(KErrNotFound, EFalse);
	        HandleCmdL( EListBoxItemFocused );
	        }
		    break;  
	   case EEvtEditorCmdNavigateToPlace:
	       {
	       HandleNavigateToPlaceCommandL();
	       break;
	       }
	   case EEvtEditorCmdShowOnMap:
	       {
	       HandleShowOnMapCommandL();
	       break;
	       }
	   case EEvtMgmtUiEventUiPosSettings:
	       {
	       if(!iPosSetLauncher)
               {
               iPosSetLauncher = CEvtMgmtUiLocSettingsLauncher::NewL();                
               }
	       iPosSetLauncher->LaunchL();
           break;
	       }
    	case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(),
                                                 AppUi()->AppHelpContextL());
            break;  
            }
        default:
        	{
			AppUi()->HandleCommandL( aCommand );
			break;        	
        	}
    	}
    EVTUIDEBUG("- CEvtMgmtUiStartupView::HandleCommandL");
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::ChangeEventStatus
// Change status of the event
// param[in] aStatus -  value to which event status should be changed
// ---------------------------------------------------------------------------
//  
void CEvtMgmtUiStartupView::ChangeEventStatusL( TEvtEventStatus aStatus )
    {      
    TInt cnt = iContainer->NumberOfMarkedItems();
    TInt index;
    if(iEvtIdArray)
        iEvtIdArray->Reset();
    else
        iEvtIdArray = new(ELeave) RArray<TEvtEventId>();
    
    if(cnt)
          {
          iDisplayNote = EFalse;
          EVTUIDEBUG1("CEvtMgmtUiStartupView::ChangeEventStatusL() selected items = %d", cnt );  
          const CArrayFix<TInt>* evtarray = iContainer->MarkItems();          
          TInt itemIndex = 0;             
          for(index=0;index<cnt;index++)
              {        
              itemIndex = iContainer->FilteredIndex((*evtarray)[index]);
              EVTUIDEBUG1("CEvtMgmtUiStartupView::ChangeEventStatusL() listboxindexid = %d", itemIndex);          
              EVTUIDEBUG1("Changing status of event Event id = %u", EventId(itemIndex) );
              
              //retrieve event details from model
              CEvtBasicEventInfo& event = iModel->EventFromModel(itemIndex);
              
              if( aStatus == EActive )
                 {               
                 //if manadatory fields are empty, display info note
                 if( event.Subject().Compare(KNullDesC)==0 || event.Place().Compare(KNullDesC)==0 )
                     {                    
                     iDisplayNote  = ETrue;
                     }
                 else
                     {
                     //if event is not having state to which it is supposed to change 
                     // then add it to id-list
                     if( event.EventStatus() !=  aStatus )
                         iEvtIdArray->Append( event.EventId() );
                     }
                  }  
              else
                  {
                  //if event is not having state to which it is supposed to change 
                  // then add it to id-list
                  if( event.EventStatus() !=  aStatus )
                      iEvtIdArray->Append( event.EventId() );
                  }
              }
          if(iEvtIdArray->Count())
              {
              iEngine->SetEventsStateL( aStatus, iEvtIdArray );
              }
          else
              {
              iWaitState = EWaitDlgDestroy;
              }
          HandleCommandL(EAknUnmarkAll);
          }
     else
          {
           TInt currentItemIndex = iContainer->CurrentItemIndex();
           if(aStatus == EActive)
                {
                if( iModel->EventFromModel(currentItemIndex).Subject().Compare(KNullDesC)==0 || 
					iModel->EventFromModel(currentItemIndex).Place().Compare(KNullDesC)==0 )
                    {                   
                    
                    HBufC* warningMsg = StringLoader::LoadLC( R_EVTUI_MANDATORYFIELDS_WARNING, iCoeEnv );
                    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*warningMsg);
                    dlg->PrepareLC(R_EVTUI_MESSAGE_QUERY);
                    dlg->RunLD();
                    CleanupStack::PopAndDestroy(warningMsg);
                    return;
                    }
                } 
           iEvtIdArray->Append( EventId( currentItemIndex ) ); 
           iEngine->SetEventsStateL(aStatus, iEvtIdArray  );
          }
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::ShowEvents
// Show all events with given status
// param[in] aStatusFilter - status filter to display events
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupView::ShowEventsL( CEvtMgmtUiEngine::TEvtEventStatusFilter aStatusFilter )
    {
	iModel->SetCurrentItemId( iContainer->CurrentItemIndex() );
	iModel->SetStatusFilter( aStatusFilter );
    UpdateViewL();
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::EnableMapFunctionalityL
// Check wether map functionality can be disabled.
// ret ETrue if map functionality should be enabled else EFalse
// ---------------------------------------------------------------------------
//
TBool CEvtMgmtUiStartupView::EnableMapFunctionalityL( )
    {
    EVTUIDEBUG("+ CEvtMgmtUiStartupView::DisableMapFunctionality()" );
    CEvtMgmtUiMapNavigationAdapter* map = CEvtMgmtUiMapNavigationAdapter::NewL();
    CleanupStack::PushL(map);
    //if no map service provider
    if ( map->MapServiceProvidersL() == 0 ) 
        {
        CleanupStack::PopAndDestroy(); //map
        EVTUIDEBUG("- CEvtMgmtUiStartupView::DisableMapFunctionality() EFalse" );
        return EFalse;
        }
    
    //Retrieve coordinate info
    TCoordinate loc;
    iEngine->RetrieveLocationL( iModel->EventId( iContainer->CurrentItemIndex() ), loc );        
    CleanupStack::PopAndDestroy(); //map
    
    if( Math::IsNaN( loc.Latitude() ) || Math::IsNaN( loc.Latitude() )
                                || loc.Latitude() == 0 || loc.Latitude() == 0 )
        { //if loc is not defined or it is not coordinate based
        EVTUIDEBUG("- CEvtMgmtUiStartupView::DisableMapFunctionality() EFalse" );
        return EFalse;
        }
    else
        {
        EVTUIDEBUG("- CEvtMgmtUiStartupView::DisableMapFunctionality() ETrue" );
        return ETrue;
        }    
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//  
void CEvtMgmtUiStartupView::DynInitMenuPaneL( TInt aResourceId, 
                                              CEikMenuPane*  aMenuPane  )
    {   
    iContainer->HandleMarkableListDynInitMenuPane( aResourceId, aMenuPane );
    
    TInt index = iContainer->CurrentItemIndex();
    TInt markItems = iContainer->NumberOfMarkedItems();
    switch ( aResourceId )
        {
        case R_EVTMGMTUI_STARTUP_MAINVIEWMENU:
        	{
        	// Order is
        	// 1. Marked
        	// 2. Index
        	// 3. Total Events 
        	
        	if(iContainer->NumberOfMarkedItems() == 0) // no item is marked
        	    {
            	//if no events then disable 'show'
            	if( iModel->TotalEvents() )
            	    {    
            		TEvtEventsStatusMask mask = iModel->EventsStatus();   	    
            	    
            	    switch( mask )
            	    	{
            	    	case CEvtMgmtUiModel::EEvtShowAttributeActive:
            	    	case CEvtMgmtUiModel::EEvtShowAttributeDraft:
            	    	case CEvtMgmtUiModel::EEvtShowAttributeCompleted:
          	        		aMenuPane->SetItemDimmed( EEvtMgmtUiEventUiEvtShow, ETrue ); 
          	        		break;
            	    	default:
          	        		aMenuPane->SetItemDimmed( EEvtMgmtUiEventUiEvtShow, EFalse ); 
            	    		break;	
            	    	}     	        
            	    }
            	 else
          	        aMenuPane->SetItemDimmed( EEvtMgmtUiEventUiEvtShow, ETrue ); 
            	 	
            	    
            	//if focus on 'create event'
    		    if( index == 0 )
    			    {
    				aMenuPane->SetItemDimmed( EEvtMgmtUiStartupViewOpen, ETrue );
    				aMenuPane->SetItemDimmed( EEvtEditorCmdDelete, ETrue );
    				aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, ETrue ); 
    				aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, ETrue );
    				aMenuPane->SetItemDimmed(EEvtEditorCmdNavigateToPlace, ETrue);
    				aMenuPane->SetItemDimmed(EEvtEditorCmdShowOnMap, ETrue);
    			    }
    			else
    			    {
    			    if( iModel->TotalEvents() ) //if events are present 
    				    {
    				    aMenuPane->SetItemDimmed( EEvtMgmtUiStartupViewOpen, EFalse );
    					aMenuPane->SetItemDimmed( EEvtEditorCmdDelete, EFalse );
    					aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, EFalse );
    					aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, EFalse );
    					
    					//change status
                        switch(iModel->EventFromModel(index).EventStatus())
                            {
                            case EActive:
                               {
                               aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, ETrue ); // State                          
                               break;
                               }
                            case EDraft:
                               {
                               aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, ETrue ); // State
                               if( !iModel->MandatoryFieldsFilled(index))
                                   aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, ETrue );    
                               break;
                               }
                            default:                      
                               break;
                            }
                        
                        // if support for map functionality is there
                       if( EnableMapFunctionalityL() )
                            {
                            aMenuPane->SetItemDimmed(EEvtEditorCmdNavigateToPlace, EFalse);
                            aMenuPane->SetItemDimmed(EEvtEditorCmdShowOnMap, EFalse);
                            }
                        else
                            {
                            aMenuPane->SetItemDimmed(EEvtEditorCmdNavigateToPlace, ETrue);
                            aMenuPane->SetItemDimmed(EEvtEditorCmdShowOnMap, ETrue);
                            }
    				    }
    				else
    					{
    					aMenuPane->SetItemDimmed( EEvtMgmtUiStartupViewOpen, ETrue );
    					aMenuPane->SetItemDimmed( EEvtEditorCmdDelete, ETrue );	
    					aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, ETrue ); 
    					aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, ETrue );
    					aMenuPane->SetItemDimmed(EEvtEditorCmdNavigateToPlace, ETrue);
    					aMenuPane->SetItemDimmed(EEvtEditorCmdShowOnMap, ETrue);
    					}
    				} 
        	    }
        	else // if there are marked items
        	    {
        	    aMenuPane->SetItemDimmed( EEvtMgmtUiEventUiEvtShow, ETrue );
        	    aMenuPane->SetItemDimmed( EEvtMgmtUiStartupViewCreate, ETrue );
        	    aMenuPane->SetItemDimmed( EEvtMgmtUiStartupViewOpen, ETrue );   
        	    aMenuPane->SetItemDimmed(EEvtEditorCmdNavigateToPlace, ETrue);
        	    aMenuPane->SetItemDimmed(EEvtEditorCmdShowOnMap, ETrue);
      	    
        	    switch(MarkedEventsStatus())
                    {
                    case EMarkedEventsStatusDraft:
                        {
                        aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, EFalse ); 
                        aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, ETrue );
                        }
                        break;
                    case EMarkedEventsStatusActive:
                        {
                        aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, ETrue ); 
                        aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, EFalse );
                        }
                        break;       
                   case EMarkedEventsStatusCompleted:       
                    default:
                        {
                        aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, EFalse ); 
                        aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, EFalse );
                        }
                        break;        
                    }
        	    }
		   
			 break;           
             }
        case R_EVTMGMTUI_STARTUP_EVTSHOWMENU:
            {
            TEvtEventsStatusMask mask = iModel->EventsStatus();
           
	        aMenuPane->SetItemDimmed( EEvtEditorCmdShowAll, EFalse );
	        aMenuPane->SetItemDimmed( EEvtEditorCmdShowActive, EFalse );
	        aMenuPane->SetItemDimmed( EEvtEditorCmdShowDrafts, EFalse );
	        aMenuPane->SetItemDimmed( EEvtEditorCmdShowCompleted, EFalse );
            
            switch( iModel->StatusFilter() )
            	{
            	case CEvtMgmtUiEngine::EEvtFilterActive:
	            	{
			        aMenuPane->SetItemDimmed( EEvtEditorCmdShowActive, ETrue );
            		break;
	            	}
            	case CEvtMgmtUiEngine::EEvtFilterDraft:
	            	{
			        aMenuPane->SetItemDimmed( EEvtEditorCmdShowDrafts, ETrue );
            		break;
	            	}
                case CEvtMgmtUiEngine::EEvtFilterCompleted:
                    {
                    aMenuPane->SetItemDimmed( EEvtEditorCmdShowCompleted, ETrue );
                    break;
                    }
            	case CEvtMgmtUiEngine::EEvtFilterAll:
	            	{
			        aMenuPane->SetItemDimmed( EEvtEditorCmdShowAll, ETrue );
            		break;
	            	}
            	default:
            		break;
            	}
            	
        	if( !(mask & CEvtMgmtUiModel::EEvtShowAttributeActive) )
              	aMenuPane->SetItemDimmed( EEvtEditorCmdShowActive, ETrue );
            if( !(mask & CEvtMgmtUiModel::EEvtShowAttributeDraft) )
              	aMenuPane->SetItemDimmed( EEvtEditorCmdShowDrafts, ETrue );
            if( !(mask & CEvtMgmtUiModel::EEvtShowAttributeCompleted) )
                aMenuPane->SetItemDimmed( EEvtEditorCmdShowCompleted, ETrue );         
            
            break;
            }
        case R_EVTMGMTUI_STARTUP_CHANGESTATUSMENU:
            {

            switch(MarkedEventsStatus())
               {
               case EMarkedEventsStatusDraft:
                   {
                   aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, EFalse ); 
                   aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, ETrue );
                   }
                   break;
               case EMarkedEventsStatusActive:
                   {
                   aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, ETrue ); 
                   aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, EFalse );
                   }
                   break;       
               case EMarkedEventsStatusCompleted:  
               default:
                   {
                   aMenuPane->SetItemDimmed( EEvtEditorCmdActivate, EFalse ); 
                   aMenuPane->SetItemDimmed( EEvtEditorCmdSaveDraft, EFalse );
                   }
                   break;        
               }
            }
            break;     
        case R_EVTMGMTUI_STARTUP_APPMENU:
        	{
			HandleHelpFeature(*aMenuPane);
        	}
			break; 
		default:
			break;
        }
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::HandleHelpFeature
// ---------------------------------------------------------------------------
//      
void CEvtMgmtUiStartupView::HandleHelpFeature( CEikMenuPane& aMenuPane ) const
    {
    // The Help menu item must be displayed only if the Feature is supported.
    // If not, the Help item must be dimmed in the Options menu.
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ))
        {
        aMenuPane.SetItemDimmed( EAknCmdHelp, EFalse );
        }
    else
        {
        aMenuPane.SetItemDimmed( EAknCmdHelp, ETrue );
        }
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::DeleteEventL()
// It will delete event from database
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupView::DeleteEventL()
    { 
    TInt cnt = iContainer->NumberOfMarkedItems();
    TInt index; 
    if(iEvtIdArray)
        iEvtIdArray->Reset();
    else
        iEvtIdArray = new(ELeave) RArray<TEvtEventId>();
    if(cnt)
        {
        EVTUIDEBUG1("CEvtMgmtUiStartupView::DeleteEventL() selected items = %d", cnt );  

        const CArrayFix<TInt>* evtarray = iContainer->MarkItems();  
        for(index=0;index<cnt;index++)
            {        
            TInt orginalIndex = iContainer->FilteredIndex((*evtarray)[index]);
            EVTUIDEBUG1("CEvtMgmtUiStartupView::DeleteEventL() listboxindexid = %d", orginalIndex );          
            EVTUIDEBUG1("Deleting event Event id = %d", EventId( orginalIndex ) );
            iEvtIdArray->Append( EventId( orginalIndex ) );
            }
        iEngine->DeleteEventsL( iEvtIdArray );
        }
    else
        {
        index = iContainer->CurrentItemIndex();
        EVTUIDEBUG1("CEvtMgmtUiStartupView::DeleteEventL() listboxindexid = %d", index );          
        if(index==KErrNotFound)
            return;        
        EVTUIDEBUG1("Deleting event Event id = %d", EventId(index) );
        iEvtIdArray->Append( EventId( index ) );
        iEngine->DeleteEventsL( iEvtIdArray );   
        }
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler::DisplayConfirmationQueryDlg()
// It shows confirmation query dialog
// -----------------------------------------------------------------------------
//
TBool CEvtMgmtUiStartupView::DisplayConfirmationQueryDlgL( const TDesC& aMsg )
    {
    TBool response= EFalse;
    CAknQueryDialog* confirmationQuery = CAknQueryDialog::NewL();     
    response = confirmationQuery->ExecuteLD(R_EVTUI_CONFIRMATION_QUERY, aMsg);         
    return response;
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::UpdateViewL()
// It will update view
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupView::UpdateViewL( )
    {     
    TInt index = iModel->UpdatedCurrentItemIndex( );
    
    iContainer->UpdateListBoxL( index );
    
    HandleCmdL( EListBoxItemFocused );
    
    if(iModel->TotalEvents())
       MenuBar()->SetMenuTitleResourceId ( R_EVTMGMTUI_STARTUP_MENUBAR_MARK );
    else
       MenuBar()->SetMenuTitleResourceId ( R_EVTMGMTUI_STARTUP_MENUBAR );
    }
	
// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::HandleCmdL()
// Derived from MEvtMgmtUiEditorExitCmdHandler
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupView::HandleCmdL( TEvtMgmtUiCmd aCommand )
	{

    switch( aCommand )
        {
        // Handle any plug-in specific internal commands here
        case EListBoxItemDoubleClicked:
            {
            if( iContainer->NumberOfMarkedItems() )
                {
                TRAPD( err, MenuBar()->TryDisplayContextMenuBarL() );
                User::LeaveIfError( err );  
                }
            else
                {
    			TInt index = iContainer->CurrentItemIndex();
    			if(index>0)
    				{
    				HandleCommandL(EEvtMgmtUiStartupViewOpen);
    				}
    			else
    			    {
                    HandleCommandL(EEvtMgmtUiStartupViewCreate);
                    }		
                }
			break;
            }
        case EListBoxItemFocused:
        		{
        		if(iIsEmbeddedApp)
        		    {
        		    if(iContainer->NumberOfMarkedItems())       
        		        {
        		        MenuBar()->SetContextMenuTitleResourceId( R_EVTMGMTUI_STARTUP_CONTEXTMENUBAR );
        		        SetCba(R_EVTMGMTUI_STARTUP_CBA_OPTIONS_EMBEDDED);
        		        }
        		    else
        		        {
        	        	if( 0 == iContainer->CurrentItemIndex() )    	        	    
        	        		SetCba(R_EVTMGMTUI_STARTUP_CBA_EMBEDDED);    	        	    
        	        	else
        	        	    SetCba(R_EVTMGMTUI_STARTUP_CBA_OPEN_EMBEDDED);
        		        }
        		    }
        		else
        		    {
        		    if(iContainer->NumberOfMarkedItems())
        		        {        		       
        		        MenuBar()->SetContextMenuTitleResourceId( R_EVTMGMTUI_STARTUP_CONTEXTMENUBAR );        		                 
                        SetCba(R_EVTMGMTUI_STARTUP_CBA_OPTIONS);
        		        }
                    else
                        {
            		    if( 0 == iContainer->CurrentItemIndex() )                     
                            SetCba(R_EVTMGMTUI_STARTUP_CBA);                        
                        else
                            SetCba(R_EVTMGMTUI_STARTUP_CBA_OPEN);
                        }
        		    }
	        	break;
	        	}
        case EScreenSizeChanged:
            {
            if( iContainer )
                {
                iContainer->SetRect( ClientRect());     
                }
            break;
            }
        case EListBoxItemDeleted:
            {
            HandleCommandL( EEvtEditorCmdDelete );
            break;
            }
        case EListBoxFilterCountChanged:
            {
            if( iContainer->FilteredCount() > 1 )
               MenuBar()->SetMenuTitleResourceId ( R_EVTMGMTUI_STARTUP_MENUBAR_MARK );
            else
               MenuBar()->SetMenuTitleResourceId ( R_EVTMGMTUI_STARTUP_MENUBAR );
            break;
            }
        default:
            {
            break;  
            }
        }
	}

// -----------------------------------------------------------------------------
// CEvtMgmtUiStartupView::SetCba
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiStartupView::SetCba( TInt aResourceId ) 
	{
	if( Cba() )
	    {
	   	TRAP_IGNORE(Cba()->SetCommandSetL(aResourceId));
	    }
	Cba()->DrawDeferred(); 
	}
		
// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::EventIdOfCurrentItem()
// It will return the event-id of the selected item 
// ---------------------------------------------------------------------------
//
TEvtEventId CEvtMgmtUiStartupView::EventIdOfCurrentItem()
    {
    TInt index = iContainer->CurrentItemIndex();
    return iModel->EventId( index );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::EventId()
// It will return the event id of event
// ---------------------------------------------------------------------------
//
TEvtEventId CEvtMgmtUiStartupView::EventId( TInt aIndex )
    {     
    return iModel->EventId( aIndex );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::MarkedEventsStatus()
// It will return type of marked events
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiStartupView::MarkedEventsStatus()
    {
    TInt index=0;
    TInt cnt = iContainer->NumberOfMarkedItems();
    const CArrayFix<TInt>* evtarray = iContainer->MarkItems();  
    
    TInt orginalIndex = iContainer->FilteredIndex((*evtarray)[index]);
    TMarkedEventsStatus status = static_cast<TMarkedEventsStatus>(iModel->EventStatus(orginalIndex));
    for( index = 1; index<cnt; index++ )
        {
        orginalIndex = iContainer->FilteredIndex((*evtarray)[index]);
        if( status != iModel->EventStatus( orginalIndex ) )
            { 
            return EMarkedEventsStatusMix;            
            }
        }
    return status;    
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::NotifyEventChangeL()
// Derived from MEvtMgmtUiEngineObserver
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupView::NotifyEventChangeL(TInt aErrorCode, TInt aState )
    {
    if( aErrorCode == KErrServerTerminated || 
        aErrorCode == KErrServerBusy ||
        aErrorCode == KErrNotReady ||
        aErrorCode == KErrNoMemory || aErrorCode == KErrDiskFull )
        {
	    TPtrC buf;
    	CTextResolver* textResolver = CTextResolver::NewLC(*iCoeEnv);
    	buf.Set(textResolver->ResolveErrorString(aErrorCode));
    	
	    CAknErrorNote* dialog = 
	        new(ELeave)CAknErrorNote(ETrue);    
	    dialog->SetTone(CAknNoteDialog::EErrorTone);
	    dialog->ExecuteLD( buf ); 
    	
    	CleanupStack::PopAndDestroy( textResolver );
    	
        EVTUIDEBUG("Exiting application as server is not ready." );
        // The User has exited the application using the Exit option from
        //.Options menu.  But, thats possible only if we exit the application
        // with EAknCmdExit.
        // A bit of convoluted logic but needed because we are using the
        // same components for the Settings UI Server as well as LC.
        
        CEikonEnv* env = CEikonEnv::Static();
       	CEikAppServer* server = env->AppServer();
		if ( server )
			{
			server->NotifyServerExit( EAknCmdExit );
			}
	    CAknAppUi* appUi = static_cast< CAknAppUi* >( env->AppUi());
	    appUi->RunAppShutter();  
        return;
        }

    if( aState == CEvtMgmtUiLbtAdapter::ELbtAdapterDelete || 
		aState == CEvtMgmtUiLbtAdapter::ELbtAdapterDeleteTriggers || 
		aState == CEvtMgmtUiLbtAdapter::ELbtAdapterSetTriggersState  || 
		aState == CEvtMgmtUiLbtAdapter::ELbtAdapterSetTriggerState )
        {
        EVTUIDEBUG1("+ CEvtMgmtUiStartupView::NotifyEventChangeL error =%d", aErrorCode ); 
           
        iWaitState = EWaitDlgDestroy;
        if(aErrorCode != KErrNone && aErrorCode != KErrCancel)
            {
            TBuf<32> err;
            err.Copy(_L("Error ="));
            err.AppendNum( aErrorCode );
            CEikonEnv::Static()->AlertWin(err);
            }      
            
        EVTUIDEBUG("- CEvtMgmtUiStartupView::NotifyEventChangeL");
        }
	

    if( aState == CEvtMgmtUiLbtAdapter::ELbtAdapterSetTriggersState  || 
		aState == CEvtMgmtUiLbtAdapter::ELbtAdapterSetTriggerState ||
    	aState == CEvtMgmtUiLbtAdapter::ELbtAdapterCreate  || 
		aState == CEvtMgmtUiLbtAdapter::ELbtAdapterUpdate ||
    	aState == CEvtMgmtUiLbtAdapter::ELbtAdapterDelete  || 
		aState == CEvtMgmtUiLbtAdapter::ELbtAdapterDeleteTriggers )
		{
        if( iModel && iContainer )
        	{
    		iModel->SetCurrentItemId( iContainer->CurrentItemIndex() );
    		if( !( aState == CEvtMgmtUiLbtAdapter::ELbtAdapterDelete  || 
					aState == CEvtMgmtUiLbtAdapter::ELbtAdapterDeleteTriggers ) )
				{
    			iModel->SetStatusFilter( CEvtMgmtUiEngine::EEvtFilterAll ); 
				}
        	}
		}
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupView::NotifyModelChangeL()
// Derived from MEvtMgmtUiEngineObserver
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupView::NotifyModelChangeL()
    {
	iEngine->UpdateModelL(iModel->EventsArray(), CEvtMgmtUiEngine::EEvtFilterAll );
	iModel->UpdateEventsStatus( );
    UpdateViewL( ); 
    }    

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::StartWaitDialogL
// ---------------------------------------------------------------------------
//  
void CEvtMgmtUiStartupView::StartWaitDialogL()
    {    
    HBufC* waitText = NULL;
    TInt cnt = iContainer->NumberOfMarkedItems();
    if(cnt>1)
        {
        if( iWaitState == EWaitDlgLaunchToDelete )
            waitText = StringLoader::LoadLC(R_LOCEV_NOTE_MULTIPLEEVT_DELETE, CEikonEnv::Static() );
        if(  iWaitState == EWaitDlgLaunchToChangeStatus && iEvtStatus == EActive )
            waitText = StringLoader::LoadLC(R_LOCEV_NOTE_MULTIPLEEVT_ACTIVATE, CEikonEnv::Static() );
        if(  iWaitState == EWaitDlgLaunchToChangeStatus && iEvtStatus == EDraft )
            waitText = StringLoader::LoadLC(R_LOCEV_NOTE_MULTIPLEEVT_DRAFT, CEikonEnv::Static() );
        }
    else
        {
        if( iWaitState == EWaitDlgLaunchToDelete )
            waitText = StringLoader::LoadLC(R_LOCEV_NOTE_SINGLEEVT_DELETE, CEikonEnv::Static() );
        if(  iWaitState == EWaitDlgLaunchToChangeStatus && iEvtStatus == EActive )
            waitText = StringLoader::LoadLC(R_LOCEV_NOTE_SINGLEEVT_ACTIVATE, CEikonEnv::Static() );
        if(  iWaitState == EWaitDlgLaunchToChangeStatus && iEvtStatus == EDraft )
            waitText = StringLoader::LoadLC(R_LOCEV_NOTE_SINGLEEVT_DRAFT, CEikonEnv::Static() );
        }
    
    CAknWaitNoteWrapper* waitNoteWrapper = CAknWaitNoteWrapper::NewL();
	// Required reinterpret_cast as CAknWaitNoteWrapper inherits privately from CActive
    CleanupStack::PushL(reinterpret_cast<CBase*>(waitNoteWrapper)); 
	
	// This is a blocking call, remember the wrapper isn't a dialog, so it doesn't need the 
	// EEikDialogFlagWait flag to make it blocking 
    waitNoteWrapper->ExecuteL(R_EVTMGMTUI_WAITDLG_CANCEL, *this, *waitText, ETrue); // this is a blocking call, remember the wrapper isn't a dialog, so it doesn't need the EEikDialogFlagWait flag to make it blocking 
       
    CleanupStack::PopAndDestroy(2); //waitText, waitNoteWrapper
    	
  	if(iDisplayNote )
      {
      iDisplayNote = EFalse;
      HBufC* warningMsg = StringLoader::LoadLC( R_EVTUI_MANDATORYFIELDS_WARNING, iCoeEnv );
      CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( *warningMsg );
      dlg->PrepareLC(R_EVTUI_MESSAGE_QUERY);
      dlg->RunLD();
      CleanupStack::PopAndDestroy(warningMsg); 
      }
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::DialogDismissedL
// ---------------------------------------------------------------------------
//  
void CEvtMgmtUiStartupView::DialogDismissedL(TInt aButtonId)
    {   
    if(aButtonId == EEikBidCancel)
        iEngine->CancelRequest();// Cancel
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::IsProcessDone
// ---------------------------------------------------------------------------
//  
TBool CEvtMgmtUiStartupView::IsProcessDone() const
    {
    if(iWaitState == EWaitDlgDestroy)
        return ETrue;
    else
        return EFalse;
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::ProcessFinished
// ---------------------------------------------------------------------------
//  
void CEvtMgmtUiStartupView::ProcessFinished()
    {      
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::StepL
// ---------------------------------------------------------------------------
//  
void CEvtMgmtUiStartupView::StepL()
    {
    if(iWaitState == EWaitDlgLaunchToDelete)
        {
        iWaitState = EWaitDlgInitiateProcess;
        DeleteEventL();        
        }
    if(iWaitState == EWaitDlgLaunchToChangeStatus)
        {
        iWaitState = EWaitDlgInitiateProcess;
        ChangeEventStatusL(iEvtStatus);        
        }
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupView::StepL
// ---------------------------------------------------------------------------
// 
TInt CEvtMgmtUiStartupView::CycleError(TInt aError)
	{ 
	if(aError != KErrNone)
		{
		CEikonEnv::Static()->AlertWin(KWaitCycleError());
		iWaitState = EWaitDlgDestroy;
		}
	return KErrNone; 
	}
	
// End of File

