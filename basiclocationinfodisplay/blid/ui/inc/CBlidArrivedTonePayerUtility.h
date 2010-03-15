/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Blid application arrived tone player utility class declaration.
*
*/



#ifndef __CBLIDARRIVEDTONEPLAYERUTILITY_H__
#define __CBLIDARRIVEDTONEPLAYERUTILITY_H__

// SYSTEM INCLUDES
#include <mdaaudiosampleplayer.h>
// USER INCLUDES

// FORWARD DECLARATION

/*
* @description
*
*/
class CBlidArrivedTonePlayerUtility : public CBase,
                        			 public MMdaAudioPlayerCallback
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CBlidArrivedTonePlayerUtility* NewL( );
        
        /**
        * Destructor.
        */
        ~CBlidArrivedTonePlayerUtility();

	public: // 	from MMdaAudioPlayerCallback	        
        /**
        * From MMdaAudioPlayerCallback Called when audio clip initialization completes.
        *
        * @since 2.0
        *
        * @param aError    IN  Resulting error code.
        * @param aDuration IN Audio clip duration.
        *
        * @return void
        */
        void MapcInitComplete( TInt aError,
                               const TTimeIntervalMicroSeconds& aDuration );
        
        /**
        * From MMdaAudioPlayerCallback Called when audio clip playback completes.
        *
        * @since 2.0
        *
        * @param aError    IN  Resulting error code.
        *
        * @return void
        */
        void MapcPlayComplete( TInt aError );
                

     private:
        /**
        * C++ default constructor.
        */
        CBlidArrivedTonePlayerUtility();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

	public:
		void PlayToneL( const TFileName& aFileName );
		void CanclePayer(); 
		
	private:
		// Audio player utility.
        CMdaAudioPlayerUtility* 	iAudioPlayer;
        TFileName 					iFileName;
        TTimeIntervalMicroSeconds	iAudioDuration;			    
    };

#endif // __CBLIDARRIVEDTONEPLAYERUTILITY_H__