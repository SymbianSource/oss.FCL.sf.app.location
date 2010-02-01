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
* Description:  Simulation PSY Configuration UI application class definition.
*
*/


#include <eikstart.h>
#include "simpsyuiapp.h"
#include "simpsyuidocument.h"

// ---------------------------------------------------------
// CSimPsyUiApplication::CSimPsyUiApplication
// ---------------------------------------------------------
//
CSimPsyUiApplication::CSimPsyUiApplication()
	{
	}

// ---------------------------------------------------------
// CApaDocument* CSimPsyUiApplication::CreateDocumentL
// ---------------------------------------------------------
//
CApaDocument* CSimPsyUiApplication::CreateDocumentL()
	{
	return new (ELeave) CSimPsyUiAppDocument(*this);
	}

// ---------------------------------------------------------
// LOCAL_C CApaApplication* NewApplication
// ---------------------------------------------------------
//	
LOCAL_C CApaApplication* NewApplication()
	{
	return new CSimPsyUiApplication();
	}	

// ---------------------------------------------------------
// GLDEF_C TInt E32Main
// ---------------------------------------------------------
//	
GLDEF_C TInt E32Main()
    {
    RDebug::Print(_L("SimPsyUiApplication::E32Main\n"));
    return EikStart::RunApplication(NewApplication);
    }


