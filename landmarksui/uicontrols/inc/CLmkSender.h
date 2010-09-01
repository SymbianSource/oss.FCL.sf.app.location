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
* Description:    LandmarksUi Content File -    This class wraps usage of CLmkEncoder and CSendAppUi
                 into one simple class.
*
*/







#ifndef CLMKSENDER_H
#define CLMKSENDER_H

//  INCLUDES
#include "MLmkAOOperationObserver.h"
#include "LmkConsts.h"
#include <e32base.h>
#include <EPos_Landmarks.h>
#include <EPos_CPosLandmark.h>
#include "CLmkLandmark.h"

// FORWARD DECLARATIONS
class CSendUi;
class CLmkEncoder;
class CPosLandmarkDatabase;
class CAknWaitDialog;
class CEikMenuPane;
class RFile;
class CMessageData;
// CLASS DECLARATION

/**
*  This class wraps usage of CLmkEncoder and CSendAppUi into one simple class.
*  This class relies on the fact that given database has been initialized.
*  It also provides a couple of static helper functions for setting menu.
*
*/
class CLmkSender : public CBase, public MLmkAOOperationObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aDb
        * @return
        */
        IMPORT_C static CLmkSender* NewL( CPosLandmarkDatabase& aDb );

        /**
        * Destructor.
        */
        virtual ~CLmkSender();

    public: // New functions

        /**
        * Displays "Send as..." option if SendUI allows and if
        * there are some visible items in the list.
        * Note: this function requires that command ELmkCmdSendDummy exists
        * in the menu pane, since it determines the position of "Send as...".
        * @param aMenuPane
        * @param aVisibleCount number of visible items in the list
        * @param aMenuTextResource customized "Send as..." substitute
        */
        IMPORT_C void DisplaySendMenuL(
                        CEikMenuPane& aMenuPane,
                        TInt aVisibleCount,
                        TInt aMenuTextResource = KZeroResourceId ) const;

        /**
        * Display send's cascade menu
        * @param aSendUi
        * @param aMenuPane
        */
        IMPORT_C void DisplaySendCascadeMenuL( CEikMenuPane& aMenuPane ) const;

        /**
        * This method wraps entire encoding and sending procedure.
        * It is done asynchronously but client doesn't observe it.
        * This class takes care of error handling etc.
        * @param aCommandId commmand id which determines the MTM
        * @aMarkedIds marked item ids
        * @aSelectedId current item, used if there aren't marked items
        */
        void CreateAndSendL( TInt aCommandId,
                             const RArray<TPosLmItemId>& aMarkedIds,
                             TPosLmItemId aSelectedId );

        /**
        * This method wraps entire encoding and sending multiple landmarks.
        * It is done asynchronously but client doesn't observe it.
        * This class takes care of error handling etc.
        * @param aCommandId commmand id which determines the MTM
        * @aLandmarks Array of CLmkLandmark,which wrap landmarks
        *             (CPosLandmark) & the respective categories.
        *
        */
        void SendLandmarksL( TInt aCommandId,
                                    const TDesC& aPackageName,
                                    const RPointerArray<CLmkLandmark>& aLandmarks );

        /**
        * This method wraps entire encoding and sending of single landmark.
        * It is done asynchronously but client doesn't observe it.
        * This class takes care of error handling etc.
        * @param aCommandId commmand id which determines the MTM
        * @param alandmark object CLmkLandmark,which wrap landmarks
        *             (CPosLandmark) & the respective categories.
        *
        */
        void SendSingleLandmarkL( TInt aCommandId,
                                      const CLmkLandmark& alandmark );
        /**
        * This method wraps entire encoding and sending of single landmark.
        * This is an overloaded method, which used only for the landmarks,
        * which exists in the database.
        * It is done asynchronously but client doesn't observe it.
        * This class takes care of error handling etc.
        * @param alandmark object CPosLandmark,which wrap landmarks
        */

        void SendSingleLandmarkL(const CPosLandmark& alandmark);
        /**
        * This method can be used to send a specific file.
        * It is used when forwarding an existing landmark package.
        * Note: if file should be deleted then it must be closed and not
        * read-only, otherwise leave occurs with a system wide error code.
        * @param aCommandId commmand id which determines the MTM
        * @aFile full file name to be sent as an attachment
        * @aDeleteSentFile if TBool then aFile is deleted after sending
        */
        void SendFileL( TInt aCommandId,
                        const TDesC& aFile,
                        TBool aDeleteSentFile );


        void SendFileL( TInt aCommandId,
                        RFile& aFile,
                        TBool aDeleteSentFile );

        /**
        * This method prepares the CLmkLandmark array, from the landmark id's passed.
        * @param[in/out] aLmkLandmark Array of CLmkLanmark
        * @param[in] 	 aMarkedIds   landmark ids.
        */
        void PrepareMultipleLandmarkForSendL(RPointerArray<CLmkLandmark>& aLmkLandmark,
        									const RArray<TPosLmItemId>& aMarkedIds);
       	/**
        * This method encapsulates CPosLandmark object in CLmkLandmark object
        * @param[in/out] aLmkLandmark object of CLmkLandmark returned
        * @param[in] 	 aPosLandmark   landmark object.
        */
        void LmkLandmarkFromPosLandmarkL(CLmkLandmark*& aLmkLandmark,
								   const CPosLandmark& aPosLandmark);



    public: // From MLmkAOOperationObserver

        void HandleOperationL( TOperationTypes aType,
                               TReal32 aProgress,
                               TInt aStatus );
    private://new functions

    void PrepareSendFilePathL(HBufC*& aFilePath);

    private:

        /**
        * C++ default constructor.
        */
        CLmkSender(CPosLandmarkDatabase& aDb);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CPosLandmarkDatabase& aDb );


        void SendL(TDes& iFile, TInt aCommandId);

        TBool QueryNewPackageNameLC(const TDesC& aCurrentPckgName, HBufC*& aNewPackageName);
 
        void SendLandmarksPackageL(TInt aCommandId,const TDesC& aPackageName,const RPointerArray<CLmkLandmark>& aLandmarks );

    	void SendSingleLandmarkL(TInt aCommandId,const RPointerArray<CLmkLandmark>& aLandmarks);

    	void LoadFilteredSendListQueryL(const CMessageData* aMessageData);

        TPtr ReplaceSpecialCharactersL(TPtrC aText);

    private:    // Data

        // Completion code of last asynchronous operation
        TInt iLastOperationResult;

        ///Own: send ui
        //CSendAppUi* iSendUi;
		CSendUi* iSendUi;
        ///Own: landmark package encoder
        CLmkEncoder* iEncoder;

        ///Own: wait note
        CAknWaitDialog* iWaitNote;

        //Added for Bug fix
        CPosLandmarkDatabase& iDb;
    };

#endif      // CLMKSENDER_H

// End of File