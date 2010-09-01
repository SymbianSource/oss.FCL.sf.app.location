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

#ifndef BCAPPLMKEDITORCONTAINER_H
#define BCAPPLMKEDITORCONTAINER_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;
class CEikRichTextEditor;


// CLASS DECLARATION

/**
*  CBCAppLmkEditorContainer  container control class.
*
*/
class CBCAppLmkEditorContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor

        /**
        * default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CBCAppLmkEditorContainer();

public: // New functions

    /**
    * Clear the output window.
    */
    void ClearOutputWindow();

public: // Functions from base classes

    // From MUINotify

    /**
    * Display text on output window
    * @param aDes text to display
    * @param aFontStyle style (italic/bold/etc) for this text
    */
    void PrintNotify(const TDesC& aDes, TUint aFontStyle = 0);
    void PrintNotify(const TDesC8& aDes, TUint aFontStyle = 0);

    /**
    * Display integer as text on output window
    * @param aInt integer to display
    */
    void PrintNotify(TInt aInt);

private: // Functions from base classes

    /**
    * From CoeControl. Calls SizeChangedL with TRAP.
    */
    void SizeChanged();

    /**
    * Handle change of control size
    */
    void SizeChangedL();

    /**
    * From CoeControl.
    */
    TInt CountComponentControls() const;

    /**
    * From CCoeControl.
    */
    CCoeControl* ComponentControl(TInt aIndex) const;

    /**
    * Event handling section, e.g Listbox events.
    */
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    /**
    * From CCoeControl,Draw.
    */
    void Draw(const TRect& aRect) const;

    /**
    * Key event handler. Handles up and down arrow keys, so that
    * output window can be scrolled.
    */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    /**
    * Change font style for subsequent text
    * @param aFontStyle new font style
    */
    void SetFontStyle(TUint aFontStyle);

    /**
    * Set or clear an individual font attribute if necessary,
    * so that it matches the required style
    * @param aFontStyle required font style
    * @param aAttribute the individual attribute to correct
    */
    void UpdateFontAttribute(TUint aFontStyle, TInt aAttribute);


private: //data
    CParaFormatLayer*   iParaFormat;   // Used by CEikRichTextEditor
    CCharFormatLayer*   iCharFormat;   // Used by CEikRichTextEditor
    CRichText*          iRichText;     // Used by CEikRichTextEditor
    CEikRichTextEditor* iOutputWindow; // CRichText object for use as our output window
    TUint               iFontStyle;    // Current style in use by output window
    };


#endif

// End of File
