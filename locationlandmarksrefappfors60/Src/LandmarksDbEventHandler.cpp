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
* Description:  Implements the class CLandmarksDbEventHandler
*
*/



#include <EPos_CPosLandmarkDatabase.h>

#include "LandmarksDbEventHandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksDbEventHandler::CLandmarksDbEventHandler(
    CPosLandmarkDatabase& aDb)
: CActive(CActive::EPriorityStandard), iDb(aDb)
    {
    CActiveScheduler::Add(this);
    ObserveEvents();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksDbEventHandler::~CLandmarksDbEventHandler()
    {
    Cancel();
    iDbObservers.Close();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksDbEventHandler::AddObserverL(
    MLandmarksDbObserver* aObserver)
    {
    User::LeaveIfError(iDbObservers.Append(aObserver));
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksDbEventHandler::RunL()
    {
    for (TInt i = 0; i < iDbObservers.Count(); i++)
        {
        // All observers must be notified. We cannot allow the observers to 
        // leave.
        TInt err;
        TRAP(err, iDbObservers[i]->NotifyDbEventL(iEvent, iStatus.Int()));
        }
    ObserveEvents();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksDbEventHandler::DoCancel()
    {
    iDb.CancelNotifyDatabaseEvent();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TInt CLandmarksDbEventHandler::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksDbEventHandler::ObserveEvents()
    {
    iStatus = KRequestPending;
    iDb.NotifyDatabaseEvent(iEvent, iStatus);
    SetActive();
    }


