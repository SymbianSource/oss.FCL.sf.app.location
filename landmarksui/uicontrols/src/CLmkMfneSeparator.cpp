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
* Description:    LandmarksUi Content File -
*
*/








// INCLUDE FILES
#include <eikenv.h>

#include "CLmkMfneSeparator.h"

CLmkMfneSeparator::CLmkMfneSeparator()
    {
    }

CLmkMfneSeparator::~CLmkMfneSeparator()
    {
    delete iText;
    }

CLmkMfneSeparator* CLmkMfneSeparator::NewL(const TDesC& aText)
    {
    CLmkMfneSeparator* self = new(ELeave) CLmkMfneSeparator();
    CleanupStack::PushL(self);
    self->SetTextL(aText);
    CleanupStack::Pop(self);
    return self;
    }

void CLmkMfneSeparator::SetTextL(const TDesC& aText)
    {
    delete iText;
    iText = NULL;
    iText = HBufC::NewL(aText.Length());
    *iText = aText;
    }

TInt CLmkMfneSeparator::MaximumWidthInPixels(
    const CFont& aFont,
    TBool /*aShrinkToMinimumSize*/)
    {
    return aFont.TextWidthInPixels(*iText);
    }

TCoeInputCapabilities CLmkMfneSeparator::InputCapabilities() const
    {
    return TCoeInputCapabilities(TCoeInputCapabilities::ENone);
    }

const TDesC& CLmkMfneSeparator::Text() const
    {
    return *iText;
    }
