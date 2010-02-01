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
* Description:    LandmarksUi Content File -
*
*/








#ifndef LMKICONDIALOG_H
#define LMKICONDIALOG_H

#include <AknDialog.h>// <eikdialg.h>

/**
 * Change Icon table for LandmarksUi.Contains all the icon available
 */
NONSHARABLE_CLASS(CLmkIconMapDialog) : public CAknDialog
    {
public:

    /**
    * LandmarksUi change Icon table constructor.
    *
    * @param aIconIndex contains the reference of the integer,
    *                   which will give the selected Index.
    *
    */
     CLmkIconMapDialog( TInt& aIconIndex );

    /**
    * LandmarksUi change Icon table desstructor.
    *
    *             C++ destructor
    */
    ~CLmkIconMapDialog();

public: // from CEikDialog

    /**
    * Prepares and runs the dialog and returns the id of the button used to dismiss
    * it. The dialog is constructed from the resource with id aResourceId and is destroyed
    * on exit.
    *
    * @param aResourceId Resource id for icon table dialog resource.
    * @return Id of the button used to dismiss dialog.
    *
    */
     TInt ExecuteLD(TInt aResourceId);

#ifdef RD_SCALABLE_UI_V2
	//From MCoeControlObserver
	void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
	void HandleDialogPageEventL(TInt aEventID);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);

#endif //RD_SCALABLE_UI_V2

public: // from CCoeControl
     TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode);

    /**
     * Handles a change to the control's resources of type aType
     * which are shared across the environment, e.g. colors or fonts.
     *
     * @since ER5U
     */
     void HandleResourceChange(TInt aType);

    /**
     * CreateCustomControlL(TInt aControlType)
     * Virtual function of base class has to be overriden to create
     * a custom controll
     */
 	 SEikControlInfo CreateCustomControlL(TInt aControlType);

protected: // from CEikDialog
    /**
     * OkToExitL(TInt)
     */
     virtual TBool OkToExitL(TInt);

    /**
     * PreLayoutDynInitL()
     */
     virtual void PreLayoutDynInitL();

     /**
     * PostLayoutDynInitL()
     */
     virtual void PostLayoutDynInitL();
    /**
     * SetSizeAndPosition(const TSize& aSize)
     */
     virtual void SetSizeAndPosition(const TSize& aSize);

private:
     virtual void CEikDialog_Reserved_1();
     virtual void CEikDialog_Reserved_2();

private: // new function
     virtual void CAknIconMapDialog_Reserved();

private: // helpers
    void DoLayout();

private:
    TInt* iIconIndex;
#ifdef RD_SCALABLE_UI_V2
    TBool iConSelected;
#endif //#ifdef RD_SCALABLE_UI_V2
    };

#endif  // LMKICONDIALOG_H
