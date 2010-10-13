/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Provides LM UI class methods.
*
*/







// INCLUDE FILES
#include "LmkUID.h"
#include "CLmkAppUi.h"
#include "landmarks.hrh"
#include "CLmkDocument.h"
#include "CLmkByLmView.h"
#include "CLmkByCategoryView.h"
#include "CLmkCategorySettingsView.h"
#include "CLmkCategoryContentsView.h"
#include "CLmkSender.h"
#include "Debug.h"
#include <AknsUtils.h>
#include <hlplch.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <AknQueryDialog.h>
#include <Landmarks.rsg>
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkAppUi");

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CLmkAppUi::ConstructL
// ----------------------------------------------------------
//
void CLmkAppUi::ConstructL()
    {
    DEBUG( CLmkAppUi::ConstructL entered );

    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible);

    TRAPD( err, iLmkSender = CLmkSender::NewL( Document().LmDbL() ));
    DEBUG1( CLmkAppUi::ConstructL err=%d,err );
    if(err == KErrLocked)
    	{
	    HBufC* noteText = StringLoader::LoadL( R_LMK_ERR_EIKON_GENERAL, iEikonEnv );
	    CleanupStack::PushL( noteText );
	    CAknErrorNote* dialog = new(ELeave)CAknErrorNote( ETrue );
	    dialog->SetTone( CAknNoteDialog::EErrorTone );
	    dialog->ExecuteLD( *noteText );
	    CleanupStack::PopAndDestroy( noteText ); //noteText
	    User::Exit(0);
    	}

    DEBUG( CLmkAppUi::ConstructL: creating views );

    CLmkBaseView* byLmView = CLmkByLmView::NewLC( *this, *iLmkSender );

    AddViewL( byLmView ); // transfer ownership to CAknViewAppUi
    CleanupStack::Pop(); // byLmView

    CLmkBaseView* byCategoryView = CLmkByCategoryView::NewLC( *this );

    AddViewL( byCategoryView ); // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // byCategoryView

    CLmkBaseView* categorySettingsView =
        CLmkCategorySettingsView::NewLC( *this );


    AddViewL( categorySettingsView );
    CleanupStack::Pop();

    CLmkBaseView* categoryContentsView =
        CLmkCategoryContentsView::NewLC( *this, *iLmkSender );

    AddViewL( categoryContentsView );
    CleanupStack::Pop();

    SetDefaultViewL( *byLmView );

    DEBUG( CLmkAppUi::ConstructL finished );
    }

// ----------------------------------------------------
// CLmkAppUi::~CLmkAppUi
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CLmkAppUi::~CLmkAppUi()
    {
    delete iLmkSender;
    }

// ----------------------------------------------------
// CLmkAppUi::HandleCommandL
// ----------------------------------------------------
//
void CLmkAppUi::HandleCommandL( TInt aCommand )
    {
    DEBUG1( CLmkAppUi::HandleCommandL entered aCommand=%d, aCommand);
    switch ( aCommand )
        {
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                                 AppHelpContextL() );
            break;
            }
        // these all are same: exit
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case ELmkCmdEditCats:
            {
            ActivateLocalViewL( TUid::Uid( ELmkCategorySettingsView ) );
            break;
            }

        default:
            break;
        }
    DEBUG( CLmkAppUi::HandleCommandL end );
    }

// ----------------------------------------------------
// CLmkAppUi::Document
// ----------------------------------------------------
//
CLmkDocument& CLmkAppUi::Document() const
    {
    CLmkDocument* document =
        static_cast<CLmkDocument*>( CEikAppUi::Document() );
    __ASSERT_DEBUG( document, Panic( KLmkPanicNullMember ) );
    // Explicit cast: the document must always be of type CLmkDocument
    return *document;
    }

// ----------------------------------------------------
// CLmkAppUi::LmkSender
// ----------------------------------------------------
//
CLmkSender& CLmkAppUi::LmkSender() const
    {
    __ASSERT_DEBUG( iLmkSender, Panic( KLmkPanicNullMember ) );
    return *iLmkSender;
    }

