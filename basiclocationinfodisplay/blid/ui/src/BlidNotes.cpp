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
* Description:  Provides blid note class methods.
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <aknnotewrappers.h> 
#include <Blid.rsg>
#include <aknstaticnotedialog.h> 
#include "BlidNotes.h"
#include "blidcommonconsts.h"

TBool BlidNotes::iNoteDisplayed = EFalse;
TBool BlidNotes::iGPSUnavailableDisplayed = EFalse;
CEikonEnv* BlidNotes::iEnv = CEikonEnv::Static();

// ================= MEMBER FUNCTIONS =======================
//-----------------------------------------------------------------------
// BlidNotes::OverWriteWaypointNoteL
// ----------------------------------------------------------------------------
//
void BlidNotes::OverWriteWaypointNoteL()
    {
    HBufC* noteText = StringLoader::LoadL( R_BLID_ERROR_MAX_WAYPOINTS, 
                                           KMaxNumberOfWaypoints,
                                           iEnv );
    CleanupStack::PushL(noteText);
    CAknInformationNote* dialog = 
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
    }
    
//-----------------------------------------------------------------------
// BlidNotes::OutOfMemoryNoteL
// ----------------------------------------------------------------------------
//
void BlidNotes::OutOfMemoryNoteL()
	{
    HBufC* noteText = StringLoader::LoadL( R_MEMLO_NOT_ENOUGH_MEMORY, iEnv );
    CleanupStack::PushL(noteText);    
    CAknErrorNote* dialog = 
        new(ELeave)CAknErrorNote(ETrue);    
    dialog->SetTone(CAknNoteDialog::EErrorTone);
    dialog->ExecuteLD( *noteText );
    
    CleanupStack::PopAndDestroy(noteText); //noteText	
	}

//-----------------------------------------------------------------------
// BlidNotes::EmptyLandmarkNoteL
// ----------------------------------------------------------------------------
//
void BlidNotes::EmptyLandmarkNoteL(const TDesC& landmarkName)
	{
    HBufC* noteText = StringLoader::LoadL( R_BLID_EMPTY_LANDMARK_DESTINATION, 
                                           landmarkName,
                                           iEnv );
    CleanupStack::PushL(noteText);
    CAknInformationNote* dialog = 
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
	}

//-----------------------------------------------------------------------
// BlidNotes::PoorGPSDataNoteL
//-----------------------------------------------------------------------
//
void BlidNotes::PoorGPSDataNoteL()
    {
    HBufC* noteText = StringLoader::LoadLC( R_BLID_GPS_TIMEOUT_NOTE, iEnv );

    CAknInformationNote* dialog = 
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
    }
    
//-----------------------------------------------------------------------
// BlidNotes::NoPsyConnectedNoteL
//-----------------------------------------------------------------------
//
void BlidNotes::NoPsyConnectedNoteL()
    {
    HBufC* noteText = StringLoader::LoadLC( R_BLID_ERROR_NO_PSY, iEnv );

    CAknInformationNote* dialog = 
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
    }
    
//-----------------------------------------------------------------------
// BlidNotes::SearchingGPSDataNoteL
//-----------------------------------------------------------------------
//
void BlidNotes::SearchingGPSDDataNoteL()
    {
    HBufC* noteText = StringLoader::LoadLC( R_BLID_WAITNOTE_SEARCHING_GPS_SATELLITES, iEnv );
	   
    CAknInformationNote* dialog = 
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );    
    CleanupStack::PopAndDestroy(noteText); //noteText
    }

//-----------------------------------------------------------------------
// BlidNotes::GPDDataAvailableL
//-----------------------------------------------------------------------
//
void BlidNotes::GPSDataAvailableL()
    {
    if(!iNoteDisplayed)
        {
        iNoteDisplayed = ETrue;
        iGPSUnavailableDisplayed = ETrue;
        HBufC* noteText = StringLoader::LoadLC( R_BLID_NOTE_GPS_DATA, iEnv );	   
        CAknInformationNote* dialog = 
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
        dialog->ExecuteLD( *noteText );    
        CleanupStack::PopAndDestroy(noteText); //noteText        
        }
    }
    
//-----------------------------------------------------------------------
// BlidNotes::GPSDeviceUnavailableNoteL
//-----------------------------------------------------------------------
//
void BlidNotes::GPSDeviceUnavailableNoteL()
    {
    if(!iGPSUnavailableDisplayed)
        {
        iGPSUnavailableDisplayed = ETrue;
        iNoteDisplayed = ETrue;
        HBufC* noteText = StringLoader::LoadLC( R_BLID_ERROR_NO_GPS_DEVICE, iEnv );	   
        CAknInformationNote* dialog = 
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
        dialog->ExecuteLD( *noteText );    
        CleanupStack::PopAndDestroy(noteText); //noteText        
        }
    }

//-----------------------------------------------------------------------
// BlidNotes::NoGPSDeviceL
//-----------------------------------------------------------------------
//
void BlidNotes::NoGPSDeviceL()
	{
    iNoteDisplayed = ETrue;
    HBufC* noteText = StringLoader::LoadLC( R_BLID_ERROR_NO_GPS_DEVICE, iEnv );	   
    CAknInformationNote* dialog = 
    new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );    
    CleanupStack::PopAndDestroy(noteText); //noteText
	}

// End of File
