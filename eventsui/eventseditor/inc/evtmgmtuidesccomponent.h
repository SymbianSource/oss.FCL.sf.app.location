/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Description component for retrieving description.
*
*/


#ifndef EVTMGMTUIDESCCOMPONENT_H_
#define EVTMGMTUIDESCCOMPONENT_H_

#include <e32base.h>        // CBase

// Forward Declaration
class CNotepadApi;

/**
 *  @class CEvtMgmtUiDescComponent
 *  Class that represents Description component of editor
 *
 *  This class acts as description component of editor.
 *  It  is used to retrieve description from notepad.
 *  It is also used to create new description using notepad.
 *  Note: Created description is not stored in Notepad application.
 *
 *  @since S60 v9.1
 */

class CEvtMgmtUiDescComponent: public CBase
    {
private:
    
    public:  // Constructor and destructor  
        /**
         * Static Two phase constructor
         * @param [in] aPrevDesc - description of existing event.
         *                       - empty for newly created event        
         */
        static CEvtMgmtUiDescComponent* NewL(const TPtrC aPrevDesc);
        
      /**
       * Static Two phase constructor
       * @param [in] aPrevDesc - description of existing event.
       *                       - empty for newly created event        
       */
      static CEvtMgmtUiDescComponent* NewLC(const TPtrC aPrevDesc);

      /**
       * Virtual destructor
       */
      virtual ~CEvtMgmtUiDescComponent(); 
      
    public:// new functions
      /**
       * It displays notepad editor and saves in descriptor variable
       * 
       * @since S60 v9.1
       * 
       * @return KErrCancel if deleted
       *         KErrAlreadyExists if not modified
       *         KErrNone if modified
       */
        TInt DisplayNotepadEditorL(); 
        
       /**
       * It opens system's memo popup dialog and 
       * saves selected memo as descriptor.
       * 
       * @since S60 v9.1
       *  
       * @return KErrCancel if cancel or no note
       *         KErrNone if note is selected
       */
        TInt DescFromExistingNotesL();
                
        /**
        * It retrieves descriptor
        * 
        * @since S60 v9.1
        * 
        * @return selected descriptor
        */
         TPtrC Desc() const;        

       
    private:  // Constructor
        /**
         * C++ Default constructor
         */       
        CEvtMgmtUiDescComponent();
  
        /**
         * Second phase of the two phase constructor
         *  @param [in] aPrevDesc - description of existing event.
         *                        - empty for newly created event        
         */ 
         void ConstructL( const TPtrC aPrevDesc );
    
    private:
        /**
         * description value
         * owns
         */
        HBufC* iDesc;
		
        /**
         * Notepad Instance
         * owns
         */
        CNotepadApi* iNotepad;
    };

#endif //EVTMGMTUIDESCCOMPONENT_H_
