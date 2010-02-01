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
* Description:    LandmarksUi Content File -
*     See class description below
*
*/







#ifndef CLMKLANDMARK_H
#define CLMKLANDMARK_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkCategory.h>

// CLASS DECLARATION

/**
*  This is a container class for a landmark.
*
*  A landmark is principally a location with a name.
*
*  The landmark object can either be created by a client or retrieved from
*  @ref CPosLandmarkDatabase.
*
*  A landmark consists of a number of landmark attributes, e.g. landmark name,
*  landmark position, coverage area, etc.
*
*  A landmark may also contain generic position fields. These position fields
*  are defined in LbsFields.h. Only text fields are supported.
*
*  CLmkLandmark contains functions for setting and getting landmark attributes
*  and position fields.
*
*  Note that CLmkLandmark is only a local representation of the landmark. To
*  update the database, call @ref CPosLandmarkDatabase::UpdateLandmarkL (or
*  @ref CPosLandmarkDatabase::AddLandmarkL for a new landmark).
*
*  @lib eposlandmarks.lib
*  @since S60 3.0
*  @version $Revision: 1.15 $, $Date: 2005/07/07 13:40:48 $
*/
class CLmkLandmark : public CBase
    {

    public:  // Constructors and destructor

        /**
        * Two-phased copy constructor.
        *
        * @param aLandmark The landmark to copy.
        * @returns A copy of the specified landmark object.
        */
        IMPORT_C static CLmkLandmark* NewL(
        const CPosLandmark*  aLandmark,
        const RPointerArray<CPosLandmarkCategory>& aCategories
       );

       IMPORT_C static CLmkLandmark* CLmkLandmark::NewL(
       const CLmkLandmark&  aLandmark);


        /**
        * Destructor.
        */
        virtual ~CLmkLandmark();

    public://New Functions

        IMPORT_C CPosLandmark* PosLandmark();
        IMPORT_C RPointerArray<CPosLandmarkCategory>& Categories();

    private:

        /**
        * C++ default constructor.
        */
        CLmkLandmark();

        CLmkLandmark(const CPosLandmark* aLandmark,
                     const RPointerArray<CPosLandmarkCategory>& aCategories);

        // Prohibit copy constructor
        CLmkLandmark(const CLmkLandmark&);

        // Prohibit assigment operator
        CLmkLandmark& operator= (const CLmkLandmark&);

        void ConstructL(const CPosLandmark* aLandmark,
                     const RPointerArray<CPosLandmarkCategory>& aCategories);

    private:    // Data

    CPosLandmark*  iLandmark;
    RPointerArray<CPosLandmarkCategory> iCategories;
    };

#endif      // CLMKLANDMARK_H

// End of File
