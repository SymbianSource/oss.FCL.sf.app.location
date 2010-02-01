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
*                Centre pop-up.
*
*/


#ifndef C_LCPOPUPLISTBOXMODEL_H
#define C_LCPOPUPLISTBOXMODEL_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class CLcLocationAppInfoArray;

/**
 *  List box model for Location Centre pop-up dialog.
 *  
 *  This class provides the implementation of the list-box model for Location
 *  Centre lists.
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLcPopupListBoxModel ) : public MDesCArray
    {
public:    
    /**
     * Constructs a new instance of Location Centre pop-up list box model.
     *
     * @param[in] aAppArray Array of Location based Applicaions and Contents/Services
     *                      The ownership of the array is transferred.
     * @return The new instance of Location Centre model object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcPopupListBoxModel* NewL( CLcLocationAppInfoArray&	aAppArray );        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * model object.
     */
    virtual ~CLcPopupListBoxModel();
    
    /**
     * Updates the Listbox model with a new Array of Location based Applications
     * and Contents/Services.
     *
     * @param[in] aAppArray Array of Location based Applicaions and Contents/Services
     *                      The ownership of the array is transferred.
     */
    void UpdateModel( CLcLocationAppInfoArray&	aAppArray );    
        
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
    CLcPopupListBoxModel(  CLcLocationAppInfoArray&	aAppArray );

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

#endif // C_LCPOPUPLISTBOXMODEL_H
