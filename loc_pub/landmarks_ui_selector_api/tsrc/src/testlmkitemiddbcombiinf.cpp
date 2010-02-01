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
* Description:    LandmarksUi Content File -  Implementation for testing TLmkItemIdDbCombiInfo class
*
*/







// INCLUDE FILES
#include <epos_landmarks.h>     // Lm typedefs, constants etc.
#include <TLmkItemIdDbCombiInfo.h>
#include <epos_cposlandmarkdatabase.h>

#include "testlmkitemiddbcombiinf.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInf::TLmkItemIdDbCombiInf()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
TLmkItemIdDbCombiInf::TLmkItemIdDbCombiInf()
	{
	}

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInf::~TLmkItemIdDbCombiInf()
// -----------------------------------------------------------------------------
//
TLmkItemIdDbCombiInf::~TLmkItemIdDbCombiInf()
    {
    }

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInf::GetItemIdL()
//
// -----------------------------------------------------------------------------
//
 void TLmkItemIdDbCombiInf::GetItemIdL()
	 {
	 TLmkItemIdDbCombiInfo info;
	 TPosLmItemId id = info.GetItemId();
	 if( id != 0 )
		 {
		 User::Leave(KErrGeneral);
		 }

	 }

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInf::GetLmDbL()
//
// -----------------------------------------------------------------------------
//
 void TLmkItemIdDbCombiInf::GetLmDbL()
	{
	TLmkItemIdDbCombiInfo info;
	CPosLandmarkDatabase* db = info.GetLmDb();
	 if( db )
		 {
		 User::Leave(KErrGeneral);
		 }
	}

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInf::SetGetItemIdL()
//
// -----------------------------------------------------------------------------
//
 void TLmkItemIdDbCombiInf::SetGetItemIdL()
	 {
	 TLmkItemIdDbCombiInfo info;
	 TPosLmItemId id = 5;
	 info.SetItemId(id);
	 if( id != info.GetItemId() )
		 {
		 User::Leave(KErrGeneral);
		 }

	 }

// -----------------------------------------------------------------------------
// TLmkItemIdDbCombiInf::SetGetLmDbL()
//
// -----------------------------------------------------------------------------
//
 void TLmkItemIdDbCombiInf::SetGetLmDbL()
	{
	TLmkItemIdDbCombiInfo info;
	CPosLandmarkDatabase* db1 = CPosLandmarkDatabase::OpenL();
	info.SetLmDb(db1);

	CPosLandmarkDatabase* db2 = info.GetLmDb();

	if( !db2 )
		 {
		 User::Leave(KErrGeneral);
		 }

    delete db1;
	ReleaseLandmarkResources();
	}

 // End of file






