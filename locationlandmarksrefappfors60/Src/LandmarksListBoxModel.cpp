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
*       Implements the CLandmarksContainerBase class
*
*/



#include "LandmarksListBoxModel.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
const MDesCArray* CLandmarksListBoxModel::MatchableTextArray() const
    {
    return iItemTextArray;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TInt CLandmarksListBoxModel::NumberOfItems() const
    {
    return iItemTextArray->MdcaCount();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TPtrC CLandmarksListBoxModel::ItemText(TInt aItemIndex) const
    {
    return iItemTextArray->MdcaPoint(aItemIndex);
    }


