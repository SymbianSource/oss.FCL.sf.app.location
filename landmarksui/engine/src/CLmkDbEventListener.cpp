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
* Description:    LandmarksUi Content File -
*
*/




// INCLUDE FILES
#include "CLmkDbEventListener.h"
#include "MLmkDbObserver.h"
#include <EPos_CPosLandmarkDatabase.h>
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KLmkEventObserversGranularity( 5 );

#if defined(_DEBUG)
_LIT( KPanicMsg, "CLmkDbEventListener" );
void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkDbEventListener::CLmkDbEventListener
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDbEventListener::CLmkDbEventListener()
    : CActive( EPriorityStandard ),
      iObservers( KLmkEventObserversGranularity )
    {
    }

// -----------------------------------------------------------------------------
// CLmkDbEventListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkDbEventListener::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CLmkDbEventListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkDbEventListener* CLmkDbEventListener::NewL()
    {
    CLmkDbEventListener* self =
        new ( ELeave ) CLmkDbEventListener();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkDbEventListener::~CLmkDbEventListener
// -----------------------------------------------------------------------------
//
CLmkDbEventListener::~CLmkDbEventListener()
    {
    Cancel();
    iObservers.Close();
    delete iDbURI;
    }

// -----------------------------------------------------------------------------
// CLmkDbEventListener::AddObserverL
// -----------------------------------------------------------------------------
//
void CLmkDbEventListener::AddObserverL(
    MLmkDbObserver& aObserver,
    CPosLandmarkDatabase& aDb )
    {
    HBufC* paramURI = aDb.DatabaseUriLC();
    if ( iDb )
        { // Only one database currently supported
        if ( iDbURI->Compare( *paramURI ) != 0 )
            {
            User::Leave( KErrNotSupported );
            }
        }

    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        {
        User::Leave( KErrAlreadyExists );
        }

    User::LeaveIfError( iObservers.Append( &aObserver ) );
    if ( iObservers.Count() == 1 )
        {
        iDb = &aDb;
        StartListening();
        }

    // For simplicity we always assign new value to iDbURI although often
    // the content remains the same as it was.
    delete iDbURI;
    iDbURI = paramURI;
    CleanupStack::Pop(); // paramURI, ownership was transferred
    }

// -----------------------------------------------------------------------------
// CLmkDbEventListener::RemoveObserver
// -----------------------------------------------------------------------------
//
void CLmkDbEventListener::RemoveObserver( MLmkDbObserver& aObserver )
    {
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        {
        iObservers.Remove( index );
        }
    if ( iObservers.Count() == 0 )
        {
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CLmkDbEventListener::StartListening
// -----------------------------------------------------------------------------
//
void CLmkDbEventListener::StartListening()
    {
    iEvent.iEventType = EPosLmEventUnknownChanges;
    iEvent.iLandmarkItemId = KPosLmNullItemId;
    __ASSERT_DEBUG( !IsActive(), Panic( KLmkPanicAlreadyActive ) );
    __ASSERT_DEBUG( iDb, Panic( KLmkPanicNullMember ) );
    iDb->NotifyDatabaseEvent( iEvent, iStatus );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CLmkDbEventListener::RunL
// -----------------------------------------------------------------------------
//
void CLmkDbEventListener::RunL()
    {
    // No need for implementing RunError() as long as RunL() is non-leaving.
    for ( TInt i( 0 ); i < iObservers.Count(); ++i )
        {
        iObservers[i]->HandleDatabaseEvent( iEvent );
        }
    StartListening();
    }

// -----------------------------------------------------------------------------
// CLmkDbEventListener::DoCancel
// -----------------------------------------------------------------------------
//
void CLmkDbEventListener::DoCancel()
    {
    if ( iDb )
        {
        iDb->CancelNotifyDatabaseEvent();
        // The policy is that when database observation is stopped we
        // release the database association. It may become invalid after that.
        iDb = NULL;
        delete iDbURI;
        iDbURI = NULL;
        }
    }

//  End of File
