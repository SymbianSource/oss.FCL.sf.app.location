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
* Description:    LandmarksUi Content File -    Separator field for CEikMfne
*
*/






#ifndef CLmkMfneSeparator_H
#define CLmkMfneSeparator_H

//  INCLUDES
#include <eikmfne.h>

// CLASS DECLARATION

/** Reimplementation of CEikMfneSeparator
 *  Allows run-time changing of text
 */
class CLmkMfneSeparator : public CEikMfneField
    {
    private:
        CLmkMfneSeparator();

    public:
        virtual ~CLmkMfneSeparator();
        static CLmkMfneSeparator* NewL(const TDesC& aText);
        void SetTextL(const TDesC& aText);
        const TDesC& Text() const;

    private: // from CEikMfneField
        TInt MaximumWidthInPixels(
            const CFont& aFont,
            TBool aShrinkToMinimumSize);

        TCoeInputCapabilities InputCapabilities() const;

    private:
        HBufC* iText;
    };

#endif
