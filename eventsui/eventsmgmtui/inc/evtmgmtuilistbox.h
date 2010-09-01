/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef C_EVTMGMTUILISTBOX_H
#define C_EVTMGMTUILISTBOX_H


#include <aknlists.h>


class CEvtMgmtUiListBoxModel;

/**
*  CEvtMgmtUiListBox exists because the list boxes in EventsUi 
*  application needs a specialized version of CAknFilteredTextListBoxModel 
*  called CEvtMgmtUiListBoxModel. CEvtMgmtUiListBox creates an instance of 
*  CEvtMgmtUiListBoxModel instead of the default CAknFilteredTextListBoxModel.
*/
class CEvtMgmtUiListBox : public CAknDoubleLargeStyleListBox
    {
    public: // Constructors and destructor
    
        /**
        * Symbian 2nd phase constructor. Should be called after the 
        * constructor.
        *
        * @param aParent the parent control
        * @param aFlags flags defining the behaviour of the listbox
        */
        void ConstructL(
            const CCoeControl* aParent,
            TInt aFlags);

    public: // from CAknColumnListBox

        /**
        * Creates the listbox model.
        */
        void CreateModelL();

    public: // from CEikColumnListBox

        /**
        * Returns a pointer to the listbox model.
        *
        * @return a pointer to the listbox model
        */
        CEvtMgmtUiListBoxModel* Model() const;

    };

#endif // C_EVTMGMTUILISTBOX_H

