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
* Description:    LandmarkUi Content File
*
*/








#ifndef LMKENGINE_H
#define LMKENGINE_H

#include <coemop.h>
#include <e32base.h>

class CLmkEngine : public CBase, public MObjectProvider
{
		public:

		protected:
			TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    	MObjectProvider* MopNext();
};

#endif      // LMKENGINE_H

// End of File
