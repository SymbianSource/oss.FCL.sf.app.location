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
* Description:   'Set Assigntone' component for retrieving ringtone.
*
*/


#ifndef EVTMGMTUIRINGTONECOMPONENT_H_
#define EVTMGMTUIRINGTONECOMPONENT_H_

#include <e32base.h>        // CBase

/**
 *  @class CEvtMgmtUiRingtoneComponent
 *  Class that represents 'Set tone' component of editor
 *
 *  This class acts as 'set tone' component of editor.
 *  It  is used to retrieve tones from memory.
 *  It is also used to record new ringtone and to pass its path to editor
 *
 *  @since S60 v9.1
 */
class CAknIconArray;
class CNewFileServiceClient;
class CFLDFileListContainer;

class CEvtMgmtUiRingtoneComponent: public CBase
    {
    private:
        /*
         *  'Set tone' popup dialog options
         */
        enum TEvtMgmtUiAssignedTone
            {
            /*
             * It indicates that 'select tone from existing' is selected
             * from popup dialog
             */
            EEvtMgmtUiAssignedToneTone,
            /*
             * It indicates that 'record new tone' is selected
             * from popup dialog
             */
            EvtMgmtUiAssignedToneRecordNew
            };
        
    public:  // Constructor and destructor  
        /**
         * Static Two phase constructor
         * @param [in] aPrevRingtone - previous ringtone of the existing event
         *                           - null for newly created event            
         */
        static CEvtMgmtUiRingtoneComponent* NewL( const TPtrC aPrevRingtone );
      /**
       * Static Two phase constructor
       * @param [in] aPrevRingtone - previous ringtone of the existing event
       *                           - null for newly created event            
       */
      static CEvtMgmtUiRingtoneComponent* NewLC( const TPtrC aPrevRingtone );

      /**
       * Virtual destructor
       */
      virtual ~CEvtMgmtUiRingtoneComponent(); 
      
    public:// new functions  
                
     /**
     * It is used to retrieve the selected ringtone's absolute path.
     * 
     * @since S60 v9.1
     * 
     * @ret absolute path of the selected ringtone
     */
     TPtrC Ringtone() const;      
                
     /**
     * It is used to retrieve the default ringtone's absolute path.
     * 
     * @since S60 v9.1
     * 
     * @ret absolute path of the default ringtone
     */
     TPtrC DefaultRingtoneL(); 
     
     /**
      * It is used to display 'Set tone' popup dialog and to retrieve ringtone.
      * 
      * @since S60 v9.1
      * 
      * @ret error if there is any error
      *      KErrNone if it is successful
      */
     TInt DisplayAudioTonePopupDlgL();  
   
      
    private:  // Constructor
        /**
         * C++ Default constructor
         */       
        CEvtMgmtUiRingtoneComponent();
  
        /**
         * Second phase of the two phase constructor
         * @param [in] aPrevRingtone - previous ringtone of the existing event     
         */ 
         void ConstructL( const TPtrC aPrevRingtone );
         
    private: // new functions
           
       /**
        * It is used to display generic popup dialog.
        * 
        * @since S60 v9.1
        *
        * @ret index of item selected if OK is pressed
        *       KErrCancel if Cancel is pressed 
        */
       TInt DisplayPopupDlgL();
       
       /*
        * It is used to record new ringtone.
        * 
        * @since S60 v5.0
        * 
        * @ret KErrNone if recording is succesful.
        *      error code if it is failed.
        */
       TInt RecordNewToneL();
       
       /*
        * It is used to generate filename for recording new file.
        * 
        * @since S60 v5.0
        * 
        * @param[out] aFileName - it will contain generated file-name
        * @ret KErrNone if recording is succesful.
        *      error code if it is failed.
        */
       TInt  FileNameForRecordingL( TDes& aFileName );
       
       /**
          * It is used display  system's 'select tone' dialog.
          * 
          * @since S60 v5.0
          * 
          * @ret error while displaying 'select tone' dialog and 
          *      retrieving ringtone from it. 
          *      KErrNone if it is successful
          */
         TInt DisplayRingtoneSelectionDlgL();   
         
       /**
         * It is used to update icons
         */
        void UpdateIconsL( CAknListQueryDialog& aDialog );    
         
        /**
         * Append icon to icon array. CAknIconArray owns icons so it is 
         * easier to it leave safely.
         * @param aIcons icon array
         * @param aIconFileWithPath icon file name with path
         * @param aSkinID skin id of the icon
         * @param aIconGraphicsIndex picture index
         * @param aIconGraphicsMaskIndex mask index
         */
        void AddIconL( CAknIconArray&       aIcons,
                       const TDesC&         aIconFileWithPath,
                       TAknsItemID          aSkinID,
                       TInt                 aIconGraphicsIndex,
                       TInt                 aIconGraphicsMaskIndex );
                             
        /**
         * Gets Events Management UI icon file path and name. Ownership
         * is transferred, item is left in the cleanup stack.
         * @return TFileName* The full name of the Events UI icon file.
         */        
        TFileName* IconFileNameLC() const;
    
    private:
        /**
         * Ringtone value
         * owns
         */
        HBufC* iRingtone;
		
        /**
         * New File Service Instance
         * owns
         */
        CNewFileServiceClient* iRecorderClient;
		
        /**
         * File List component
         * owns
         */
        CFLDFileListContainer*  iSelectRingtoneDlg;
    };

#endif //EVTMGMTUIRINGTONECOMPONENT_H_
