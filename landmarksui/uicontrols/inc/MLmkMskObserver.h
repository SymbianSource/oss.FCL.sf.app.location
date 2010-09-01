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
* Description:    LandmarksUi Content File -    Provides an observer interface to Landmarks application
*				 for updation of middle softkey.
*
*/






#ifndef MMSKOBSERVER_H
#define MMSKOBSERVER_H

// CLASS DECLARATIONS
/**
*  This interface defines specification to update MSK in landmarks
*  application view w.r.t backend changes in Landmarks/Categores.
*  It is implemented by the containers of the application to update
*  the associated view. This callback will be called from the respective
*  selectors,when there is any change noticed in the Landmarks/Categories.
*/
class MLmkMskObserver
	{
	public:
		/**
        * Notifies the container that Landmarks/Categories have changed,
        * so that containers can update their respective views.
        * @param aEnableMsk to enable/disable MSK
        */
		virtual void UpdateMskContainerL()=0;
		virtual void UpdateMskContainerForFilterL(){};
	};

#endif //MMSKOBSERVER_H