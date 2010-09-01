/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Current Position info pop-up.
*
*/


#ifndef __CBLIDCURRENTPOSITIONDLG_H__
#define __CBLIDCURRENTPOSITIONDLG_H__

//  INCLUDES
#include <aknPopup.h>   // CAknPopupList

// FORWARD DECLARATIONS
class CAknSingleHeadingPopupMenuStyleListBox;
class CBlidBaseView;
class MBlidLocation;
class CBlidEng;

// CLASS DECLARATION

/**
 * @internal Only Blid application internal use supported!
 *
 * Current location info dialog.
 */
class CBlidCurrentPositionDlg : public CAknPopupList
    {
    public:
        /**
         * Creates a new instance of this class.
         */
        static CBlidCurrentPositionDlg* NewL( CBlidBaseView& aView,
        									  MBlidLocation* aLoc,
        									  CBlidEng* aEng );

        /**
         * Run the dialog.
         */
        void ExecuteLD();
    
        /**
         * Destructor.
         * Also cancels and dismisses this popup list, in other words makes
         * ExecuteLD return as if cancel was pressed.
         */
        ~CBlidCurrentPositionDlg();
        
	public: // new function
		void ChangeDlgSoftKey();        

    private: // from CAknPopupList
		/*!
		@function CBlidCurrentPositionDlg

		@discussion Perform the first phase of two phase construction 
		*/
		CBlidCurrentPositionDlg( CBlidBaseView& aView,
							   MBlidLocation* aLoc,
							   CBlidEng* aEng );
		    						   
        /**
         * By default Symbian 2nd phase constructor is private.
         */
		void ConstructL();

		void ProcessCommandL(TInt aCommandId);
		
    private: // data members
        /// Own: default selection listbox
        CAknSingleHeadingPopupMenuStyleListBox* iListBox;
        CBlidBaseView& iView;
        // Not owned: A pointer to location model
        MBlidLocation* iLoc;
        // Not owned: A pointer to blid's engine
        CBlidEng* iEng;
        
        TBool isDlgExited;
        
        TBool isLmSaved;
        
    };

#endif // __CBLIDCURRENTPOSITIONDLG_H__
            
// End of File
