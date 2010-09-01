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
* Description:  List-box model class for Simulation PSY file selector dialog.
*
*/


#ifndef C_SIMPSYFILESELECTORMODEL_H
#define C_SIMPSYFILESELECTORMODEL_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <bamdesca.h>

/**
 *  List-box model class for Simulation PSY file selector dialog.
 *  
 *  This class provides the implementation of the list-box model for Simulation
 *  PSY file selector model. The class loads all the available Simulation PSY
 *  files present in the system and allows the configure one of the Simulation
 *  files for obtaining simulated Location information.
 * 
 *  @lib simpsyui.exe
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CSimPsyFileSelectorModel ) : public MDesCArray
    {
public:    
    /**
     * Constructs a new instance of Simulation PSY file selector list
     * box model.
     *
     * @return The new instance of list-box model object.
     * @leave System wide error code if the object creation fails.         
     */
    static CSimPsyFileSelectorModel* NewL();        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this object.
     */
    virtual ~CSimPsyFileSelectorModel();
    
    /** 
     * Returns the simulation file name corresponding to the
     * selected item.
     *
     * @param  aIndex Index of the item whose file name is requested
     * @return TPtrC  File name at the requested index
     */
    TPtrC SimulationFileNameL( TInt aIndex );      
        
public: // Inherited from Base classes
    /**
     * Inherited from MDesCArray
     */
    virtual TInt MdcaCount() const;

    /**
     * Inherited from MDesCArray
     */
    virtual TPtrC16 MdcaPoint( TInt aIndex ) const;

private:
    /**
     * Overloaded constructor
     */
    CSimPsyFileSelectorModel();

    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();
    
    /**
     * Gets the correct file path
     */
    void GetCorrectPathL( TFileName& aFilePath );
    
    /**
     * Appends all the sps and nmea files in the specified
     * path to the Filename array
     */
    void AppendFileL( const TDesC&	aDir );

private: // data
    
    /**
     * Buffer for packing the MdcaPoint
     *
     * Owns
     */
    HBufC16*					iBuffer;
    
    /**
     * Simulation PSY File names
     */
	RArray< TFileName >			iFileNameArray;
    };
    
#endif // C_SIMPSYFILESELECTORMODEL_H
