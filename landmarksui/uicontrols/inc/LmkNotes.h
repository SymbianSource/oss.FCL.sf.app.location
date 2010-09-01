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
* Description:    LandmarksUi Content File -    Landmarks Ui note class definition
*
*/







#ifndef __LMKNOTES_H__
#define __LMKNOTES_H__

#include <e32std.h>
#include <AknInfoPopupNoteController.h>


// CLASS DECLARATION

/**
*  LmkNotes class.
*/
class LmkNotes
    {
    public: // new functions
        /**
        * Display category deletes confirmation query
        * @param aAmount
        * @return positive value if yes, else zero or negative
        */
        IMPORT_C static TInt CategoryConfirmationQueryL( CEikonEnv* aEnv, const TInt aAmount );

        /**
        * Display landmark deletes confirmation query
        * @param aAmount
        * @return positive value if yes, else zero or negative
        */
        IMPORT_C static TInt LandmarkConfirmationQueryL( CEikonEnv* aEnv,const TInt aAmount=1);

        /**
        * Display landmark information note.
        * @param resource id
        */
        IMPORT_C static void InformationNoteL( CEikonEnv* aEnv, const TInt aResourceId );

        /**
        * Display category name query
        * @param aDataText, empty or existing name
        * @param resource id (CAknTextQueryDialog)
        * @return positive value if pressed ok, else negative
        */
        IMPORT_C static TInt CategoryNameQueryL( TDes& aDataText,
       											 const TInt aResourceId );
        /**
        * Display landmark information note.
        * @param resource id
        * @param extra text to be displayed
        */
         IMPORT_C static void InformationNotewithTextL( CEikonEnv* aEnv, TInt aResourceId,
                                                  TPtrC aTextPtr );
        /**
        * Display Out of memory note
        *
        */
        IMPORT_C static void OutOfMemoryNoteL( CEikonEnv* aEnv );
        /**
        * Display names in a message query ( List of names, one in each line )
        * @param aMsg, set of names to be displayed in the query
        */
        IMPORT_C static void MessageQueryL( CEikonEnv* aEnv, TDes& aMsg, TInt aCount );
        /**
         * Display an information note for show on map use case
         * when selected landmarks are empty
         *
         */
		IMPORT_C static void AllLandmarksEmptyNoteL( CEikonEnv* aEnv );

		/**
         * Display an information note for show on map use case when
         * a landmark are empty
         *
         * @param aLandmarkName the name of the landmark which does
         *                      not have coordinates
         */
		IMPORT_C static void LandmarksEmptyNoteL( CEikonEnv* aEnv,TPtrC aLandmarkName );

		/**
         * Display an information note for category show on map use
         * case when all landmarks are empty
         *
         * @param aLandmarkName the name of the landmark which does
         *                      not have coordinates
         */
		IMPORT_C static void CategoryEmptyNoteL( CEikonEnv* aEnv, TPtrC aCategory );

		/**
         * Display an information note for category show on map use
         * case when all landmarks are empty
         *
         * @param aLandmarkName the name of the landmark which does
         *                      not have coordinates
         */
		IMPORT_C static void LandmarkInCategoryEmptyNoteL( CEikonEnv* aEnv, TPtrC aCategory, TPtrC aLandmark);

		/**
         * Display an information popup note after 1 sec.
         * @param aMsg This is the message to be displayed.
         * @param aPosition Position of the info popup.
         */
		IMPORT_C static CAknInfoPopupNoteController* ShowDelayedInfoPopupL( TDesC& aMsg,TPoint& aPosition );
		/**
         * This query dialog will be displayed, when the landmark, already
         * has location coordinate, which is being replaced with new coordinate
         * It asks whether to replace or not.
         */
		IMPORT_C static TInt ShowOwerriteLocationQueryL( CEikonEnv* aEnv );
		/**
         * This message query displays text with the link.
         * @return returns the button id
         */
		IMPORT_C static TInt MessageQueryWithLinkL( CEikonEnv* aEnv, TInt aHeadingText,
											   TInt aMsgQueryText, TInt aLinkText,
											   TCallBack aCallBack );
		/**
         * This message query is a generic message query.
         *
         */
		IMPORT_C static TInt GenericMessageQueryL( CEikonEnv* aEnv, TInt aMsgQueryText, TInt aHeadingText );

    };

#endif // __LMKNOTES_H__

// End of File
