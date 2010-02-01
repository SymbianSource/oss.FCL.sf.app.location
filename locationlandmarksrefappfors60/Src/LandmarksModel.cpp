/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CLandmarksModel class
*
*/



#include <eikenv.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <AknsItemID.h>

#include <EPos_CPosLandmark.h>

#include <lmrefapp.mbg>
#include "LandmarksCommonData.h"
#include "LandmarksModel.h"


const TInt KDefaultIconIndex = 0;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksModel::CLandmarksModel()
	{
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksModel::ConstructL()
	{
	// The list box model uses MDesCArray interface class to take in text items.
	iListItems = new (ELeave) CDesCArraySeg(KGranularity);
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksModel* CLandmarksModel::NewL()
    {
    CLandmarksModel* self = new (ELeave) CLandmarksModel();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksModel::~CLandmarksModel()
	{
	delete iListItems;
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksModel::SetItemIds(RArray<TPosLmItemId>* aItemIds)
    {
    iItemIds = aItemIds;

    // Empty descriptor model
    iListItems->Reset();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CDesCArraySeg* CLandmarksModel::ItemList()
    {
    return iListItems;
    }
		
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksModel::SetCurrentItem(TInt aIndex) 
    {
    iCurrentItem = aIndex;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TPosLmItemId CLandmarksModel::CurrentItemId()
    {
    if (!iItemIds || iCurrentItem < 0 || 
        iCurrentItem > iItemIds->Count() - 1)
        {
        return KPosLmNullItemId;
        }

    return (*iItemIds)[iCurrentItem];
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksModel::GetItemIdsL( 
    const CArrayFix<TInt>& aSelectionIndexes,
    RArray<TPosLmItemId>& aItemIds )
    {
    aItemIds.Reset();
    if ( iItemIds )
        {
        for ( TInt i = 0; i < aSelectionIndexes.Count(); i++ )
            {
            if ( i < iItemIds->Count() )
                {
                aItemIds.AppendL( (*iItemIds)[aSelectionIndexes[i]] );
                }
            else
                {
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CIconList* CLandmarksModel::CreateIconListL()
    {
    CIconList* iconList = 
        (CIconList*) new (ELeave) CArrayPtrFlat<CGulIcon>(KGranularity);
    CleanupStack::PushL(iconList);
    
    // Create the default landmark icon consisting of its image and mask
    TAknsItemID aknsItemId = {0,0};
    CGulIcon* icon = AknsUtils::CreateGulIconL(NULL, aknsItemId,
        KLandmarksMbmFileName, 
        EMbmLmrefappDefault_lm, 
        EMbmLmrefappDefault_lm_mask);
    CleanupStack::PushL(icon);
    icon->SetBitmapsOwnedExternally(EFalse);
    iconList->AppendL(icon);
    CleanupStack::Pop(2, iconList);

    // initialze iIconList. We don't need to delete any possible previous 
    // iIconList since ownership is transferred to calling object.
    iIconList = iconList;

    return iIconList;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksModel::RepopulateModelL(
    CArrayPtr<CPosLandmark>* aLandmarks)
    {
    TInt nrOfReadItems = aLandmarks->Count();

    // Format and insert the read landmarks
    for (TInt i = 0; i < nrOfReadItems; i++)
        {
        CPosLandmark* landmark = (*aLandmarks)[i];
        HBufC* formattedListItem = FormatListItemLC(*landmark);
        iListItems->AppendL(*formattedListItem);
        CleanupStack::PopAndDestroy(formattedListItem);
        }
    }

// -----------------------------------------------------------------------------
// Formats a list item to the following format "X\tlandmarkName\t"
// where X = icon index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CLandmarksModel::FormatListItemLC(CPosLandmark& aLandmark)
    {
    // Extract name
    TPtrC name;
    aLandmark.GetLandmarkName(name);
    // 10 extra chars for icon index and column separator
    const TInt KExtraChars = 10;
    HBufC* formattedNameBuf = HBufC::NewLC(name.Length() + KExtraChars);
    TPtr formattedNamePtr = formattedNameBuf->Des();

    // Extract icon info
    TPtrC mbmIconFile;
    TInt iconIndex, maskIndex;
    TInt res = aLandmark.GetIcon(mbmIconFile, iconIndex, maskIndex);

    // Format list item
    if (res == KErrNone)
        {
        // Create icon and append it to icon array. 
        TRAPD(err, AppendIconL(mbmIconFile, iconIndex, maskIndex));
        if (err == KErrNone)
            {
            // Append the array index where the icon is appended
            formattedNamePtr.AppendNum(iIconList->Count() - 1);
            }
        else
            {
            formattedNamePtr.AppendNum(KDefaultIconIndex);
            }
        }
    else // res == KErrNotFound
        {
        formattedNamePtr.AppendNum(KDefaultIconIndex);
        }
    formattedNamePtr.Append(KTab);
    formattedNamePtr.Append(name);

    return formattedNameBuf;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksModel::AppendIconL(
    const TDesC& aMbmFile, 
    TInt aIconOffset, 
    TInt aMaskOffset)
    {
    TAknsItemID aknsItemId = {0,0};
    CGulIcon* icon = AknsUtils::CreateGulIconL(NULL, aknsItemId,
        aMbmFile, 
        aIconOffset, 
        aMaskOffset);
    CleanupStack::PushL(icon);
    icon->SetBitmapsOwnedExternally(EFalse);
    iIconList->AppendL(icon);
    CleanupStack::Pop(icon);
    }


