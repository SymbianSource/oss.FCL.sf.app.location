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
* Description:  Location Centre API interface.
*
*/


// SYSTEM INCLUDES

// USER INCLUDES
#include "lcservice.h"
#include "lcserviceimpl.h"
#include "lclocationappfilter.h"
#include "lcappexitobserver.h"
#include "lcnotification.h"

// ---------- Member funtions for CLcService::CLcLaunchParam -----------------

// ---------------------------------------------------------------------------
// CLcService::CLcLaunchParam::CLcLaunchParam
// ---------------------------------------------------------------------------
//
CLcService::CLcLaunchParam::CLcLaunchParam( 
					CLcLocationAppInfo::TLcLaunchMode aLaunchMode)
	:iLaunchMode( aLaunchMode )
	{
	}

// ---------------------------------------------------------------------------
// CLcService::CLcLaunchParam::~CLcLaunchParam
// ---------------------------------------------------------------------------
//	
CLcService::CLcLaunchParam::~CLcLaunchParam()
	{
	delete iId;
	}

// ---------------------------------------------------------------------------
// CLcService::CLcLaunchParam::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CLcService::CLcLaunchParam* CLcService::CLcLaunchParam::NewL( 
							const TDesC&     						aAppId,
								  CLcLocationAppInfo::TLcLaunchMode aLaunchMode )
	{
	CLcService::CLcLaunchParam* self = NewLC( aAppId, aLaunchMode );
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// CLcService::CLcLaunchParam::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CLcService::CLcLaunchParam* CLcService::CLcLaunchParam::NewLC( 
						const TDesC&     					    aAppId,
						   	  CLcLocationAppInfo::TLcLaunchMode aLaunchMode )
	{
	CLcService::CLcLaunchParam* self = new ( ELeave )CLcService::CLcLaunchParam( aLaunchMode );
	CleanupStack::PushL( self );
	self->ConstructL( aAppId );
	return self;
	}

// ---------------------------------------------------------------------------
// TPtrC  CLcService::CLcLaunchParam::Id
// ---------------------------------------------------------------------------
//
TPtrC  CLcService::CLcLaunchParam::Id() const
	{
    // Check whether the application id exists, incase it doesnt exist then
    // return a NULL string.
    if( !iId )
        {
        return TPtrC();
        }
    // Return the Application Id.
    return *iId;	
	}

// ---------------------------------------------------------------------------
// CLcLocationAppInfo::TLcLaunchMode  CLcLaunchParam::LaunchMode
// ---------------------------------------------------------------------------
//	
CLcLocationAppInfo::TLcLaunchMode  CLcService::CLcLaunchParam::LaunchMode() const
	{
	return iLaunchMode;
	}
	
// ---------------------------------------------------------------------------
// void CLcService::CLcLaunchParam::ConstructL
// ---------------------------------------------------------------------------
//	
void CLcService::CLcLaunchParam::ConstructL( const TDesC& 	aAppId )
	{
	iId = aAppId.AllocL();
	}
	
// ---------- Member funtions for CLcService ---------------------------------

// ---------------------------------------------------------------------------
// CLcService::CLcService
// ---------------------------------------------------------------------------
//
CLcService::CLcService()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
         
// ---------------------------------------------------------------------------
// CLcService::~CLcService
// ---------------------------------------------------------------------------
//
CLcService::~CLcService()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    delete iImplementation;
    }
        
