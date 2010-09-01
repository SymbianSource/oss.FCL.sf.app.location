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
* Description:  Tone/File Utility.
*
*/


// System Includes
#include <apgcli.h>
#include <DRMHelper.h>
#include <f32file.h>
#include <bautils.h>

// User Includes
#include "evttoneutils.h"
#include "evtdebug.h"

// Constants
// Rich audio file MIME types
_LIT(KAac, "audio/aac");
_LIT(KMp3, "audio/mp3");
_LIT(KMpeg, "audio/mpeg");
_LIT(K3gpp, "audio/3gpp");
_LIT(KMp4, "audio/mp4");
_LIT(KAmrWb, "audio/amr-wb");
_LIT(KWavX, "audio/x-wav");
_LIT(KWav, "audio/wav");
_LIT( KRngMimeType, "application/vnd.nokia.ringing-tone" );

// ================ Member funtions for CEvtVibraHandler class ===============

// -----------------------------------------------------------------------------
// EvtToneUtils::DataType()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool EvtToneUtils::IsRNGTypeL( const TDesC& aFileName )
    {
    TPtrC type(EvtToneUtils::DataTypeL( aFileName ).Des());
    
    if( type.CompareF( KRngMimeType ) == 0 )
        return ETrue;
    else
    	return EFalse;
    }

// -----------------------------------------------------------------------------
// EvtToneUtils::IsFilePlayableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TBool EvtToneUtils::IsFilePlayableL( const TDesC& aFileName )
    {
    TPtrC type(EvtToneUtils::DataTypeL( aFileName ).Des());
        
    if ( type == KAac  || type == KMp3 || type == KMpeg ||
         type == K3gpp || type == KMp4 || type == KAmrWb ||
         type == KWavX || type == KWav )
        {
        ContentAccess::CContent* content = NULL;
        TRAPD( err, content = ContentAccess::CContent::NewL( aFileName ) );
        if ( err == KErrNone && content )
            {
            // ECanPlay checks rights as well as EIsProtected
            TInt canPlay( 0 );
            content->GetAttribute( ContentAccess::ECanPlay, canPlay );
            delete content;
            return canPlay;        
            }
        return EFalse;
        }
    
    return ETrue; // Other MIMEs can be played without DRM check.    
    }
    
// -----------------------------------------------------------------------------
// EvtToneUtils::DataTypeL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDataType EvtToneUtils::DataTypeL( const TDesC& aFileName )
    {
    RApaLsSession apaLsSession;
    User::LeaveIfError( apaLsSession.Connect() );
    CleanupClosePushL( apaLsSession );
    
	TDataType dataType;
   	TUid dummyUid( KNullUid );
   	
	User::LeaveIfError( apaLsSession.AppForDocument( aFileName, dummyUid, dataType ) );
	
    CleanupStack::PopAndDestroy(); // apaLsSession
    
    return dataType;
    }
// -----------------------------------------------------------------------------
// EvtToneUtils::IsExistL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TBool EvtToneUtils::IsExistL( const TDesC& aFileName )
    {
	TBool isPresent = EFalse;
	
    RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	if( BaflUtils::FileExists(fsSession, aFileName) )
    	{
    	isPresent = ETrue;
    	}
    fsSession.Close();
    
    return isPresent;
    }
