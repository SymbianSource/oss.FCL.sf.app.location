/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Debug File.
*
*/



// INCLUDE FILES
#include "evtdebug.h"
#include <flogger.h>
#include <e32svr.h>
#include <f32file.h>
#include <bautils.h>
// CONSTANTS

/// Folder where the log resides
_LIT( KEvtLogFolder, "epos" );

/// The name of the log file
_LIT( KEvtLogFileName, "eventsui[%S].txt" );

/// Events App Name
_LIT( KEvtLogAppFileName, "evtmgmtui" );

/// Handler App Name
_LIT( KEvtLogHanlderFileName, "evthandler" );

/// The format in which the time is formatted in log
_LIT( KEvtLogTimeFormat, "%02d.%02d:%02d:%06d ");

/// The length of the string produced by KLocLogTimeFormat
const TInt KEvtLogTimeFormatLength = 16;

/// How many characters a log line can contain
const TInt KEvtLogLineLength = 256;

/// Length of the Log file name.
const TInt KEvtLogFileLength = 32;

// ========================== OTHER EXPORTED FUNCTIONS =========================



// -----------------------------------------------------------------------------
// Debug
// Generates a log file if c:\logs\epos\ folder exists
// -----------------------------------------------------------------------------
//
EXPORT_C void Debug( TRefByValue<const TDesC> aText, ... )
    {    
    VA_LIST args;
    VA_START( args, aText );
    
    TBuf<KEvtLogLineLength> buf;
    buf.FormatList( aText, args );

    #ifdef _DEBUG
    RDebug::Print(buf);
    #endif

	RProcess currentProcess;
	TParse parser;
	TBuf<KEvtLogFileLength> name;
	
	parser.Set( currentProcess.FileName(), NULL, NULL );
	TPtrC filename( parser.Name() );
	
	if( !filename.Compare( KEvtLogAppFileName() ) )
		name.Format(KEvtLogFileName(), &filename);
	else
		name.Format(KEvtLogFileName(), &KEvtLogHanlderFileName() );
	
    RFileLogger logger;
    
    TInt ret=logger.Connect();
    if (ret==KErrNone)
        {
        logger.SetDateAndTime( EFalse,EFalse );
        logger.CreateLog( KEvtLogFolder, name, EFileLoggingModeAppend );       
        TBuf<KEvtLogTimeFormatLength> timeStamp;
        TTime now;
        now.HomeTime();
        TDateTime dateTime;
        dateTime = now.DateTime();
        timeStamp.Format( KEvtLogTimeFormat, 
            dateTime.Hour(), dateTime.Minute(),
            dateTime.Second(), dateTime.MicroSecond() );
        buf.Insert( 0, timeStamp );

        logger.Write(buf);
        }

    logger.Close();

    VA_END( args );
    }
    
// -----------------------------------------------------------------------------
// CreateDir
// Create a log Directory
// -----------------------------------------------------------------------------
//
EXPORT_C void CreateDir()
	{
	_LIT( KEvtLogFolderPath, "C:\\logs\\epos\\" );
    RFs fsSession;
	TRAP_IGNORE(fsSession.Connect());
	if(!BaflUtils::FolderExists(fsSession, KEvtLogFolderPath()))
		{
		fsSession.MkDir( KEvtLogFolderPath() );
		}
    fsSession.Close();
	}
	
//  End of File  
