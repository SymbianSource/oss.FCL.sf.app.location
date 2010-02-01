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
* Description:    LandmarksUi Content File -    Dialog menu observer.
*
*/








#ifndef MLMKDLGMENUOBSERVER_H
#define MLMKDLGMENUOBSERVER_H

// FORWARD DECLARATIONS
class CEikMenuPane;

// CLASS DECLARATION

/**
* MLmkDlgMenuObserver class
*/
class MLmkDlgMenuObserver
    {
    public: // New functions
        /**
        * If observer is set for a dialog then this call back method is
        * called from the dialog's DynInitMenuPaneL().
        * @param aResourceId
        * @param aMenuPane
        */
        virtual void DynInitDlgMenuPaneL( TInt aResourceId,
                                          CEikMenuPane* aMenuPane ) = 0;
    };

#endif      // MLMKDLGMENUOBSERVER_H

// End of File
