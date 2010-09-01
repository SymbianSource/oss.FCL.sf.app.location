/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides SimPsyUi listbox model that holds the
*				          currently selected NMEA File.
*
*/


// INCLUDE FILES
#include <e32keys.h>
#include <f32file.h>
#include <StringLoader.h> // StringLoader
#include <centralrepository.h>
#include <simpsyui.rsg>
#include "SimulationPSYInternalCRKeys.h"

#include "simpsyuilbmodel.h"

const TInt	KTabWidth = 1;
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSimPsyUiLBModel::NewL()
// Static Constuctor that completes the Symbian two-phase 
// construction. 
// ---------------------------------------------------------
CSimPsyUiLBModel* CSimPsyUiLBModel::NewL()
    {
    CSimPsyUiLBModel* lbmodel = NewLC();
    CleanupStack::Pop( lbmodel );
    return lbmodel;
    }

// ---------------------------------------------------------
// CSimPsyUiLBModel::NewLC()
// Static Constuctor that completes the Symbian two-phase 
// construction. It places the created object on the 
// Clean-up stack
// ---------------------------------------------------------
CSimPsyUiLBModel* CSimPsyUiLBModel::NewLC()
    {
    CSimPsyUiLBModel* lbmodel = new (ELeave) CSimPsyUiLBModel();
    CleanupStack::PushL( lbmodel );
    lbmodel->ConstructL();
    return lbmodel;
    }

// ---------------------------------------------------------
// CSimPsyUiLBModel::CSimPsyUiLBModel()
// C++ Default Constructor
// ---------------------------------------------------------
CSimPsyUiLBModel::CSimPsyUiLBModel()
    {
    }

// ---------------------------------------------------------
// CSimPsyUiLBModel::ConstructL()
// Symbian two phased constructor
// ---------------------------------------------------------
void CSimPsyUiLBModel::ConstructL()
    {
    iFileNameText = StringLoader::LoadL( R_SIMPSYUI_FILENAME_TEXT );
    TInt lLength = iFileNameText->Length();
    
    /* The length of the buffer is the length needed for 
     * the File name text, tab keys and the maximum file
     * name length
     */
    iBuffer = HBufC::NewL(KMaxFileName + KTabWidth + KTabWidth + lLength ); 
    
    }

// ---------------------------------------------------------
// CSimPsyUiLBModel::~CSimPsyUiLBModel()
// C++ Destructor
// ---------------------------------------------------------
CSimPsyUiLBModel::~CSimPsyUiLBModel()
    {
    delete iBuffer;
    iBuffer = NULL;
    
    delete iFileNameText;
    iFileNameText = NULL;
    }
    
// ---------------------------------------------------------
// CSimPsyUiLBModel::MdcaCount()
// Returns the number of descriptor elements in a descriptor array.

// @return The number of descriptor elements in a descriptor array.
// ---------------------------------------------------------
TInt CSimPsyUiLBModel::MdcaCount() const
    {
    return 1;
    }

// ---------------------------------------------------------
// CSimPsyUiLBModel::MdcaPoint()
// Indexes into a descriptor array.
//
// @param aIndex The position of the descriptor element within
// 				 a descriptor array. The position is relative 
//				 to zero; i.e. zero implies the first descriptor
//				 element in a descriptor array. 
//	@return 	 A 16 bit non-modifiable pointer descriptor 
//				 representing the descriptor element located at 
//				 position aIndex within a descriptor array. 
// ---------------------------------------------------------
TPtrC16 CSimPsyUiLBModel::MdcaPoint( TInt aIndex ) const
    {
    	TRAP_IGNORE(MdcaPointL(aIndex));
    	return *iBuffer;
    }

// ---------------------------------------------------------
// CSimPsyUiLBModel::MdcaPointL()
// Indexes into a descriptor array.
//
// @param aIndex The position of the descriptor element within
// 				 a descriptor array. The position is relative 
//				 to zero; i.e. zero implies the first descriptor
//				 element in a descriptor array. 
//	@return 	 A 16 bit non-modifiable pointer descriptor 
//				 representing the descriptor element located at 
//				 position aIndex within a descriptor array. 
// ---------------------------------------------------------
TPtrC16 CSimPsyUiLBModel::MdcaPointL( TInt /*aIndex*/ ) const
{
	TPtr ptr( iBuffer->Des() );
    ptr.Zero();
    ptr.Append( EKeyTab );
    
    ptr.Append( iFileNameText->Des() );

    ptr.Append( EKeyTab );
    
    TBuf<KMaxFileName> nmeaFileName;
	TParse FileParser;
	/* The cenrep contains the filename with full path.
	 * Hence only the filename is extracted and displayed in the Edwin
	 */
	CRepository* repository = CRepository::NewLC(KCRUidSimulationPSY);
	repository->Get(KCRKeySimPSYSimulationFile, nmeaFileName);
	FileParser.Set(nmeaFileName,NULL,NULL);
	nmeaFileName.Copy(FileParser.NameAndExt());
	CleanupStack::PopAndDestroy(repository);
	
    ptr.Append(nmeaFileName);
 
    return *iBuffer;	
}

