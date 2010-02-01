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
* Description:    LandmarksUi Content File -    LMK application UI class definition.
*
*/








#ifndef CLMKAPPUI_H
#define CLMKAPPUI_H

// INCLUDES
#include "MLmkKeyProcessor.h"
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include <AknTabObserver.h>

// FORWARD DECLARATIONS
class CLmkDocument;
class CLmkSender;

// CLASS DECLARATION

/**
* Application UI class.
*/
class CLmkAppUi
: public CAknViewAppUi,
  public MAknTabObserver,
  public MLmkKeyProcessor
    {
    public: // Constructors and destructor
        /**
        * C++ default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CLmkAppUi();

    public: // New functions
        /**
        * Return reference to document
        * @return reference to CLmkDocument
        */
        CLmkDocument& Document() const;

        /**
        * Return reference to landmark sender
        * @return reference to CLmkSender
        */
        CLmkSender& LmkSender() const;

    protected: // From MAknTabObserver
        void TabChangedL(TInt aIndex);

    public: // From  MLmkKeyProcessor
        TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );
        MAknTabObserver* TabObserver();

    public: // From CEikAppUi
        void HandleCommandL(TInt aCommand);
        void HandleViewDeactivation(const TVwsViewId &aViewIdToBeDeactivated,const TVwsViewId &aNewlyActivatedViewId);

    private: // Data
        /// Own: sender
        CLmkSender* iLmkSender;
    };

#endif // CLMKAPPUI_H

// End of File
