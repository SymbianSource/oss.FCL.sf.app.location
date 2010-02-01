/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Provides landmarks note class methods.
*
*/







// INCLUDE FILES
#include <aknmessagequerydialog.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <AknQueryDialog.h>
#include <lmkui.rsg>
#include <aknconsts.h>
#include "LmkNotes.h"
#include "LmkConsts.h"

// ================= MEMBER FUNCTIONS =======================
//-----------------------------------------------------------------------
// LmkNotes::CategoryConfirmationQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt LmkNotes::CategoryConfirmationQueryL( CEikonEnv* aEnv, const TInt aAmount )
    {
    HBufC* noteText = NULL;

    if ( aAmount > 1)
        {
        noteText = StringLoader::LoadLC( R_LMK_DELETE_CATEGORIES,
                                         aAmount, aEnv );
        }
    else
        {
        noteText = StringLoader::LoadLC( R_LMK_DELETE_CATEGORY, aEnv );
        }

    CAknQueryDialog* query =
        new( ELeave ) CAknQueryDialog( *noteText );
    TInt ret = query->ExecuteLD( R_LMK_QUERY );//*noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
    return ret;
    }

//-----------------------------------------------------------------------
// LmkNotes::LandmarkConfirmationQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt LmkNotes::LandmarkConfirmationQueryL( CEikonEnv* aEnv, const TInt aAmount )
    {
    HBufC* noteText = NULL;

    if ( aAmount > 1 )
        {
        noteText = StringLoader::LoadLC( R_LMK_DELETE_LANDMARKS,
                                         aAmount, aEnv );
        }
    else
        {
        noteText = StringLoader::LoadLC( R_LMK_DELETE_LANDMARK, aEnv );
        }

    CAknQueryDialog* query =
        new( ELeave ) CAknQueryDialog( *noteText );
    TInt ret = query->ExecuteLD( R_LMK_QUERY );//*noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
    return ret;
    }

//-----------------------------------------------------------------------
// LmkNotes::InformationNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void LmkNotes::InformationNoteL( CEikonEnv* aEnv, const TInt aResourceId )
    {
    HBufC* noteText = StringLoader::LoadLC( aResourceId, aEnv );

    CAknInformationNote* dialog =
        new( ELeave ) CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
    }

//-----------------------------------------------------------------------
// LmkNotes::CategoryNameQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt LmkNotes::CategoryNameQueryL( TDes& aDataText,
                                             const TInt aResourceId )
    {
    CAknTextQueryDialog* dlg = new( ELeave ) CAknTextQueryDialog(aDataText,
                                                    CAknQueryDialog::ENoTone);
    return dlg->ExecuteLD(aResourceId);
    }

//-----------------------------------------------------------------------
// LmkNotes::InformationNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void LmkNotes::InformationNotewithTextL( CEikonEnv* aEnv,
           TInt aResourceId, TPtrC aTextPtr )
    {
    HBufC* noteText = StringLoader::LoadLC( aResourceId, aTextPtr, aEnv );

    CAknInformationNote* dialog =
        new( ELeave ) CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
    }
//-----------------------------------------------------------------------
// LmkNotes::OutOfMemoryNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void LmkNotes::OutOfMemoryNoteL( CEikonEnv* aEnv )
	{
    HBufC* noteText = StringLoader::LoadL( R_LMK_MEMLO_NOT_ENOUGH_MEMORY, aEnv );
    CleanupStack::PushL(noteText);
    CAknErrorNote* dialog =
        new(ELeave)CAknErrorNote(ETrue);
    dialog->SetTone(CAknNoteDialog::EErrorTone);
    dialog->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy(noteText); //noteText
	}
//-----------------------------------------------------------------------
// LmkNotes::MessageQueryL()
// ----------------------------------------------------------------------------
//
EXPORT_C void LmkNotes::MessageQueryL( CEikonEnv* aEnv, TDes& aMsg, TInt aCount )
	{
	CAknMessageQueryDialog* dialog = new (ELeave) CAknMessageQueryDialog;
    HBufC* headerText;
    headerText =  StringLoader::LoadL(R_LMK_CATEGORIES_QUERY_PROMPT,
                                      aCount,
                                      aEnv );
    dialog->SetHeaderTextL(*headerText);
    delete headerText;
    dialog->SetMessageTextL( aMsg );
    dialog->ExecuteLD(R_LMK_MESSAGE_QUERY_DIALOG);
	}
