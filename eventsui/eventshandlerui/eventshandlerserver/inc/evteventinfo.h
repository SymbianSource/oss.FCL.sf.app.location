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
* Description:  event Info Struct
*
*/


#ifndef C_EVTEVENTINFO_H
#define C_EVTEVENTINFO_H

#include <e32base.h>

#include "evtdefs.h"

/**
*  Event Strucutre to Que.
*
*  @since Series 60 9.2
*/
class TEvtEventInfo
    { 
    public :        
		/**
        * Copy constructor
        */
        TEvtEventInfo(TEvtEventId aId, TEvtFireAccuracy aAcc, TBool aPlayTone, TBool aSnooze)
	        {
	        iEventId = aId;
	        iAccuracy = aAcc;
	        iPlayTone = aPlayTone;
	        iSnooze = aSnooze;
	        };
			
		/**
        * inline function used for comparision
        */
        static inline TInt Match(const TEvtEventInfo& aFirst, const TEvtEventInfo& aSecond)
            {
            if (aFirst.iEventId == aSecond.iEventId)
                return 1;
            return 0;
            };
			
    public :
		// Event's Information like Event Id, Accuracy, Playtone and Snooze
    	TEvtEventId iEventId;
    	TEvtFireAccuracy iAccuracy;
    	TBool iPlayTone;
    	TBool iSnooze;
 };

#endif //C_EVTEVENTINFO_H

// End of File
