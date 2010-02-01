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
* Description:  Container class for Location Centre Application UI.
*
*/


#ifndef C_LCCONTAINER_H
#define C_LCCONTAINER_H

// SYSTEM INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>

// FORWARD DECLARATIONS
class CLcAppUi;
class CLcListBoxModel;
class CAknSingleLargeStyleListBox;
class MLcEventHandler;

/**
 *  Location Centre Application's Container class. This class is the chief
 *  windowing component of Location Centre which displays the List of
 *  Location based Applications and Contents/Services
 *
 *  @lib lcapp.exe
 *  @since S60 v5.0
 */
class CLcContainer : public CCoeControl,
					 public MEikListBoxObserver
    {
public:
	/**
	 * Static Two phase constructor
	 *
	 * @param[in] aRect, 		 Client Rectangle for this window owning control.
	 * @param[in] aEventHandler, Location Centre Event Handle.
	 * @param[in] aListBoxModel, List box model.
	 */
	static CLcContainer* NewL( const TRect& 			aRect,
									 MLcEventHandler&	aEventHandler,
									 CLcListBoxModel&	aListBoxModel );
	
  	/**
  	 * Virtual destructor
  	 */
    ~CLcContainer();
    
    /**
     * Returns the help context
     *
     * @param aContext  The help context that has to be determined
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    
    /**
     * Returns the Selected List box item
     *
     * @return TInt Index corresponding selected List box item.
     */
    TInt GetFocussedItem();     
 
    /**
     * Updates the Container object with a new list of Location based Applications
     * and Contents/Services.
     *
     * @param[in] aListBoxModel, New List box model object.
     */
 	void UpdateL( CLcListBoxModel&	aListBoxModel );
 	
 	/**
 	 * Updates the listbox icons for Location Centre List Box.
 	 */
	void UpdateIconsL(); 	
 	
protected: // Inherited from Base classes

  	/**
  	 * Inherited from CCoeControl
  	 */  
    void SizeChanged();

  	/**
  	 * Inherited from CCoeControl
  	 */
    TInt CountComponentControls() const;

  	/**
  	 * Inherited from CCoeControl
  	 */
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
  	/**
  	 * Inherited from CCoeControl
  	 */    
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                         			   TEventCode aType );
                         			   
	/**
	 * Inherited from CCoeControl
	 */
	void HandleResourceChange( TInt aType );	                         			                    			                   			   
                         			           
    /**
     * Inherited from MEikListBoxObserver
     */
    void HandleListBoxEventL( CEikListBox*      aListBox, 
                              TListBoxEvent     aEventType );
                              
    /**
     * Handles Focus Change to list 
     */
    void FocusChanged( TDrawNow aDrawNow );                              
                                                           			            
private:
	
	/**
	 * C++ Default constructor
	 */
    CLcContainer( MLcEventHandler&	aEventHandler,
    			  CLcListBoxModel&	aListBoxModel );
    
	/**
	 * Second phase of the two phase constructor
	 */     
    void ConstructL( const TRect& 	aRect );
    
    /**
     * Sets the Title text
     * @param aResourceText Resource to create title
     */
    void MakeTitleL( TInt aResourceText );
            
	/**
	 * Creates the List box and List box model.
	 */     
    void CreateListboxL();
            
public:
	/**
	 * List box
	 * Owns
	 */
	CAknSingleLargeStyleListBox*	iListBox;

    /**
     * Help context
     */
    TCoeContextName                 iContextName;
    
    /**
     * Location Centre Event handler
     */
    MLcEventHandler&				iEventHandler;
    
    /**
     * Location Centre list box model
     */    
    CLcListBoxModel*				iListBoxModel;	    
	};

#endif  // C_LCCONTAINER_H

// End of File
