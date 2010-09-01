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
* Description:  Container class for Events Management UI startup view view.
*
*/


#ifndef C_EVTMGMTUISTARTUPCONTAINER_H
#define C_EVTMGMTUISTARTUPCONTAINER_H

// SYSTEM INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>

#include "evtdefs.h"
#include "evtmgmtuicmdhdlr.h"

// FORWARD DECLARATIONS
class CAknIconArray;
class CEvtMgmtUiListBox;
class CEvtMgmtUiModel;
class CEikMenuPane;
class CAknSearchField;
/**
 * Events management UI start-up view's container class. This class is 
 * the chief windowing component of Events management UI start-up view
 *
 * @lib evtmgmtui.exe
 * @since S60 v5.0
 */
class CEvtMgmtUiStartupContainer : public CCoeControl,
					               public MEikListBoxObserver
    {
public: 
	/**
	 * Static Two phase constructor
	 *
	 * @param[in] aRect, 		 Client Rectangle for this window owning control.
	 */
	static CEvtMgmtUiStartupContainer* NewL( const TRect&  aRect, CEvtMgmtUiModel* aModel,
                                    MEvtMgmtUiCmdHdlr& aCmdHandler );
	
  	/**
  	 * Virtual destructor
  	 */
    ~CEvtMgmtUiStartupContainer();
    
    /**
     * Returns the help context
     *
     * @param aContext  The help context that has to be determined
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
 	
public : //New functions
    
    /**
   * Update listbox
   */
    void UpdateListBoxL(TInt aSelectedItemIndex);
   
    /**
    * Return the index of the selected item 
    */
    TInt CurrentItemIndex();
  
    /** 
    * Set the Title text
    */ 
    void MakeTitleL();    
    
    /** 
     * It will handle mark functionality.
     */ 
    void HandleMarkCmdL( TInt aIndex, TBool aIsMarked );
    
    /** 
     * It will display mark menu items dynamically.
     */ 
    void HandleMarkableListDynInitMenuPane(TInt aResourceId, CEikMenuPane *aMenu);
    
    /*
     * It will return count of marked items
     */
    TInt NumberOfMarkedItems(); 
    
    /*
     * It will return marked items
     */
    const CArrayFix<TInt>* MarkItems();
    
    /*
     * It will return Filtered Index of the passed index
     */
    TInt FilteredIndex(TInt aIndex);
    
    /*
     * It will return Filtered number of list items
     */
    TInt FilteredCount();
     
private: //new functions
    
    /**
     * Creates the List box and List box model.
     */     
    void CreateListboxL(CEvtMgmtUiModel* aModel);
    
    /**
     * Creates the Find Box.
     */     
    void CreateFindBoxL();
    
    /**
     * Check for FindBox Visibilty.
     */     
    void HandleFindBoxVisibility();
    
    /**
     * Update icons
     */
    void UpdateIconsL();    
     
    /**
     * Append icon to icon array. CAknIconArray owns icons so it is 
     * easier to it leave safely.
     * @param aIcons icon array
     * @param aIconFileWithPath icon file name with path
     * @param aSkinID skin id of the icon
     * @param aIconGraphicsIndex picture index
     * @param aIconGraphicsMaskIndex mask index
     */
    void AddIconL( CAknIconArray&       aIcons,
                   const TDesC&         aIconFileWithPath,
                   TAknsItemID          aSkinID,
                   TInt                 aIconGraphicsIndex,
                   TInt                 aIconGraphicsMaskIndex );
     
    /**
     * Append Color icon to icon array. CAknIconArray owns icons so it is 
     * easier to it leave safely.
     * @param aIcons icon array
     * @param aIconFileWithPath icon file name with path
     * @param aSkinID skin id of the icon
     * @param aIconGraphicsIndex picture index
     * @param aIconGraphicsMaskIndex mask index
     */
    void AddColorIconL( CAknIconArray&      aIcons,
	                   const TDesC&         aIconFileWithPath,
	                   TAknsItemID          aSkinID,
	                   TInt                 aIconGraphicsIndex,
	                   TInt                 aIconGraphicsMaskIndex );
                         
    /**
     * Gets Events Management UI icon file path and name. Ownership
     * is transferred, item is left in the cleanup stack.
     * @return TFileName* The full name of the Events UI icon file.
     */        
    TFileName* IconFileNameLC() const;

   
    
protected: // Inherited from Base classes
  	/**
  	 * Inherited from CCoeControl
  	 */  
    void SizeChanged();

    /**
     * Inherited from CCoeControl
     */  
    void FocusChanged( TDrawNow aDrawNow );

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
                                                           			            
private:
	
	/**
	 * C++ Default constructor
	 */
    CEvtMgmtUiStartupContainer(MEvtMgmtUiCmdHdlr& aCmdHandler);
    
	/**
	 * Second phase of the two phase constructor
	 */     
    void ConstructL( const TRect& 	aRect, CEvtMgmtUiModel* aModel );
    
private:
	/**
	 * List box
	 * Owns
	 */
    CEvtMgmtUiListBox*	iListBox;
	
	/**
	 * Find box
	 * Owns
	 */
	CAknSearchField*				iFindBox;

    /**
     * Help context
     */
    TCoeContextName                 iContextName;     

    /**
     * Command Handler
     */
    MEvtMgmtUiCmdHdlr& 				iCmdHandler;

    /**
     * List Model Count before List model is updated.
     */
    TInt iPrevCount;
    
	};

#endif  // C_EVTMGMTUISTARTUPCONTAINER_H

// End of File  
