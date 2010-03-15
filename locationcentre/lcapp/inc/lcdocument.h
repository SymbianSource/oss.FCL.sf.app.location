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
* Description:  Document class for Location Centre Application UI.
*
*/


#ifndef C_LCDOCUMENT_H
#define C_LCDOCUMENT_H

// SYSTEM INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

/**
 *  Location Centre Application's document class.
 *
 *  @lib lcapp.exe
 *  @since S60 v5.0
 */
class CLcDocument : public CAknDocument
    {
public:
	/**
	 * Static Two phase constructor
	 */
    static CLcDocument* NewL( CEikApplication&    aApp );

  	/**
  	 * Virtual destructor
  	 */
    virtual ~CLcDocument();

private:
	/**
	 * C++ Default constructor
	 */
    CLcDocument( CEikApplication&     aApp );

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

#endif  // C_LCDOCUMENT_H

// End of File

