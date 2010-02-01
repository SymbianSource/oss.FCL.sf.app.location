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
* Description:    LandmarksUi Content File -
*
*/








// INCLUDE FILES
#include "CLmkLandmark.h"
#include <e32math.h>


// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CLmkLandmark::CLmkLandmark()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
// CLmkLandmark::CLmkLandmark()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkLandmark::CLmkLandmark()
{
}


// -----------------------------------------------------------------------------
// CLmkLandmark::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkLandmark::ConstructL(const CPosLandmark* aLandmark,
                     const RPointerArray<CPosLandmarkCategory>& aCategories)
    {
    iLandmark = CPosLandmark::NewL(*aLandmark);
    TInt count = aCategories.Count();
    for (TInt i(0); i < count ; i++)
        {
        CPosLandmarkCategory* category = CPosLandmarkCategory::NewL(*aCategories[i]);
        CleanupStack::PushL(category);
        User::LeaveIfError(iCategories.Append(category));
        CleanupStack::Pop();//category
        }
    }


// -----------------------------------------------------------------------------
// CLmkLandmark::NewL
// Two-phased copy constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkLandmark* CLmkLandmark::NewL(
    const CPosLandmark*  aLandmark,
    const RPointerArray<CPosLandmarkCategory>& aCategories)
    {
    CLmkLandmark* self = new (ELeave) CLmkLandmark();
    CleanupStack::PushL(self);
    self->ConstructL(aLandmark,aCategories);
    CleanupStack::Pop();
    return self;
    }

EXPORT_C CLmkLandmark* CLmkLandmark::NewL(
    const CLmkLandmark&  aLandmark)
    {
    CLmkLandmark* self = new (ELeave) CLmkLandmark();
    CleanupStack::PushL(self);
    self->ConstructL(aLandmark.iLandmark,aLandmark.iCategories);
    CleanupStack::Pop();
    return self;
    }
// Destructor
CLmkLandmark::~CLmkLandmark()
    {
    delete iLandmark;
    iCategories.ResetAndDestroy();
    }

EXPORT_C CPosLandmark* CLmkLandmark::PosLandmark()
    {
    return iLandmark;
    }
EXPORT_C RPointerArray<CPosLandmarkCategory>& CLmkLandmark::Categories()
    {
    return iCategories;
    }

//  End of File
