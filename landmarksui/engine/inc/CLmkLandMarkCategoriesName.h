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
* Description:    LandmarksUi Content File -    CLmkLandmarkCategoriesName holds category names for perticuler Landmark
*
*/








#ifndef CLMKLANDMARKCATNAMES_H
#define CLMKLANDMARKCATNAMES_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <EPos_Landmarks.h>

typedef TBuf<KPosLmMaxCategoryNameLength> TCategoryName;

/**
* CLmkLandMarkCategoriesName class.
* This holds category name of perticuler landmark.
*/
class CLmkLandMarkCategoriesName : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Creates CLmkLandMarkCategoriesName
        * @return newly instantiated object
        */
        IMPORT_C static CLmkLandMarkCategoriesName* NewL();
        /**
        * Creates CLmkLandMarkCategoriesName fromn Existing one...
        * @return newly instantiated object
        */
		IMPORT_C static CLmkLandMarkCategoriesName* NewL(CLmkLandMarkCategoriesName& aLmkLandMarkCategoriesName);
        /**
        * Destructor.
        */
        virtual ~CLmkLandMarkCategoriesName();

    protected:  // Constructors and destructor
        /**
        * C++ constructor.
        * @return newly instantiated object
        */
        CLmkLandMarkCategoriesName();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public :
        /**
        * Gets All Category Names.
        * @param aCategoryNameArray
        */
         IMPORT_C void GetCategoryNames(
        /* OUT */       RArray<TCategoryName>&  aCategoryNameArray
        ) const;

        /**
        * Sets Category Names.
        * @param aCategoryNameArray
        */
		IMPORT_C  void SetCategoryNames(
		/* IN */ 		RArray<TCategoryName> &  aCategoryNameArray
		);

        /**
        * Add Category name to array.
        * @param aCategoryNameArray
        */
         IMPORT_C void AddCategoryName(const TDesC &aCategoryName) ;

    private:    // Data

        // Holds Category names
        RArray< TCategoryName >  iCategoriesName;
    };

#endif      // CLMKLANDMARKCATNAMES_H

// End of File
