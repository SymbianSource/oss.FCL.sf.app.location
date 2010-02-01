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
#include "CLmkEventListenerSingleton.h"
#include "CLmkDbEventListener.h"
#include "LmkConsts.h"
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkEventListenerSingleton" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// -----------------------------------------------------------------------------
// CLmkEventListenerSingleton::CLmkEventListenerSingleton
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkEventListenerSingleton::CLmkEventListenerSingleton()
    : CCoeStatic( KLmkEventListenerSingletonUid )
    {
    }

// -----------------------------------------------------------------------------
// CLmkEventListenerSingleton::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkEventListenerSingleton::ConstructL()
    {
    __ASSERT_DEBUG( !iEventListener, Panic( KLmkPanicIllegalConstruction ) );
    iEventListener = CLmkDbEventListener::NewL();
    }

// -----------------------------------------------------------------------------
// CLmkEventListenerSingleton::~CLmkEventListenerSingleton
// -----------------------------------------------------------------------------
//
CLmkEventListenerSingleton::~CLmkEventListenerSingleton()
    {
    delete iEventListener;
    }

// -----------------------------------------------------------------------------
// CLmkEventListenerSingleton::AddObserverL
// -----------------------------------------------------------------------------
//
void CLmkEventListenerSingleton::AddObserverL(
    MLmkDbObserver& aObserver,
    CPosLandmarkDatabase& aDb )
    {
	CLmkEventListenerSingleton* singleton =
        static_cast<CLmkEventListenerSingleton*>(
            CCoeEnv::Static( KLmkEventListenerSingletonUid ) );

	if ( !singleton )
		{
		singleton = new ( ELeave ) CLmkEventListenerSingleton();
        // CCoeEnv takes the ownership, but we must ensure that
        // construction is complete, otherwise singleton can't be used.
        CleanupStack::PushL( singleton );
        singleton->ConstructL();
        CleanupStack::Pop( singleton );
		}

    singleton->iEventListener->AddObserverL( aObserver, aDb );
    }

// -----------------------------------------------------------------------------
// CLmkEventListenerSingleton::RemoveObserver
// -----------------------------------------------------------------------------
//
void CLmkEventListenerSingleton::RemoveObserver(
    MLmkDbObserver& aObserver )
    {
	CLmkEventListenerSingleton* singleton =
        static_cast<CLmkEventListenerSingleton*>(
            CCoeEnv::Static( KLmkEventListenerSingletonUid ) );
	if ( singleton )
        {
        singleton->iEventListener->RemoveObserver( aObserver );
        }
    // If singleton does not exist observer removal is unnecessary
    }

//  End of File
