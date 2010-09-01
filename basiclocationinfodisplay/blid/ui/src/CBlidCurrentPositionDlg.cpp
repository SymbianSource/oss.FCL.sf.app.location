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
* Description:  Provides blid main view class methods.
*
*/


//  SYSTEM INCLUDE FILES
#include <aknlists.h>
#include <blid.rsg>
#include <avkon.hrh>
#include <aknborders.h>

//  USER INCLUDE FILES
#include "CBlidCurrentPositionDlg.h"
#include "CBlidBaseView.h"
#include "CBlidOwnPosLBModel.h"
#include "MBlidLocation.h"
#include "CBlidEng.h"
#include "CBlidDocument.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidCurrentPositionDlg::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidCurrentPositionDlg* CBlidCurrentPositionDlg::NewL( CBlidBaseView& aView,
        						   						MBlidLocation* aLoc,
        						   						CBlidEng* aEng )
    {
    CBlidCurrentPositionDlg* self = new(ELeave)
        CBlidCurrentPositionDlg( aView, aLoc, aEng );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// ----------------------------------------------------------------------------
// CBlidCurrentPositionDlg::CBlidCurrentPositionDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidCurrentPositionDlg::CBlidCurrentPositionDlg(  CBlidBaseView& aView,
        						   				   MBlidLocation* aLoc,
        						   				   CBlidEng* aEng ):
    iView(aView)
    {
    iLoc = aLoc;
    iEng = aEng;
    isDlgExited = EFalse;
    isLmSaved = EFalse;
    }

// ----------------------------------------------------------------------------
// CBlidCurrentPositionDlg::ExecuteLD
//
// ----------------------------------------------------------------------------
//    
void CBlidCurrentPositionDlg::ExecuteLD()
    {
    CAknPopupList::ExecuteLD();
    }

// ----------------------------------------------------------------------------
// CBlidCurrentPositionDlg::~CBlidCurrentPositionDlg
// 
// ----------------------------------------------------------------------------
//
CBlidCurrentPositionDlg::~CBlidCurrentPositionDlg()
    {
    delete iListBox;
    iLoc = NULL;
    }

// ----------------------------------------------------------------------------
// CBlidCurrentPositionDlg::ProcessCommandL
// 
// ----------------------------------------------------------------------------
//
void CBlidCurrentPositionDlg::ProcessCommandL(TInt aCommandId)
    {
    switch (aCommandId)
        {
        case EAknSoftkeySave:
            {
       		iView.SaveCurrentPositionL();
            break;
            }
        default:
            {
            isDlgExited = ETrue;
            CAknPopupList::ProcessCommandL(aCommandId);
            break;
            }
        }
    }    

// ----------------------------------------------------------------------------
// CBlidCurrentPositionDlg::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//    
void CBlidCurrentPositionDlg::ConstructL()
    {
    iListBox = new (ELeave) CAknSingleHeadingPopupMenuStyleListBox;

    CAknPopupList::ConstructL(iListBox, 
                              R_BLID_CURRENTLOCATION_SOFTKEYS_SAVE__EXIT,
                              AknPopupLayouts::EDynMenuWindow );
    
    iListBox->ConstructL(this, EAknListBoxDisableHighlight |
    								 EAknGenericListBoxFlags );
    
    SetBorder(AknBorderId::EAknBorderNotePopup);
	
	HBufC* textFromResourceFile;
    CEikonEnv* env = CEikonEnv::Static();
    textFromResourceFile = env->AllocReadResourceLC(R_BLID_TITLE_POSITION);
    SetTitleL( *textFromResourceFile );
    CleanupStack::PopAndDestroy();  // textFromResourceFile
        
    MDesCArray* model = CBlidOwnPosLBModel::NewLC( iLoc, iEng->SettingsModel());
    iListBox->Model()->SetItemTextArray( model );
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    CleanupStack::Pop(); // model

    // Make scroll arrows
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EOff,CEikScrollBarFrame::EOn );      
    }
    
// ---------------------------------------------------------
// CBlidCurrentPositionDlg::ChangeDlgSoftKey
// ---------------------------------------------------------
//    
void CBlidCurrentPositionDlg::ChangeDlgSoftKey( )
	{
	if( !isDlgExited )
		{
		if( iLoc != NULL )
			{
			if( iLoc->IsGPSDataAvailable() )
				{
				ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeySave, ETrue );			
				}
			else
				{
				ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeySave, EFalse );			
				}
			DrawNow();					
			}
		}
	}
// End Of File
