/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  List box class for Events Management UI.
*
*/

#include "evtmgmtuilistbox.h"
#include "evtmgmtuilistboxmodel.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiListBox::ConstructL(
    const CCoeControl* aParent,
    TInt aFlags)
    {
    // Continue construction of the object.
    CEikFormattedCellListBox::ConstructL(aParent, aFlags);
    
    if(iModel)
        {
        delete iModel;
        iModel = NULL;
        }
    
    // Create a specialized model.
    iModel = CEvtMgmtUiListBoxModel::NewL();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiListBox::CreateModelL()
    {
    // Do nothing. Model is already constructed in constructor.
    // This method must be overridden to prevent base class to recreate a new, 
    // for our purpuses, non-suitable model.
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CEvtMgmtUiListBoxModel* CEvtMgmtUiListBox::Model() const
    {
    return static_cast <CEvtMgmtUiListBoxModel*> (iModel);
    }


