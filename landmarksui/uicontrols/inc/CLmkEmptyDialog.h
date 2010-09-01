/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    For EmptyDialog
*
*/







#ifndef EMPTYDIALOG_H
#define EMPTYDIALOG_H

// System includes
#include <AknDialog.h>  // CAknDialog

NONSHARABLE_CLASS(CLmkEmptyDialog) : public CAknDialog
 	{

	public : // Construct and destructor
	    CLmkEmptyDialog();
	    ~CLmkEmptyDialog();

	public: // From CCoeControl
	    void PreLayoutDynInitL();
	    void PostLayoutDynInitL();
	    void SetSizeAndPositionL(const TSize& aSize);
	    TBool OkToExitL(TInt aButtonId);

	public: 	//For Forcefully Exiting from Dialog
	    void EmptyDialogExitL();
	};

#endif  // #ifndef EMPTYDIALOG_H

// End of File
