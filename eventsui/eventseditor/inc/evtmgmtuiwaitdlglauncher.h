/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Encapuslates the Wait dialog in an Active Object interface 
*
*/


#ifndef EVTMGMTUIWAITDLGLAUNCHER_H
#define EVTMGMTUIWAITDLGLAUNCHER_H

// System Includes
#include <AknWaitDialog.h>
#include <AknProgressDialog.h>
#include <e32base.h>

// Class Declaration
/**
* MLmkDlgMenuObserver class 
*/
class MEvtMgmtUiWaitDialogLauncherObserver
    {
    public: // New functions
        virtual void HandleDialogDismissed( TInt aButtonId ) = 0;
    };
/**
 * Class used to encapsulate the AknWaitDialog in an Active object interface.
 * Provides the functionality to launch the Wait dialog in an asynchronous mode
 * so that the current running context is not blocked.
 */
NONSHARABLE_CLASS ( CEvtMgmtUiWaitDialogLauncher ) : public CActive,
                                                     public MProgressDialogCallback
    {
    public:
        /**
         * Two phased constructor for creating the Wait dialog launcher.
         *
         * @return CEvtMgmtUiWaitDialogLauncher* Pointer to the created object.
         */
        static CEvtMgmtUiWaitDialogLauncher* NewL();
        
        /**
         * Destructor.
         */        
        ~CEvtMgmtUiWaitDialogLauncher();
        
        /**
         * Issues an asynchronous request to start the Wait dialog. The termination
         * of the dialog either when User terminated or when terminated using the 
         * StopWaitingDialog call will be notified using the MProgressDialogCallback
         * observer object.
         */
        void StartWaitDialogL();
        
        /**
         * Issues an asynchronous request to the stop the running Wait dialog. The
         * actual dismissal of the dialog will be communicated using the 
         * MProgressDialogCallback observer object.
         * 
         * @return TInt The Button ID with which the dialog was dismissed
         */
        TInt StopWaitDialogL();
        
        /**
         * Issues an synchronous request to start the Wait dialog. The termination
         * of the dialog either when User terminated or when terminated using the 
         * StopWaitingDialog call will be notified using the MProgressDialogCallback
         * observer object.
         */
        void StartWaitDialogSyncL();
        
        /**
         * Issues an synchronous request to the stop the running Wait dialog. The
         * actual dismissal of the dialog will be communicated using the 
         * MProgressDialogCallback observer object.
         * 
         * @return TInt The Button ID with which the dialog was dismissed
         */        
        TInt StopWaitDialogSyncL();
        
        /**
         * Sets an observer, so that, client can be notified about the
         * button id, after the dialog dismissal.
         */
        void SetObserver( MEvtMgmtUiWaitDialogLauncherObserver* aObserver );
    
        /*
         * Allow user to cancel busy dialog
         * @param[in] aIsCancelAvailable - True if user is allowed to cancel else false 
         */
        void AllowUserToCancel( TBool aIsCancelAvailable );
       
        /*
         * It will set text to be displayed on wait dialog.
         * 
         * @since Symbian v9.1
         * 
         * @param[in] aText - text to be displayed on wait dialog.
         */
        void SetTextL(const TDesC& aText);
        
        /*
         * It will set text to be displayed on wait dialog.
         * 
         * @since Symbian v9.1
         * 
         * @param[in] aResourceId - resource id of the text to be displayed 
         *                          on wait dialog.
         */
        void SetTextL( const TInt aResourceId );
        
    private:
        /**
         * Inherited from MProgressDialogCallback. The scope of the function is
         * made private since, this should be called only from the 
         * MProgressDialogCallback interface and from the Wait dialog alone.
         */
        void DialogDismissedL( TInt aButtonId );
        
    private:
        /**
         * Overloaded C++ constructor
         */
        CEvtMgmtUiWaitDialogLauncher();
        
        /**
         * Second phase constructor
         */
        void ConstructL();
        
    protected:
        /**
         * Inherited from CActive
         */
        void RunL();
        
        /**
         * Inherited from CActive
         */
        void DoCancel();
    
    private:
        
        /**
         * Button ID with which the dialog was cancelled
         */
        TInt                        iButtonId;
         
        /**
         * Wait Note
         * Owns
         */
        CAknWaitDialog*             iWaitNote;
        
        /**
         * Active Scheduler re-loop to make Stop synchronous
         */
        CActiveSchedulerWait*       iSchedWait;
        /**
         * ref
         * Observer for callback, when the dialog is dissmissed.
         */
        MEvtMgmtUiWaitDialogLauncherObserver*       iObserver;
        
        /*
         * It determines weather user is allowed to cancel dialog.
         */
        TBool iIsCancelAvailable;
        
        /*
         * Owns
         * text to be displayed on wait dialog.
         */
        HBufC*  iWaitText;
    };
    
#endif // EVTMGMTUIWAITDLGLAUNCHER_H   
            
// End of File
