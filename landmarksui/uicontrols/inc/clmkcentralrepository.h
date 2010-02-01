/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    This class listens to the changes in the Central Repository
*                for specific keys and notifies the observer.
*
*/







#ifndef __CLMKCENTRALREPOSITORY_H__
#define __CLMKCENTRALREPOSITORY_H__

//  System Includes
#include <e32base.h>

// User Includes
#include "mlmkcentralrepositoryobserver.h"

// Forward Declarations
class CRepository;

// Constant Declarations

// Class Definition
/**
 * This class listens to the change in value of specified key, in the central repository.
 * When there is any change, it notifies to the observer.
 * This class can also be used, without notification, just to get key values from
 * central repository.
 * To make it start listening on a specific key value change, create an instance of
 * this class using the argumented NewL/NewLC functions.
 * A request can only be cancelled, when the object of this class
 * is deleted.
 */
NONSHARABLE_CLASS( CLmkCentralRepository ) : public CActive
	{
	public:
		/**
         * Creates new central repository listener
         *
		 * @return CLmkCentralRepository*	Pointer to self
         */
        static CLmkCentralRepository* NewL();


		/**
         * Creates new central repository listener
         * Leaves the object on the Clean up stack
   		 *
		 * @return CLmkCentralRepository*	Pointer to self
         */
        static CLmkCentralRepository* NewLC();

        /**
         * Creates new central repository listener
         *
         * @param aKey                              Key, on which change is being listened.
         * @param  aObserver                        Observer, which will be called when
         *                                          there is any change in the specified key
         *                                          value.
		 * @return CLmkCentralRepository*	Pointer to self
         */
        static CLmkCentralRepository* NewL( TUint32 aKey ,
        					MLmkCentralRepositoryObserver*	aObserver);


		/**
         * Creates new central repository listener
         * Leaves the object on the Clean up stack
         *
         * @param aKey                              Key, on which change is being listened.
         * @param  aObserver                        Observer, which will be called when
         *                                          there is any change in the specified key
         *                                          value.
		 * @return CLmkCentralRepository*	Pointer to self
         */
        static CLmkCentralRepository* NewLC( TUint32 aKey ,
        					MLmkCentralRepositoryObserver*	aObserver);


		/**
		 * Destructor
		 */
		~CLmkCentralRepository ();

	public:

		/**
		 * Start notification from the Central repository server for any changes in the
		 * in the specified key value.
		 *
		 * @panic             Panics with code KLmkPanicNullMember, when no observer
		 *                    is set.
		 *                    Panics with code KLmkKeyNotSet, when no key is set.
		 *
		 * @return TInt       KErrNone if successful, KErrAlreadyExists if there is already
		 *                    a notification from this CRepository on this setting, or an
		 *                    error from IPC or server side resource allocation failure.
		 */
		TInt StartNotification();

		/**
		* Gets the value for the specified key, from central repository.
		* @param        aKey    Key for which, value is being fetched.
		* @param[out]aValue  Fetched value from central repository.
		*/
		TInt GetKeyValue(TUint32 aKey, TInt& aValue);

    protected:
        /**
         * Inherited from CActive
         */
        void RunL();

        /**
         * Inherited from CActive
         */
        void DoCancel();

	private:
		/**
 		 * Constructor
		 */
		CLmkCentralRepository( TUint32 aKey ,
        					   MLmkCentralRepositoryObserver*	aObserver);

		/**
		 * Second Phase Constructor
		 */
		void ConstructL();
			/**
		 * Second Phase Constructor
		 */
		void Construct2L();


	private:

		/**
	     * Key, which is being observed for any change, in its value.
	     */
		const TUint32 iKey;
	    /**
	     * Reference to the Settings observer
	     */
		MLmkCentralRepositoryObserver*     iObserver;

		/**
		 * Reference to the Central repository object
		 *
		 * Owns
		 */
		CRepository*                    iRepository;
	};


#endif //__CLMKCENTRALREPOSITORY_H__