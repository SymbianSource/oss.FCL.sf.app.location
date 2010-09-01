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
*     See class description below
*
*/


#ifndef __LANDMARKS_APPUI_H__
#define __LANDMARKS_APPUI_H__


#include "LandmarksOperationObserver.h"
#include <aknViewAppUi.h>
#include <AknProgressDialog.h>
#include <AknTabObserver.h> 


class CLandmarksApplicationEngine;
class CLandmarksCategoriesView;
class CAknNavigationDecorator;
class CLandmarksListView;
class CAknProgressDialog;

class CLandmarksAppUi : 
    public CAknViewAppUi, 
    public MLandmarksOperationObserver, 
    public MAknTabObserver,
    public MProgressDialogCallback
    {
    public:
        CLandmarksAppUi();
        void ConstructL();
        ~CLandmarksAppUi();
        
    public: // from CAknViewAppUi

        void HandleCommandL(TInt aCommand);

        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType);
        
        void HandleResourceChangeL( TInt aType );

    public: // from MLandmarksOperationObserver

        /**
        * NotifyOperationProgressL notifies the progress and status of an
        * asynchronous operation.
        *  
        * @param aOperation the type of operation that is reported.
        * @param aProgress the progress of the operation.
        * @param aErrorCode the status of the operation.
        */
        void NotifyOperationProgressL(
            TOperation aOperation, 
            TInt aProgress, 
            TInt aErrorCode);

    public: // from MAknTabObserver
        
        /** Called by NaviDecorator when tab is changed
        * @param aIndex new tab seleected
        */
        void TabChangedL(TInt aIndex);

    public: // from MProgressDialogCallback

        /**
        * Called when the dialog has been dismissed.
        * 
        * @param aButtonId the button that was pressed to dismiss the dialog.
        */
        void DialogDismissedL(TInt aButtonId);

    private:

        /**
        * Performs the construction phase of this object that is dependent 
        * that the application engine is initialized.
        */
        void PostConstructL();

        /**
        * Launches a progress dialog displayed when the default landmark 
        * database is initialized.
        */
        void LaunchProgressDialogL();

    private: // Data

        //! decorates the navi pane (with navigation arrows etc)
        CAknNavigationDecorator* iNaviDecorator;

        //! the application engine
        CLandmarksApplicationEngine* iEngine;

        /** displays a progress bar when initializing the 
            default landmark database. */
        CAknProgressDialog* iProgressDialog;

        /** indicates if exit has been pressed before the 
            progressDialog is fully deleted */
        TBool iDelayedExit;
        
    };

#endif // __LANDMARKS_APPUI_H__

