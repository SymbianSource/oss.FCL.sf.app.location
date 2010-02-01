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



#ifndef T_LCREGXMLTAGINFO_H
#define T_LCREGXMLTAGINFO_H

//  SYSTEM INCLUDES

// USER INCLUDES
#include    "lcregxmltagtype.h"

// CLASS DECLARATION

/**
*  Helper class for converting from/to xml markup data.
*
*/
class TLcRegXmlTagInfo
    {
public:  // Constructors and destructor

   /**
    * C++ default constructor.
    */
    TLcRegXmlTagInfo();

public: // New functions
   /**
    * Get tag type for the specified tag name.
    *
    * @param aTagName A tag name defining the current tag.
    * @return Tag type for tag name.
    */
    static TLcRegXmlTagType TagTypeL(
    	const TDesC8& aTagName );
    
   /**
    * Get tag attribute type for the specified tag name.
    *
    * @param aTagName A tag name defining the current tag.
    * @return Tag type for tag attribute.
    */
	static TLcRegXmlTagType TagAttrTypeL(TLcRegXmlTagType aTagType,
    	const TDesC8& aAttrName );
   
private:
    static TBool EqualsTagL(
        const TDesC8& aTag,
        const TDesC& aDefinedTag
    );
    };

#endif      // T_LCREGXMLTAGINFO_H

// End of File
