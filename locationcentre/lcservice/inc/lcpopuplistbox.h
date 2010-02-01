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
* Description:  List box used to display the Location Centre pop-up.
*
*/


#ifndef C_LCPOPUPLISTBOX_H
#define C_LCPOPUPLISTBOX_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <aknlists.h>

// FORWARD DECLARATIONS
class CLcPopupListBoxModel;
class CLcLocationAppInfoArray;

/**
 *  List box for Location Centre pop-up dialog.
 *  
 *  This class provides the extension of Single Large style list box which is
 *  used to display Location Centre pop-up.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLcPopupListBox ) : public CAknSingleGraphicPopupMenuStyleListBox
    {
public:
    /**
     * Overloaded constructor
     */
    CLcPopupListBox( CLcLocationAppInfoArray*	aAppArray );

    /**
     * Second phase of the two phase constructor
     */
    void ConstructL( CCoeControl*              aParent );          

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * listbox object.
     */
    virtual ~CLcPopupListBox();
    
    /**
     * Updates the Listbox model with a new Array of Location based Applications
     * and Contents/Services.
     *
     * @param[in] aAppArray Array of Location based Applicaions and Contents/Services
     *                      The ownership of the array is transferred.
     */
    void UpdateListBoxL( CLcLocationAppInfoArray*	aAppArray );
    
	/**
	 * Updates the listbox icons.
	 */
	void UpdateIconsL();
    
    /** 
     * Returns the Application's Identifer corresponding to the selected
     * item
     */
    TPtrC AppInfoId() const;
    	    
private: // Inherited from Base classes.
    /**
     * Imherited from CCoeControl.
     */
    void HandleResourceChange( TInt    aType );

private: // data
	/**
	 * Application Information array
	 *
	 * Owns.
	 */
	 CLcLocationAppInfoArray*	iAppArray;
	 
    /**
     * Pop-up List box model.
     */
    CLcPopupListBoxModel*	    iListBoxModel;  
    };

#endif // C_LCPOPUPLISTBOX_H
