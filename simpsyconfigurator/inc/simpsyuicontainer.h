/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Simulation PSY Configuration UI application container class.
*
*/


#ifndef CSIMPSYUICONTAINER_H_
#define CSIMPSYUICONTAINER_H_

#include <coecntrl.h>
#include <eiklbo.h>

// FORWARD DECLARATIONS
class CAknDoubleStyleListBox;
class CSimPsyUiAppUi;

class CSimPsyUiContainer : public CCoeControl
    {
public:
    void ConstructL(const TRect& 			aRect,
    					  CSimPsyUiAppUi*	aAppUi);
    
    ~CSimPsyUiContainer();
    
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

	void	UpdateListBox();

protected:
	void FocusChanged(TDrawNow aDrawNow);
	void HandleResourceChange(TInt aType);
	
private: // From CCoeControl
    /**
     * From CoeControl,SizeChanged.
     */
    void SizeChanged();

    /**
     * From CoeControl,CountComponentControls.
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl,ComponentControl.
     */
    CCoeControl* ComponentControl(TInt aIndex) const;	
    
private:
	void CreateListboxL();
	
private: //data
	CAknDoubleStyleListBox* iListBox;
	CSimPsyUiAppUi*			iAppUi;
    };

#endif // CSIMPSYUICONTAINER_H_