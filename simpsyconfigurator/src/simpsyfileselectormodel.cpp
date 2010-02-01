/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  List-box model class for Simulation PSY file selector dialog.
*
*/


// SYSTEM INCLUDES
#include <eikappui.h>
#include <eikenv.h>
#include <eikapp.h>

// USER INCLUDES
#include "simpsyfileselectormodel.h"

// CONSTANT DEFINTION
const TInt KSimPsyBufferSize = 128;
const TInt KSimPsyFileIconId = 0;
_LIT( KNmeaFilePath1,"\\system\\data\\" );
_LIT( KNmeaFilePath2,"\\data\\SimFile\\" );
_LIT( KSPSFileExtension, ".SPS");
_LIT( KNMEAFileExtension, ".NME");

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CSimPsyFileSelectorModel::CSimPsyFileSelectorModel
// ---------------------------------------------------------------------------
//
CSimPsyFileSelectorModel::CSimPsyFileSelectorModel()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }

// ---------------------------------------------------------------------------
// CSimPsyFileSelectorModel::~CSimPsyFileSelectorModel
// ---------------------------------------------------------------------------
//
CSimPsyFileSelectorModel::~CSimPsyFileSelectorModel()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete the MdcaPoint buffer
    delete iBuffer;
    
    // Reset and close the file name array
    iFileNameArray.Reset();
    iFileNameArray.Close();
    }

// ---------------------------------------------------------------------------
// void CSimPsyFileSelectorModel::ConstructL
// ---------------------------------------------------------------------------
//
void CSimPsyFileSelectorModel::ConstructL()
    {
    // Create the Buffer for packing the MdcaPoint
    iBuffer = HBufC16::NewL( KSimPsyBufferSize );
    
    TRAP_IGNORE( AppendFileL( KNmeaFilePath2 ));    
	TRAP_IGNORE( AppendFileL( KNmeaFilePath1 ));           
    }

// ---------------------------------------------------------------------------
// CSimPsyFileSelectorModel* CSimPsyFileSelectorModel::NewL
// ---------------------------------------------------------------------------
//
CSimPsyFileSelectorModel* CSimPsyFileSelectorModel::NewL()
    {
    CSimPsyFileSelectorModel* self = new ( ELeave ) CSimPsyFileSelectorModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// TPtrC CSimPsyFileSelectorModel::SimulationFileNameL
// ---------------------------------------------------------------------------
//
TPtrC CSimPsyFileSelectorModel::SimulationFileNameL( TInt aIndex )
	{   	
	if ( aIndex < 0 || aIndex > ( iFileNameArray.Count() - 1 ))
	    {
	    User::Leave( KErrArgument );
	    }
	return ( iFileNameArray[ aIndex ]);
	}
	
// ---------------------------------------------------------------------------
// TInt CSimPsyFileSelectorModel::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CSimPsyFileSelectorModel::MdcaCount() const
    {
    return iFileNameArray.Count();    	
    }
    
// ---------------------------------------------------------------------------
// TPtrC16 CSimPsyFileSelectorModel::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC16 CSimPsyFileSelectorModel::MdcaPoint( TInt aIndex ) const
    {            
    // Pack the contents into the Buffer. We dont have to re-allocate the buffer
    // everytime because the length of the name field is restricted in the
    // Location Centre Registration API
		    
    // Tabulator Defintion
    _LIT( KTab, "\t" );
    
    // Create formatted item string.
    // list_single_large_pane:
    // list item string format: "0\tTextLabel"
    // where 0 is an index to icon array
	TPtr16	buf( iBuffer->Des());
	buf.Zero();
	
	// Append the Icon Index
	buf.AppendNum( KSimPsyFileIconId );
	
	// Append the Tab Key
	buf.Append( KTab );
	
	// Append only the file name without the full path.
    TParsePtrC parsedEntry( iFileNameArray[ aIndex ] );
    TPtrC fileName( parsedEntry.NameAndExt() );    	        	
	buf.Append( fileName );
	
    return buf;
    }			

// ---------------------------------------------------------------------------
// void CSimPsyFileSelectorModel::GetCorrectPathL
// ---------------------------------------------------------------------------
//
void CSimPsyFileSelectorModel::GetCorrectPathL( TFileName& aFilePath )
	{
    CEikAppUi* appUi = 
    		static_cast< CEikAppUi* >( CEikonEnv::Static()->AppUi());
    TFileName* dllDrive = 
    	new( ELeave ) TFileName( appUi->Application()->AppFullName());
    CleanupStack::PushL( dllDrive );
	
	TParse parse;
	User::LeaveIfError( parse.Set( *dllDrive, NULL, NULL ));
    User::LeaveIfError( parse.Set( parse.Drive(), &aFilePath, NULL ));
	aFilePath = parse.FullName();
	CleanupStack::PopAndDestroy( dllDrive );
	}

// ---------------------------------------------------------------------------
// void CSimPsyFileSelectorModel::AppendFileL
// ---------------------------------------------------------------------------
//	
void CSimPsyFileSelectorModel::AppendFileL( const TDesC&	aDir )
	{
    // Open up a session to the file server
    RFs	fs;
    User::LeaveIfError( fs.Connect());
    CleanupClosePushL( fs );
        
    // Obtain a list of entries for the system\data folder
    TFileName folder( aDir );
    GetCorrectPathL( folder );        
    CDir* entryList = NULL;
    User::LeaveIfError( fs.GetDir( folder, 
    							   KEntryAttNormal, 
    							   ESortNone,
    							   entryList ));
	// The scan was successful
	CleanupStack::PushL( entryList );
	TInt count = entryList->Count();
	for ( TInt i = 0; i < count; i++ )
		{
		TFileName filename ((( *entryList)[i]).iName);
		filename.UpperCase();
		
		// Check for the extension. The extension can be only .sps or
		// .nme
		TParsePtrC parsedEntry( filename );
		TPtrC ext( parsedEntry.Ext());     	
    	if ( !ext.Compare( KSPSFileExtension ) || 
    		 !ext.Compare( KNMEAFileExtension ) )
    		{
    		// SPS or NMEA file. Append to the File name array.
    		filename.Zero();
    		filename.Copy( folder );
    		filename.Append((( *entryList)[i]).iName );
    		iFileNameArray.Append( filename );
    		}
		}
	CleanupStack::PopAndDestroy( entryList );
	
    // Pop and destroy the file session	
    CleanupStack::PopAndDestroy();		
	}
