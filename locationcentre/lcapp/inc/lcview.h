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
* Description:  View class for Location Centre Application UI.
*
*/


#ifndef C_LCAPPVIEW_H_
#define C_LCAPPVIEW_H_

//  SYSTEM INCLUDES
#include <aknview.h>
#include <aknviewappui.h>

// FORWARD DECLARATIONS
class CLcContainer;
class MLcEventHandler;
class CLcListBoxModel;

/**
 *  Location Centre Application's View class.
 *
 *  @lib lcapp.exe
 *  @since S60 v5.0
 */
class CLcView : public CAknView
    {
public:
	/**
	 * Static Two phase constructor
	 *
	 * @param[in] aEventHandler, Location Centre Event Handle.
	 * @param[in] aListBoxModel, List box model.
	 * @return CLcView*, Pointer to the newly created object.
	 */                                              
    static CLcView* NewL( MLcEventHandler&	aEventHandler,
    					  CLcListBoxModel&	aListBoxModel );
    
	/**
	 * Static Two phase constructor.
	 * Leaves the created instance on the Cleanup Stack.
	 *
	 * @param[in] aEventHandler, Location Centre Event Handle.
	 * @param[in] aListBoxModel, List box model.
	 * @return CLcView*, Pointer to the newly created object.
	 */                                                
    static CLcView* NewLC( MLcEventHandler&	aEventHandler,
    					   CLcListBoxModel&	aListBoxModel );
    
    /**
     * C++ Destructor.
     */
    virtual ~CLcView ();

	/**
	 * Handles the event generated when the Screen size is changed when
	 * this View is being displayed.
	 */
	void ScreenSizeChanged();
	
    /**
     * Updates the View object with a new list of Location based Applications
     * and Contents/Services.
     *
     * @param[in] aListBoxModel, New List box model object.
     */	
	void UpdateL( CLcListBoxModel&	aListBoxModel );
	
 	/**
 	 * Updates the listbox icons for Location Centre List Box.
 	 */	
	void UpdateIconsL();
	
    /**
     * Returns the Selected List box item
     *
     * @return TInt Index corresponding selected List box item.
     */
    TInt GetFocussedItem();	
	    
public: // Inherited from Base classes
    
  	/**
  	 * Inherited from CAknView
  	 */     
    void DoActivateL( const TVwsViewId& aPrevViewId,
                            TUid        aCustomMessageId,
                      const TDesC8&     aCustomMessage );

  	/**
  	 * Inherited from CAknView
  	 */ 
    void DoDeactivate();

  	/**
  	 * Inherited from CAknView
  	 */ 
    TUid Id() const;

  	/**
  	 * Inherited from CAknView
  	 */ 
    void HandleCommandL(TInt aCommand);
   
  	/**
  	 * Inherited from CAknView
  	 */    
    void DynInitMenuPaneL( TInt 			aResourceId, 
    					   CEikMenuPane* 	aMenuPane );
    					                   
private:
	/**
	 * Overloaded C++ Contructor.
	 */
    CLcView( MLcEventHandler&	aEventHandler,
    		 CLcListBoxModel&	aListBoxModel );

	/**
	 * Second phase of the two phase constructor.
	 */
    void ConstructL();
    
    /**
     * Handles the visbility of the S60 Help feature.
     *
     * @param aMenuPane, Menu pane object on which the Help feature must
     *                   be evaluated.
     */
    void HandleHelpFeature( CEikMenuPane& aMenuPane ) const;
        
private:
	/**
	 * Location Centre Container.
	 * Owns.
	 */
    CLcContainer*   	iContainer;
    
	/**
	 * Location Centre Event Handler.
	 * Reference.
	 */    
    MLcEventHandler&    iEventHandler;
    
	/**
	 * Location Centre Listbox model.
	 * Reference.
	 */    
    CLcListBoxModel*	iListBoxModel;
    
    };          

#endif // C_LCAPPVIEW_H_

