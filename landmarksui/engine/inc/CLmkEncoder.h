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
* Description:    LandmarksUi Content File -    This class wraps functionality related to landmark encoding into
*                a simpler interface.
*
*/







#ifndef CLMKENCODER_H
#define CLMKENCODER_H

//  INCLUDES
#include <EPos_Landmarks.h>
#include "MLmkAOOperationObserver.h"
#include "CLmkLandmark.h"


// FORWARD DECLARATIONS
class CPosLandmarkEncoder;
class CPosLandmarkDatabase;
class CLmkAOOperation;
class CPosLandmark;

// CLASS DECLARATION
/**
 * This class wraps functionality related to landmark encoding into
 * a simpler interface.
 */
class CLmkEncoder : public CBase, public MLmkAOOperationObserver
    {
	public:  // Constructors and destructor

		/**
		* Create CLmkEncoder object
        * @param aDb landmark database
        * @param aObserver calling class setting itself as observer to this class
        * @param aProgressNote Note to show the progress of encoding operation
		* @return newly instantiated object
		*/
		IMPORT_C static CLmkEncoder* NewL( CPosLandmarkDatabase& aDb,
                                           MLmkAOOperationObserver& aObserver,
                                           TBool aProgressNote );

		/**
		* Destructor.
		*/
		~CLmkEncoder();

    protected: //from MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                               TReal32 aProgress,
                               TInt aStatus );

    public: // New functions
        /**
        * Set package name
        * @param aName
        */
        //IMPORT_C void SetPackageNameL( const TDesC& aName );

        /**
        * Start encoding package. Possibly existing file is deleted first.
        * @param aFile where parsed data is stored
        * @param aLandmarkIdArray
        */
        IMPORT_C void StartEncodingL( const TDesC& aFile,
                        const RArray<TPosLmItemId>& aLandmarkIdArray );

        /**
        * Start encoding package. Possibly existing file is deleted first.
        * This API can also be used for encoding unsaved landmarks
        * @param aFile where parsed data is stored
        * @param aLandmarks
        */
        IMPORT_C void StartEncodingL(const TDesC& aFileName,
                    	const RPointerArray<CLmkLandmark>& aLandmarks );

        /**
        * Cancel encode operation
        */
        IMPORT_C void CancelEncodeOperationL();

    private:  // New functions
    	/**
        * This function is used for setting the name for landmarks package
        * @param aName Name to be set for landmarks package
        */
    	void SetPackageNameL( const TDesC& aName );

        /**
        * Finalize the encode process. It uses the Landmarks Framework method
        * to signal the finalizing the encode process.
        */
        void FinalizeEncodingL();

        /**
        * Checks if any Active Object is already running. This is used whenever
        * a new Active Object operation is started.
        */
        void LeaveIfInUseL();

        /**
        * Instantiates iEncoder & prepares file to which encoded data is written.
        */
        void PrepareEncoderL(const TDesC& aFileName);

    private:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb landmark database
        * @param aObserver calling class sets itself as the observer to this class
        * @param aProgressNote Note to show the progress of landmark encoding process
        * @return newly instantiated object
        */
        CLmkEncoder( CPosLandmarkDatabase& aDb,
                    MLmkAOOperationObserver& aObserver,
                    TBool aProgressNote );

        /**
		* By default Symbian 2nd phase constructor is private.
        * @param aFile
		*/
        void ConstructL();

    private:    // Data
        ///Ref:
        CPosLandmarkDatabase& iDb;

        ///Ref:
        MLmkAOOperationObserver& iObserver;

        TBool iProgressNote;

        TBool iExporting; // detailed encoder status

        ///Own: EPos encoder
        CPosLandmarkEncoder* iEncoder;

        ///Own:
        CLmkAOOperation* iAOOperation;
    };

#endif      // CLMKENCODER_H

// End of File
