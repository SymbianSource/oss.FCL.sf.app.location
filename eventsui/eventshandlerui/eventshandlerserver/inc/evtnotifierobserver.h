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
* Description:  Observer class for event notifier handler
*
*/



#ifndef EVTNOTIFIEROBSERVER_H
#define EVTNOTIFIEROBSERVER_H

//  INCLUDES

#include <e32base.h> 

#include "evtinfonoteinterface.h"
#include "evtevent.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Interface class for event handlers. event handlers will
*  use reference of this interface to get event viewer response & 
*  other global objects. 
*
*  @since Series 60 9.1
*/
class MEvtNotifierObserver
    {
    public: 

        /**
        * Interface for handler to report operation result.
		* @param aId The id of current handler
		* @param aErr The operation err code. KErrNone if operation
		*        has been executed successfully.
        */
        virtual void HandlerCompleteL( 
        	TEvtInfoNoteResult aResult,
        	CEvtEvent* aEvent ) = 0;
    };

#endif  // EVTNOTIFIEROBSERVER_H

// End of File
