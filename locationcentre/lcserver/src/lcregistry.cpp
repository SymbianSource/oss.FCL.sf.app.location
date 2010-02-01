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
* Description:  Location Centre Server object.
*
*/


// SYSTEM INCLUDES
#include <PathInfo.h>
#include <apgcli.h>
#include <documenthandler.h>            // CDocumentHandler
#include <pathinfo.h>

// USER INCLUDES
#include "lcregistry.h"
#include "lcregistrationparser.h"
#include "lcregistryupdate.h"
#include "lcregistrationupdatenotifier.h"
#include "lcregapporder.h"
#include "lcregistryobserver.h"
#include "lcregappinfo.h"
#include "lcregappstore.h"
#include "lcdebug.h"

// CONSTANT DEFINTIONS
//Forward declaration. Function for sorted ordering of appinfo array
static TInt PositionDescOrdering( const CLcRegAppInfo& aAppInfo1, 
			  				   	  const CLcRegAppInfo& aAppInfo2 );

		  					   	  
		  					   	  
// ----- Member funtions for CLcRegistry ---------------------------------

// ---------------------------------------------------------------------------
// CLcRegistry::CLcRegistry
// ---------------------------------------------------------------------------
//
CLcRegistry::CLcRegistry( MLcRegistryObserver& aRegistryObserver )
	:CActive(EPriorityStandard),
	iRegistryObserver( aRegistryObserver )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.Initiallize all the variable here
    }
         
// ---------------------------------------------------------------------------
// CLcRegistry::~CLcRegistry
// ---------------------------------------------------------------------------
//
CLcRegistry::~CLcRegistry()
    {
	Cancel();
	    
    // C++ Destructor. Free all resources associated with this class.
	delete iLcRegUpdate;
	delete iRegAppStore;
	delete iLcRegAppOrder;
	
	//close the file server session.
	iFs.Close();
	// free filename array memory
	iFileNameArry.Reset();		
	iFileNameArry.Close();
	
	//delete all items from the appinfo array.
	iAppInfoArray.ResetAndDestroy();
	iAppInfoArray.Close();
	
	iAppInfoNewArray.ResetAndDestroy();
	iAppInfoNewArray.Close();
    }
        
