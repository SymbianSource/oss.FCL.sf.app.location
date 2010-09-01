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
* Description:  Simulation PSY Configuration UI application document class.
*
*/


#ifndef CSIMPSYUIAPPDOCUMENT_H_
#define CSIMPSYUIAPPDOCUMENT_H_

#include <AknDoc.h>

class CSimPsyUiAppDocument: public CAknDocument
    {
 public:
    CSimPsyUiAppDocument(CEikApplication& aApp):CAknDocument(aApp){};
 private: // from CAknDocument
    CEikAppUi* CreateAppUiL();
    };

#endif // CSIMPSYUIAPPDOCUMENT_H_