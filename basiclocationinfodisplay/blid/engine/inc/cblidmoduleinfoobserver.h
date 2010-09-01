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
* Description:  Blid engine object.
*
*/



#ifndef CBLIDMODULEINFOOBSERVER_H
#define CBLIDMODULEINFOOBSERVER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class RPositionServer;
class MBlidLocation;   

/**
* An active object that handles the note dialog execution
*/

NONSHARABLE_CLASS( CBlidModuleinfoObserver ) : public CActive
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         */
        static CBlidModuleinfoObserver* NewL( RPositionServer& 	aServer,
								 			  MBlidLocation&	aObserver );

        /**
         * Destructor.
         */
        virtual ~CBlidModuleinfoObserver();
        
    private:
        /**
         * C++ default constructor.
         */
        CBlidModuleinfoObserver( RPositionServer& 	aServer,
								 MBlidLocation&	aObserver );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();  

    private: // From CActive
        void RunL();
        void DoCancel();
    
    public:
        void StartNotification();   

    private:
		RPositionServer& iServer;
		MBlidLocation&	iObserver;   
    };


#endif  // CBLIDMODULEINFOOBSERVER_H