// ---------------------------------------------------------------------------
// CLcRegistry* CLcRegistry::NewL
// ---------------------------------------------------------------------------
//
CLcRegistry* CLcRegistry::NewL( MLcRegistryObserver& aRegistryObserver )
    {
    CLcRegistry* self = NewLC( aRegistryObserver );
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcRegistry* CLcRegistry::NewLC
// ---------------------------------------------------------------------------
//
CLcRegistry* CLcRegistry::NewLC( MLcRegistryObserver& aRegistryObserver )
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcRegistry* self = new ( ELeave )CLcRegistry( aRegistryObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcRegistry::ConstructL
// ---------------------------------------------------------------------------
//
void CLcRegistry::ConstructL()
    {
    // connect to file server
    // and pass this session to all the class required.
	User::LeaveIfError(iFs.Connect());
    
    // creation of dynamic registration notifier object
    iLcRegUpdate	= CLcRegistryUpdate::NewL( *this, iFs);

    // creation of private info object
    iRegAppStore	= CLcRegAppStore::NewL(iFs); 

    // creation of application order settings object
    iLcRegAppOrder	= CLcRegAppOrder::NewL( *iRegAppStore );

	// reset the new applist array count here
	iAppInfoNewArray.Reset();    
	/* Now Check if any update needed to that list.
	 * To do that start all the system drive scan and collect
	 * all the registered file names
	 */     
	iRegistryHasToUpdate = EFalse;

	// registered for notification in private directory from file server
    iLcRegUpdate->StartFileNotifation();
	// Add it to Active scheduler
    CActiveScheduler::Add( this );    
    // issue the request to active scheduker
    CheckForUpdationL();
    }  

// ---------------------------------------------------------------------------
// void CLcRegistry::CheckForUpdationL()
// ---------------------------------------------------------------------------
//
void CLcRegistry::CheckForUpdationL()
    {
    // inform registry engine that registration process is under updation
    iRegistryObserver.RegistryUnderUpdation();
    // set the file count to zero
    iregFileCount = 0;

    // Scan all the drives and get all the registered file names
	ScanDrivesAndGetFileNameL();

    DEBUG3("Updated File list : %d", iFileNameArry.Count(), 0, 0 );
    
	// issue a request to active scheduler
	IssueRequest();
    }  


// ---------------------------------------------------------------------------
// void CLcRegistry::IssueRequest
// ---------------------------------------------------------------------------
//
void CLcRegistry::IssueRequest()
    {
	if(!IsActive())
		{
		SetActive();
		TRequestStatus *status = &iStatus;
		User::RequestComplete(status,KErrNone);			
		}
    }  

// ---------------------------------------------------------------------------
// TInt CLcRegistry::GetAllRegisteredAppsList
// ---------------------------------------------------------------------------
//
TInt CLcRegistry::GetAllRegisteredAppsList( RPointerArray<CLcAppInfo>& appInfoArray )
    {
    //DEBUG( "+ CLcRegistry::GetAllRegisteredAppsList");
        
    TInt error = KErrNone;
    if( iAppInfoArray.Count() <= 0 )
	    {
	    error = KErrNotFound;	
	    }
    else
	    {
	    for(TInt i = 0; i < iAppInfoArray.Count(); i++)
		    {
		    // check wheather application is present
		    // append only present application
		    if( iAppInfoArray[i]->IsAppPresent() )
			    {
				error = appInfoArray.Append( iAppInfoArray[i] );
				if ( error )
    				{
    				break;
    				}
			    }
		    }
	    }	    
	DEBUG3("Get Registered Apps : %d", appInfoArray.Count(), 0, 0 );
    //DEBUG( "- CLcRegistry::GetAllRegisteredAppsList");    
	return error;
    }  

// ---------------------------------------------------------------------------
// TInt CLcRegistry::GetApplicationInfo
// ---------------------------------------------------------------------------
//
TInt CLcRegistry::GetApplicationInfo( const TDesC& 	aAppUid,
									  CLcBasicAppInfo*&	aLcRegAppInfo )   
	{
    //DEBUG( "+ CLcRegistry::GetApplicationInfo");

	for(TInt i = 0; i< iAppInfoArray.Count(); i++)
		{
		if( !aAppUid.Compare( iAppInfoArray[i]->Id()) &&
		    iAppInfoArray[i]->IsAppPresent())		
			{
			aLcRegAppInfo = iAppInfoArray[i];
			return KErrNone;	
			}
		}
		
    //DEBUG( "- CLcRegistry::GetApplicationInfo");
    		
	return KErrNotFound;		
	}

// ---------------------------------------------------------------------------
// void CLcRegistry::HandleDynamicRegistrationL
// ---------------------------------------------------------------------------
//
void CLcRegistry::HandleDynamicRegistrationL()
    {
    DEBUG("+CLcRegistry::HandleDynamicRegistrationL");
    iRegistryHasToUpdate = ETrue;
	CheckForUpdationL();
    }  

// ---------------------------------------------------------------------------
// void CLcService::ScanDrivesAndGetFileNameL
// ---------------------------------------------------------------------------
//
void CLcRegistry::ScanDrivesAndGetFileNameL()
    {
    DEBUG( "+ CLcRegistry::ScanDrivesAndGetFileNameL");    
    
    iFileNameArry.Reset();
	//declare the directory to be scan through
	_LIT(KPriDirName, ":\\private\\10283138\\import\\" );
    TFileName srcDirName( KPriDirName );
	TDriveList dirList;
	
    if(iFs.DriveList( dirList ) != KErrNone)
	    {
	    return;	
	    }

    TBuf<KMaxFileName> rootDir;
    //creating the CDirscan object
    CDirScan* scanner = CDirScan::NewLC(iFs); 
    TChar driveLetter = '?';
    TDriveInfo info;
    TInt error;
 
    //Scan for all drives in the system 
    TInt driveListSize = dirList.Length();
    for(TInt dir(0); dir < driveListSize; dir++)
        {
        error = iFs.Drive(info, dir);
        if  ((error == KErrNone) && 
            (dirList[dir] && info.iType != EMediaNotPresent))
            {
            RFs::DriveToChar(dir, driveLetter);
            rootDir.Zero();//Clear the previous contents                  
            rootDir.Append(driveLetter);
            rootDir.Append( srcDirName );

			CDir *currentDir = NULL;
			// Go through the files only
			scanner->SetScanDataL( rootDir, KEntryAttNormal, ESortNone );
			
			TRAP( error, scanner->NextL( currentDir ));
			if( error )
				{
				continue;
				}
			TInt entryCount = currentDir->Count();           
		    while(entryCount--)
        		{
			    TBuf<KMaxFileName> lookup;
				TEntry entry=( *currentDir )[entryCount];
				//Clear the previous values
				lookup.Zero();
				//Append next one
				lookup.Append(scanner->FullPath());
				lookup.Append(entry.iName);
				//Store it in for future reference
				iFileNameArry.Append(lookup);
        		}
			delete currentDir;					
            }
        }
    CleanupStack::PopAndDestroy( scanner );
    
    DEBUG( "- CLcRegistry::ScanDrivesAndGetFileNameL");     
    }  

// ----------------------------------------------------------------------------
// CLcRegistry::RunL
// ----------------------------------------------------------------------------
//
void CLcRegistry::RunL()
    {
	TUint mmcId 	= 0;
	// This check is for if some update events come
	// during registration updation going on.
	if( iRegistryHasToUpdate )
		{
		DEBUG( "Dynamic notification recived");
		// Add all the appinfo to current list from new created list
		for( TInt i = 0; i < iAppInfoNewArray.Count(); i++ )
			{
			User::LeaveIfError( iAppInfoArray.Append( iAppInfoNewArray[i] ));
			}
		iAppInfoNewArray.Reset();		
		// end of loop set the flag as EFalse			
		iRegistryHasToUpdate = EFalse;
		DEBUG( "Notify execute");
		IssueRequest();
		}
	else
		{
		CLcRegAppInfo* regAppInfo = NULL;				
		if( iregFileCount < iFileNameArry.Count() )
			{
			// Check wheather registration file name is present in the
			// old registered list.If not then add as a new registration service
			TBool lResult = DoesApplicationExists( iFileNameArry[ iregFileCount ],
												   regAppInfo );
			if ( lResult )
				{
				// Check if the Application suffered an Upgrade. If its an upgrade
				// then its equivalent to a new installation.
				if ( IsAppUpgraded( regAppInfo->FileName()))
					{
					// Remove the element from the Old array
					TInt index = iAppInfoArray.Find( regAppInfo );
					CLcRegAppInfo* info = iAppInfoArray[index];
					iAppInfoArray.Remove( index );
					delete info;
					 
					// Parse new registration info validate and append it to list
					TRAPD( err, regAppInfo = 
									CreateAppRegInfoL(iFileNameArry[ iregFileCount ]));
					if( !err )
						{
						CleanupStack::PushL( regAppInfo );
						InsertAppRegInfoL( regAppInfo );
						CleanupStack::Pop( regAppInfo );
						}
					iregFileCount++;
					IssueRequest();				
					}
				else 
					{
					// Check and Set the Present status of the Application
					if( !regAppInfo->IsAppPresent() && 
						!GetMmcId( mmcId ) && 
						!mmcId == regAppInfo->MmcId())
						{
						// This condition is only when the Application was registered
						// prior to this and was on an MMC which was not mounted
						regAppInfo->SetFilePresenceStatus(ETrue);
						}
						
					// Remove the element from the Old array
					TInt index = iAppInfoArray.Find( regAppInfo );
					iAppInfoArray.Remove( index );
					// push the object to cleanup stack before calling 
					// InsertAppRegInfoL.
					CleanupStack::PushL( regAppInfo );
					InsertAppRegInfoL( regAppInfo );
					CleanupStack::Pop( regAppInfo );
					
					iregFileCount++;
					IssueRequest();					
					}				
				}
			else
				{
				// Parse new registration info validate and append it to list
				TRAPD( err, regAppInfo = 
								CreateAppRegInfoL( iFileNameArry[ iregFileCount ] ));
				if( !err )
					{
					CleanupStack::PushL( regAppInfo );
					InsertAppRegInfoL( regAppInfo );
					CleanupStack::Pop( regAppInfo );
					}
				iregFileCount++;
				IssueRequest();			
				}			
			}
		else
			{
			// After completing if part if there is any application present 
			// in the old array then check those application for absent or removal case.
	    	if( iAppInfoArray.Count() > 0 )
		    	{
				//Get current MMC id.
				if( GetMmcId( mmcId ) )
					{
					// Check for all Absent applications
					while( iAppInfoArray.Count())
						{
						regAppInfo = iAppInfoArray[0];
						iAppInfoArray.Remove( 0 );
						
						if( regAppInfo->MmcId() != mmcId )
							{
							regAppInfo->SetFilePresenceStatus(EFalse);
							CleanupStack::PushL( regAppInfo );
							InsertAppRegInfoL( regAppInfo );
							CleanupStack::Pop( regAppInfo );
							}
						else
							{							
							iLcRegAppOrder->RemoveArrayL( regAppInfo->Id() );
							delete regAppInfo;
							}							
						}
					}
				
				// Check for all de-registered applications
				while( iAppInfoArray.Count())
					{
					regAppInfo = iAppInfoArray[0];
					iAppInfoArray.Remove( 0 );							
					iLcRegAppOrder->RemoveArrayL( regAppInfo->Id() );
					delete regAppInfo;							
					}		
		    	}
			iAppInfoArray.ResetAndDestroy();
			FinalRegistrationUpdateL();	    	
			}		
		} 	
    }

// ---------------------------------------------------------------------------
// CLcServerShutDownTimer::RunError
// ---------------------------------------------------------------------------
//        
TInt CLcRegistry::RunError( TInt aError )
    {
    // Dont leave. Continue with the next operation
    return aError;
    }


// ---------------------------------------------------------------------------
// void CLcRegistry::FinalRegistrationUpdateL
// ---------------------------------------------------------------------------
//
void CLcRegistry::FinalRegistrationUpdateL()
    {
    // Resync all the existing applications. Their order could have been
    // changed because of the possible changes in the index ordering
    
    // Temporary array to assist copying of all content from one place
    // to another
    RPointerArray<CLcRegAppInfo> tempArray;
    while ( iAppInfoArray.Count())
        {
        CLcRegAppInfo* info = iAppInfoArray[0];
        iAppInfoArray.Remove( 0 );
        tempArray.Append( info );
        }
    
    // Insert back all temporary elements back into the Info Array
    TInt appcount = tempArray.Count();
	for( TInt i = 0; i < appcount; i++ )
		{
		User::LeaveIfError( iAppInfoArray.Append( tempArray[i] ));
		}
		
    // Now destroy all the elements back in the temporary array.
    tempArray.ResetAndDestroy();
    tempArray.Close();
    
	// Add all the appinfo to current list from new created list
	appcount = iAppInfoNewArray.Count();
	for( TInt i = 0; i < appcount; i++ )
		{
		User::LeaveIfError(iAppInfoArray.Append( iAppInfoNewArray[i] ));
		}
	iAppInfoNewArray.Reset();		
	
	DEBUG1("New List Count : %d", iAppInfoArray.Count())
			
	// Send a notification to registry engine that updation completed
	iRegistryObserver.RegistryUpdated();
    }  

// ----------------------------------------------------------------------------
// CLcRegistry::IsInMMc
// ----------------------------------------------------------------------------
//
TBool CLcRegistry::IsInMMc( TFileName aFileName )
	{
	TBool lResult = EFalse;
	TParse parse;
	parse.Set(aFileName,NULL,NULL);
	TPtrC lDrive = parse.Drive();
    TInt err,mmcDrive,fileDrive;
    err = RFs::CharToDrive( PathInfo::MemoryCardRootPath()[0], mmcDrive );    
    if (err == KErrNone)
        {
        err = RFs::CharToDrive( lDrive[0], fileDrive );    
        if( err == KErrNone )
	        {
	        if( mmcDrive == fileDrive )
		        {
				lResult = ETrue;	        	
		        }
	        }
        }
	return lResult;		
	}

// ----------------------------------------------------------------------------
// CLcRegistry::DoesApplicationExists
// ----------------------------------------------------------------------------
//
TBool CLcRegistry::DoesApplicationExists( TFileName 		aFileName,
										  CLcRegAppInfo*& 	alcRegAppInfoPtr )
	{
	// This function checks wheather registration info with same file name
	// is present in the old list. If present then return ETrue or else EFalse.
	TInt count = iAppInfoArray.Count();
	for( TInt i = 0; i < count; i++ )
		{
		alcRegAppInfoPtr = iAppInfoArray[i];
		if( alcRegAppInfoPtr )
			{
			if( aFileName.Compare( alcRegAppInfoPtr->FileName()) == 0 )	
				{
				return ETrue;	
				}			
			}
		}
	return EFalse;		
	}

// ----------------------------------------------------------------------------
// CLcRegistry::CreateAppRegInfoL
// ----------------------------------------------------------------------------
//
CLcRegAppInfo* CLcRegistry::CreateAppRegInfoL(  TFileName	aFileName )
	{
	DEBUG( "+ CLcRegistry::CreateAppRegInfoL" );

    CLcRegistrationParser* regParser = CLcRegistrationParser::NewL( iFs );
 	CleanupStack::PushL( regParser );
    
	//Parse the file content and get all the registration info.
	CLcRegAppInfo* info = regParser->ParseFileL( aFileName );
	
	CleanupStack::PopAndDestroy( regParser );
	
	CleanupStack::PushL( info );	
	
	//Check that same UUID based appinfo is already registered or not.
	if( DoesSameUuidPresent( info ))
		{
		User::Leave( KErrAlreadyExists );	
		}
		
	//validate the application or document
	if ( info->ApplicationType() == ELcNativeApplication ||
		 info->ApplicationType() == ELcDocument )
	 	{
	 	//DEBUG("+ Validate")
	 	ValidateRegisteredServiceL( *info );
	 	//DEBUG("- Validate")
	 	}

	// Set the language specific name for the service
	if( info->IsAppNamePresent())
	    {
	    info->SetAppLangSpecificNameL(iFs);    
	    }
    else
        {
        SetAbsentAppNameL( info );
        }	    
	
	
	// Set the file name for future reference.
	info->SetFileNameL( aFileName );
	
	TUint mmcId;
											
	//Get current MMC id.
	if( !GetMmcId( mmcId ) && IsInMMc( aFileName ))
		{
		info->SetMmcId(mmcId);
		}		
				
	CleanupStack::Pop( info );		
	
	DEBUG( "- CLcRegistry::CreateAppRegInfoL" );		
	return info;		
	}

// ----------------------------------------------------------------------------
// CLcRegistry::InsertAppRegInfoL
// ----------------------------------------------------------------------------
//
void CLcRegistry::InsertAppRegInfoL( CLcRegAppInfo*		aAppInfo )
	{
	TInt	index = 0;
	
	// Get the required order of the 
	// service from the central repository
	//Add the appinfo to array
	if( iLcRegAppOrder->GetRegisteredAppOrder( aAppInfo->Id(),
											   index ))
		{
		
		// this is for new regapp info addition
		// to db and get the final order
		index = iLcRegAppOrder->AppendArrayL( aAppInfo->Id());
		
	    for ( TInt i = 0; i < iAppInfoArray.Count(); i++ )
	        {
	        // Get the new order for the respective elements and set the correct values.
	        CLcRegAppInfo* info = iAppInfoArray[i];
	        TInt currentIndex = 0;
	        iLcRegAppOrder->GetRegisteredAppOrder( info->Id(),
										           currentIndex );
            // Set the new Index
            info->SetArrayIndex( currentIndex );											           
	        }
	        
		// Set the index of all elements in the new array
        for ( TInt i = 0; i < iAppInfoNewArray.Count(); i++ )
	        {
	        // Get the new order for the respective elements and set the correct values.
	        CLcRegAppInfo* info = iAppInfoNewArray[i];
	        TInt currentIndex = 0;
	        iLcRegAppOrder->GetRegisteredAppOrder( info->Id(),
										           currentIndex );
            // Set the new Index
            info->SetArrayIndex( currentIndex );											           
	        }
		}
    
    // Set the array index
    aAppInfo->SetArrayIndex( index );
							
	User::LeaveIfError( 
				iAppInfoNewArray.InsertInOrder( aAppInfo,
												PositionDescOrdering ));		
	}

// ----------------------------------------------------------------------------
// CLcRegistry::DoesSameUuidPresent
// ----------------------------------------------------------------------------
//
TBool CLcRegistry::DoesSameUuidPresent( CLcRegAppInfo* aLcRegAppInfo )
	{
    // Check in the Old array	
	for( TInt i = 0; i < iAppInfoArray.Count(); i++ )
		{
		CLcRegAppInfo* lcRegAppInfoPtr = iAppInfoArray[i];
		if( lcRegAppInfoPtr )
			{
			if( !aLcRegAppInfo->Id().Compare( lcRegAppInfoPtr->Id()))	
				{
				return ETrue;
				}			
			}
		}

    // Now check in the new array
	for( TInt i = 0; i < iAppInfoNewArray.Count(); i++ )
		{
		CLcRegAppInfo* lcRegAppInfoPtr = iAppInfoNewArray[i];
		if( lcRegAppInfoPtr )
			{
			if( !aLcRegAppInfo->Id().Compare( lcRegAppInfoPtr->Id()))	
				{
				return ETrue;
				}			
			}
		}
	
	// No match found	
	return EFalse;		
	}

// ----------------------------------------------------------------------------
// CLcRegistry::DoCancel
// ----------------------------------------------------------------------------
//
void CLcRegistry::DoCancel()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// void CLcService::ValidateRegisteredAppL
// ---------------------------------------------------------------------------
//
void CLcRegistry::ValidateRegisteredServiceL( CLcRegAppInfo& aApplicationInfo )
    {
    DEBUG( "+ CLcRegistry::ValidateRegisteredServiceL" )
	
	RApaLsSession lsSession;
	User::LeaveIfError( lsSession.Connect() );
	CleanupClosePushL( lsSession );
    
	//Check the application type.If Application then validate it
	if( aApplicationInfo.ApplicationType() == ELcNativeApplication )
		{
		DEBUG("+ App Validation")
	    TLex lexer(aApplicationInfo.ApplicationData());
	    TUint32 lAppSid = 0;
	    TUint limit = 0xFFFFFFFF; 
	    User::LeaveIfError( lexer.BoundedVal(lAppSid, EHex, limit) );
	    TApaAppInfo appInfo;
		User::LeaveIfError( lsSession.GetAppInfo( appInfo, TUid::Uid( lAppSid )));
		DEBUG("- App Validation")
		}
	else if( aApplicationInfo.ApplicationType() == ELcDocument )
		{		
		DEBUG("+ Document Validation")
		//Validate document
		CDocumentHandler* documentHandler = CDocumentHandler::NewL();						
		CleanupStack::PushL( documentHandler );
		// empty datatype for automatic recognization
		TUint8* contentTypeString = NULL;
		FindContentTypeFromFileL( lsSession,
								  contentTypeString,
								  &aApplicationInfo );
		User::LeaveIfNull( contentTypeString );
										  
		CleanupStack::PushL( contentTypeString );								 	
		documentHandler->CanOpenL( TDataType( TPtrC8( contentTypeString )));
		CleanupStack::PopAndDestroy( 2, documentHandler );			
		DEBUG("- Document Validation")
		}
	
	// Now set the system characteristics for application / document / weburl
	SetAppSysCharacteristicsL( lsSession, aApplicationInfo );		
	
	CleanupStack::PopAndDestroy(); //lsSession
	
	DEBUG( "- CLcRegistry::ValidateRegisteredServiceL" )      
    }  


// ---------------------------------------------------------------------------
// void CLcRegistry::FindContentTypeFromFileL
// ---------------------------------------------------------------------------
//
void CLcRegistry::FindContentTypeFromFileL( RApaLsSession apaSession,
										    TUint8*& aContentTypeString,
										    CLcRegAppInfo* aApplicationInfo )
	{
    TDataRecognitionResult dataType;
    TInt ret;

	// Create a buffer to hold data from the file
	TInt bufferSize = 0;
    TInt seekPosition = 0;
	apaSession.GetMaxDataBufSize(bufferSize);
	HBufC8* buffer = HBufC8::NewLC(bufferSize);
	TPtr8 buf = buffer->Des();

	RFile file;
    HBufC* fileName = HBufC::NewLC( KMaxPath );
    fileName->Des().Copy(aApplicationInfo->ApplicationData());
	
    ret = file.Open( iFs,
    				 *fileName, 
		  			 EFileShareAny | EFileRead );
	if( !ret )
		{
		// Find current file pointer position
	    file.Seek(ESeekStart, seekPosition);
		// Read from file
		file.Read(buf);
		// return file pointer to original position
	    file.Seek(ESeekStart, seekPosition);
	    // Ask the application architecture to find the file type
	    ret = apaSession.RecognizeData(*fileName, buf, dataType);

	    if (ret == KErrNone &&
	        (dataType.iConfidence == CApaDataRecognizerType::ECertain) ||
	        (dataType.iConfidence == CApaDataRecognizerType::EProbable))
	        {
	        // If the file type was found, try to match it to a known file type
	        TPtrC8 mimeTypePtr = dataType.iDataType.Des8();
	        TInt len = mimeTypePtr.Length() + 1;
	        aContentTypeString = new(ELeave) TUint8 [len];

	        TPtr8 contentTypeStringPtr(aContentTypeString, len);
	        contentTypeStringPtr.Copy(mimeTypePtr);
	        contentTypeStringPtr.ZeroTerminate();
	        }
		file.Close();	        
		}
	CleanupStack::PopAndDestroy(fileName); //fileName		
	CleanupStack::PopAndDestroy(buffer); //buffer			
	}

// -----------------------------------------------------------------------------
// CLcRegistry::MmcIdL
// Get the id of the MMC card.
// -----------------------------------------------------------------------------
//
TInt CLcRegistry::GetMmcId( TUint& aUid )
    {
    TDriveInfo info;
    info.iType = EMediaNotPresent;
    TInt mmcDrive;
    TInt err;
    TChar cha = '?';
    err = RFs::CharToDrive( PathInfo::MemoryCardRootPath()[0], mmcDrive );    
    if (err == KErrNone)
        {
        err = iFs.Drive( info, mmcDrive );
        }
    if( err != KErrNone ||
        info.iType == EMediaNotPresent )
        {
        // MMC locked or not installed        
        err = KErrNotFound;
        }
    else
        {
        TVolumeInfo volumeInfo;
        err = iFs.Volume( volumeInfo, mmcDrive );
        if( err == KErrNone )
            {
            aUid = volumeInfo.iUniqueID;
            }
        }       
    return err;
    }


// ----------------------------------------------------------------------------
// CLcRegistry::SetAppSysCharacteristicsL
// ----------------------------------------------------------------------------
//
void CLcRegistry::SetAppSysCharacteristicsL( RApaLsSession&		alsSession,
											 CLcRegAppInfo&		aRegAppInfo )
	{
	TFileName fileName;
	if( aRegAppInfo.ApplicationType() == ELcNativeApplication )
		{
		// for application we should check
		// where the binary is present
	    TLex lexer(aRegAppInfo.ApplicationData());
	    TUint32 lAppSid = 0;
	    TUint limit = 0xFFFFFFFF; 
	    User::LeaveIfError( lexer.BoundedVal(lAppSid, EHex, limit) );
	    TApaAppInfo appInfo;
		User::LeaveIfError( alsSession.GetAppInfo( appInfo, TUid::Uid( lAppSid )));
		// check if the dll file is in ROM or not
		fileName.Copy( appInfo.iFullName );
		}
	else if( aRegAppInfo.ApplicationType() == ELcDocument )
		{
		// for document we should check for the document file name
		fileName.Copy( aRegAppInfo.ApplicationData() );
		}
	else if( aRegAppInfo.ApplicationType() == ELcWebUrl )
		{
		// for web url it will be registration filename
		fileName.Copy( iFileNameArry[ iregFileCount ] );
		}
		
	if( fileName.Length() > 0 )
		{
		// parse the file name and check the drive letter
		TParse parse;
		parse.Set( fileName, NULL, NULL );
		TPtrC drive = parse.Drive();
		if( drive[0] ==  PathInfo::RomRootPath()[0] )
			{
			aRegAppInfo.SetSystemCharacteristics( CLcLocationAppInfo::ESysCharRomBased);
			}
		else
			{
			aRegAppInfo.SetSystemCharacteristics( CLcLocationAppInfo::ESysCharNone);	
			}
		}
	}

// -----------------------------------------------------------------------------
// CLcRegistry::IsAppUpgraded
// -----------------------------------------------------------------------------
//	
TBool CLcRegistry::IsAppUpgraded( TFileName aFileName )
	{
	DEBUG( "+ CLcRegistry::CheckforAppUpgrade(");
	
	// If filename is already there in the old list and presence
	// flag also true then check the time stamp between current file and old file
	TEntry oldEntry,newEntry;
	TFileName newFileName = iFileNameArry[ iregFileCount ];
	iFs.Entry( aFileName, oldEntry );
	iFs.Entry( newFileName, newEntry );
	
	if( oldEntry.iModified == newEntry.iModified ) 	
		{
		return EFalse;
		}
	else
		{
		return ETrue;	
		}		
	}

// -----------------------------------------------------------------------------
// CLcRegistry::SetAbsentAppNameL
// -----------------------------------------------------------------------------
//
void CLcRegistry::SetAbsentAppNameL( CLcRegAppInfo*    aAppInfo )
    {
	RApaLsSession lsSession;
	User::LeaveIfError( lsSession.Connect() );
	CleanupClosePushL( lsSession );
	//Check the application type.If Application then validate it
	if( aAppInfo->ApplicationType() == ELcNativeApplication )
		{
	    TLex lexer(aAppInfo->ApplicationData());
	    TUint32 lAppSid = 0;
	    TUint limit = 0xFFFFFFFF; 
	    User::LeaveIfError( lexer.BoundedVal(lAppSid, EHex, limit) );
	    TApaAppInfo appNameInfo;
		User::LeaveIfError( lsSession.GetAppInfo( appNameInfo, TUid::Uid( lAppSid )));
		aAppInfo->SetNameL(appNameInfo.iCaption);
		}
	else
		{
		aAppInfo->SetNameL( aAppInfo->ApplicationData() );	
		}
    CleanupStack::PopAndDestroy(); //lsSession
    }  		


// ---------------------------------------------------------------------------
// static TInt PositionDescOrdering
// Ordering function for inserting the elements into the applist array.
// The ordering is done first based on the previous index. 
//
// @param CLcRegAppInfo& First appinfo Element
// @param CLcRegAppInfo& Second appinfo Element
// @return 1. zero, if the two objects are equal.
//
//		   2. a negative value, if the first object is less than the second.
//
// 		   3. a positive value, if the first object is greater than the second.
// 
// ---------------------------------------------------------------------------	
TInt PositionDescOrdering( const CLcRegAppInfo& aAppInfo1, 
		  				   const CLcRegAppInfo& aAppInfo2 )
    {
    CLcRegAppInfo* appInfo1 = const_cast< CLcRegAppInfo*>(&aAppInfo1);
    CLcRegAppInfo* appInfo2 = const_cast< CLcRegAppInfo*>(&aAppInfo2);
    return ( appInfo1->Index() - appInfo2->Index());
	}
//End of file

