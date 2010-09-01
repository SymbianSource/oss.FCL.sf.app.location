/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    CLmkLandmarkCategoriesName holds category names for particular Landmark
*
*/







/**
* CLmkLandMarkCategoriesName class.
* This class holds category names for perticuler landmark.
*/

#include "CLmkLandMarkCategoriesName.h"

// ---------------------------------------------------------
// CLmkLandMarkCategoriesName::NewL
// ---------------------------------------------------------
//
EXPORT_C CLmkLandMarkCategoriesName* CLmkLandMarkCategoriesName::NewL()
{
	CLmkLandMarkCategoriesName * self = new (ELeave) CLmkLandMarkCategoriesName();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

// ---------------------------------------------------------
// CLmkLandMarkCategoriesName::NewL
// ---------------------------------------------------------
//
EXPORT_C CLmkLandMarkCategoriesName* CLmkLandMarkCategoriesName::NewL(CLmkLandMarkCategoriesName& aLmkLandMarkCategoriesName)
{
	CLmkLandMarkCategoriesName * self  = CLmkLandMarkCategoriesName::NewL();
	RArray<TCategoryName>   categoryNameArray;
	aLmkLandMarkCategoriesName.GetCategoryNames(categoryNameArray);
	self->SetCategoryNames(categoryNameArray);
	categoryNameArray.Close();
	return self;
}

// ---------------------------------------------------------
// CLmkLandMarkCategoriesName::~CLmkLandMarkCategoriesName()
// ---------------------------------------------------------
//

CLmkLandMarkCategoriesName::~CLmkLandMarkCategoriesName()
{
	iCategoriesName.Close();
}

// ---------------------------------------------------------
// CLmkLandMarkCategoriesName::CLmkLandMarkCategoriesName()
// ---------------------------------------------------------
//

CLmkLandMarkCategoriesName::CLmkLandMarkCategoriesName()
{

}

// ---------------------------------------------------------
// CLmkLandMarkCategoriesName::ConstructL()
// ---------------------------------------------------------
//
void CLmkLandMarkCategoriesName::ConstructL()
{
//may be needed
}

// ---------------------------------------------------------
// CLmkLandMarkCategoriesName::GetCategoryNames()
// ---------------------------------------------------------
//
EXPORT_C void CLmkLandMarkCategoriesName::GetCategoryNames(RArray<TCategoryName>&  aCategoryNameArray) const
{
	TInt totalCategories = iCategoriesName.Count();
	for( TInt cnt = 0; cnt < totalCategories; cnt++ )
	{
			aCategoryNameArray.Append(iCategoriesName[cnt]);
	}
}

// ---------------------------------------------------------
// CLmkLandMarkCategoriesName::SetCategoryNames()
// ---------------------------------------------------------
//

EXPORT_C void CLmkLandMarkCategoriesName::SetCategoryNames(RArray<TCategoryName> &  aCategoryNameArray)
{
	iCategoriesName.Reset();
	TInt totalCategories = aCategoryNameArray.Count();
	for( TInt cnt =0; cnt < totalCategories; cnt++ )
	{
			iCategoriesName.Append(aCategoryNameArray[cnt]);
	}
}

// ---------------------------------------------------------
// CLmkLandMarkCategoriesName::AddCategoryName()
// ---------------------------------------------------------
//
EXPORT_C void CLmkLandMarkCategoriesName::AddCategoryName(const TDesC &aCategoryName)
{
	iCategoriesName.Append(aCategoryName);
}
// End of File
