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
* Description:  Filter Model class for Events management UI start-up view.
*
*/


#ifndef CEVTMGMTUIFILTERMODEL_H
#define CEVTMGMTUIFILTERMODEL_H

// SYSTEM INCLUDES
#include <e32base.h>        // CBase
#include <bamdesca.h>       // MDesCArray

// FORWARD DECLARATIONS
class CAknSearchField;

/**
 * Events Management UI start-up view listbox Filter model
 *
 * @lib evtmgmtui.exe
 * @since S60 v5.0
 */
class CEvtMgmtUiFilterModel : public CBase, public MDesCArray
    {
public: 
    
  	/**
  	 * Virtual destructor
  	 */
    virtual ~CEvtMgmtUiFilterModel();

	/**
	 * Static Two phase constructor
	 *
	 * @return CEvtMgmtUiFilterModel*, Pointer to the newly created object.
	 */  
    static CEvtMgmtUiFilterModel* NewLC();

	/**
	 * Static Two phase constructor
	 *
	 * @return CEvtMgmtUiFilterModel*, Pointer to the newly created object.
	 */  
    static CEvtMgmtUiFilterModel* NewL();
    
protected:   
    /**
    * Default constructor
    */
    CEvtMgmtUiFilterModel();
       
  	/**
  	 * Two phased constructor
  	 */    
    void ConstructL();
    
public:
  	/**
  	 * Set the Item text Array
  	 */
	void SetItemTextArray(MDesCArray* aActualItemTextArray);

  	/**
  	 * Set the Search Field
  	 */
	void SetSearchField(CAknSearchField* aSearchField);

  	/**
  	 * Returns the String at zero index.
  	 */
	TPtrC16 GetZeroIndex();
	
  	/**
  	 * Inherited from MDesCArray
  	 */
    TInt    MdcaCount() const;
    
  	/**
  	 * Inherited from MDesCArray
  	 */    
    TPtrC16 MdcaPoint( TInt aIndex ) const;
                                        
private:

  	/**
  	 * Obtain the current Search text.
  	 */
    void GetSearchTextL(HBufC*& aString) const;

  	/**
  	 * Update the text based on search string.
  	 */
    void UpdateTextL() const;
    
    
private:
    /**
      * events array
      * Owns
      */
   CAknSearchField* iSearchField;
   
    /**
      * events array
      * Owns
      */
   MDesCArray* iActualItemTextArray;
   
   HBufC* iBuffer;
    };

#endif // CEVTMGMTUIFILTERMODEL_H
// End of File
