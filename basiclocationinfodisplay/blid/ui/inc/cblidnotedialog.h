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
* Description:  execute the note dialog.
*
*/


#ifndef CBLIDNOTEDIALOG_H
#define CBLIDNOTEDIALOG_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* An active object that handles the note dialog execution
*/

class CBlidNoteDialog : public CActive
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         */
        static CBlidNoteDialog* NewL();

        /**
         * Destructor.
         */
        virtual ~CBlidNoteDialog();
        
    private:
        /**
         * C++ default constructor.
         */
        CBlidNoteDialog();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();  

    private: // From CActive
        void RunL();
        void DoCancel();
    
    public:
        void ExecuteNoteDialog();   

    private:
        TBool iIsNoteDisplayed;             
   
    };


#endif  //CBLIDNOTEDIALOGOBJECT_H