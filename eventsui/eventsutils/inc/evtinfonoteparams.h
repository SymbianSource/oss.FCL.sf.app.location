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
* Description:  Exchange parameters for exchange between the Events UI server
*                Event Info Note pop-up
*
*/


#ifndef C_EVTINFONOTEPARAMS_H
#define C_EVTINFONOTEPARAMS_H

// System Includes
#include <e32base.h>
#include <s32strm.h>

// User Includes

// Forward Declarations

/**
 * Input paramters for Events UI info note display
 *
 * This class provides the Internalize and Externalize functionalities
 * for the input parameters in Events UI info display. The parameters
 * currently contain
 * 1. The Name of the trigger
 * 2. Detailed description of the trigger
 * 3. Trigerring accuracy
 * 4. Tone String
 * 5. Tone Repeat Flag
 * 6. Tone Enable Flag
 * 7. Snooze Flag
 *
 * @since S60 v9.1
 */
class CEvtInfoNoteInputParam : public CBase
    {
public:
    /**
     * Enumeration to define the accuracy of the fired triggers
     * @since S60 v9.1      
     */
    enum TEvtTriggerAccuracy
        {
        /**
         * Enum value for an accurate trigger
         */
        EEvtAccurateTrigger = 0,
         
        /**
         * Enum value for an inaccurate trigger
         */
        EEvtInaccurateTrigger, 
        /**
         * Enum value for a heavily inaccurate trigger
         */
        EEvtHighlyInaccurateTrigger
        };
        	
public:
    /**
     * Static two phase constructor
     * @since S60 v9.1      
     */
    IMPORT_C static CEvtInfoNoteInputParam* NewL();
    
    /**
     * Destructor.
     * @since S60 v9.1     
     */
    virtual ~CEvtInfoNoteInputParam();
    
    /**
     * Retrieves the subject/name field for the trigger.
     *
     * @since S60 v9.1     
     * @return, The subject/name of the trigger
     */      
    IMPORT_C TPtrC Subject() const;
    
    /**
     * Sets the subject/name field for the trigger.
     *
     * @since S60 v5.0     
     * @param aSubject The subject/name of the trigger.
     */     
    IMPORT_C void SetSubjectL( const TDesC&  aSubject );    

    /**
     * Retrieves the Description field for the trigger.
     *
     * @since S60 v9.1     
     * @return, The Description of the trigger
     */      
    IMPORT_C TPtrC Description() const;
    
    /**
     * Sets the Description field for the trigger.
     *
     * @since S60 v9.1    
     * @param aDescripion The Description of the trigger.
     */     
    IMPORT_C void SetDescriptionL( const TDesC&  aDescription );
    
     /**
     * The accuracy with which the trigger has been fired.
     *
     * @since S60 v9.1     
     * @return Triggering Accuracy      
     */
    IMPORT_C TEvtTriggerAccuracy TrigerringAccuracy() const;

    /** 
     * Sets the Mode in which the Location Application would be launched.
     *
     * @since S60 v9.1    
     * @param[in] aAccuracy Triggering Accuracy
     */
    IMPORT_C void SetTrigerringAccuracy( TEvtTriggerAccuracy  aAccuracy );
        
    /**
     * Externalizes the contents of this Array into a buffer.
     *
     * @since S60 v5.0
     * @param aWriteStream  Buffer onto which the contents of this structure
     *                      would be externalized.
     */
    IMPORT_C void ExternalizeL( RWriteStream&   aWriteStream );
   
    /**
     * Internalized the contents of the buffer into this array.
     * 
     * @since S60 v5.0
     * @param aReadStream   Buffer from which the contents of this
     *                      structures would be populated.
     */
    IMPORT_C void InternalizeL( RReadStream&    aReadStream );
    
    /**
     * Retrieves the Tone field for the trigger.
     *
     * @since S60 v9.1     
     * @return, The Tone of the trigger
     */      
    IMPORT_C TPtrC Tone() const;
    
    /**
     * Sets the Tone field for the trigger.
     *
     * @since S60 v9.1    
     * @param aDescripion The Tone of the trigger.
     */     
    IMPORT_C void SetToneL( const TDesC&  aDescription );
    
    /**
     * Tone Repeatition 
     *
     * @since S60 v9.1     
     * @return Boolean value to indicate whether tone is to be repeated
     */
    IMPORT_C TBool  ToneRepeat() const;

    /** 
     * Tone Repeatition 
     *
     * @since S60 v9.1    
     * @param[in] aRepeat Boolean value to indicate whether tone is to be repeated
     */
    IMPORT_C void SetToneRepeat( TBool  aRepeat );      
    
    /**
     * Tone Enabled 
     *
     * @since S60 v9.1     
     * @return Boolean value to indicate whether tone is enabled
     */
    IMPORT_C TBool  ToneEnabled() const;

    /** 
     * Tone Enabled 
     *
     * @since S60 v9.1    
     * @param[in] aRepeat Boolean value to indicate whether tone is enabled
     */
    IMPORT_C void SetToneEnabled( TBool  aRepeat );   
    
    /**
     * Show Snooze 
     *
     * @since S60 v9.1     
     * @return Boolean value to indicate whether snooze is shown
     */
    IMPORT_C TBool  ShowSnooze() const;

    /** 
     * Show Snooze 
     *
     * @since S60 v9.1    
     * @param[in] aSnooze Boolean value to indicate whether snooze is shown
     */
    IMPORT_C void SetShowSnooze( TBool  aSnooze );   
    	                
protected:
    /**
     * C++ constructor made protected for this class must not be instantiated
     * directly
     */
    CEvtInfoNoteInputParam();
    
    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();
            
private:  // Data members
    /**
     * Buffer which holds the Subject/Name field of the trigger
     * Owns
     */
    HBufC*                      iSubject;
    
    /**
     * Buffer which holds the Description field of the trigger
     * Owns
     */
    HBufC*                      iDescription;
    
    /**
     * Tone field for the trigger. If there is expectation that no tone
     * must be played, then this can be left empty
     */
    HBufC*                      iTone;
    
    /**
     * Boolean value to denote whether the tone is to be repeated.
     */
    TBool                       iToneRepeat;
    
    /**
     * Boolean value to denote whether the tone is enabled.
     */
    TBool                       iToneEnabled;
    
    /**
     * Enum value for the Trigger accuracy
     */
    TEvtTriggerAccuracy         iAccuracy;
    
    /**
     * Boolean value to show Snooze.
     */
    TBool                       iShowSnooze;
    };

#endif // C_EVTINFONOTEPARAMS_H
