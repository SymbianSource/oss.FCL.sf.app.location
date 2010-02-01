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
* Description:  Document class for Events Handler Application.
*
*/


#ifndef C_EVTHANDLERAPPDOCUMENT_H
#define C_EVTHANDLERAPPDOCUMENT_H

// SYSTEM INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

/**
 *  Events Handler App's document class.
 *
 *  @lib lcapp.exe
 *  @since S60 v5.0
 */
class CEvtHandlerAppDocument : public CAknDocument
    {
public:
	/**
	 * Static Two phase constructor
	 */
    static CEvtHandlerAppDocument* NewL( CEikApplication&    aApp );

  	/**
  	 * Virtual destructor
  	 */
    virtual ~CEvtHandlerAppDocument();

private:
	/**
	 * C++ Default constructor
	 */
    CEvtHandlerAppDocument( CEikApplication&     aApp );

	/**
	 * Second phase of the two phase constructor
	 */
    void ConstructL();

private: // Inherited from Base class

	/**
	 * Inherited from CAknDocument
	 */
    CEikAppUi* CreateAppUiL();
    };

#endif  // C_EVTHANDLERAPPDOCUMENT_H

// End of File

