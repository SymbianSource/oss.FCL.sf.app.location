/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: LocationPickerToolBar declaration
*
*/

#ifndef LOCATIONPICKERTOOLBAR_H
#define LOCATIONPICKERTOOLBAR_H

#include <HbToolBar>

// forward declarations
class HbAction;
class LocationPickerAppWindow;

/**  Class used to create toolbar for location picker
 *
 */
class LocationPickerToolBar : public HbToolBar
{
    Q_OBJECT
public:

    /** Constructor
      */
    LocationPickerToolBar( LocationPickerAppWindow *aWindow, QGraphicsItem *aParent = 0 );
    
    /** disable tabs if no locations
     */
    void disableTabs();

private slots:

    /** Slot triggered when All tab is selected
      */
    void AllTabTriggered();

    /** Slot triggered when Collection tab is selected
      */
    void CollectionTabTriggered();

    /** Slot triggered when Search tab is selected
      */
    void SearchTabTriggered();

private:
    // Actions defined for the tabs
    HbAction *mAllAction;
    HbAction *mCollectionAction;
    HbAction *mSearchAction;

    // pointer to the main window
    LocationPickerAppWindow* mWindow;
};


#endif // LOCATIONPICKERTOOLBAR_H
