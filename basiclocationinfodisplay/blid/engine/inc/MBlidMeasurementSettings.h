/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Blid measurement settings can be modified
*                and read with help of this API.
*
*/



#ifndef __MBLIDMEASUREMENTSETTINGS_H__
#define __MBLIDMEASUREMENTSETTINGS_H__

// CLASS DECLARATION

/**
*  This Blid application engine API is only used by Blid application.
*  Blid settings can be modified and read with help of this API.
*
*/
class MBlidMeasurementSettings
    {
    public: // New functions

        /**
         * Set the Measurement Settings Vaue 
         *
         * @return None 
         */
        virtual void HandleSysMeasureValueL( ) = 0;
    };

#endif      // __MBLIDMEASUREMENTSETTINGS_H__

// End of File
