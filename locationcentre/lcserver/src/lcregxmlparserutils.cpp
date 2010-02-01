/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Location Centre Server object.
*
*/


// SYSTEM INCLUDES
#include <utf.h>

// USER INCLUDES
#include "lcregxmlparserutils.h"
#include "lcregxmltagtype.h"

// CONSTANTS
const TInt KBufLength = 30;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// LcRegXmlParserUtils::Utf8ToUnicodeL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void LcRegXmlParserUtils::Utf8ToUnicodeL(
    const TDesC8& aUtf8,
    TDes16& aUnicode)
    {
    // Create a small output buffer
    TBuf16<KBufLength> output;
    // Create a buffer for the unconverted text
    TPtrC8 remUtf8(aUtf8);

    TBool convertingReady = EFalse;
    while (!convertingReady) // conversion loop
        {
        // Start conversion. When the output buffer is full, return the
        // number of characters that were not converted
        const TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(
            output, remUtf8);

        // check to see that the descriptor isn't corrupt - leave if it is
        if (err == CnvUtfConverter::EErrorIllFormedInput)
            {
            User::Leave(KErrCorrupt);
            }
        else if (err < 0) // future-proof against "TError" expanding
            {
            User::Leave(KErrGeneral);
            }

        // Do something here to store the contents of the output buffer.
        if (aUnicode.Length() + output.Length() > aUnicode.MaxLength())
            {
            User::Leave(KErrNoMemory);
            }
        aUnicode.Append(output);

        // Finish conversion if there are no unconverted characters in the
        // remainder buffer
        if (err == 0)
            {
            convertingReady = ETrue;
            }
        else
            {
            // Remove the converted source text from the remainder buffer.
            // The remainder buffer is then fed back into loop
            remUtf8.Set(remUtf8.Right(err));
            }
        }
    }

// -----------------------------------------------------------------------------
// LcRegXmlParserUtils::OnlyWhiteSpaces
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool LcRegXmlParserUtils::OnlyWhiteSpaces(
    const TDesC8& aStr)
    {
    for (TInt i=0; i<aStr.Length(); i++)
        {
        TChar ch = TChar(aStr[i]);
        if (!ch.IsSpace())
            {
            return EFalse;
            }
        }

    return ETrue;
    }
//  End of File
