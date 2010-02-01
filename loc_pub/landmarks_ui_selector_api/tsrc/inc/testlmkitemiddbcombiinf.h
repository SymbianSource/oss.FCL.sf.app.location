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
* Description:    LandmarksUi Content File -  Class used for testing TLmkItemIdDbCombiInfo class
*
*/







#ifndef TLMKITEMIDDBCOMBIINF_H
#define TLMKITEMIDDBCOMBIINF_H

//  INCLUDES
#include <e32base.h>            // CBase

/**
* Class used for testing TLmkItemIdDbCombiInfo class.
*
*/
class TLmkItemIdDbCombiInf
{

   public:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */

        TLmkItemIdDbCombiInf();

        /**
        * Destructor.
        */
        ~TLmkItemIdDbCombiInf();

   public: // Functions for getting/setting the info

        /**
        * Set & get the item id.
        */
        void GetItemIdL();

        /**
        * Set & get the Landmarks Db handle.
        */

        void GetLmDbL();

        /**
        * Set & get the item id.
        */
        void SetGetItemIdL();

        /**
        * Set & get the Landmarks Db handle.
        */

        void SetGetLmDbL();


private: // own data

};


#endif // TLmkItemIdDbCombiInf_H
