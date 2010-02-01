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
* Description:  List box defintion for Location System UI
*
*/


#ifndef C_LCLISTBOX_H
#define C_LCLISTBOX_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <aknlists.h>

// FORWARD DECLARATIONS
class CLcLocationAppInfoArray;
class CLcListBoxModel;

/**
 *  List box for Location Centre
 *  
 *  This class specializes the CAknSingleLargeStyleListBox class for Location
 *  Centre. This class takes the ownership of the listbox model and exposes
 *  functions for updation of the list.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
class CLcListBox : public CAknSingleLargeStyleListBox
    {
public:    
    /**
     * Constructs a new instance of Location Centre list box.
     *
     * @param[in] aAppArray Array of Location based Applicaions and Contents/Services
     *                      The ownership of the array is transferred.
     * @param[in] aParent	Parent Window.
     * @return The new instance of Location Centre listbox object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcListBox* NewL( 	   
    							   CLcLocationAppInfoArray*  aAppArray,
    						 const CCoeControl* 		     aParent,
    						 	   MEikListBoxObserver*	     aObserver );      

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre listbox object.
     */
    virtual ~CLcListBox();

    /**
     * Updates the list box contents. This function deletes the previous instance
     * of the Location based Applications and Contents/Services application
     * information and replaces it with this new list.
     *
     * @param[in] aAppArray Array of Location based Applicaions and Contents/Services
     *                      The ownership of the array is transferred.
     */
    void UpdateListBoxContentsL( CLcLocationAppInfoArray*      aAppArray );

private:
    /**
     * Overloaded constructor
     */
    CLcListBox();

    /**
     * Second phase of the two phase constructor
     */
    void ConstructL( 	   CLcLocationAppInfoArray*  aAppArray,
    			 	 const CCoeControl* 		   	 aParent,
    			 	 	   MEikListBoxObserver*	     aObserver );
    			 	 	   
    /**
     * Updates the Location Centre List-box's Icons array to point to the 
     * latest set of icons.
     */
    void UpdateIconsL();    			 	 	   

private: // data
	/**
	 * Listbox model
	 *
	 * Owns
 	 */
 	CLcListBoxModel*				iListBoxModel;
    };

#endif // C_LCLISTBOX_H
