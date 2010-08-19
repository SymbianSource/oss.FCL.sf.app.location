/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Debug contains debug macros.
*
*/







#ifndef DEBUG_H
#define DEBUG_H
// INCLUDES
#include <e32std.h>
#include "LmkDebugConfig.h"

#ifdef __LMK_DEBUG_FLAG

#define DEBUG(TEXT) Debug( _L(#TEXT) );
#define DEBUG1(TEXT, ARG1) Debug( _L(#TEXT), (ARG1) );
#define DEBUG2(TEXT, ARG1, ARG2) Debug( _L(#TEXT), (ARG1), (ARG2) );
#define DEBUG3(TEXT, ARG1, ARG2, ARG3) Debug( _L(#TEXT), (ARG1), (ARG2), (ARG3) );

void Debug( TRefByValue<const TDesC> aText, ...);

#else

#define DEBUG(TEXT);
#define DEBUG1(TEXT, ARG1);
#define DEBUG2(TEXT, ARG1, ARG2);
#define DEBUG3(TEXT, ARG1, ARG2, ARG3);

#endif // __LMK_DEBUG_FLAG

#endif /* DEBUG_H */