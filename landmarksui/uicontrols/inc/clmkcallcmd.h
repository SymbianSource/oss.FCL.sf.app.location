/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Description : This file contains class which implements methods for
*                interfacing with Call UI component for calling phonenumber
*
*/







#ifndef CLMKCALLCMD_H
#define CLMKCALLCMD_H

//  INCLUDES
#include <AiwDialDataTypes.h>
#include <AiwCommon.h>
#include <MPhCltEmergencyCallObserver.h>

class CAiwServiceHandler;
class CAiwGenericParamList;
class CPhCltEmergencyCall;
// CLASS DECLARATION
/**
* CLmkCallCmd class
* Implements the methods which interact with Telephony subsystem
* (CallUI) through AIW FW.
*/
class CLmkCallCmd:public CBase, public MPhCltEmergencyCallObserver
	{
	public:  // Constructors and destructor
		/**
		* Two-phased constructor.
		* @return
		*/
		IMPORT_C static CLmkCallCmd* NewL( );

		/**
		* Destructor.
		*/
		virtual ~CLmkCallCmd();

	private:  // Constructors and destructor
		/**
		* Default Constructor
		*/
		CLmkCallCmd();

		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		void ConstructL( );

	public:
		/**
		* Attaches Application's menu pane and the interest resource to AIW FW service
		*
		* @param aMenuResourceId application menu which includes service interest resource
		* @param aInterestResourceId application resource containing service interest
        */
		void AttachMenuPaneL(TInt aMenuResourceId, TInt aInterestResourceId);

		/**
		* Sets up AIW menu handler service for application (Calling Phone number )
		* @param aMenuPane application menu pane
		* @param aMenuResourceId application menu which includes service interest resource
		* @param aBaseMenuCmdId base commandId which AIW FW uses for attaching application service interst
        */

		void InitializeMenuPaneL( CEikMenuPane& aMenuPane,
								  TInt aMenuResourceId,
								  TInt aBaseMenuCmdId );
		/**
		* Configures clinet's service criteria (array of service interest) to AIW FW
		* @param aInterestId application's service interest
		*/
		void AttachInterestL ( RCriteriaArray& aInterestId );

		/**
		* Overloaded version
		*/
		void AttachInterestL ( TInt aInterestId );

		/**
		* Execute AIW menu service for calling phone number
		* @param aCommandId application menu command
		* @param aPhoneNum  phone number to which call is to be made
		*/
		void ExecuteAIWCallCmdL( TInt aCommandId, TDesC& aPhoneNum );

		/**
        * This function makes a call to AIW service handler to get
        * the mapped AIW menu id for the , menu id clicked.
        */
        TInt ServiceCmdByMenuCmd( TInt aMenuCmd );

	private: //new functions
		TBool HandleEmergencyCallL( TDesC& aPhoneNum );
		void  HandleNormalCallL( TInt aCommandId, TDesC& aPhoneNum );
		//MPhCltEmergencyCallObserver
		void  HandleEmergencyDialL( const TInt aStatus );

	private: // Data members
		// AIW Call Service Handler instance.
		CAiwServiceHandler* iAiwCallServiceHandler;
		//Emergency call handler
		CPhCltEmergencyCall* iEmergencyCall;
	};
#endif
//  End of File
