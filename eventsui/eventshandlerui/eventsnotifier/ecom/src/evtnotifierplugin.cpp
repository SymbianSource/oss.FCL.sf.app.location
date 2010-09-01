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
* Description:  Entry function definition and ECOM plugin definition
*
*/


// System Includes
#include <ecom/implementationproxy.h>
#include <AknNotifierWrapper.h>

//User Includes
#include "evtnotifierpluginuid.hrh"
#include  "evtinfonoteinterface.h"
#include "evtdebug.h"

const MEikSrvNotifierBase2::TNotifierPriority KNotifierPriority = 
                                        MEikSrvNotifierBase2::ENotifierPriorityHigh;
                                        
// Constant Definitions
const TInt KMaxSynchReplyBufLength = 256;                                        
	
// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Helper method to perform cleanup of the CArrayPtrFlat array object.
// ---------------------------------------------------------------------------
//
void CleanupArray( TAny*        aArray )
    {
	EVTUIDEBUG( "+ CEvtInfoNotePlugin::CleanupArray" );
    CArrayPtrFlat< MEikSrvNotifierBase2 >* subjects = 
            static_cast< CArrayPtrFlat< MEikSrvNotifierBase2 >* >( aArray );
    TInt lastInd = subjects->Count() - 1;
    for ( TInt i = lastInd; i >= 0; i-- )
        {
        subjects->At( i )->Release();
        }    	    
    delete subjects;
	EVTUIDEBUG( "- CEvtInfoNotePlugin::CleanupArray" );
    }

// ---------------------------------------------------------------------------
// Creates an array of Notifer objects
// ---------------------------------------------------------------------------
//
CArrayPtr< MEikSrvNotifierBase2 >* DoCreateNotifierArrayL()
    {
	EVTUIDEBUG( "+ CEvtInfoNotePlugin::DoCreateNotifierArrayL" );
    // Create an array of Notifier objects
    CArrayPtrFlat< MEikSrvNotifierBase2 >* subjects =
                    new ( ELeave )CArrayPtrFlat< MEikSrvNotifierBase2 >( 3 );    
    CleanupStack::PushL( TCleanupItem( CleanupArray, subjects ));

    CAknCommonNotifierWrapper* master = 
        CAknCommonNotifierWrapper::NewL( 
                                    KEvtInfoNote,
                                    KEvtInfoNoteChannel,
                                    KNotifierPriority,
                                    _L("evtinfonote.dll"),
                                   KMaxSynchReplyBufLength);
	  		   
    subjects->AppendL( master );
    
    CleanupStack::Pop( subjects );
	EVTUIDEBUG( "- CEvtInfoNotePlugin::DoCreateNotifierArrayL" );
    return( subjects );
    }

// ---------------------------------------------------------------------------
// ECOM plugin instantiation function
// ---------------------------------------------------------------------------
//
CArrayPtr< MEikSrvNotifierBase2 >* NotifierArray()
    {
	EVTUIDEBUG( "+ CEvtInfoNotePlugin::NotifierArray" );
    CArrayPtr< MEikSrvNotifierBase2 >* array = NULL;
    TRAPD( error, array = DoCreateNotifierArrayL());
    if ( error )
        {
        return NULL;
        }
	EVTUIDEBUG( "- CEvtInfoNotePlugin::NotifierArray" );
    return array;
    }

/**
 * Events UI Notifier ECom implementation table.
 */
const TImplementationProxy KEvtNotifiersImplTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY( KEvtNotiferPluginImplUid, NotifierArray )
	};

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Gate/factory function
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	EVTUIDEBUG( "+ CEvtInfoNotePlugin::ImplementationGroupProxy" );
	aTableCount = sizeof( KEvtNotifiersImplTable ) / sizeof( TImplementationProxy );
	EVTUIDEBUG( "- CEvtInfoNotePlugin::ImplementationGroupProxy" );
	return KEvtNotifiersImplTable;
	}