// ----------------------------------------------------
// CLmkAppUi::TabChangedL
// ----------------------------------------------------
//
void CLmkAppUi::TabChangedL(TInt aIndex)
    {

    // Gets a subpane control by ID
    CAknNavigationControlContainer* naviCtrlContainer =
        static_cast<CAknNavigationControlContainer*>
        ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    __ASSERT_DEBUG( naviCtrlContainer, Panic( KLmkPanicNullMember ) );

    // Return topmost object from navigation pane's object stack.
    CAknNavigationDecorator* decorator = naviCtrlContainer->Top();
    __ASSERT_DEBUG( decorator && decorator->ControlType() ==
                    CAknNavigationDecorator::ETabGroup,
                    Panic( KLmkPanicNullMember ) );

    CAknTabGroup* tabGroup =
        static_cast< CAknTabGroup* > ( decorator->DecoratedControl() );
    __ASSERT_DEBUG( tabGroup && aIndex >= 0 && aIndex < tabGroup->TabCount(),
                    Panic( KLmkPanicNullMember ) );
    ActivateLocalViewL( TUid::Uid( tabGroup->TabIdFromIndex( aIndex ) ) );
    }

// ----------------------------------------------------
// CLmkAppUi::ProcessKeyEventL
// ----------------------------------------------------
//
TBool CLmkAppUi::ProcessKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CLmkAppUi::TabObserver
// ----------------------------------------------------
//
MAknTabObserver* CLmkAppUi::TabObserver()
    {
    return this;
    }

// ----------------------------------------------------
// CLmkAppUi::HandleViewDeactivation
// ----------------------------------------------------
//
void CLmkAppUi::HandleViewDeactivation(const TVwsViewId &aViewIdToBeDeactivated,
                                            const TVwsViewId &    aNewlyActivatedViewId  )
    {
    // closing all intermediate state events
	iAvkonEnv->CloseAllIntermediateStates();

    if( aNewlyActivatedViewId.iAppUid ==  TUid::Uid( KLmkAppUID3 ) )
        {
        	if ( aNewlyActivatedViewId.iViewUid  == TUid::Uid( ELmkByCategoryView )  && aViewIdToBeDeactivated.iViewUid  == TUid::Uid( ELmkByLmView))
        	{
        	    // Gets a subpane control by ID
        	    CAknNavigationControlContainer* naviCtrlContainer = NULL;
            TRAP_IGNORE(naviCtrlContainer =
                static_cast<CAknNavigationControlContainer*>
                ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) ));

            // Return topmost object from navigation pane's object stack.
            CAknNavigationDecorator* decorator = naviCtrlContainer->Top();
            CAknTabGroup* tabGroup =
                static_cast< CAknTabGroup* > ( decorator->DecoratedControl() );

        		if(tabGroup)
        		{
        			tabGroup->SetActiveTabById(ELmkByCategoryView);
        			tabGroup->DrawDeferred();
        			decorator->DrawDeferred();
        		}
        	}

        	if( aNewlyActivatedViewId.iViewUid  == TUid::Uid( ELmkByLmView) && aViewIdToBeDeactivated.iViewUid  == TUid::Uid( ELmkByCategoryView) )
        	{
        	    // Gets a subpane control by ID
        		CAknNavigationControlContainer* naviCtrlContainer = NULL;
            TRAP_IGNORE(naviCtrlContainer =
                static_cast<CAknNavigationControlContainer*>
                ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) ) );

            // Return topmost object from navigation pane's object stack.
            CAknNavigationDecorator* decorator = naviCtrlContainer->Top();
            CAknTabGroup* tabGroup =
                static_cast< CAknTabGroup* > ( decorator->DecoratedControl() );
        		if(tabGroup)
        		{
        			tabGroup->SetActiveTabById(ELmkByLmView);
        			tabGroup->DrawDeferred();
        			decorator->DrawDeferred();
        		}
        	}
        }
    }

// End of File
