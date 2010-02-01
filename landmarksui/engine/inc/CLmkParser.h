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
* Description:    LandmarksUi Content File -    This class provides methods to interact with Landmark Framework parser
*
*/







#ifndef CLMKPARSER_H
#define CLMKPARSER_H

//  INCLUDES

#include <e32base.h>
#include "MLmkAOOperationObserver.h"
#include <EPos_CPosLandmarkCategory.h>
#include "CLmkLandmark.h"

// FORWARD DECLARATIONS
class CPosLandmarkParser;
class CPosLandmarkDatabase;
class CLmkAOOperation;
class CPosLandmark;
class CLmkDbUtils;
class CLmkParseAllWrapper;
class RFile;
class CLmkLandMarkCategoriesName;
class TDataType;

#if 0
// FORWARD DECLARATIONS
class RFile;
class CLmkDbUtils;
class CPosLandmarkParser;
class CPosLandmarkDatabase;
class CLmkAOOperation;
class CPosLandmark;
class CLmkParseAllWrapper;

class CLmkLandMarkCategoriesName;
#endif

// CLASS DECLARATION
/**
* CLmkParser class
* This class provides methods to interact with Landmarks Framework parser for parsing
* the landmarks received in the landmarks package via messaging
*
* It returns the parsed landmark data (object) to the client of this class
*
* This class is mainly used by the Landmarks Message viewer application classes for
* displaying landmarks received in landmarks package
*/
class CLmkParser : public CBase, public MLmkAOOperationObserver
    {
	public:  // Constructors and destructor

		/**
		* Create CLmkParser object
        * @param aFile File to be parsed
        * @param aDb handle to landmark database
        * @param aObserver Caller can set itself as observer of parser class
        * @param aProgressNote Note for showing the progress of the parsing operation
		* @return newly instantiated object
		*/
		IMPORT_C static CLmkParser* NewL( const TDesC& aFile,
                                          CPosLandmarkDatabase& aDb,
                                          MLmkAOOperationObserver& aObserver,
                                          TBool aProgressNote );

		/**
		* This method creates a new parser object
		* @param aFile File to be parsed
        * @param aDb handle to landmark database
        * @param aObserver Caller can set itself as observer of parser class
        * @param aProgressNote Note for showing the progress of the parsing operation
		* @return newly instantiated object
		*/

		IMPORT_C static CLmkParser* NewL(RFile& aFile,
                                          CPosLandmarkDatabase& aDb,
                                          MLmkAOOperationObserver& aObserver,
                                          TBool aProgressNote );

		/**
		* Destructor.
		*/
		~CLmkParser();

    protected: //from MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                               TReal32 aProgress,
                               TInt aStatus );

    public: // New functions
        /**
        * Gets the name of the file to be parsed.
        * @return descriptor containing filename
        */
        IMPORT_C TPtrC FileName();

        /**
        * Gets the handle to file which needs to be parsed.
        * @return reference to file handle
        */
        IMPORT_C RFile& FileHandle();

		/**
        * Check if the handle to file which needs parsing exists or not
        * @return reference to file handle
        */
        IMPORT_C TBool IsFileHandle();

        /**
        * Initializes the landmark database which is already open.
        * This is an Asynchronous method
        * The client of this class should call this method before calling other methods.
        */
        IMPORT_C void InitializeDbL();

        /**
        * This method initiates the landmark parsing operation
        * This is an Asynchronous method.
        */
        IMPORT_C void StartParsingL();

        /**
        * This method provides the number of latest parsed landmarks from
        * the landmark package.
        * @return number of parsed landmarks
        */
        IMPORT_C TInt NumOfParsedLandmarks();

        /**
        * Returns the name of the package using collection data information.
        * Contains empty descriptor if name does not exist.
        * @return Package name
        */
        IMPORT_C TPtrC PackageName();

        /**
        * Get Landmark at the specified position
        * @param aIndex Position of Landmark in the package
        * @return CPosLandmark
        */
        IMPORT_C CPosLandmark* LandmarkLC( TInt aIndex );

        /**
        * Get Landmark packaged in CLmkLandmark at the specified position
        * @param aIndex Position of Landmark in the package
        * @return CLmkLandmark
        */
        IMPORT_C CLmkLandmark* LmkLandmarkLC( TInt aIndex );

        /**
        * Get CategoriesNames at the specified position
        * @param aIndex
        * @return CLmkLandMarkCategoriesName
        */
		IMPORT_C CLmkLandMarkCategoriesName* CategoryNameLC( TInt aIndex );
        /* Change observer of asynchronous operations
        * @param aNewObserver Observer which need to be changed
        */
        IMPORT_C void ChangeObserver( MLmkAOOperationObserver& aNewObserver );

	/**
        * Start importing all landmarks in the package to database
        */
        IMPORT_C void StartImportingL(TBool aIncludeNewCategories);

        /**
        * Start importing selected landmarks to database
        * This is an asynchronous operation
        * @param aSelected Array containing the landmarks in the landmarks package
        */
        IMPORT_C void StartImportingL( const RArray<TUint>& aSelected, TBool aIncludeNewCategories );

        /**
        * Cancel importing of landmarks to database
        */
        IMPORT_C void CancelImporting();

    private:  // New functions
        /**
        * This method checks if Active Object operation is already ongoing or not
        * This is useful when a new Active Object operation is started
        */
        void LeaveIfInUseL();

        /*
        * This function resolves the MIME Type from the list present in the
        * resource file
        */
        TInt ResolveMimeTypeL( RFile& aFile, TDataType& aDataType ) const;

    private:  // Constructors and destructor
        /**
        * C++ default constructor.
        */
        CLmkParser( CPosLandmarkDatabase& aDb,
                    MLmkAOOperationObserver& aObserver,
                    TBool aProgressNote );

        /**
		* By default Symbian 2nd phase constructor is private.
		* @param aFile Full file path
		*/
        void ConstructL( const TDesC& aFile );

        /**
		* By default Symbian 2nd phase constructor is private.
		* @param aFile File handle
		*/
        void ConstructL(RFile& aFile  );

    private:    // Data
        ///Ref:
        CPosLandmarkDatabase& iDb;

        ///Ref:
        MLmkAOOperationObserver* iObserver;

        TBool iProgressNote;

        ///Own: EPos parser
        CPosLandmarkParser* iParser;

        // remove the wrapper when CPosLandmarkParser supports indexing:
        CLmkParseAllWrapper* iParseWrapper;

        ///Own:
        CLmkAOOperation* iAOOperation;

        ///Own:
        CLmkDbUtils* iDbUtils;

        /// Own: full name of the parsed file
        HBufC* iFileName;

        // Added for passing FileHandle

        RFile* iFileHandle;

        TBool iFileHandleFlag;
    };

#endif      // CLMKPARSER_H

// End of File
