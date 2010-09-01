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


// System Includes

// User Includes
#include "evtinfonoteparams.h"

// Constant function defintions

//
// ------------------------- Member Function definition ----------------------

// ----------------------------------------------------------
// CEvtInfoNoteInputParam::CEvtInfoNoteInputParam()
// ----------------------------------------------------------
//
CEvtInfoNoteInputParam::CEvtInfoNoteInputParam()
    {
    }

// ----------------------------------------------------------
// CEvtInfoNoteInputParam::~CEvtInfoNoteInputParam()
// ----------------------------------------------------------
//
CEvtInfoNoteInputParam::~CEvtInfoNoteInputParam()
    {
    // Delete the Subject field
    delete iSubject;
    
    // Delete the Description field
    delete iDescription;
    
    // Delete the Tone field
    delete iTone;  
    }

// ----------------------------------------------------------
// void CEvtInfoNoteInputParam::ConstructL()
// ----------------------------------------------------------
//    
void CEvtInfoNoteInputParam::ConstructL()
    {
    }

// ----------------------------------------------------------
// CEvtInfoNoteInputParam* CEvtInfoNoteInputParam::NewL()
// ----------------------------------------------------------
//
EXPORT_C CEvtInfoNoteInputParam* CEvtInfoNoteInputParam::NewL()
    {
    CEvtInfoNoteInputParam* self = new ( ELeave ) CEvtInfoNoteInputParam;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// TPtrC CEvtInfoNoteInputParam::Subject
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CEvtInfoNoteInputParam::Subject() const
    {
    // Check whether the subject exists, incase it doesnt exist then
    // return a NULL string else return Subject.
    if ( iSubject )
        {
        return *iSubject;
        }
    else
        {
        return TPtrC();
        }
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteInputParam::SetSubjectL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtInfoNoteInputParam::SetSubjectL( const TDesC&   aSubject )
    {   
    delete iSubject;
    iSubject = NULL;
    
    // Copy the subject data from the allocated string
    iSubject = aSubject.AllocL();
    }
    
// ---------------------------------------------------------------------------
// TPtrC CEvtInfoNoteInputParam::Description
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CEvtInfoNoteInputParam::Description() const
    {
    // Check whether the description exists, incase it doesnt exist then
    // return a NULL string else return description.
    if ( iDescription )
        {
        return *iDescription;
        }
    else
        {
        return TPtrC();
        }
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteInputParam::SetDescriptionL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtInfoNoteInputParam::SetDescriptionL( const TDesC&   aDescription )
    {   
    TInt KMaxDescriptionLength      = 256;
    TInt length = ( aDescription.Length()) > KMaxDescriptionLength ? KMaxDescriptionLength : aDescription.Length();

    delete iDescription;
    iDescription = NULL;
    iDescription = HBufC::NewL( length );

    iDescription->Des().Copy( aDescription.Ptr(), length );
    }    

// ---------------------------------------------------------------------------
// TUint32 CEvtInfoNoteInputParam::TrigerringAccuracy
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtInfoNoteInputParam::TEvtTriggerAccuracy 
                    CEvtInfoNoteInputParam::TrigerringAccuracy() const
    {
    return iAccuracy;
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteInputParam::SetTrigerringAccuracy
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtInfoNoteInputParam::SetTrigerringAccuracy( 
                    CEvtInfoNoteInputParam::TEvtTriggerAccuracy  aAccuracy )
    {
    iAccuracy = aAccuracy;
    }
    
// ---------------------------------------------------------------------------
// void CEvtInfoNoteInputParam::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtInfoNoteInputParam::ExternalizeL( RWriteStream&   aWriteStream )
    {
    // The format for packing the buffer would be as follows
    // Trigger Accuracy | SubjectLength | Subject | DescriptionLength | Description
    //
    // where
    // Trigger Accuracy     - The accuracy of the trigger which is fired
    // SubjectLength        - The Length of the subject field
    // Subject              - Subject
    // DescriptionLength    - The Length of the description field
    // Description          - Description field
    // Snooze Shown        	- Snnoze field
    // Tone Enabled        	- Tone Enabled field
    // ToneLength    		- The Length of the Tone field
    // Tone         		- Tone field
    // Tone Repeat        	- Tone Repeat field
    
    // First pack the Trigger accuracy
        
    aWriteStream.WriteUint32L( iAccuracy );
        
    // Pack the length of the subject data. If there is no subject
    // data then pack zero.
    TUint length = 0;
    if ( iSubject )
        {
        length = iSubject->Length();
        }
    aWriteStream.WriteUint32L( length );
    
    // If there is a valid subject field pack it onto the buffer.
    if ( length )
        {
        aWriteStream.WriteL( iSubject->Des(), length );
        }
    
    // Pack the length of the description data. If there is no description
    // data then pack zero.
    length = 0;
    if ( iDescription )
        {
        length = iDescription->Length();
        }
    aWriteStream.WriteUint32L( length );
    
    // If there is a valid description field pack it onto the buffer.
    if ( length )
        {
        aWriteStream.WriteL( iDescription->Des(), length );
        }
    
    // Write the show snooze field
    aWriteStream.WriteUint32L( iShowSnooze );
    
    // Write the tone enabled field
    aWriteStream.WriteUint32L( iToneEnabled );
    
    // Pack the length of the tone data. If there is no tone
    // data then pack zero.
    length = 0;
    if ( iTone )
        {
        length = iTone->Length();
        }
    aWriteStream.WriteUint32L( length );
    
    // If there is a valid tone field pack it onto the buffer.
    if ( length )
        {
        aWriteStream.WriteL( iTone->Des(), length );
        }
    
    // Write the tone repeat field
    aWriteStream.WriteUint32L( iToneRepeat );
                       
    aWriteStream.CommitL();
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteInputParam::InternalizeL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CEvtInfoNoteInputParam::InternalizeL( RReadStream&    aReadStream )
    {
    // Clear the Subject field
    delete iSubject;
    iSubject = NULL;
    
    // Clear the Description field
    delete iDescription;
    iDescription = NULL;
    
    // The first element in the array if the Trigger accuracy variable
    iAccuracy = static_cast< TEvtTriggerAccuracy >( aReadStream.ReadUint32L());
        
    // Now get the length of the subject field.
    TUint32 length = aReadStream.ReadUint32L();
    if ( length >= KMaxTInt/2 )
        {
        User::Leave( KErrArgument );
        }        
    // Set the subject only if the length is a +ve value.
    if ( length )
        {
        iSubject = HBufC::NewL( length );
        TPtr subject( iSubject->Des());
        aReadStream.ReadL( subject, length );
        }
        
    // Now get the length of the description field.
    length = aReadStream.ReadUint32L();
    if ( length >= KMaxTInt/2 )
        {
        User::Leave( KErrArgument );
        }        
    // Set the description only if the length is a +ve value.
    if ( length )
        {
        iDescription = HBufC::NewL( length );
        TPtr description( iDescription->Des());
        aReadStream.ReadL( description, length );
        }
        
    // Set the snooze show
    iShowSnooze = aReadStream.ReadUint32L(); 
        
    // Set the tone enabled
    iToneEnabled = aReadStream.ReadUint32L(); 
        
    // Set the tone field
    // Now get the length of the tone field.
    length = aReadStream.ReadUint32L();
    if ( length >= KMaxTInt/2 )
        {
        User::Leave( KErrArgument );
        } 
    // Set the tone only if the length is a +ve value.
    if ( length )
        {
        iTone = HBufC::NewL( length );
        TPtr tone( iTone->Des());
        aReadStream.ReadL( tone, length );
        }                  
    // Set the tone repeat
    iToneRepeat = aReadStream.ReadUint32L();          
    }

// ---------------------------------------------------------------------------
// TPtrC CEvtInfoNoteInputParam::Tone
// ---------------------------------------------------------------------------
//      
EXPORT_C TPtrC CEvtInfoNoteInputParam::Tone() const
    {
    // Check whether the tone exists, incase it doesnt exist then
    // return a NULL string else return Tone.
    if ( iTone )
        {
        return *iTone;
        }
    else
        {
        return TPtrC();
        }
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteInputParam::SetToneL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtInfoNoteInputParam::SetToneL( const TDesC&  aTone )
    {
    delete iTone;
    iTone = NULL;
    
    // Copy the tone data from the allocated string
    iTone = aTone.AllocL();        
    }

// ---------------------------------------------------------------------------
// TBool  CEvtInfoNoteInputParam::ToneRepeat
// ---------------------------------------------------------------------------
//
EXPORT_C TBool  CEvtInfoNoteInputParam::ToneRepeat() const
    {
    return iToneRepeat;
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteInputParam::SetToneRepeat
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtInfoNoteInputParam::SetToneRepeat( TBool  aRepeat )
    {
    iToneRepeat = aRepeat;
    }

// ---------------------------------------------------------------------------
// TBool  CEvtInfoNoteInputParam::ToneEnabled
// ---------------------------------------------------------------------------
//
EXPORT_C TBool  CEvtInfoNoteInputParam::ToneEnabled() const
    {
    return iToneEnabled;
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteInputParam::SetToneEnabled
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtInfoNoteInputParam::SetToneEnabled( TBool  aEnabled )
    {
    iToneEnabled = aEnabled;
    }

// ---------------------------------------------------------------------------
// TBool  CEvtInfoNoteInputParam::ShowSnooze
// ---------------------------------------------------------------------------
//
EXPORT_C TBool  CEvtInfoNoteInputParam::ShowSnooze() const
    {
    return iShowSnooze;
    }

// ---------------------------------------------------------------------------
// void CEvtInfoNoteInputParam::SetShowSnooze
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtInfoNoteInputParam::SetShowSnooze( TBool  aSnooze )
    {
    iShowSnooze = aSnooze;
    }
