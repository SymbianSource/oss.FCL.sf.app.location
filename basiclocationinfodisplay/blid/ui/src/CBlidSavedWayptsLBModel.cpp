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
* Description:  Provides SavedWayptsView's details.
*
*/


// INCLUDE FILES
#include <StringLoader.h> 
#include <AknUtils.h>
#include <blid.rsg>
#include "CBlidSavedWayptsLBModel.h"
#include "MBlidLocation.h"
#include "MBlidSettings.h"
#include "MBlidRouter.h"
#include "bliduiconsts.h"
#include "Blid.hrh"
#include "Blidutils.h"
#include "Debug.h"
#include "lbscommon.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidSavedWayptsLBModel::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidSavedWayptsLBModel* CBlidSavedWayptsLBModel::NewLC( MBlidLocation* aLocationModel,
                                               MBlidSettings* aSettings,MBlidRouter* aRouter )
    {
    CBlidSavedWayptsLBModel* ptr =
		new (ELeave) CBlidSavedWayptsLBModel( aLocationModel, aSettings,aRouter );
	CleanupStack::PushL(ptr);
	ptr->ConstructL();
	return ptr;
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsLBModel::CBlidSavedWayptsLBModel
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidSavedWayptsLBModel::CBlidSavedWayptsLBModel(MBlidLocation* aLocationModel,
                                       MBlidSettings* aSettings,MBlidRouter* aRouter):
    iLocationModel( aLocationModel ), iSettingsModel( aSettings ),iRouter(aRouter)
    {
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsLBModel::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidSavedWayptsLBModel::ConstructL()
    {
    iBuffer = HBufC::NewL( KBlidLBLineMaxLen );
    }

// ---------------------------------------------------------
// CBlidSavedWayptsLBModel::~CBlidSavedWayptsLBModel
// ---------------------------------------------------------
//
CBlidSavedWayptsLBModel::~CBlidSavedWayptsLBModel()
    {
  	delete iBuffer;
    }


// ----------------------------------------------------------------------------
// CBlidSavedWayptsLBModel::MdcaCount
// ----------------------------------------------------------------------------
//
TInt CBlidSavedWayptsLBModel::MdcaCount() const
    {

    TInt count(iRouter->Count());
	return count;
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsLBModel::MdcaPoint
// ----------------------------------------------------------------------------
//
TPtrC16 CBlidSavedWayptsLBModel::MdcaPoint(TInt aIndex) const
    {
    TPtr ptr( iBuffer->Des() );
    ptr.Zero();
    HBufC* name = (HBufC*)NULL;
    TBuf<KBlidWaypointNameMaxLen> temp;
    temp.Zero();
    if(aIndex < iRouter->Count())
        {
        name = iRouter->At(aIndex).Name();    
        }
    if ( name )
    	{
        _LIT(KFormatString,"%d");
        ptr.Format(KFormatString,1);
        ptr.Append( EKeyTab );  
        ptr.Append( *name );
        }
    else
        {        
        ptr.Append( EKeyTab );                
        }
        
    AknTextUtils::LanguageSpecificNumberConversion( ptr );
    delete name;
    return iBuffer->Des();
   }


