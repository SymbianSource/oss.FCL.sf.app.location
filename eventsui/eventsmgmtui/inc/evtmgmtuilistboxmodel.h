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
* Description:  List box model class for Events Management UI.
*
*/

#ifndef C_EVTMGMTUILISTBOXMODEL_H
#define C_EVTMGMTUILISTBOXMODEL_H

// INCLUDES
#include <AknUtils.h>

// FORWARD DECLARATIONS
class CEvtMgmtUiFilterModel;

// CLASS DECLARATION

/**
*  CEvtMgmtUiListBoxModel bypasses the default filtering algorithm provided by 
*  CAknFilteredTextListBoxModel. The filtering responsibility is forwarded to 
*  the CCoeControl instance owning the list box. That instance must update the
*  model when the filter has been modified.
*/
class CEvtMgmtUiListBoxModel : public CAknFilteredTextListBoxModel
    {
    public:
	    /**
	     * Constructs a new instance of List box Model.
	     *
	     * @return The new instance of List box Model.
	     * @leave System wide error code if the object creation fails.         
	     */
	    static CEvtMgmtUiListBoxModel* NewL( );
    
		/**
		 * Constructs a new instance of List box Model.
		 * Leaves the created instance on the cleanup stack.
		 *
		 * @return The new instance of List box Model.
		 * @leave System wide error code if the object creation fails.         
		 */
		static CEvtMgmtUiListBoxModel* NewLC( );  
		
    public: // from CAknFilteredTextListBoxModel

        /**
        * Gets an array of strings to be displayed in the listbox for 
        * incremental matching of typed characters. This array always contains 
        * the total number of items rather than a subset filtered out from the 
        * model.
        *
        * @return A text array for matching.
        */
        const MDesCArray* MatchableTextArray() const;

        /**
        * Gets the number of text items in this list box model. This number is 
        * always the total number of items in the model rather than a subset 
        * filtered out from the model
        *
        * @return The number of text items in this list box model
        */
        TInt NumberOfItems() const;

        /**
        * Gets the text to be displayed in the list box control for a specific 
        * item.
        *
        * @param aItemIndex Index of an item
        * @return Text string to be displayed
        */
        TPtrC ItemText(TInt aItemIndex) const;

        /**
        * Set the Zero Row String.
        *
        * @param aZeroIndexString Zero Index String
        */
        void SetItemTextArrayToFilterModel();
        
        /**
         * Set the FindBox to Filter model.
         */
        void SetFindToFilterModel(CAknSearchField* aSearchField);
    public:
        /**
         * Inherited from MDesCArray
         */
        TInt    MdcaCount() const;
        
        /**
         * Inherited from MDesCArray
         */    
        TPtrC16 MdcaPoint( TInt aIndex ) const;

        /**
         * Destructor
         */    
        ~CEvtMgmtUiListBoxModel();

    private:
        /**
         * 2nd phase constructor
         */ 
        void ConstructL();

        /**
         * Default constructor
         */ 
        CEvtMgmtUiListBoxModel();
        
    private:

        /**
         * FilterModel Instance
		 * Own:
         */    
        CEvtMgmtUiFilterModel* iEvtMgmtUiFilterModel;

    };

#endif // C_EVTMGMTUILISTBOXMODEL_H

// End of File
