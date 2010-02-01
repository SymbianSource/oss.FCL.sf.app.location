/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains methofs which wraps functionality related
*                to landmark database initialization into a simpler interface.
*
*/







// INCLUDE FILES
#include "Debug.h"
#include "CLmkDbUtils.h"
#include "CLmkDbInitializer.h"
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
// We wrap two async. tasks (db initialize and compress) into one
// using estimated 0.2/0.8 progress distribution:
const TReal32 KLmkInitMaxProgress( TReal32( 0.2 ) );

#if defined(_DEBUG)
_LIT( KPanicMsg, "CLmkDbInitializer" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkDbInitializer::CLmkDbInitializer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDbInitializer::CLmkDbInitializer(
    MLmkAOOperationObserver& aObserver,
    TBool aProgressNote )
    : iObserver( aObserver ),
      iProgressNote( aProgressNote )
    {
    }

// -----------------------------------------------------------------------------
// CLmkDbInitializer::NewL
// -----------------------------------------------------------------------------
//
CLmkDbInitializer* CLmkDbInitializer::NewL(
    CPosLandmarkDatabase& aDb,
    MLmkAOOperationObserver& aObserver,
    TBool aProgressNote )
	{
	CLmkDbInitializer* self =
        new ( ELeave ) CLmkDbInitializer( aObserver, aProgressNote );
	CleanupStack::PushL( self );
	self->ConstructL( aDb );
	CleanupStack::Pop(); //self
    return self;
	}

// -----------------------------------------------------------------------------
// CLmkDbInitializer::ConstructL
// -----------------------------------------------------------------------------
//
void CLmkDbInitializer::ConstructL( CPosLandmarkDatabase& aDb )
	{
    iDbUtils = CLmkDbUtils::NewL( aDb, *this, iProgressNote );
	}

// -----------------------------------------------------------------------------
// CLmkDbInitializer::~CLmkDbInitializer
// -----------------------------------------------------------------------------
//
CLmkDbInitializer::~CLmkDbInitializer()
	{
	delete iDbUtils; // also cancels possibly ongoing operation
	}

// -----------------------------------------------------------------------------
// CLmkDbInitializer::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkDbInitializer::HandleOperationL(
TOperationTypes aType,
TReal32 aProgress,
TInt aStatus )
	{
	DEBUG2( CLmkDbInitializer::HandleOperationL entered: progress=%d status=%d,
	    aProgress, aStatus );

	__ASSERT_DEBUG( aType == EInitialize,
	            Panic( KLmkPanicUnknownOperation ) );

	// Modify progress information because we have wrapped two tasks into one:
	if ( aType == EInitialize && aStatus == KPosLmOperationNotComplete )
		{
		aProgress = aProgress * KLmkInitMaxProgress;
		}

	if( iProgressNote || aStatus != KPosLmOperationNotComplete )
		{
		// Caller either requested to receive all events, or we
		// have completed entirely and inform that to the caller.

		/*
		 *In the called method 'aProgress' is not used
		 *This code change of passing '0' instead of aPrgress is
		 *because of compiler error for winscw-urel target
		 *and this is a temparary fix
		 */
		//iObserver.HandleOperationL( EInitAndCompact, aProgress, aStatus );
		iObserver.HandleOperationL( EInitialize, 0, aStatus );
		}
	}

// -----------------------------------------------------------------------------
// CLmkDbInitializer::StartInitAndCompactL
// -----------------------------------------------------------------------------
//
void CLmkDbInitializer::StartInitAndCompactL()
	{
	DEBUG( CLmkDbInitializer::StartInitAndCompactL entered );
	iDbUtils->InitializeDbL();
	}

//  End of File
