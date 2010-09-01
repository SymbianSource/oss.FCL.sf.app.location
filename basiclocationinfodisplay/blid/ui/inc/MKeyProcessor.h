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
* Description: 
*     Interface which allows subscribers to consume key event or pass the event
*     to other view class.
*       
*
*/


#ifndef MKEYPROCESSOR_H
#define MKEYPROCESSOR_H

// INCLUDES
#include <w32std.h>

// FORWARD DECLARATIONS
class MAknTabObserver;

// ===========================================================================
// MKeyProcessor
// ===========================================================================

/**
*	An interface a CCoeControl class can implement to forward the keyevents 
*   before the actual control gets the key events. This is useful in 
*   situations where the control consumes keys which you need.
*/
class MKeyProcessor
    {
    public:
    /**
     * process the keyevent befor it reaches the control
     * @param aKeyEvent the event fired
     * @param aType the type of event
     * @return ETrue if key event was consumed, otherwise EFalse
     */
    virtual TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                    TEventCode aType ) = 0;

	/**
     * returns the pointer to MAknTabObserver interface
     * @return pointer to the MAknTabObserver interface
     */
    virtual MAknTabObserver* TabObserver() = 0;
    };

            
#endif //MKEYPROCESSOR_H

// End of File
