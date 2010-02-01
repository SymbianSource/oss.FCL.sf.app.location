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
* Description:    LandmarksUi Content File -    The class declared in this file is the defination of the observer
*				 for the CLmkDbSearchUtils.
*
*/







#ifndef MLMKDBSEARCHUTILSOBSERVER_H
#define MLMKDBSEARCHUTILSOBSERVER_H

// INCLUDE FILES
#include <EPos_Landmarks.h>

class MLmkDbSearchUtilsObserver
	{
	public:
		/**
		* Callback function for db search. The callback function will get the
		* set of Landmarks id as the output of the search.
		*
		* @param aArray array of TPosLmItemId's
		*/
		virtual void HandleCategorySearchNotifyL(RArray<TPosLmItemId> aArray) = 0;

		virtual void HandleLmkSearchNotifyL() = 0;
	};

#endif //MLMKDBSEARCHUTILSOBSERVER_H
