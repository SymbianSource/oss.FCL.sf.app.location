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
* Description: 
*     Implements interface for Blid Engine.
*
*/


#ifndef BLID_ENG_H__
#define BLID_ENG_H__

//  INCLUDES
#include <e32base.h>
#include <s32std.h>
#include <lbspositioninfo.h>
#include <lbs.h>
#include <hwrmlight.h>


// FORWARD DECLARATION
class MBlidSettings;
class MBlidLocation;
class MBlidRouter;
class TNamedCoordinate;


// CLASS DECLARATION
/**
 *  Implements entry point class to BlidEng.dll
 */
class CBlidEng : public CBase
    {
    public:
        /**
        * Enumerations for model creation
        */
        enum TBlidModelType
            {
            EBlidSettings,
            EBlidLocation,
            EBlidRouter
            };
       
       enum TAppMode
			{
			EAppForeground = 1,
			EAppBackground
			};
			
	   enum	TBacklightState
		   {
		   EBlidBacklightAlwaysOn,
		   EBlidBacklightNormal
		   };

    public:
        /**
         * Two-phased constructor.
         *
         * @return CBlidEng object.
         */
        IMPORT_C static CBlidEng* NewL();

        /**
         * Destructor.
         */
        virtual ~CBlidEng();

    private:
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * C++ default constructor.
         */
        CBlidEng();

    public: //new functions
        /**
         * Create stream
         * @param aStore A reference to CStreamStore
         * @param aRestore A reference to CStreamStore
         * @return TStreamId
         */ 
        IMPORT_C TStreamId StoreL(CStreamStore& aStore,
                                  CStreamStore& aRestore);

        /**
         * Open Stream
         * @param aStore A reference to CStreamStore
         * @param aStreamId 
         */
        IMPORT_C void RestoreL( const CStreamStore& aStore,
                        TStreamId aStreamId );

        /**
         * Settings model getter.
         * Ownership not transferred.
         *
         * @return pointer to the settings model
         */
        IMPORT_C MBlidSettings* SettingsModel() const;

        /**
         * Location model getter.
         * Ownership not transferred.
         *
         * @return pointer to the location model
         */
        IMPORT_C MBlidLocation* LocationModel() const;

        /**
         * Router model getter.
         * Ownership not transferred.
         *
         * @return pointer to the router model
         */
        IMPORT_C MBlidRouter* RouterModel() const;

        /**
         * Model creator.
         *
         * @param aModel model identificator.
         */
        IMPORT_C void CreateModelL( TBlidModelType aModel );
        
        /**
         * Returns the distance to the currently set destination
         *
         * @param aDistance. Holds the distance to the destination 
         * on return.
         */
        IMPORT_C TInt GetDistanceToDestination(TReal32& aDistance);
        
        /**
         * Determines if user has arrived to the 
         * set destination using an algorithm described in the
         * defination. 
         *
         * @ return value
         * Returns ETrue if current position is in vicinity of the 
         * set destination 
         * 
         * EFalse otherwise.         
         */
        IMPORT_C TBool HasArrivedToDestination();
        
        /**
         * Sets the application mode (i.e Background or Foreground)
         *
         * @param aMode. 
         * EAppBackground if Background, 
         * EAppForeground if Foreground                 
         */
        IMPORT_C void SetAppMode(TAppMode aMode);
        
        /**
         * Sets the Backlight state
         *
         * @param aState. 
         * EBlidBacklightAlwaysOn if backlight should always be on
		 * EBlidBacklightNormal if backlight should be as per general settings 
         */
        IMPORT_C void SetBackLightStateL(TBacklightState aState);
    
        /**
         * Returns the application mode
         * 
         * @return value
         * EAppForeground if application in foreground
         * EAppBackground if application in background
         */
        IMPORT_C TAppMode AppMode();

    private: // new functions
        /**
         * Save engine's data to file
         * @param aStream A reference to RWriteStream
         */ 
        void ExternalizeL( RWriteStream& aStream ) const;  

        /**
         * Load initial data from file 
         * @param aSteam A reference to RReadStream
         */
        void InternalizeL( RReadStream& aStream );

        /**
         * Load initial data from file 
         * @param aSteam A reference to RReadStream
         */
        void InternalizeV1L( RReadStream& aStream );

    private:    // data
        /// Own: Settings model
        MBlidSettings* iSettings;

        /// Own: Location model
        MBlidLocation* iLocation;

        /// Own: Router model
        MBlidRouter* iRouter;

        /// Own: iServer
        RPositionServer iServer; 
        
        /// stream id
        mutable TStreamId iStreamId;   
                
        //Application mode (foreground/background)
        TAppMode iAppMode;
        
        //Own: Pointer to Backlight resource class
        CHWRMLight*     iHWRMLight;
    };

#endif // BLID_ENG_H__

// End of File
