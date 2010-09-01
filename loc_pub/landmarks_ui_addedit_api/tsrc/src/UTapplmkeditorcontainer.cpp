/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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






/*
Change History
22-05-2007	To resolve tool IDO warnings	Komala Nagaraju
*/

// INCLUDE FILES
#include "UTAppLmkEditorContainer.h"

#include <eikrted.h>
#include <barsread.h>
#include <txtrich.h>
#include <aknutils.h>
#include <UTAppLmkEditor.rsg>
#include <fontids.hrh>
#include <gulfont.h>
#include <aknnotewrappers.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUTAppLmkEditorContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
    Window().SetShadowDisabled(EFalse);

    SetBlank();

    // construct main text display window
    TFontSpec fontspec = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont)->FontSpecInTwips();

    TCharFormat defaultCharFormat( fontspec.iTypeface.iName, fontspec.iHeight );
    TCharFormatMask defaultCharFormatMask;

    defaultCharFormatMask.SetAttrib(EAttFontTypeface);
    defaultCharFormatMask.SetAttrib(EAttFontHeight);

    iParaFormat = CParaFormatLayer::NewL();
    iCharFormat = CCharFormatLayer::NewL(defaultCharFormat,defaultCharFormatMask);

    iRichText = CRichText::NewL(iParaFormat, iCharFormat);

    iOutputWindow = new (ELeave) CEikRichTextEditor(TGulBorder(TGulBorder::ENone));
    iOutputWindow->ConstructL(this,0,0,EEikEdwinOwnsWindow|EEikEdwinInclusiveSizeFixed|
        EEikEdwinKeepDocument|EEikEdwinUserSuppliedText|
        EEikEdwinLineCursor|EEikEdwinAlwaysShowSelection);

    SetRect(aRect);
    ActivateL();
    }

// Destructor
CUTAppLmkEditorContainer::~CUTAppLmkEditorContainer()
    {
    delete iParaFormat;
    iParaFormat = NULL;

    delete iCharFormat;
    iCharFormat = NULL;

    delete iOutputWindow;
    iOutputWindow = NULL;

    delete iRichText;
    iRichText = NULL;
    }

// ---------------------------------------------------------
// CUTAppLmkEditorContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::SizeChanged()
{
    TRAP_IGNORE(SizeChangedL());
}

// ---------------------------------------------------------
// CUTAppLmkEditorContainer::SizeChangedL()
// Methods that can leave and are needed by SizeChanged
// are collected into this method.
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::SizeChangedL()
{
    TSize outputRect;
	AknLayoutUtils::LayoutMetricsSize(AknLayoutUtils::EMainPane, outputRect);

    iOutputWindow->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
    iOutputWindow->CreateScrollBarFrameL();
    iOutputWindow->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

    iOutputWindow->SetDocumentContentL(*iRichText,CEikEdwin::EUseText);
    iOutputWindow->SetRect(outputRect);
    iOutputWindow->ActivateL();
    iOutputWindow->SetFocus(ETrue);
}

// ---------------------------------------------------------
// CUTAppLmkEditorContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CUTAppLmkEditorContainer::CountComponentControls() const
    {
    return 0; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CUTAppLmkEditorContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CUTAppLmkEditorContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }

// ---------------------------------------------------------
// CUTAppLmkEditorContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    gc.Clear(aRect);    }

// ---------------------------------------------------------
// CUTAppLmkEditorContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }


// ---------------------------------------------------------
// CUTAppLmkEditorContainer::SetFontStyle(TUint aFontStyle)
// Set font style for output window.
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::SetFontStyle(TUint aFontStyle)
{
    // Set current main window style to match that requested
    UpdateFontAttribute(aFontStyle, CEikGlobalTextEditor::EBold);
    UpdateFontAttribute(aFontStyle, CEikGlobalTextEditor::EItalic);
    UpdateFontAttribute(aFontStyle, CEikGlobalTextEditor::EUnderline);
}


