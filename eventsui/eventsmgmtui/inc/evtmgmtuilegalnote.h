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
* Description:  Asynchronous display of legal and disclaimer note.
*
*/


#ifndef C_EVTMGMTUILEGALNOTE_H
#define C_EVTMGMTUILEGALNOTE_H

// System Includes

//  System Includes
#include <e32base.h>

// User Includes

// User Includes

// Forward Declarations
class CRepository;

// extern data types

// global Functions

// constants go here

/**
 *  Class that displays the Legal and disclaimer note.
 *  This class asynchronously displays the legal note.. 
 *
 *  @since S60 v9.1
 */
class CEvtMgmtUiLegalNote : public CActive
    {    
public:
    /**
     * Constructs a new instance of Legal Note launcher.
     *
     * @return The new instance of Legal Note launcher.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtMgmtUiLegalNote* NewL( CAknAppUi& aAppUi );
    
    /**
     * Constructs a new instance of Legal Note launcher.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Legal Note launcher.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtMgmtUiLegalNote* NewLC( CAknAppUi& aAppUi );  
    
    /**
     * Destructor
     */
    virtual ~CEvtMgmtUiLegalNote ();
    
    /**
     * Display the Legal and Disclaimer Note.
     */
    void DisplayNoteL( );

	// Inherited from CActive 
    void RunL();
    
    void DoCancel();
    
    TInt RunError( TInt aError ); 

private: 
    
    /**
     * Default C++ Constructor.
     */
    CEvtMgmtUiLegalNote( CAknAppUi& aAppUi ); 

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data
    
    /**
     *  CRepository Instance
     */
    CRepository*                    iRepository;
    
    CAknAppUi&												iAppUi;

    };

#endif // C_EVTMGMTUILEGALNOTE_H
