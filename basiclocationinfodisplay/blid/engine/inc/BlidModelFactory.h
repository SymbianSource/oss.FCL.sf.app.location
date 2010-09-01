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
* Description:  A factory class which can create instances of
*                MBlidSettings
*
*/



#ifndef __CBLIDMODELFACTORY_H__
#define __CBLIDMODELFACTORY_H__

//  INCLUDES
#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbs.h>

// FORWARD DECLARATIONS
class MBlidSettings;
class MBlidLocation;
class MBlidRouter;
class CBlidEng;
// CLASS DECLARATION

/**
*  A factory class which creates needed models.
*  This factory class creates instances of
*  MBlidSettings.
*
*/
class BlidModelFactory
    {
    public: // new functions
        /**
         * Factory method for Setting model creation.
         * @return Pointer to settings model.
         */
        static MBlidSettings* SettingsL();

        /**
         * Factory method for Location model creation.
         * @param aServer RPositionServer variable
         * @return Pointer to location model.
         */
        static MBlidLocation* LocationL( RPositionServer& aServer, CBlidEng& aEngine );

        /**
         * Factory method for Router model creation.
         * @return Pointer to router model.
         */
        static MBlidRouter* RouterL(); 
    };

#endif      // __BLIDMODELFACTORY_H__

// End of File