// ---------------------------------------------------------
// CUTAppLmkEditorContainer::UpdateFontAttribute(TUint aFontStyle,
//                                         TInt aAttribute)
// Do the real work of setting the font style.
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::UpdateFontAttribute(TUint aFontStyle,
                                             TInt aAttribute)
{
    // Ensure an individual attribute is on or off as requested
    if ((aFontStyle & aAttribute) != (iFontStyle & aAttribute))
    {
        TRAP_IGNORE(iOutputWindow->BoldItalicUnderlineEventL(aAttribute));
        iFontStyle ^= aAttribute;
    }
}


// ---------------------------------------------------------
// CUTAppLmkEditorContainer::PrintNotify(const TDesC& aDes, TUint aFontStyle)
// Print text into output window.
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::PrintNotify(const TDesC& aDes, TUint aFontStyle)
{
    SetFontStyle(aFontStyle);
    TChar ch;

    for (TInt i = 0; i < aDes.Length(); i++)
    {
        RDebug::Print(_L("aDes[i]=%d"), aDes[i]);

        // Add linebreak if character is '\n', otherwise
        // add character as it is.
        ch = aDes[i] != 10 ? (TChar)aDes[i] : (TChar)CEditableText::ELineBreak;
        TRAP_IGNORE(iRichText->InsertL(iOutputWindow->TextLength(), ch));
    }

    TRAP_IGNORE(iOutputWindow->NotifyNewDocumentL());

    TRAP_IGNORE(iOutputWindow->SetCursorPosL(iOutputWindow->TextLength(), EFalse));
}


// ---------------------------------------------------------
// CUTAppLmkEditorContainer::PrintNotify(const TDesC8& aDes, TUint aFontStyle)
// Print text into output window.
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::PrintNotify(const TDesC8& aDes, TUint aFontStyle)
{
    SetFontStyle(aFontStyle);
    TChar ch;

    for (TInt i = 0; i < aDes.Length(); i++)
    {
        RDebug::Print(_L("aDes[i]=%d"), aDes[i]);

        // Add linebreak if character is '\n', otherwise
        // add character as it is.
        ch = aDes[i] != 10 ? (TChar)aDes[i] : (TChar)CEditableText::ELineBreak;
        TRAP_IGNORE(iRichText->InsertL(iOutputWindow->TextLength(), ch));
    }

    TRAP_IGNORE(iOutputWindow->NotifyNewDocumentL());

    TRAP_IGNORE(iOutputWindow->SetCursorPosL(iOutputWindow->TextLength(), EFalse));
}


// ---------------------------------------------------------
// CUTAppLmkEditorContainer::PrintNotify(TInt aIn)
// Print integer into output window.
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::PrintNotify(TInt aInt)
{
    // Display number in ascii decimal
    TBuf<50> buf;
    buf.Num(aInt);
    PrintNotify(buf);
}

// ---------------------------------------------------------
// CUTAppLmkEditorContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,
//                                    TEventCode aType)
// Handle key event. Only up and down key arrow events are
// consumed in order to enable scrolling in output window.
// ---------------------------------------------------------
//
TKeyResponse CUTAppLmkEditorContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
    if(aType != EEventKey)
        return EKeyWasNotConsumed;

    if(aKeyEvent.iCode == EKeyUpArrow)
    {
        iOutputWindow->MoveCursorL(TCursorPosition::EFLineUp,EFalse);
        return EKeyWasConsumed;
    }

    if(aKeyEvent.iCode == EKeyDownArrow)
    {
        iOutputWindow->MoveCursorL(TCursorPosition::EFLineDown,EFalse);
        return EKeyWasConsumed;
    }

    return EKeyWasNotConsumed;
}


// ---------------------------------------------------------
// CUTAppLmkEditorContainer::ClearOutputWindow()
// Clear the output window.
// ---------------------------------------------------------
//
void CUTAppLmkEditorContainer::ClearOutputWindow()
{
    iRichText->Reset();
    TRAP_IGNORE(iOutputWindow->NotifyNewDocumentL());
}

// End of File