//-----------------------------------------------------------------------
// LmkNotes::AllLandmarksEmptyNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void LmkNotes::AllLandmarksEmptyNoteL( CEikonEnv* aEnv )
	{
    HBufC* noteText = StringLoader::LoadLC( R_LM_SHOW_ERROR_NO_COORDINATES, aEnv );

    CAknInformationNote* dialog =
        new( ELeave ) CAknInformationNote( R_AKN_ERROR_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
	}

//-----------------------------------------------------------------------
// LmkNotes::LandmarksEmptyNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void LmkNotes::LandmarksEmptyNoteL( CEikonEnv* aEnv, TPtrC aLandmarkName )
	{
    HBufC* noteText = StringLoader::LoadLC( R_LM_SHOW_ERROR_NO_DATA,
    										aLandmarkName,
    										aEnv );

    CAknInformationNote* dialog =
        new( ELeave ) CAknInformationNote( R_AKN_ERROR_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
	}

//-----------------------------------------------------------------------
// LmkNotes::CategoryEmptyNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void LmkNotes::CategoryEmptyNoteL( CEikonEnv* aEnv, TPtrC aCategoryName )
	{
    HBufC* noteText = StringLoader::LoadLC( R_LM_SHOW_ERROR_CATEGORY_NO_COORDINATES,
    										aCategoryName,
    										aEnv );

    CAknInformationNote* dialog =
        new( ELeave ) CAknInformationNote( R_AKN_ERROR_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy(noteText); //noteText
	}

//-----------------------------------------------------------------------
// LmkNotes::CategoryEmptyNoteL
// ----------------------------------------------------------------------------
//
EXPORT_C void LmkNotes::LandmarkInCategoryEmptyNoteL( CEikonEnv* aEnv, TPtrC aCategoryName, TPtrC aLandmark)
	{
	CPtrC16Array* array = new (ELeave) CPtrC16Array(2);
	CleanupStack::PushL(array);
	array->AppendL( aLandmark );
	array->AppendL( aCategoryName );

    HBufC* noteText = StringLoader::LoadLC( R_LM_SELECT_FROM_MAP,
											*array,
    										aEnv );

    CAknInformationNote* dialog =
        new( ELeave ) CAknInformationNote( R_AKN_ERROR_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy(2); //noteText, array
	}

//-----------------------------------------------------------------------
// LmkNotes::ShowDelayedInfoPopupL
// ----------------------------------------------------------------------------
//

EXPORT_C CAknInfoPopupNoteController* LmkNotes::ShowDelayedInfoPopupL( TDesC& aMsg,TPoint& /*aPosition*/ )
	{
	CAknInfoPopupNoteController*infopopup  = CAknInfoPopupNoteController::NewL ();
	infopopup->SetTextL(aMsg);
	return infopopup;

	}

//-----------------------------------------------------------------------
// LmkNotes::ShowOwerriteLocationQueryL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt LmkNotes::ShowOwerriteLocationQueryL( CEikonEnv* aEnv )
	{
    HBufC* noteText = NULL;
    noteText = StringLoader::LoadLC( R_LM_QUERY_EDITOR_REPLACE,aEnv);
    CAknQueryDialog* query =
        new( ELeave ) CAknQueryDialog( *noteText );
    TInt ret = query->ExecuteLD( R_LMK_QUERY );

    CleanupStack::PopAndDestroy(noteText); //noteText
    return ret;
	}

//-----------------------------------------------------------------------
// LmkNotes::MessageQueryWithLinkL()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt LmkNotes::MessageQueryWithLinkL( CEikonEnv* aEnv, TInt aHeadingText,
											   TInt aMsgQueryText, TInt aLinkText,
											   TCallBack aCallBack)
	{
    HBufC* headPaneText = StringLoader::LoadLC(
                      aHeadingText, ( aEnv ));

    HBufC* msgQueryText = StringLoader::LoadLC(
          aMsgQueryText, ( aEnv ));


    HBufC* linkText = StringLoader::LoadLC(
          aLinkText, ( aEnv ));

	HBufC* msgQText = HBufC::NewLC ( msgQueryText->Length() +
				 					 KNewLine().Length() +
				 					 linkText->Length() ) ;

	msgQText->Des().Copy(*msgQueryText);
    msgQText->Des().Append(KNewLine);
    msgQText->Des().Append(*linkText);

    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*msgQText);
	dlg->PrepareLC(R_LMK_MESSAGE_QUERY_DIALOG2);
	dlg->Heading()->SetTextL(*headPaneText);
	dlg->SetLinkTextL(*linkText);
	dlg->SetLink(aCallBack);
	TInt result = dlg->RunLD();
	CleanupStack::PopAndDestroy(4);
	return result;
	}

//-----------------------------------------------------------------------
// LmkNotes::GenericMessageQueryL()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt LmkNotes::GenericMessageQueryL( CEikonEnv* aEnv, TInt aMsgQueryText,
											  TInt aHeadingText )
	{
	HBufC* msgQueryText = StringLoader::LoadLC(
          aMsgQueryText, ( aEnv ));
    HBufC* headPaneText = StringLoader::LoadLC(
                      aHeadingText, ( aEnv ));
	CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*msgQueryText);
    dlg->PrepareLC(R_LMK_MESSAGE_QUERY_DIALOG3);
    dlg->Heading()->SetTextL(*headPaneText);
	TInt result = dlg->RunLD();
	CleanupStack::PopAndDestroy(2);//msgQueryText,headPaneText
	return result;
	}
// End of File
