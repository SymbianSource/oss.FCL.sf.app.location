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
* Description:  Tone class definition.
*
*/


#ifndef C_EVTTONEACTION_H
#define C_EVTTONEACTION_H

// System Includes

// User Includes
#include "evtaction.h"

// Forward Declarations

// extern data types

// global Functions
        
/**
 *  @class CEvtToneAction
 *  Class that captures information for the Tone action Type.
 *	This class allows for Internalizing and Externalizing
 *	the Tone action.
 *  This class is used to store the information of Tone action.
 *
 *  @lib evtstorage.lib
 *  @since S60 v9.1
 */
class CEvtToneAction : public CBase
    {
        
public:  
    /**
     * Constructs a new instance of Event Tone.
     *
     * @return The new instance of Event Tone object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtToneAction* NewL();
    
    /**
     * Constructs a new instance of Event Tone.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Event object.
     * @leave System wide error code if the object creation fails.
     */
    IMPORT_C static CEvtToneAction* NewLC();  

    /**
    * Destructor.
    */
    virtual ~CEvtToneAction();

public: // Get and Set methods
        
    /**
     * Gets the Tone for the Location Events.
     * The Tone is defined by the Location EventsUi on Creation.
     * A reference to the object's internal string is returned.
     *
     * @since S60 v9.1    
     * @return Tone String for the Location Event.
     */
    IMPORT_C TPtrC FileName() const;

    /** 
     * Set the Tone String for the Location Event. The Tone is 
     * copied.
     *
     * @since S60 v9.1  
     * @param[in] aTone Tone String for the Location Event. 
     *                  The Tone string is copied.
     * @leave KErrNoMemory If there is not enough memory to copy the string.
     */
    IMPORT_C void SetFileNameL( const TDesC& aFileName );
    
    /**
     * Gets the ToneLoop flag for the Location Event.
     * The ToneLoop is defined by the Location EventsUi on Creation.
     *
     * @since S60 v9.1    
     * @return TBool ToneLoop flag for the Location Event.
     */
    IMPORT_C TBool ToneLoop() const;
    
    /** 
     * Set the ToneLoop flag for playing tone for Location Event.
     *
     * @since S60 v9.1  
     * @param[in] TBool ToneLoop flag for the Location Event. 
     */
    IMPORT_C void SetToneLoop( const TBool aRepeat );

    /**
     * Externalizes the contents into a CEvtAction.
     *
     * @since S60 v5.0
     * @param aAction  CEvtAction onto which the contents of 
     *                      would be externalized.
     */
    IMPORT_C void ExternalizeL( CEvtAction&   aAction );
   
    /**
     * Internalized the contents of the CEvtAction.
     * 
     * @since S60 v5.0
     * @param aAction  CEvtAction from which the contents would be
     *                      populated.
     */
    IMPORT_C void InternalizeL( CEvtAction&   aAction );

private:

    /**
     * Externalizes the contents into a buffer.
     *
     * @since S60 v5.0
     * @param aWriteStream  Buffer onto which the contents of 
     *                      would be externalized.
     */
    void ExternalizeL( RWriteStream&   aWriteStream );
   
    /**
     * Internalized the contents of the buffer.
     * 
     * @since S60 v5.0
     * @param aReadStream   Buffer from which the contents would be
     *                      populated.
     */
    void InternalizeL( RReadStream&    aReadStream );

    /**
     * Returns the total size for the Internalize/Externalize buffer.
     *
     * @since S60 v5.0
     * @return TInt Total Size of Interalize/Externalize buffer
     */
    TInt TotalSize( );

private: // methods
    /**
     * Default C++ Constructor.
     */
    CEvtToneAction();

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data
	    
	/**
	 * Tone String for Event.
	 */
	HBufC*				            iFileName;
    
	/**
	 * Loop for tone.
	 */
	TBool					        iToneLoop;

    };

#endif // C_EVTTONEACTION_H
