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
* Description: Address comparison source implementation for maptile
*              service.
*
*/

#include "mylocationlogger.h"
#include "addresscomparision.h"


CAddressComparision* CAddressComparision::NewL()
{
    CAddressComparision* self = new (ELeave) CAddressComparision();
    return self;
}

CAddressComparision::CAddressComparision()
{

}

// Need to enhance the functionality for address comparison
TBool CAddressComparision::IsAddressChangedL(const CPosLandmark& /*aLandmarks*/,
        const TInt32 aCntId, const TUidSourceType aAddressType)
{
    __TRACE_CALLSTACK;
    
    
    TBool addressChanged = ETrue;
    CLookupDatabase* lookupdb = NULL;
    lookupdb = CLookupDatabase::NewLC(KMylocationsLookupDatabaseName);

    TLookupItem lookupItem;
    lookupItem.iUid = aCntId;
    lookupItem.iSource = aAddressType;
    User::LeaveIfError(lookupdb->Open());
    addressChanged=lookupdb->FindEntryL(lookupItem);
	addressChanged=ETrue;
    lookupdb->Close();
    CleanupStack::PopAndDestroy(lookupdb);
    return addressChanged;//addressChanged;
}

//end of file
