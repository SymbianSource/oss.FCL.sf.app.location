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
* Description:  Provides blid main view class methods.
*
*/


// INCLUDE FILES
#include <blid.rsg>
#include <EPos_CPosLandmarkDatabase.h>
#include <eikmenub.h> // CEikMenuBar
#include <avkon.hrh>
#include <aknViewAppUi.h>
#include <AknQueryDialog.h> 
#include <StringLoader.h>
#include <AknsConstants.h>
#include "Blid.hrh"
#include "CBlidSavedWayptsContainer.h"
#include "CBlidEng.h"
#include "CBlidDocument.h"
#include "MBlidLocation.h"
#include "CBlidAppUi.h"
#include "CBlidSavedWayptsView.h"
#include "aknlists.h"
#include "CLmkEditorDlg.h"
#include "bliduiconsts.h"


//CONSTANTS
enum TPanicCode
    {
    KBlidNullPointer = 1
    };


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidSavedWayptsView::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidSavedWayptsView* CBlidSavedWayptsView::NewLC( MKeyProcessor& aKeyProcessor )
    {
    CBlidSavedWayptsView* self = new (ELeave) CBlidSavedWayptsView(aKeyProcessor);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
// ----------------------------------------------------------------------------
// CBlidSavedWayptsView::CBlidSavedWayptsView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidSavedWayptsView::CBlidSavedWayptsView( MKeyProcessor& aKeyProcessor ):
    CBlidBaseView( aKeyProcessor )
    {
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsView::ConstructL(const TRect& aRect)
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidSavedWayptsView::ConstructL()
    {
    BaseConstructL( R_BLID_SAVEDWAYPTS_VIEW );

    iSettings = iEngine->SettingsModel();
    __ASSERT_DEBUG(iSettings,
        		   User::Panic(_L("CBlidSavedWayptsView"),
        		   KBlidNullPointer));
    }

// ----------------------------------------------------------------------------
// CBlidMainView::~CBlidSavedWayptsView
// ----------------------------------------------------------------------------
//
CBlidSavedWayptsView::~CBlidSavedWayptsView()
    {
    if ( iContainer )
        {
        static_cast<CBlidAppUi*>(AppUi())->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// ---------------------------------------------------------
// CBlidSavedWayptsView::NotifyL
// ---------------------------------------------------------
//
void CBlidSavedWayptsView::NotifyL( const TBool aOnlineMode )
    {
    if( iContainer )
        {
        iContainer->UpdateL();
        }
    CBlidBaseView::NotifyL( aOnlineMode );    
    }

// ----------------------------------------------------------------------------
// TUid CBlidSavedWayptsView::Id
// ----------------------------------------------------------------------------
//
TUid CBlidSavedWayptsView::Id() const
    {
    return TUid::Uid(EBlidSavedWayptsView);
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CBlidSavedWayptsView::HandleCommandL(TInt aCommand)
    {
    iContainer-> HandleMarkCommandL( aCommand );
    
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            static_cast<CBlidAppUi*>(AppUi())->ActivateLocalViewL( iPreviousViewid.iViewUid );
            break;
            }
        case EBlidCmdRenameWaypt:
            {
		  	if(RenameWaypointL())
            	{
                static_cast<CBlidDocument*>(AppUi()->Document())->SaveL();
                }
            break;
            }
        case EBlidCmdDeleteWaypt:
            {
            HandleWaypointsDeletionL();            
            break;
            }
        default:
            {
            static_cast<CBlidAppUi*>(AppUi())->HandleCommandL( aCommand );
            break;
            }
        } // switch
        
    if( iContainer )
        {
        iContainer->UpdateL();
        }
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsView::DoActivateL
// ----------------------------------------------------------------------------
//
void CBlidSavedWayptsView::DoActivateL(
   const TVwsViewId& aPrevViewId,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    #ifdef _DEBUG
    RDebug::Print(_L("CBlidSavedWayptsView::DoActivateL() called"));
    #endif

    if ( !iContainer )
        {
        iContainer = CBlidSavedWayptsContainer::NewL(*this, this, ClientRect(), 
            iLocation, iSettings,iRouter, *this);
        static_cast<CBlidAppUi*>(AppUi())->AddToStackL( *this, iContainer );
        iLocation->SetObserver(*this, EBlidSavedWayptsView);      
        if(!iMSKVisible)
            {
            SetMSKVisible();
            }
        MenuBar()->SetContextMenuTitleResourceId(R_BLID_SAVEDWAYPTS_VIEW_OK_MENUBAR);	
        
        #ifdef RD_SCALABLE_UI_V2
        iContainer->SetListBoxObserver(this);
        #endif // RD_SCALABLE_UI_V2        
        }
   iPreviousViewid = aPrevViewId;
   }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CBlidSavedWayptsView::DoDeactivate()
    {
    if ( iContainer )
        {
        static_cast<CBlidAppUi*>(AppUi())->RemoveFromViewStack( *this, iContainer );
        }
    delete iContainer;
    iContainer = NULL;
    }

// ---------------------------------------------------------
// CBlidSavedWayptsView::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CBlidSavedWayptsView::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    AknSelectionService::HandleMarkableListDynInitMenuPane(aResourceId, 
                                                           aMenuPane,
                                                           iContainer->iListBox);
    switch(aResourceId)
        {
        case R_BLID_SAVEDWAYPTS_VIEW_OK_MENU:
        case R_BLID_SAVEDWAYPTS_MENU:
            {
            if( iRouter->Count() <= 0 )
                {
                aMenuPane->SetItemDimmed( EBlidCmdRenameWaypt, ETrue );
                aMenuPane->SetItemDimmed( EBlidCmdDeleteWaypt, ETrue );                
                }
                            
            const CArrayFix<TInt>& indexes = *iContainer->iListBox->SelectionIndexes();
            if(indexes.Count() != 0)
            	{
                aMenuPane->SetItemDimmed( EBlidCmdRenameWaypt, ETrue );
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
// CBlidSavedWayptsView::RenameWaypointL()
// ----------------------------------------------------------------------------
//
TBool CBlidSavedWayptsView::RenameWaypointL()
    {
    TInt index(0);
    TBuf<KBlidWaypointNameMaxLen> textData;
    index = iContainer->SelectedRow();
    TNamedCoordinate currentWaypoint;
    if(index < iRouter->Count() && index >= 0)
        {
        currentWaypoint = iRouter->At(index);        
        }
    HBufC* waypointName = currentWaypoint.Name();
    textData = waypointName->Des();
    TInt errorCode(KErrNone);
    TBool returnValue(ETrue);
    TInt removedIndex;
        
    do
    	{
    	if(errorCode != KErrNone)
    		{
    		delete waypointName;
    		return EFalse;
    		}
      	CAknTextQueryDialog* dlg = new(ELeave) CAknTextQueryDialog(textData,
                                               CAknQueryDialog::ENoTone);
                                               
       	if (!dlg->ExecuteLD(R_BLID_WAYPOINT_QUERY) || 
       	    (waypointName->Compare(textData) == 0 ) )
        	{
        	delete waypointName;
           	return EFalse;           
           	}
        
      	}while( CheckIfExistL( textData, errorCode, removedIndex ) );
      	
    delete waypointName;
      	
    if ( index == KErrCancel )
    	{
        return EFalse;
        }
    
    TInt retVal(KErrNone);
    if ( index >=0 )
    	{
        //Check if CheckIfExistL has removed a waypoint below index
        if((removedIndex != -1 ) && 
           (removedIndex < index) )
        	{
        	--index;
        	}

        
        retVal = iRouter->RemoveL( index );
        if(retVal == KErrDiskFull)
        	{        	
        	returnValue = EFalse;
        	BlidNotes::OutOfMemoryNoteL();	
        	}
        }
   
    if(retVal == KErrNone)
    	{
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
	    TInt retVal = iRouter->AppendL( waypoint, ETrue );
	    if(retVal == KErrDiskFull)
	    	{
	    	returnValue = EFalse;
	        BlidNotes::OutOfMemoryNoteL();
	        }
	    CleanupStack::Pop(); //waypoint
        // Set the actual index of renamed wp
        retVal = iRouter->FindWaypointIndex( waypoint );
        if( retVal != KErrNotFound )
            {
            iContainer->iListBox->SetCurrentItemIndex(retVal);
            }
    	}    
    return returnValue;
	}

// ----------------------------------------------------------------------------
// CBlidSavedWayptsView::DeleteWaypointL()
// ----------------------------------------------------------------------------
//
TBool CBlidSavedWayptsView::DeleteWaypointL(TInt aIndex)
    {
    if ( aIndex == KErrCancel )
    	{
        return EFalse;
    	}
      
    if ( aIndex >=0 )
    	{
    	TInt retVal = iRouter->RemoveL( aIndex );        
    	if(retVal == KErrDiskFull)
        	{
        	BlidNotes::OutOfMemoryNoteL(); 
        	return EFalse;       	
        	}
        if(iRouter->Count() <= 0)
            {
            SetMSKNotVisible();            
            }
        }    
    return ETrue;
  	}


// ----------------------------------------------------------------------------
// CBlidSavedWayptsView::DeleteWaypointsL()
// ----------------------------------------------------------------------------
//
TBool CBlidSavedWayptsView::DeleteWaypointsL(const CArrayFix<TInt>& aIndexes)
    {
    const TInt count( aIndexes.Count() );
    TBool retVal = 0;
    TBool deleteOpRetVal(ETrue);
    
    if(count == 1)
        {
        TInt index = aIndexes.At( 0 );
        TDesC* name = iRouter->At( index ).Name();        
        CleanupStack::PushL(name);
        HBufC* buffer = StringLoader::LoadL(R_BLID_QUERY_DELETE_WAYPOINT,
                                            (*name),
                                            iCoeEnv);                                                    
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        CleanupStack::PushL(dlg);
        dlg->SetPromptL(buffer->Des());
        retVal = dlg->ExecuteLD(R_BLID_CONFIRMATION_QUERY);
        if( retVal )
            {
            deleteOpRetVal = DeleteWaypointL(index);
            }
        else
        	{
        	deleteOpRetVal = EFalse;
        	}
        CleanupStack::Pop(2);
        delete buffer;
        delete name;
        if((iRouter->Count() != 0) && (index == iRouter->Count()))
            {
            iContainer->iListBox->SetCurrentItemIndex(index-1);
            }
        }
    else
        {        
        HBufC* buffer = StringLoader::LoadL(R_BLID_QUERY_DELETE_WAYPOINTS,
                                        count,
                                        iCoeEnv);                                            
        CleanupStack::PushL(buffer);
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        CleanupStack::PushL(dlg);
        dlg->SetPromptL(buffer->Des());
        retVal = dlg->ExecuteLD(R_BLID_CONFIRMATION_QUERY);
        if( retVal )
            {
            TInt markedArray[KMaxNumberOfWaypoints] = {0};
            
            for ( TInt i(0 ); i < count; ++i )
            	{
                TInt ix = aIndexes.At( i );                
                markedArray[ix] = 1;
                }
            TInt waypointCount = iRouter->Count();
            for( TInt i= waypointCount-1; i >= 0; --i )
            	{
            	if(markedArray[i])
            		{
            		deleteOpRetVal = DeleteWaypointL(i);
            		if(!deleteOpRetVal)
            			{
            			break;
            			}
            		}
            	}
            }
        else
        	{
        	deleteOpRetVal = EFalse;
        	}
        CleanupStack::Pop(2); // buffer, dlg        
        delete buffer;
        TInt index = iContainer->iListBox->CurrentItemIndex();
        if( (iRouter->Count()!=0) && (index == -1) )
            {
            iContainer->iListBox->SetCurrentItemIndex(iRouter->Count() - 1);
            }
        }
    return deleteOpRetVal;
    }
    
// ----------------------------------------------------------------------------
// CBlidSavedWayptsView::HandleWaypointsDeletionL()
// ----------------------------------------------------------------------------
//
void CBlidSavedWayptsView::HandleWaypointsDeletionL()
    {
    const CArrayFix<TInt>& indexes = *iContainer->iListBox->SelectionIndexes();
    TBool retVal(ETrue);
    if (indexes.Count() == 0)
        {
        TInt index(0);
        index = iContainer->SelectedRow();
                
        TDesC* name;
        if( index < 0 )
            {
            return;
            }
        name = iRouter->At( index ).Name();
        CleanupStack::PushL(name);
        HBufC* buffer = StringLoader::LoadL(R_BLID_QUERY_DELETE_WAYPOINT,
                                            (*name),
                                            iCoeEnv);                                            
        CleanupStack::PushL(buffer);
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        CleanupStack::PushL(dlg);
        dlg->SetPromptL(buffer->Des());
        if( dlg->ExecuteLD(R_BLID_CONFIRMATION_QUERY) )
            {
            retVal = DeleteWaypointL(index);
            }
        else
        	{
        	retVal = EFalse;
        	}  
        CleanupStack::Pop(3); // buffer, dlg, name
        delete buffer;
        delete name;
        if((iRouter->Count()!=0) && (index == iRouter->Count()))
            {
            iContainer->iListBox->SetCurrentItemIndex(index-1);
            }
        }
     else
        {
        retVal = DeleteWaypointsL(indexes);
        if( retVal )
            {
            iContainer->iListBox->ClearSelection();	
            }            	
        }
    if(retVal)
    	{
    	static_cast<CBlidDocument*>(AppUi()->Document())->SaveL();
    	}    
    }


#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CBlidSavedWayptsView::HandleListBoxSelectionL
// ---------------------------------------------------------------------------
//
void CBlidSavedWayptsView::HandleListBoxSelectionL()
    {
    MenuBar()->TryDisplayContextMenuBarL();        
    }
#endif // RD_SCALABLE_UI_V2
// End of File
