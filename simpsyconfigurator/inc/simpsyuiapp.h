/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Simulation PSY Configuration UI application class.
*
*/


#ifndef  CSIMPSYUIAPPLICATION_H_
#define  CSIMPSYUIAPPLICATION_H_

#include "simpsyuiconsts.h"
#include <aknapp.h>

class CSimPsyUiApplication: public CAknApplication
    {
public:
    CSimPsyUiApplication();
    CApaDocument* CreateDocumentL();
   	TUid AppDllUid() const { return TUid::Uid(KSimPsyUiAppUid);	};
    };

#endif // CSIMPSYUIAPPLICATION_H_