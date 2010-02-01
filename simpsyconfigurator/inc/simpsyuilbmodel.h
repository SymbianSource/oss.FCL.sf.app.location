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


#ifndef CSIMPSYUILBMODEL_H_
#define CSIMPSYUILBMODEL_H_

// INCLUDES
#include <e32base.h>    // CBase
#include <bamdesca.h>  	// MDesCArray

// CLASS DECLARATION
/**
*  CSimPsyUiLBModel    
*/

class CSimPsyUiLBModel : public CBase, public MDesCArray
	{
	public: // Constructors and destructor
		/**
        * Two-phased constructor
        */
        static CSimPsyUiLBModel* NewLC();

       /**
        * Two-phased constructor
        */
        static CSimPsyUiLBModel* NewL();

        /**
        * Destructor.
        */
        ~CSimPsyUiLBModel();
        
	private:
		/**
		* Second phase of the two phase constructor
		*/
		void ConstructL();

		/**
		* C++ default constructor.
		*/
		CSimPsyUiLBModel();
		
	private: // From MDesCArray
        TInt MdcaCount() const;
        
        TPtrC16 MdcaPoint( TInt aIndex ) const;
      
   	private:
   		TPtrC16 MdcaPointL( TInt aIndex ) const;
   		
	private: //data

        // Owns: Data Buffer holding the name of the currently
        //		 selected NMEA file.
        HBufC* iBuffer;
        
        // Owns: Sim PSY FileName
        HBufC* iFileNameText;
	};

#endif // CSIMPSYUILBMODEL_H_

// End of File SimPsyUiLBModel.h
