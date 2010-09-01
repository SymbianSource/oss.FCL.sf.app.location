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
* Description:  Interface class for Events Info Note DLL
*
*/


// System Includes
#include <eikenv.h>
#include <eiknotapi.h>

// User Includes
#include "evtinfonote.h"
#include "evtdebug.h"

// Constant Definitions

// CONSTANTS
const TInt KEvtInfoNoteArrayIncrement = 2;


// ----------------------- Local Functions -----------------------------------

// ---------------------------------------------------------
// LOCAL_C void CreateEventsInfoNoteNotifiersL
// Instantiates all the components of Events Info Note
// notifiers
// ---------------------------------------------------------
LOCAL_C void CreateEventsInfoNoteNotifiersL( 
                    CArrayPtrFlat< MEikSrvNotifierBase2 >* aNotifiers )
    {      
    // Create the Trigger Event fire info note
    CEvtInfoNote* infoNote = CEvtInfoNote::NewL();
    CleanupStack::PushL( infoNote );
    aNotifiers->AppendL( infoNote );
    CleanupStack::Pop( infoNote );  
    }

// ------------------------- Global Functions -----------------------------------

// ---------------------------------------------------------
// EXPORT_C CArrayPtr< MEikSrvNotifierBase2 >* NotifierArray
// Polymorphic DLL entry point
// ---------------------------------------------------------

EXPORT_C CArrayPtr< MEikSrvNotifierBase2 >* NotifierArray()
    {
	EVTUIDEBUG( "+ CEvtInfoNoteInterface::NotifierArray" );
    CArrayPtrFlat< MEikSrvNotifierBase2 >* notifiers = NULL;
	TRAPD( err, notifiers = 
	           new CArrayPtrFlat< MEikSrvNotifierBase2 >( KEvtInfoNoteArrayIncrement ));
	if ( !err && notifiers )
    	{
    	TRAPD( err, CreateEventsInfoNoteNotifiersL( notifiers ));
    	if( err )
        	{
        	TInt count = notifiers->Count();
        	while( count-- )
        	    {
        	    ( *notifiers )[ count ]->Release();
        	    }           	
        	delete notifiers;
        	notifiers = NULL;
        	}   	    
    	}           
	EVTUIDEBUG( "- CEvtInfoNoteInterface::NotifierArray" );
    return notifiers;
    }
