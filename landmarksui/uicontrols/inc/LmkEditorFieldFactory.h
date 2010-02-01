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
* Description:    LandmarksUi Content File -    Landmark editor field factory class.
*
*/







#ifndef __LMKEDITORFIELDFACTORY_H__
#define __LMKEDITORFIELDFACTORY_H__

#include <e32std.h>
// FORWARD DECLARATIONS
class MLmkEditorField;
class MLmkFieldData;
class MLmkEditorUiBuilder;


// CLASS DECLARATION

/**
* Landmark editor field factory class.
*/
class LmkEditorFieldFactory
    {
    public:  // destructor
        /**
        * Factory for creating lmk editor fields.
        * @param aField field which is created
        * @param aUiBuilder UI builder
		* @return a contact editor field.
        */
        static MLmkEditorField* CreateFieldL(
                MLmkFieldData& aField,
                MLmkEditorUiBuilder& aUiBuilder );

		static MLmkEditorField* CreateFieldL(
				MLmkFieldData& aField,
				MLmkEditorUiBuilder& aUiBuilder,
				const TDesC& aCategoryName );
    };

#endif // __LMKEDITORFIELDFACTORY_H__

// End of File
