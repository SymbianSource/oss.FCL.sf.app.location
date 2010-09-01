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
* Description:    This class provides methods for fetching the ID of selected
*			   	  landmark or category and the handle to database to which either
*			   	  landmark or category belongs.
*
*/








// INCLUDE FILES
#include "TLmkItemIdDbCombiInfo.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInfo::TLmkItemIdDbCombiInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
EXPORT_C TLmkItemIdDbCombiInfo::TLmkItemIdDbCombiInfo() : iLmDb(NULL), iLmItemId(0)
	{
	}

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInfo::~TLmkItemIdDbCombiInfo
// -----------------------------------------------------------------------------
//
EXPORT_C TLmkItemIdDbCombiInfo::~TLmkItemIdDbCombiInfo()
    {
    }

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInfo::SetItemId
//
// -----------------------------------------------------------------------------
//
EXPORT_C void TLmkItemIdDbCombiInfo::SetItemId( TPosLmItemId & aLmItemId)
	 {
	 iLmItemId =  aLmItemId;
	 }

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInfo::SetLmDb
//
// -----------------------------------------------------------------------------
//
 EXPORT_C void TLmkItemIdDbCombiInfo::SetLmDb( CPosLandmarkDatabase* aDb)
	{
	iLmDb = aDb;
	}

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInfo::GetItemId
//
// -----------------------------------------------------------------------------
//
 EXPORT_C TPosLmItemId TLmkItemIdDbCombiInfo::GetItemId() const
	 {
	 return iLmItemId;
	 }

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInfo::GetLmDb
//
// -----------------------------------------------------------------------------
//
 EXPORT_C CPosLandmarkDatabase* TLmkItemIdDbCombiInfo::GetLmDb() const
	 {
	 return iLmDb;
	 }

 // End of file






