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
* Description:  Blid application note class definition
*
*/


#ifndef BLIDNOTES_H
#define BLIDNOTES_H

// CLASS DECLARATION
class CEikonEnv;
/**
*  BlidNotes application class.
*/
class BlidNotes 
    {
    public:
        /**
         * Enumerations for model creation
         */
        enum TBlidNotes
            {            
            EBlidOverWriteWaypointNote,
            EBlidGPSConnectionNotAccurate
            };
    public: // new functions        
        /** 
         * Notify user that when user is saving a waypoint 
         * and all waypoint slots are used.
         */
        static void OverWriteWaypointNoteL();

         /** 
         * Notify user if GPS device is not sending proper data
         * i.e. when less than 3 satellites in view.
         */
        static void PoorGPSDataNoteL();
        
        /** 
         * Notifies user that no PSY has been enabled 
         *
         */
        static void NoPsyConnectedNoteL();
        
        /** 
         * Notifies user that GPS data is being searched and 
         * located
         *
         */
        static void SearchingGPSDDataNoteL();
        
        /** 
         * Notifies user that GPS data is being searched and 
         * located         
         */
        static void GPSDataAvailableL();
        
        /** 
         * Notifies user that GPS data is being searched and 
         * located         
         */
        static void GPSDeviceUnavailableNoteL();
        
        /** 
         * Notifies user that the landmark set as destination
         * is empty and does not have coordinate data
         *
         */
        static void EmptyLandmarkNoteL(const TDesC& landmarkName);        
        
        /** 
         * Notifies user that the system is out of memory
         *
         */
        static void OutOfMemoryNoteL();
        
        /** 
         * Displays the "No GPS data" note
         *
         */
        static void NoGPSDeviceL();
        
    public: //Data    
        /*
         Boolean to check if the GPSDataAvailableL has
         already been displayed.
        */
        static TBool iNoteDisplayed;
        
        /**
         * Boolean to check if the GPS unavailable note is 
         * displayed or not
         */
        static TBool iGPSUnavailableDisplayed;
        
        //environment variable
        static CEikonEnv* iEnv;

    };

#endif // BLIDNOTES_H

// End of File
