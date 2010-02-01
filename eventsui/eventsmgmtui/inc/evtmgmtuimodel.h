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
* Description:  Model class for Events management UI start-up view.
*
*/


#ifndef CEVTMGMTUIMODEL_H
#define CEVTMGMTUIMODEL_H

// SYSTEM INCLUDES
#include <e32base.h>        // CBase
#include <bamdesca.h>       // MDesCArray
#include "evtdefs.h"
#include "evtmgmtuiengine.h"

// Forward Declarations
class CEvtBasicEventInfo;
typedef TUint32 TEvtEventsStatusMask;

/**
 * Events Management UI start-up view listbox model
 *
 * @lib evtmgmtui.exe
 * @since S60 v5.0
 */
class CEvtMgmtUiModel : public CBase, public MDesCArray
    {
public: 
  	/**
  	 * Enumeration to Currently shown Events
  	 */
    enum TEvtShowAttribute
        {
        EEvtShowAttributeActive = 0x0001,
        EEvtShowAttributeDraft = 0x0002,
        EEvtShowAttributeCompleted = 0x0004
        };
public: 
    
  	/**
  	 * Virtual destructor
  	 */
    virtual ~CEvtMgmtUiModel();

	/**
	 * Static Two phase constructor
	 *
	 * @return CEvtMgmtUiModel*, Pointer to the newly created object.
	 */  
    static CEvtMgmtUiModel* NewLC();

	/**
	 * Static Two phase constructor
	 *
	 * @return CEvtMgmtUiModel*, Pointer to the newly created object.
	 */  
    static CEvtMgmtUiModel* NewL();
    
protected:   
    /**
    * Default constructor
    */
    CEvtMgmtUiModel();
       
  	/**
  	 * Two phased constructor
  	 */    
    void ConstructL();
    
public:
    /*
     * It will return Events array object reference.
     * @ret Events array object reference
     */
    RPointerArray<CEvtBasicEventInfo>& CEvtMgmtUiModel::EventsArray();
    
	/**
     * returns eventinfo object.
     * @param[in] aIndex index of event in array
     */
	CEvtBasicEventInfo& EventFromModel( TInt aIndex );
	
	/**
     * Returns total events.     
	 * @ret Number of Events in the model
     */
	TInt TotalEvents();
	
  	/**
  	 * Return Event Id based on the Index
     * @param[in] aIndex index of event in array
	 * @ret Event Id
  	 */ 
	TEvtEventId EventId( TInt aIndex );
	
  	/**
  	 * Returns the Updated current Item Index
	 * Called after the model is updated.
	 * @ret Updated current item Index
  	 */ 
	TInt UpdatedCurrentItemIndex();
	
  	/**
  	 * Set the Current Item Index
	 * Called before the model is updated.
     * @param[in] aIndex index of event in array
  	 */ 
	void SetCurrentItemId( TInt aItemIndex  );
 
  	/**
  	 * Check for Mandatory Fields of an Event
	 * @param aIndex Index in the model.
     * @param[in] aIndex index of event in array
	 * @ret ETrue If mandatory fields are filled.
  	 */ 
	TBool MandatoryFieldsFilled(  TInt aIndex  );
	
  	/**
  	 * Return Event status based on the Index on the model
     * @param[in] aIndex index of event in array
	 * @ret Event Status based on the Index
  	 */ 
	TEvtEventStatus EventStatus( TInt aIndex );
	
  	/**
  	 * Update the Events Status Mask
	 * Called after the Model is updated.
  	 */ 
	void UpdateEventsStatus();
	
  	/**
  	 * Return Bit Mask which has the Status of all the 
	 * Events present in the Model
	 * @ret Status Bit Mask of the events present in model
  	 */ 
	TEvtEventsStatusMask EventsStatus();
	
  	/**
  	 * Currently active Status Filter
	 * @ret Currently active Status Filter
  	 */ 
	CEvtMgmtUiEngine::TEvtEventStatusFilter StatusFilter( );
	
  	/**
  	 * Set the Status Filter
     * @param[in] aFilter Filter to be Set for the model
  	 */ 
	void SetStatusFilter( CEvtMgmtUiEngine::TEvtEventStatusFilter
                                        aFilter );
	                          
private:
  	/**
  	 * Return Index based on the Status Filter
  	 */ 
	TInt IndexBasedOnFilter( TInt aIndex ) const;
	
  	/**
  	 * Return Index without the Status Filter
  	 */ 
	TInt IndexWithoutFilter( TInt aIndex ) const;
                                        
private:
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
      * Update Type
      */
    enum TEvtModelUpdateState
        {
        EEvtCreate = 0x0001,
        EEvtModify,
        EEvtDelete 
        };
private:
    /**
      * events array
      * Owns
      */
   RPointerArray<CEvtBasicEventInfo> iEventsArray;
       
    /**
     * Conversion buffer
     * Owns
     */
    HBufC*          iBuffer;
    
    /**
     * Buffer containing the text for Create new trigger
     * Owns
     */    
    HBufC*          iCreateNew;   
    
    /**
     * Buffer containing the text for Unnamed String
     * Owns
     */    
    HBufC* 	        iUnnamed;
    
    /**
     * Active Triggers count
     */    
    TInt 			iActiveTriggers;
    
    /**
     * Draft Triggers count
     */    
    TInt 			iDraftTriggers;
    
    /**
     * Completed Triggers count
     */    
    TInt            iCompletedTriggers;
    
    /**
     * Currently Focused Event Id to be maintained before model Update
     */    
    TEvtEventId 	iCurrentEventId;
    
    /**
     * Currently Focused Event Index to be maintained before model Update
     */    
    TInt		 	iCurrentEventIndex;
    
    /**
     * Bitmask Flag to hold the status of the currently present Events
     */    
    TEvtEventsStatusMask iEventsStatusMask;
    
    /**
     * Status Filter used to show the events based on Event Status
     */    
    CEvtMgmtUiEngine::TEvtEventStatusFilter iStatusFilter;
    };

#endif // CEVTMGMTUIMODEL_H
// End of File
