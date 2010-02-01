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
* Description:  Tone/File Utility.
*
*/


#ifndef EVTTONEUTILS_H
#define EVTTONEUTILS_H

// System Includes
#include <e32base.h>
// Class Definition
    
/**
 *  Static Utility class for Tone/File.
 *
 *  @lib 
 *  @since S60 v9.1
 */
class EvtToneUtils
    {

    public: // new functions
        
        /**
         * Check if File Exist
         * return ETrue if the file exist
         */
		static TBool IsExistL( const TDesC& aFileName );
		
        /**
         * Check for Tone Type
         * return ETrue if the file is RNG Type
         */
        static TBool IsRNGTypeL( const TDesC& aFileName );
        
        /**
         * Check if File is Playable(Non DRM protected)
         * return ETrue if the file is playable
         */
		static TBool IsFilePlayableL( const TDesC& aFileName );
        
        /**
         * Get the Data Type
         * return the Data Type
         */
		static TDataType DataTypeL( const TDesC& aFileName );

    };

#endif EVTTONEUTILS_H