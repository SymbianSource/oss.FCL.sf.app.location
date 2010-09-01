/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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








// System Include
#include <centralrepository.h>
#include <locnotprefplugindomaincrkeys.h>
#include <lmkerrors.h>

// INCLUDE FILES
#include "clmkcentralrepository.h"

// CONSTANTS
const TUint32 KNullKey = 0;
#if defined(_DEBUG)
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkCentralRepository");

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif


// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CLmkCentralRepository::CLmkCentralRepository()
// ---------------------------------------------------------------------------
//
CLmkCentralRepository::CLmkCentralRepository( TUint32 aKey ,
        					MLmkCentralRepositoryObserver*	aObserver)
	:CActive( EPriorityStandard ),
	iKey( aKey ),
	iObserver( aObserver )
	{
	}

// ---------------------------------------------------------------------------
// CLmkCentralRepository::~CLmkCentralRepository()
// ---------------------------------------------------------------------------
//
CLmkCentralRepository::~CLmkCentralRepository()
	{
	Cancel();
	delete iRepository;
	iRepository = NULL;
	}

// ---------------------------------------------------------------------------
// CLmkCentralRepository::NewL()
// ---------------------------------------------------------------------------
//
CLmkCentralRepository* CLmkCentralRepository::NewL()
	{
    CLmkCentralRepository* self =
                        CLmkCentralRepository::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CLmkCentralRepository::NewLC()
// ---------------------------------------------------------------------------
//
CLmkCentralRepository* CLmkCentralRepository::NewLC()
	{
    CLmkCentralRepository* self =
                        new(ELeave) CLmkCentralRepository( KNullKey,NULL );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CLmkCentralRepository::NewL()
// ---------------------------------------------------------------------------
//
CLmkCentralRepository* CLmkCentralRepository::NewL( TUint32 aKey ,
        					MLmkCentralRepositoryObserver*	aObserver)
	{
    CLmkCentralRepository* self =
                        CLmkCentralRepository::NewLC( aKey, aObserver);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CLmkCentralRepository::NewLC()
// ---------------------------------------------------------------------------
//
CLmkCentralRepository* CLmkCentralRepository::NewLC( TUint32 aKey ,
        					MLmkCentralRepositoryObserver*	aObserver)
	{
    CLmkCentralRepository* self =
                        new(ELeave) CLmkCentralRepository( aKey,aObserver );
	CleanupStack::PushL(self);
	self->Construct2L();
	return self;
	}

// ---------------------------------------------------------------------------
// CLmkCentralRepository::Construct2L()
// ---------------------------------------------------------------------------
//
void CLmkCentralRepository::Construct2L()
	{
	ConstructL();
	StartNotification();
	}
// ---------------------------------------------------------------------------
// CLmkCentralRepository::ConstructL()
// ---------------------------------------------------------------------------
//
void CLmkCentralRepository::ConstructL()
	{
	CActiveScheduler::Add( this );
	//creates central repository
	iRepository = CRepository::NewL(TUid::Uid( KCRUidLocNotationPref ));
	}

// --------------------------------------------------------------------------
// CLmkCentralRepository::RunL()
// ---------------------------------------------------------------------------
//
void CLmkCentralRepository::RunL()
    {
    // Issue Notification
    iObserver->HandleCentralRepositoryChangeL();

    // Schedule for listening to change events again
    StartNotification();
    }

// --------------------------------------------------------------------------
// CLmkCentralRepository::DoCancel()
// ---------------------------------------------------------------------------
//
void CLmkCentralRepository::DoCancel()
    {
    if( iKey != KNullKey)
	    {
	     // Cancel the outstanding CR notification request
    	iRepository->NotifyCancel( iKey );
	    }
    }

// --------------------------------------------------------------------------
// CLmkCentralRepository::StartNotification()
// ---------------------------------------------------------------------------
//
TInt CLmkCentralRepository::StartNotification()
    {
    __ASSERT_DEBUG( iObserver, Panic( KLmkPanicNullMember ) );
    __ASSERT_DEBUG( iKey != KNullKey, Panic( KLmkPanicKeyNotSet ) );
    TInt err = KErrNone;
    if( !IsActive())
        {
        iStatus = KRequestPending;
        // Request for notification
        err = iRepository->NotifyRequest( iKey, iStatus);
        if( KErrNone == err )
	        {
	        SetActive();
	        }
        }
    return err;
    }

// --------------------------------------------------------------------------
// CLmkCentralRepository::GetKeyValue()
// ---------------------------------------------------------------------------
//
TInt CLmkCentralRepository::GetKeyValue(TUint32 aKey, TInt& aValue)
	{
	return iRepository->Get( aKey,aValue );
	}
// End of File

