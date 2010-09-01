/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Events Handler Server App Ui.
*
*/

#ifndef C_EVTSERVERAPPUI_H
#define C_EVTSERVERAPPUI_H

// INCLUDES
#include <aknappui.h> 

    class CEventReceiver;
        
    class CEvtServerAppUi : public CAknAppUi
    {
    public:
        /**
         * Default Constructor
         */
        CEvtServerAppUi();
        
        /**
         * Default Destructor
         */
        ~CEvtServerAppUi();        
        
        /**
         * ConstructL.
         * 2nd phase constructor.
         */
        void ConstructL();
        
        /**
         * Sets the event receiver.
         *
         * @param aEventReceiver is set while creating AppUi.
         *        Appui will send wssession events( EEventScreenDeviceChanged,
         *        & EEventWindowVisibilityChanged) to CEventReceiver that 
         *        distributes it to its listeners.
         *         
         */
       void SetEventReceiver( CEventReceiver& aEventReceiver );

        
    protected:

       /**
        * Handles window server events.
        *
        * @param aEvent The window server event that occurred.
        * @param aDestination The control associated with the event.
        *
        */      
        void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );

    private:
        //handle to the event reciever.
        CEventReceiver* iEventReceiver;
    }; 

#endif  //C_EVTSERVERAPPUI_H

