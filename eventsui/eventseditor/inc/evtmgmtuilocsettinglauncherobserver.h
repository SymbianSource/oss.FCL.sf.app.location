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
* Description:  It is observer class to handle launch of location 
* settings.
*/


#ifndef EVTMGMTUILOCSETTINGLAUNCHEROBSERVER_H_
#define EVTMGMTUILOCSETTINGLAUNCHEROBSERVER_H_

#include <e32base.h>        // CBase

class MEvtMgmtUiLocSettingsLauncherObserver
    {
public:
    /**
     * It will be used to notify observer that lauching positioning settings
     * has been completed.
     * @param aErrorCode It will contain the status of the operation
     *                   KErrNone if successful otherwise error status code          
     */
    virtual void NotifyLocSettingLaucherChangeL( TInt aErrorCode )=0;
    };
#endif //EVTMGMTUILOCSETTINGLAUNCHEROBSERVER_H_