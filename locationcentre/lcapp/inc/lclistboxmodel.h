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
* Description:  List box model for the list-box used to display the Location
*                Centre pop-up and the list in Location Centre Application.
*
*/


#ifndef C_LCLISTBOXMODEL_H
#define C_LCLISTBOXMODEL_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <bamdesca.h>  // MDesCArray

// FORWARD DECLARATIONS
class CLcLocationAppInfoArray;
class CLcListBoxModel;

/**
 *  List box model for Location Centre
 *  
 *  This class provides the implementation of the list-box model for Location
 *  Centre lists. Currently this is used in both the Location Centre pop-up
 *  and in Location Centre Application.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
class CLcListBoxModel : public CBase,
						public MDesCArray
    {
public:    
    /**
     * Constructs a new instance of Location Centre list box model.
     *
     * @param[in] aAppArray Array of Location based Applicaions and Contents/Services
     *                      The ownership of the array is transferred.
     * @return The new instance of Location Centre model object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcListBoxModel* NewL( CLcLocationAppInfoArray*	aAppArray );        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * model object.
     */
    virtual ~CLcListBoxModel();
    
    /**
     * Updates the Listbox model with a new Array of Location based Applications
     * and Contents/Services.
     *
     * @param[in] aAppArray Array of Location based Applicaions and Contents/Services
     *                      The ownership of the array is transferred.
     */
    void UpdateModel( CLcLocationAppInfoArray*	aAppArray );
    
    /**
     * Accessor method for retrieving the Application Information array.
     *
     * @return CLcLocationAppInfoArray, Reference to the existing Application
     * 									Information array.
     */
    CLcLocationAppInfoArray& AppArray();
    
    /** 
     * Returns the Application's Identifer corresponding to the index passed.
     * 
     * @param[in] aIndex, Index of the Location Application whose Identifier
     *                    is requested. 
     */
    TPtrC AppInfoId( TInt aIndex ) const;     
        
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
    CLcListBoxModel(  CLcLocationAppInfoArray*	aAppArray );

    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();

private: // data
    
    /**
     * Buffer for packing the MdcaPoint
     *
     * Owns
     */
    HBufC16*					iBuffer;
    
    /**
     * Application Information Array
     */
    CLcLocationAppInfoArray*	iAppArray;
    };

#endif // C_LCLISTBOXMODEL_H
