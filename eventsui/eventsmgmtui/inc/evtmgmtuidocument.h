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
* Description:  Document class for Events Management UI.
*
*/


#ifndef C_EVTMGMTUIDOCUMENT_H
#define C_EVTMGMTUIDOCUMENT_H

// SYSTEM INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

/**
 * Event Management UI document class.
 *
 * @lib evtmgmtui.exe
 * @since S60 v5.0
 */
class CEvtMgmtUiDocument : public CAknDocument
    {
public:
	/**
	 * Static Two phase constructor
	 */
    static CEvtMgmtUiDocument* NewL( CEikApplication&    aApp );

  	/**
  	 * Virtual destructor
  	 */
    virtual ~CEvtMgmtUiDocument();

private:
	/**
	 * C++ Default constructor
	 */
    CEvtMgmtUiDocument( CEikApplication&     aApp );

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

#endif  // C_EVTMGMTUIDOCUMENT_H

// End of File

