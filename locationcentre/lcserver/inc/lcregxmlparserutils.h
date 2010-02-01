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
*     See class description below
*
*/



#ifndef LCREGXMLPARSERUTILS_H
#define LCREGXMLPARSERUTILS_H

//  INCLUDES
#include <e32base.h>
#include <e32math.h>


// CLASS DECLARATION

/**
*  Helper class for using strings etc.
*/
class LcRegXmlParserUtils
    {
public: // New functions
    /**
    * Converts a string from UTF-8 to Unicode.
    *
    * @param aUtf8 A string in UTF-8 format.
    * @param aUnicode A unicode string.
    * @return System wide error code.
    */
    static void Utf8ToUnicodeL(
    /* IN */    const TDesC8& aUtf8,
    /* IN */    TDes16& aUnicode
    );

   /**
    * Checks if a string only contains whitespaces or not.
    *
    * @param aStr The string to check.
    * @return ETrue if string only contain whitespaces, otherwise EFalse.
    */
    static TBool OnlyWhiteSpaces(
    /* IN */        const TDesC8& aStr
    );
    };

#endif      // LCREGXMLPARSERUTILS_H

// End of File
