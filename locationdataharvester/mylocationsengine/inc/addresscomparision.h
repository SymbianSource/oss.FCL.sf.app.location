/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Address comparison header file implementation.
*
*/

#ifndef ADDRESSCOMPARISION_H_
#define ADDRESSCOMPARISION_H_
#include <EPos_CPosLandmarkDatabase.h>
#include "mylocationslookupdb.h"
class CAddressComparision : public CBase
    {
public:
  
    static CAddressComparision* NewL(); 
    /**
        * Is edited address changed .
        * @param aLandmarks handle to all type of address
        * @param aCntId contact id.
        * @param aAddressType type of address
        * @return Status code(ETrue address is  changed ,EFalse is not changed)
        */
    TBool IsAddressChangedL( const CPosLandmark& aLandmarks, const TInt32 aCntId,
                             const TUidSourceType aAddressType );
    
private:
    
    /**
     * CAdressComparision.
     * C++ default constructor. 
     */
    CAddressComparision();
       
    // Handle to landmarks db
    CPosLandmarkDatabase* iLandmarkDb;

    // Handle to the My locations database manager
    CLookupDatabase *iLookUpDatabase;
    };
#endif /* ADDRESSCOMPARISION_H_ */
