/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*       Implements the CLandmarksView class
*
*/



#include <eikmenup.h>
#include <aknViewAppUi.h>
#include <aknnavide.h>

#include <lbspositioninfo.h>
#include <epos_poslandmarkserialization.h>

#include <lmrefapp.rsg>
#include "LmRefApp.hrh"
#include "LandmarksView.h"
#include "LandmarksAppUi.h"
#include "LandmarksApplicationEngine.h"
#include "LandmarksContainer.h"
#include "LandmarksEditDialog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksView::CLandmarksView( 
    CAknNavigationDecorator* aNaviDecorator,
    CLandmarksApplicationEngine& aEngine )
:   iEngine( aEngine ), 
    iNaviDecorator( aNaviDecorator )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksView::ConstructL()
    {
    BaseConstructL( R_LMREFAPP_VIEW );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksView* CLandmarksView::NewLC( 
    CAknNavigationDecorator* aNaviDecorator,
    CLandmarksApplicationEngine& aEngine )
    {
    CLandmarksView* self = new ( ELeave ) CLandmarksView( 
        aNaviDecorator, aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksView::~CLandmarksView()
    {
    delete iContainer;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TUid CLandmarksView::Id() const
    {
    return TUid::Uid( ELandmarksViewId );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksView::HandleCommandL( TInt aCommand )
    {
        {
        switch ( aCommand )
            {
            case ELandmarksAddCurrent:
                AddLandmarkL( ETrue ); // use current location
                break;

            case ELandmarksAddBlank:
                AddLandmarkL( EFalse ); // create blank
                break;

            case ELandmarksDelete:
                iContainer->DeleteLandmarkL();
                break;

            case ELandmarksOpen:
                {
                TPckgC<TPosLmItemId> lmId( iContainer->CurrentLandmark() );
                
                AppUi()->ActivateLocalViewL( 
                    TUid::Uid( ELandmarksInfoViewId ),
                    TUid::Uid( ELandmarksViewInfoById ),
                    lmId );
                break;
                }

            default:
                AppUi()->HandleCommandL( aCommand );
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksView::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    if ( !iContainer )
        {
        iContainer = new ( ELeave ) CLandmarksContainer( 
            *this, iEngine, iNaviDecorator );
        iContainer->SetMopParent( this );
	    iContainer->ConstructL( ClientRect() );
        }
   
    // Enable receiving of keyboard events.
    AppUi()->AddToStackL( *this, iContainer );

    // Make view visible.
    iContainer->MakeVisible( ETrue );

    // Notify that this view is active
    iEngine.NotifyViewActivated( Id(), ETrue );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksView::DoDeactivate()
    {
    // Hide view.
    iContainer->MakeVisible( EFalse );

    // Notify that this view is inactive
    iEngine.NotifyViewActivated( Id(), EFalse );

    // Disable receiving keyboard events.
    AppUi()->RemoveFromStack( iContainer );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksView::HandleResourceChange( TInt aType )
    {
    if ( iContainer )
        {
        iContainer->HandleResourceChange( aType );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksView::DynInitMenuPaneL( 
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
        {
        if ( aResourceId == R_LMREFAPP_MENU )
            {
            // If no landmark is selected we need to hide some command options.
            TBool isListEmpty  = !iContainer->IsItemSelected();
            aMenuPane->SetItemDimmed( ELandmarksDelete, isListEmpty );
            aMenuPane->SetItemDimmed( ELandmarksOpen, isListEmpty );
            }
        }

    CAknView::DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksView::AddLandmarkL( TBool aUseCurrentLocation )
    {
    // Create new landmark
    CPosLandmark* newLandmark = CPosLandmark::NewLC();

    if ( EditNewLandmarkL( *newLandmark, aUseCurrentLocation ) )
        {
        iEngine.AddLandmarkL( *newLandmark );
        }

    CleanupStack::PopAndDestroy( newLandmark );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CLandmarksView::EditNewLandmarkL( CPosLandmark& aLandmark, TBool aFetchCurrentLocation )
    {
    // Create edit dialog
    TBool isEdited( EFalse );
    CLandmarksEditDialog* editDialog = CLandmarksEditDialog::NewLC( 
        aLandmark, aFetchCurrentLocation, isEdited, iEngine );

    // Remove tabs from this view
    iNaviDecorator->MakeVisible( EFalse );

    // Dialog must not leave without restoring tab group. Must be trapped.
    TRAPD( err, editDialog->ExecuteLD( R_LMREFAPP_EDIT_DIALOG ) );

    // Restore tabs to this view
    iNaviDecorator->MakeVisible( ETrue );

    User::LeaveIfError( err );
    CleanupStack::Pop( editDialog );
    return isEdited;
    }