// ---------------------------------------------------------------------------
// CLcService* CLcService::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CLcService* CLcService::NewL()
    {
    CLcService* self = NewLC();
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcService* CLcService::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CLcService* CLcService::NewLC()
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcService* self = 
                new ( ELeave )CLcService();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcService::ConstructL
// ---------------------------------------------------------------------------
//
void CLcService::ConstructL()
    {
    // Second phase of the two phase constructor.
    
    // Create the Location Centre API implementation
    iImplementation = CLcServiceImpl::NewL();
    }

// ---------------------------------------------------------------------------
// void CLcService::LaunchLocationCentreL
// ---------------------------------------------------------------------------
//
EXPORT_C void CLcService::LaunchLocationCentreL(
            CLcLocationAppInfo::TLcLaunchMode   aLaunchMode,                        
            MLcAppExitObserver*                 aChainedAppExitObserver )
    {
    // The Filter construction without any arguments will act as no filtering
    // applied on the list of Location based Applications and Contents/Services.
    TLcLocationAppFilter filter; 
        
    iImplementation->LaunchLocationCentreL( filter,
                                            aLaunchMode,
                                            aChainedAppExitObserver );
    }
                    
// ---------------------------------------------------------------------------
// void CLcService::LaunchLocationCentreL
// ---------------------------------------------------------------------------
//
EXPORT_C void CLcService::LaunchLocationCentreL( 
              const TLcLocationAppFilter&             aLocationAppFilter,
                    CLcLocationAppInfo::TLcLaunchMode aLaunchMode,                        
                    MLcAppExitObserver*               aChainedAppExitObserver )
    {
    iImplementation->LaunchLocationCentreL( aLocationAppFilter,
                                            aLaunchMode,
                                            aChainedAppExitObserver );
    }
    
// ---------------------------------------------------------------------------
// void CLcService::LaunchLocationCentreL
// ---------------------------------------------------------------------------
//
EXPORT_C void CLcService::LaunchLocationCentreL(
                  const RPointerArray<CLcLaunchParam>& aIncludeAppArray,
                  MLcAppExitObserver*           aChainedAppExitObserver )
    {
    iImplementation->LaunchLocationCentreL( aIncludeAppArray,
                                            aChainedAppExitObserver );
    }
    
// ---------------------------------------------------------------------------
// void CLcService::LaunchLocationCentreL
// ---------------------------------------------------------------------------
//
EXPORT_C void CLcService::LaunchLocationCentreL( 
                    const RArray<TPtrC>&              aExcludeAppArray,
                    CLcLocationAppInfo::TLcLaunchMode aLaunchMode,
                    MLcAppExitObserver*               aChainedAppExitObserver )
    {
    iImplementation->LaunchLocationCentreL( aExcludeAppArray,
                                            aLaunchMode,
                                            aChainedAppExitObserver );    
    }

// ---------------------------------------------------------------------------
// void CLcService::SelectLocationApplicationL
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CLcService::SelectLocationApplicationL()
    {
    // The Filter construction without any arguments will act as no filtering
    // applied on the list of Location based Applications and Contents/Services.
    // coverity[var_decl : FALSE]
    TLcLocationAppFilter filter;    
    // coverity[uninit_use_in_call : FALSE]
    return iImplementation->SelectLocationApplicationL( filter );
    }

// ---------------------------------------------------------------------------
// void CLcService::SelectLocationApplicationL
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CLcService::SelectLocationApplicationL(
                        const TLcLocationAppFilter& aLocationAppFilter )
    {
    return iImplementation->SelectLocationApplicationL( aLocationAppFilter );      
    }
    
// ---------------------------------------------------------------------------
// void CLcService::SelectLocationApplicationL
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CLcService::SelectLocationApplicationL(
                            const RArray<TPtrC>&    aAppArray,
                                  TBool             aIncludeFlag )
    {
    return iImplementation->SelectLocationApplicationL( aAppArray,
                                                        aIncludeFlag );        
    }

// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray* CLcService::GetLocationApplicationsL
// ---------------------------------------------------------------------------
//    
EXPORT_C CLcLocationAppInfoArray* CLcService::GetLocationApplicationsL()
    {
    // The Get Location Applications call is implemented as a Get Location
    // based Applications function which doesn't have any filtering. So, we
    // need to create an empty filter and pass it to this function.
    
    // The Filter construction without any arguments will act as no filtering
    // applied on the list of Location based Applications and Contents/Services.
    TLcLocationAppFilter filter;
    return iImplementation->GetLocationApplicationsL( filter );
    }

// ---------------------------------------------------------------------------
// void CLcService::GetLocationApplications
// ---------------------------------------------------------------------------
//    
EXPORT_C void CLcService::GetLocationApplications(
            TRequestStatus&             aStatus,
            CLcLocationAppInfoArray*&   aAppInfoArray )
    {
    TLcLocationAppFilter filter;    
    iImplementation->GetLocationApplications( aStatus,
    										  filter,
    										  aAppInfoArray );
    }
    
// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray* CLcService::GetLocationApplicationsL
// ---------------------------------------------------------------------------
//    
EXPORT_C CLcLocationAppInfoArray* CLcService::GetLocationApplicationsL(
            const TLcLocationAppFilter&     aLocationAppFilter )
    {
    return iImplementation->GetLocationApplicationsL( aLocationAppFilter );
    }
    
// ---------------------------------------------------------------------------
// void CLcService::GetLocationApplicationsL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CLcService::GetLocationApplications(
                            TRequestStatus&             aStatus,
                      const TLcLocationAppFilter&       aLocationAppFilter,
                            CLcLocationAppInfoArray*&   aAppInfoArray )
    {
    iImplementation->GetLocationApplications( aStatus,
                                              aLocationAppFilter,
                                              aAppInfoArray );
    }

// ---------------------------------------------------------------------------
// void CLcService::CancelGetLocationApplications
// ---------------------------------------------------------------------------
//     
EXPORT_C void CLcService::CancelGetLocationApplications()
    {
    iImplementation->CancelGetLocationApplications(); 
    }

// ---------------------------------------------------------------------------
// void CLcService::LaunchLocationApplicationL
// ---------------------------------------------------------------------------
// 
EXPORT_C void CLcService::LaunchLocationApplicationL( 
                const TDesC&                      aAppIdentifier,
                CLcLocationAppInfo::TLcLaunchMode aLaunchMode,
                MLcAppExitObserver*               aChainedAppExitObserver )
    {
    iImplementation->LaunchLocationApplicationL( aAppIdentifier,
                                                 aLaunchMode,
                                                 aChainedAppExitObserver );
    }

// ---------------------------------------------------------------------------
// void CLcService::SetObserverL
// ---------------------------------------------------------------------------
// 
EXPORT_C void CLcService::SetObserverL( MLcNotification&    aObserver )
    {
    iImplementation->SetObserverL( aObserver );
    }

// ---------------------------------------------------------------------------
// TInt CLcService::RemoveObserver
// ---------------------------------------------------------------------------
//                                         
EXPORT_C TInt CLcService::RemoveObserver()
    {
    return iImplementation->RemoveObserver();
    }
                                                           
// End of File

