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
* Description:    LandmarksUi Content File -    Application View control class for Landmark Message Viewer.
*
*/







#ifndef __CLMKMSGVIEWERAPPVIEW_H
#define __CLMKMSGVIEWERAPPVIEW_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class MAknsSkinInstance;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

/**
* Application View control class.
*/
class CLmkMsgViewerAppView : public CCoeControl
    {
    public: // New methods

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CLmkMsgViewerAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return a pointer to the created instance of CLmkMsgViewerAppView.
        */
        static CLmkMsgViewerAppView* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CLmkMsgViewerAppView object, which will draw itself
        * to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CLmkMsgViewerAppView.
        */
        static CLmkMsgViewerAppView* NewLC( const TRect& aRect );

        /**
        * ~CLmkMsgViewerAppView
        * Virtual Destructor.
        */
        virtual ~CLmkMsgViewerAppView();

    public:  // Functions from base classes

        /**
        * From CCoeControl, Draw
        * Draw this CAnimationAppView to the screen.
        * @param aRect the rectangle of this view that needs updating
        */
        void Draw( const TRect& aRect ) const;

    protected:
        // From CCoeControl
        TInt CountComponentControls() const;
        CCoeControl *ComponentControl(TInt aIndex) const;

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        * Perform the second phase construction of a
        * CLmkMsgViewerAppView object.
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL(const TRect& aRect);

        /**
        * CLmkMsgViewerAppView.
        * C++ default constructor.
        */
        CLmkMsgViewerAppView();

    private:

        //! Ref: A pointer to skin instance
        MAknsSkinInstance* iSkinInstance;

        //! Own: A pointer to BasicBackgroundControlContext
        CAknsBasicBackgroundControlContext* iBackgroundSkinContext;
    };

#endif // __CLMKMSGVIEWERAPPVIEW_H

// End of File