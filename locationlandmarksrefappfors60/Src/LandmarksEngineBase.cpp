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
* Description:  Implements the CLandmarksEngineBase class
*
*/



#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLandmarkSearch.h>

#include "LandmarksEngineBase.h"
#include "LandmarksLmOpWrapper.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksEngineBase::CLandmarksEngineBase(
    CPosLandmarkDatabase& aDb) 
:   CActive(CActive::EPriorityStandard), 
    iDb(aDb)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEngineBase::ConstructL()
    {
    iSearcher = CPosLandmarkSearch::NewL(iDb);
    iLmOpWrapper = new (ELeave) CLandmarksLmOpWrapper();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksEngineBase::~CLandmarksEngineBase()
    {
    delete iLmOpWrapper;
    delete iSearcher;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEngineBase::SetPrio(CActive::TPriority aPriority)
    {
    iPriority = aPriority;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEngineBase::SetPriorityAndSetActive()
    {
    if (iPriority != Priority())
        {
        CActive::SetPriority(iPriority);
        }
    SetActive();
    }

  
