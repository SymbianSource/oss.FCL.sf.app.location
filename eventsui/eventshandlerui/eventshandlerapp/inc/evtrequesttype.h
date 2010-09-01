/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file that holds all the Request type.
*
*/


#ifndef EVTREQUESTTYPE_H
#define EVTREQUESTTYPE_H

// constants go here
/**
 * @enum TEvtRequestType
 * Type of Request that will be communicated to the Ui App.
 *
 */
enum TEvtRequestType
    {
    /**
     * The Request to Display Editor
     */
    ERequestEditor                                = 6000,
    
    /**
     * The Request to Display Notepad
     */
    ERequestNotepad
    };

#endif // EVTREQUESTTYPE_H
