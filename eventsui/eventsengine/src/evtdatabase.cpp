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
* Description:  Class that directly interacts with Sqlite database.
*
*/


// System Includes
#include <s32strm.h>

// User Includes
#include "evtdatabase.h"
#include "evtdbnotifier.h"
#include "evtaction.h"
#include "evtconsts.h"
#include "evtdebug.h"

// Constant
const TInt KBufferLength = 512;
const TInt KMaxIntLength = 22;

// ================ Member funtions for CEvtDatabase class ==================

// ---------------------------------------------------------------------------
// CEvtDatabase::CEvtDatabase
// ---------------------------------------------------------------------------
//
CEvtDatabase::CEvtDatabase( )
    {
    }
    
// ---------------------------------------------------------------------------
// CEvtDatabase::~CEvtDatabase
// ---------------------------------------------------------------------------
//
CEvtDatabase::~CEvtDatabase()
    {
    // Close Database handle
    CloseDatabase();
    
    delete iDbNotifier;
    } 
    
// ---------------------------------------------------------------------------
// CEvtDatabase::NewL
// ---------------------------------------------------------------------------
//
CEvtDatabase* CEvtDatabase::NewL( )
    {
	CEvtDatabase* self = NewLC( );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtDatabase::NewLC
// ---------------------------------------------------------------------------
//
CEvtDatabase* CEvtDatabase::NewLC( )
    {
	CEvtDatabase* self = new ( ELeave )CEvtDatabase( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtDatabase::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::ConstructL()
    {
	EVTUIDEBUG( "+ CEvtDatabase::OpenDatabaseL()" );
	EVTUIDEBUG( "Calling OpenDatabaseL()" );
    // Open the handle to Database
    OpenDatabaseL( );
    
	EVTUIDEBUG( "Calling CEvtDbNotifier::NewL()" );
    // Notifier is instantiated to notify and listen to database change.
    iDbNotifier = CEvtDbNotifier::NewL( *this );
	EVTUIDEBUG( "+ CEvtDatabase::OpenDatabaseL()" );
    } 

// ---------------------------------------------------------------------------
// CEvtDatabase::CreateOpenDatabaseL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::OpenDatabaseL( )
	{
	EVTUIDEBUG( "+ CEvtDatabase::OpenDatabaseL()" );
  	
  	// First try to open the database.
	TInt err = iDb.Open( KEventsDbSecureFName() );
	
	// If the database is not present, then a database and schema has to be
  	// created
	if( KErrNotFound == err )
		{
		CreateOpenDatabaseL( );		// Create Database
		CreateSchemaL( );			// Create Schema
		}
	else if( KErrNone != err )
		User::LeaveIfError( err );	// Leave if Open fails for any other reason
	EVTUIDEBUG( "- CEvtDatabase::OpenDatabaseL()" );
	}

// ---------------------------------------------------------------------------
// CEvtDatabase::CloseDatabaseL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::CloseDatabase()
	{
	EVTUIDEBUG( "+ CEvtDatabase::CloseDatabase()" );
	// Close db handle
	iDb.Close();
	EVTUIDEBUG( "- CEvtDatabase::CloseDatabase()" );
	}

// ---------------------------------------------------------------------------
// CEvtDatabase::CreateDatabaseL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::CreateOpenDatabaseL( )
	{
	EVTUIDEBUG( "+ CEvtDatabase::CreateOpenDatabaseL()" );

	// To create a Secure Database, we first need to create the Security policy.
	// Security policy once defined for a database, can not be changed.
	TSecurityPolicy defaultPolicy( TSecurityPolicy::EAlwaysPass );
	RSqlSecurityPolicy securityPolicy;
	TInt err = securityPolicy.Create( defaultPolicy );
	User::LeaveIfError( err );

    // Define Security Policy
    const TSecurityPolicy KSchemaPolicy( ECapabilityLocation );
    const TSecurityPolicy KReadWritePolicy( ECapabilityLocation );

	// Set the security Policy for Schema and tables
	securityPolicy.SetDbPolicy( RSqlSecurityPolicy::ESchemaPolicy, 
															KSchemaPolicy );
	securityPolicy.SetDbPolicy( RSqlSecurityPolicy::EReadPolicy, 
															KReadWritePolicy );
	securityPolicy.SetDbPolicy( RSqlSecurityPolicy::EWritePolicy, 
															KReadWritePolicy );
	
	// CreateL will create and open the handle to database.
	iDb.CreateL( KEventsDbSecureFName(), securityPolicy );
	
	// Close the Security Policy
	securityPolicy.Close();
	
	EVTUIDEBUG( "- CEvtDatabase::CreateOpenDatabaseL()" );
	}

// ---------------------------------------------------------------------------
// CEvtDatabase::CreateSchemaL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::CreateSchemaL()
	{
	EVTUIDEBUG( "+ CEvtDatabase::CreateSchemaL()" );
	// Create Tables in database
	CreateTablesL();
	// Create Triggers in database
	CreateTriggerL();
	// Create Indexes in database
	CreateIndexL();
	EVTUIDEBUG( "- CEvtDatabase::CreateSchemaL()" );
	}

// ---------------------------------------------------------------------------
// CEvtDatabase::CreateTablesL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::CreateTablesL()
	{
	EVTUIDEBUG( "+ CEvtDatabase::CreateTablesL()" );
	
	// Leave if the creation of tables fail.
	User::LeaveIfError( 
			iDb.Exec( KCreateEventTable ) );
	User::LeaveIfError( 
			iDb.Exec( KCreateActionTable ) );
	EVTUIDEBUG( "- CEvtDatabase::CreateTablesL()" );
	}
	
// ---------------------------------------------------------------------------
// CEvtDatabase::CreateTriggerL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::CreateTriggerL()
	{
	EVTUIDEBUG( "+ CEvtDatabase::CreateTriggerL()" );
	// Triggers are used to maintain the referential integrity between
	// Event and Action tables.
	// This avoids Insert, Update and Delete anamolies.
	 
	// Leave if the creation of triggers fail.
	User::LeaveIfError( 
			iDb.Exec( KCreateTriggerActionEventInsert ) );
	User::LeaveIfError( 
			iDb.Exec( KCreateTriggerActionEventUpdate ) );
	User::LeaveIfError( 
			iDb.Exec( KCreateTriggerActionEventDelete ) );
				
	EVTUIDEBUG( "- CEvtDatabase::CreateTriggerL()" );
	}

// ---------------------------------------------------------------------------
// CEvtDatabase::CreateIndexL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::CreateIndexL()
	{
	EVTUIDEBUG( "+ CEvtDatabase::CreateIndexL()" );
    // Indexes are created to improve the performance of select Queries
    // Following indexes are created
    // 1. Index to "Select Event based on Status and Sort by Subject"
    // 2. Index to "Select Event based on Unique Event Id"
    // 3. Index to "Select Event based on Event Id" - (LBT Engine's Id)
    // 4. Index to "Select Action based on Unique Event Id"
	 
	// Leave if the creation of indexes fail.
	User::LeaveIfError( 
			iDb.Exec( KCreateEventStatusSubjectIndex ) );
	User::LeaveIfError( 
			iDb.Exec( KCreateEventEvtIdIndex ) );
	User::LeaveIfError( 
			iDb.Exec( KCreateActionEvtIdIndex ) );
			
	EVTUIDEBUG( "- CEvtDatabase::CreateIndexL()" );
	}

// ---------------------------------------------------------------------------
// CEvtDatabase::GetEventsL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::GetEventsL( 
                            TEvtEventStatus    aEventStatus,
                            RPointerArray<CEvtBasicEventInfo>&     aEventArray )
    {
	EVTUIDEBUG( "+ CEvtDatabase::GetEventsL() - Status" );
    RSqlStatement stmt;
    
    // Prepare the SQL statement to Get Events based on Status
	stmt.Prepare( iDb, KSelectBasicEventOnStatus );
	
	// Set the Status parameter in SQL statement
	TInt paramIndex = stmt.ParameterIndex( KStatus );
	stmt.BindInt( paramIndex,aEventStatus );
	
	// Pass the SQL statement to get events.
    GetEventsL( stmt, aEventArray );
	
	// Close statement
	stmt.Close();
	EVTUIDEBUG( "- CEvtDatabase::GetEventsL() - Status" );
    }

// ---------------------------------------------------------------------------
// CEvtDatabase::GetEventsL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::GetEventsL( 
                            RPointerArray<CEvtBasicEventInfo>&     aEventArray )
    {
	EVTUIDEBUG( "+ CEvtDatabase::GetEventsL() - without Status" );
    RSqlStatement stmt;
    
    // Prepare the SQL statement to Get Events based on Status
	stmt.Prepare( iDb, KSelectBasicEvent );
	
	// Pass the SQL statement to get events.
    GetEventsL( stmt, aEventArray );
	
	// Close statement
	stmt.Close();
	EVTUIDEBUG( "- CEvtDatabase::GetEventsL() - without Status" );
    } 
    
// ---------------------------------------------------------------------------
// CEvtDatabase::GetEventL
// ---------------------------------------------------------------------------
//
CEvtEvent* CEvtDatabase::GetEventL( TEvtEventId      aEventId )
    {
	EVTUIDEBUG( "+ CEvtDatabase::GetEventL()" );

	RSqlStatement stmt;
	CEvtEvent* evt = NULL;
	
	// Prepare the SQL statement to Get Events based on Id
	stmt.Prepare( iDb, KSelectEventFromEventId );
	
	// Set the Id parameter in SQL statement
	TInt paramIndex=stmt.ParameterIndex( KEventId );
	stmt.BindInt( paramIndex, aEventId );
	
	// Constants representing the columns in Query
	TInt columnIndexStatus=0; 
	TInt columnIndexSubject=1; 
	TInt columnIndexPlace=2; 
	TInt columnIndexRadius=3; 
	TInt columnIndexDescription=4; 
	TInt columnIndexRepeat=5; 
	TInt columnIndexActionId=6;
	TInt columnIndexAction=7;  
	TInt columnIndexActionEvtId=8; 

    // Flag to loop out after the first element is found in database
    // This is not required as we maintain unique Id for every event.
    // But to ensure that we do not allocate many events object to evt,
    // this flag is defined.
    TBool flag( ETrue );

	// Set the other attributes of the Requested Event
	while( flag && stmt.Next() == KSqlAtRow )
		{
		flag = EFalse;
        evt = CEvtEvent::NewLC();

    	// Set the Id of the Requested Event
    	evt->SetEventId(aEventId);

		TInt value = stmt.ColumnInt( columnIndexStatus );
		evt->SetEventStatus( static_cast<TEvtEventStatus>(value) );
		
		TPtrC subject=stmt.ColumnTextL( columnIndexSubject );
		evt->SetSubjectL(subject);
		
		TPtrC place=stmt.ColumnTextL( columnIndexPlace );
		evt->SetPlaceL(place);
    
		TReal realvalue = stmt.ColumnReal( columnIndexRadius );
        evt->SetRadius( realvalue );
		
		TPtrC desc=stmt.ColumnTextL( columnIndexDescription );
		evt->SetDescriptionL(desc);
        
		value = stmt.ColumnInt( columnIndexRepeat );
        evt->SetRepeat(value);
        
		TInt64 value64 = stmt.ColumnInt64( columnIndexActionId );
		if( value64 != 0 )
			{			
    		// Create action instance to set for Event
    		// ownership is not passed.
    		CEvtAction* actn = CEvtAction::NewLC();
			
			// Set the Unique Id for Action
			actn->SetId( value64 );
			
			// Set Action to an Event only if there is an Action String.
			TPtrC action = stmt.ColumnTextL( columnIndexAction );
	    	actn->SetActionL( action );
    		
    		// Set the Event Id
			TUint32 value32 = stmt.ColumnInt( columnIndexActionEvtId );
	    	actn->SetEvtId( value32 );
        
        	// Set the Action to Event
	    	evt->SetActionL( actn );
	    	
    		// Pop action after setting.
    		CleanupStack::Pop( actn );
			}
		
		// Pop the Event object
		CleanupStack::Pop( evt );
		}
	stmt.Close();
	
	if( flag )
        User::Leave( KErrNotFound );	
        
	EVTUIDEBUG( "- CEvtDatabase::GetEventL()" );
	return evt;
    } 
    
// ---------------------------------------------------------------------------
// CEvtDatabase::UpdateEvent
// ---------------------------------------------------------------------------
//
void CEvtDatabase::UpdateEventL( 
                            CEvtEvent&          aEvtEvent )
    {
	EVTUIDEBUG( "+ CEvtDatabase::UpdateEventL()" );
	
	// Update Event's Attributes.
    UpdateEventTableL( aEvtEvent );
	
	if( aEvtEvent.HasAction() )
		{
		aEvtEvent.Action().SetEvtId( aEvtEvent.EventId() );
		if( CheckIfActionPresentL( aEvtEvent.Action().Id() ) )
			UpdateActionL( aEvtEvent.Action() );
		else
			CreateActionL( aEvtEvent.Action() );
		}
	else
		{
			DeleteActionL( aEvtEvent.EventId() );
		}
		
	// Notify the database change for Modify
	iDbNotifier->DbChangedL( );
	EVTUIDEBUG( "- CEvtDatabase::UpdateEventL()" );
    } 
    
// ---------------------------------------------------------------------------
// CEvtDatabase::AddEventL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::AddEventL( CEvtEvent&     aEvtEvent )
    {
	EVTUIDEBUG( "+ CEvtDatabase::AddEventL()" );
    
	RSqlStatement stmt;
	TInt paramIndex;
	
	// We need to create the streams to insert large texts.
	RSqlParamWriteStream subjectStream;
	RSqlParamWriteStream placeStream;
	RSqlParamWriteStream descStream;
	CleanupClosePushL( subjectStream );
	CleanupClosePushL( placeStream );
	CleanupClosePushL( descStream );
	
	// Prepare the SQL statement to Get Events based on Status
	stmt.Prepare( iDb, KInsertEvent );
	
	// Set Status
	paramIndex = stmt.ParameterIndex( KStatus );
	stmt.BindInt( paramIndex, aEvtEvent.EventStatus() );
	
	// Set Subject
	paramIndex = stmt.ParameterIndex( KSubject );
	if( KNullDesC() == aEvtEvent.Subject() )
		{
	    stmt.BindNull( paramIndex );
		}
	else
		{
		subjectStream.BindText( stmt, paramIndex );
		subjectStream.WriteL( aEvtEvent.Subject() );
		subjectStream.CommitL();
		}
	
	// Set Place
	paramIndex = stmt.ParameterIndex( KPlace );
	if( KNullDesC() == aEvtEvent.Place() )
		{
	    stmt.BindNull( paramIndex );
		}
	else
		{
		placeStream.BindText( stmt, paramIndex );
		placeStream.WriteL( aEvtEvent.Place() );
		placeStream.CommitL();
		}
	
	// Set Radius
	paramIndex = stmt.ParameterIndex( KRadius );
	stmt.BindReal( paramIndex, aEvtEvent.Radius() );
	
	// Set Description
	paramIndex = stmt.ParameterIndex( KDescription );
	if( KNullDesC() == aEvtEvent.Description() )
		{
	    stmt.BindNull( paramIndex );
		}
	else
		{
		descStream.BindText( stmt, paramIndex );
		descStream.WriteL( aEvtEvent.Description() );
		descStream.CommitL();
		}
	
	// Set Repeat
	paramIndex = stmt.ParameterIndex( KRepeat );
	stmt.BindInt( paramIndex, aEvtEvent.Repeat() );
    
    // Set Event Id
	paramIndex = stmt.ParameterIndex( KEventId );
	stmt.BindInt( paramIndex, aEvtEvent.EventId() );
	
	// Execute the Remove SQL statement
	User::LeaveIfError( stmt.Exec() );
	
	//Close the streams
	CleanupStack::PopAndDestroy( &descStream );
	CleanupStack::PopAndDestroy( &placeStream );
	CleanupStack::PopAndDestroy( &subjectStream );
	    
	stmt.Close();
    
	// Insert into Action table If action is present for the event.  
	if( aEvtEvent.HasAction() )
		{
		aEvtEvent.Action().SetEvtId( aEvtEvent.EventId() );
			CreateActionL( aEvtEvent.Action() );
		}
        
	// Notify the database change for Insert
	iDbNotifier->DbChangedL( );
	EVTUIDEBUG( "- CEvtDatabase::AddEventL()" );
    }  
 
// ---------------------------------------------------------------------------
// CEvtDatabase::RemoveEventsL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::RemoveEventsL( const RArray<TEvtEventId>&     aEventArray )
    {
	EVTUIDEBUG( "+ CEvtDatabase::RemoveEventsL()" ); 
    // Removing an Event in database should be done in an order of
    // - remove Action entry and then remove Event entry
    
    // Leave if there are no array elements
    if( aEventArray.Count() == 0 )
        User::Leave( KErrArgument );
    
    RSqlStatement stmt;
    
    HBufC8* buf = HBufC8::NewLC(KBufferLength);
    
    buf->Des().Zero();
    TInt tempLength = KOr().Length()+KMultiActionId().Length()+KMaxIntLength;

    for(TInt i=0; i<aEventArray.Count(); i++ )
        {
        if(buf->Length() == 0)
            {
            buf->Des().Zero();
            buf->Des().Append(KDeleteMultiAction());
            buf->Des().Append(KMultiActionId());
            buf->Des().AppendNum(aEventArray[i]);
            }
        else
            {
            buf->Des().Append(KOr());
            buf->Des().Append(KMultiActionId());
            buf->Des().AppendNum(aEventArray[i]);
            }

        if( i == aEventArray.Count()-1 || KBufferLength - buf->Length() < tempLength )
            {
            // Prepare the SQL statement to Update Status based on Id
            stmt.Prepare( iDb, *buf );
            
            // Execute the Remove SQL statement
            User::LeaveIfError( stmt.Exec() );
            buf->Des().Zero();
            
            // Close the Connection
            stmt.Close();
            }
        }
    
    buf->Des().Zero();
    tempLength = KOr().Length()+KMultiEventId().Length()+KMaxIntLength;

    for(TInt i=0; i<aEventArray.Count(); i++ )
        {
        if(buf->Length() == 0)
            {
            buf->Des().Zero();
            buf->Des().Append(KDeleteMultiEvent());
            buf->Des().Append(KMultiEventId());
            buf->Des().AppendNum(aEventArray[i]);
            }
        else
            {
            buf->Des().Append(KOr());
            buf->Des().Append(KMultiEventId());
            buf->Des().AppendNum(aEventArray[i]);
            }

        if( i == aEventArray.Count()-1 || KBufferLength - buf->Length() < tempLength )
            {
            // Prepare the SQL statement to Update Status based on Id
            stmt.Prepare( iDb, *buf );
            
            // Execute the Remove SQL statement
            User::LeaveIfError( stmt.Exec() );
            buf->Des().Zero();
            
            // Close the Connection
            stmt.Close();
            }
        }
    
    CleanupStack::PopAndDestroy( buf ); //buf
	        	
	// Notify the database change for Delete
	iDbNotifier->DbChangedL( );
	EVTUIDEBUG( "- CEvtDatabase::RemoveEventsL()" ); 
    }       
 
// ---------------------------------------------------------------------------
// CEvtDatabase::UpdateEventsStatusL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::UpdateEventsStatusL( const RArray<TEvtEventId>& aEventArray,  TEvtEventStatus  aEvtStatus )
    {
    EVTUIDEBUG( "Sync UpdateEventsStatusL"); 
    
    // Leave if there are no array elements
    if( aEventArray.Count() == 0 )
        User::Leave( KErrArgument );
    
    RSqlStatement stmt;
    
    HBufC8* buf = HBufC8::NewLC(KBufferLength);
    buf->Des().Zero();
    TInt tempLength = KOr().Length()+KMultiEventId().Length()+KMaxIntLength;

    for(TInt i=0; i<aEventArray.Count(); i++ )
        {
        if(buf->Length() == 0)
            {
            buf->Des().Zero();
            buf->Des().Append(KUpdateMultiStatus());
            buf->Des().AppendNum(aEvtStatus);
            buf->Des().Append(KWhere());
            buf->Des().Append(KMultiEventId());
            buf->Des().AppendNum(aEventArray[i]);
            }
        else
            {
            buf->Des().Append(KOr());
            buf->Des().Append(KMultiEventId());
            buf->Des().AppendNum(aEventArray[i]);
            }

        if( i == aEventArray.Count()-1 || KBufferLength - buf->Length() < tempLength )
            {
            // Prepare the SQL statement to Update Status based on Id
            stmt.Prepare( iDb, *buf );
            
            // Execute the Remove SQL statement
            User::LeaveIfError( stmt.Exec() );
            buf->Des().Zero();
            
            // Close the Connection
            stmt.Close();
            }
        }

    CleanupStack::PopAndDestroy( buf ); //buf
                
    // Notify the database change for Delete
    iDbNotifier->DbChangedL( );
    }     
 
// ---------------------------------------------------------------------------
// CEvtDatabase::UpdateEventStatusL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::UpdateEventStatusL( 
                            TEvtEventId      aEvtId,
                            TEvtEventStatus    aEvtStatus)
    {
	EVTUIDEBUG( "+ CEvtDatabase::UpdateEventStatusL()" ); 
	RSqlStatement stmt;
	TInt paramIndex;
	
	// Prepare the SQL statement for Status updation
	stmt.Prepare( iDb, KUpdateEventStatus );
	
	// Set the Status parameter in SQL statement
	paramIndex=stmt.ParameterIndex( KStatus );
	stmt.BindInt( paramIndex, aEvtStatus );
	
	// Set the Id parameter in SQL statement
	paramIndex=stmt.ParameterIndex( KEventId );
	stmt.BindInt64( paramIndex, aEvtId );
	
	// Execute the Update SQL statement
	User::LeaveIfError( stmt.Exec() );
	
	stmt.Close();
	
	// Notify the database change for Modify
	iDbNotifier->DbChangedL( );
	EVTUIDEBUG( "- CEvtDatabase::UpdateEventStatusL()" ); 
    } 
         
// ---------------------------------------------------------------------------
// CEvtDatabase::RemoveEventL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::RemoveEventL( TEvtEventId      aEventId )
    {
	EVTUIDEBUG( "+ CEvtDatabase::RemoveEventL()" ); 
    
    // To maintain the Referential Integrity, we have to first delete
    // Action and then Event
	DeleteActionL( aEventId );
	DeleteEventL( aEventId );
	
	// Notify the database change for Delete
	iDbNotifier->DbChangedL( );
	EVTUIDEBUG( "- CEvtDatabase::RemoveEventL()" ); 
	} 
	
// ---------------------------------------------------------------------------
// CEvtDatabase::GetEventsL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::GetEventsL( 
                            RSqlStatement&      aStmt,
                            RPointerArray<CEvtBasicEventInfo>&     aEventArray )
    {
	EVTUIDEBUG( "+ CEvtDatabase::GetEventsL() - Generic" ); 
    
	TInt columnIndexEventId=0; 
	TInt columnIndexStatus=1; 
	TInt columnIndexSubject=2; 
	TInt columnIndexPlace=3;  
	TInt columnIndexRepeat=4; 
	/*
	Note that the column can be looked up if necessary
	although this is more expensive
	TInt columnIndex=aStmt.ColumnIndex("location");
	*/
	
	// Keep the count of the number of Events object that are pushed on to
	// Cleanup Stack. So that those many objects can be popped up later.
	TInt count=0;
	
	while( aStmt.Next() == KSqlAtRow )
		{
        count++;
		CEvtBasicEventInfo* evt = CEvtBasicEventInfo::NewLC();
		
        TUint32 value32 = aStmt.ColumnInt( columnIndexEventId );
		evt->SetEventId( value32 );

		TInt value = aStmt.ColumnInt( columnIndexStatus );
		evt->SetEventStatus( static_cast<TEvtEventStatus>( value ) );
		
		TPtrC subject=aStmt.ColumnTextL( columnIndexSubject );
		evt->SetSubjectL( subject );
		
		TPtrC place=aStmt.ColumnTextL( columnIndexPlace );
		evt->SetPlaceL( place );
        
		value = aStmt.ColumnInt( columnIndexRepeat );
		evt->SetRepeat(value);
		        
        aEventArray.Append( evt );
		}
	
	// count number of objects from Cleanup stack
    CleanupStack::Pop( count );
	EVTUIDEBUG( "- CEvtDatabase::GetEventsL() - Generic" ); 
    } 
    
// ---------------------------------------------------------------------------
// CEvtDatabase::UpdateEventTableL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::UpdateEventTableL( 
                            CEvtEvent&          aEvtEvent )
    {
	EVTUIDEBUG( "+ CEvtDatabase::UpdateEventTableL()" ); 
    // Update all the attributes of an Event
	  
	RSqlStatement stmt;
	TInt paramIndex;
	
	// We need to create the streams to insert large texts.
	RSqlParamWriteStream subjectStream;
	RSqlParamWriteStream placeStream;
	RSqlParamWriteStream descStream;
	CleanupClosePushL( subjectStream );
	CleanupClosePushL( placeStream );
	CleanupClosePushL( descStream );
	
	// Prepare the SQL statement to Get Events based on Status
	stmt.Prepare( iDb, KUpdateEvent );
	
	// Set Status
	paramIndex = stmt.ParameterIndex( KStatus );
	stmt.BindInt( paramIndex, aEvtEvent.EventStatus() );
	
	// Set Subject
	paramIndex = stmt.ParameterIndex( KSubject );
	if( KNullDesC() == aEvtEvent.Subject() )
		{
	    stmt.BindNull( paramIndex );
		}
	else
		{
		subjectStream.BindText( stmt, paramIndex );
		subjectStream.WriteL( aEvtEvent.Subject() );
		subjectStream.CommitL();
		}
	
	// Set Place
	paramIndex = stmt.ParameterIndex( KPlace );
	if( KNullDesC() == aEvtEvent.Place() )
		{
	    stmt.BindNull( paramIndex );
		}
	else
		{
		placeStream.BindText( stmt, paramIndex );
		placeStream.WriteL( aEvtEvent.Place() );
		placeStream.CommitL();
		}
	
	// Set Radius
	paramIndex = stmt.ParameterIndex( KRadius );
	stmt.BindReal( paramIndex, aEvtEvent.Radius() );
	
	// Set Description
	paramIndex = stmt.ParameterIndex( KDescription );
	if( KNullDesC() == aEvtEvent.Description() )
		{
	    stmt.BindNull( paramIndex );
		}
	else
		{
		descStream.BindText( stmt, paramIndex );
		descStream.WriteL( aEvtEvent.Description() );
		descStream.CommitL();
		}
	
	// Set Repeat
	paramIndex = stmt.ParameterIndex( KRepeat );
	stmt.BindInt( paramIndex, aEvtEvent.Repeat() );
    
    // Set Id
	paramIndex = stmt.ParameterIndex( KEventId );
	stmt.BindInt( paramIndex, aEvtEvent.EventId() );
	
	// Execute the Remove SQL statement
	User::LeaveIfError( stmt.Exec() );
	
	//Close the streams
	CleanupStack::PopAndDestroy( &descStream );
	CleanupStack::PopAndDestroy( &placeStream );
	CleanupStack::PopAndDestroy( &subjectStream );
	    
	stmt.Close();

	EVTUIDEBUG( "- CEvtDatabase::UpdateEventTableL()" );
    }
    
// ---------------------------------------------------------------------------
// CEvtDatabase::DeleteEventL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::DeleteEventL( 
                            	TEvtEventId          aEventId )
    {
	EVTUIDEBUG( "+ CEvtDatabase::DeleteEventL()" );
    
	RSqlStatement stmt;
	TInt paramIndex;
	
	// Prepare the SQL statement to Remove Action based on Id
	stmt.Prepare( iDb, KDeleteEvent );
	
	// Set the Id parameter in SQL statement
	paramIndex = stmt.ParameterIndex( KEventId );
	stmt.BindInt( paramIndex, aEventId );
	
	// Execute the Remove SQL statement
	User::LeaveIfError( stmt.Exec() );
	
	stmt.Reset();
	stmt.Close();

	EVTUIDEBUG( "- CEvtDatabase::DeleteEventL()" );
    }
    
// ---------------------------------------------------------------------------
// CEvtDatabase::CheckIfEventPresentL
// ---------------------------------------------------------------------------
//
TBool CEvtDatabase::CheckIfEventPresentL( 
                            TEvtEventId          aId )
    {
	EVTUIDEBUG( "+ CEvtDatabase::DeleteEventL()" );

	RSqlStatement stmt;
	
	// Prepare the SQL statement to Get Action based on EvtId
	stmt.Prepare( iDb, KSelectEvent );
	
	// Set the Id parameter in SQL statement
	TInt paramIndex=stmt.ParameterIndex( KEventId );
	stmt.BindInt( paramIndex, aId ); 

    // Flag to Check if the the Action is present
    TBool flag( EFalse );

	// Set the other attributes of the Requested Event
	while( !flag && stmt.Next() == KSqlAtRow )
		{
		flag = ETrue;
		}
    
    stmt.Close();
	EVTUIDEBUG( "- CEvtDatabase::DeleteEventL()" );
	
    return flag;
    }
    
// ---------------------------------------------------------------------------
// CEvtDatabase::CheckIfActionPresentL
// ---------------------------------------------------------------------------
//
TBool CEvtDatabase::CheckIfActionPresentL( 
                            TInt64          aId )
    {
	EVTUIDEBUG( "+ CEvtDatabase::CheckIfActionPresentL()" );

	RSqlStatement stmt;
	
	// Prepare the SQL statement to Get Action based on EvtId
	stmt.Prepare( iDb, KSelectAction );
	
	// Set the Id parameter in SQL statement
	TInt paramIndex=stmt.ParameterIndex( KId );
	stmt.BindInt64( paramIndex, aId ); 

    // Flag to Check if the the Action is present
    TBool flag( EFalse );

	// Set the other attributes of the Requested Event
	while( !flag && stmt.Next() == KSqlAtRow )
		{
		flag = ETrue;
		}
    
    stmt.Close();
	EVTUIDEBUG( "- CEvtDatabase::CheckIfActionPresentL()" );
	
    return flag;
    }
    
// ---------------------------------------------------------------------------
// CEvtDatabase::GetActionId
// ---------------------------------------------------------------------------
//
TInt64 CEvtDatabase::GetActionId( 
                            TEvtEventId          aId )
    {
	EVTUIDEBUG( "+ CEvtDatabase::GetActionId()" );
    TInt64 id=0;
	TInt columnIndexActionId=0;
    
	RSqlStatement stmt;
	
	// Prepare the SQL statement to Get Action based on EvtId
	stmt.Prepare( iDb, KSelectActionFromEvent );
	
	// Set the Id parameter in SQL statement
	TInt paramIndex=stmt.ParameterIndex( KEvtId );
	stmt.BindInt( paramIndex, aId ); 

    // Flag to Check if the the Action is present
    TBool flag( EFalse );

	// Set the other attributes of the Requested Event
	while( !flag && stmt.Next() == KSqlAtRow )
		{
		id = stmt.ColumnInt64( columnIndexActionId );
		flag = ETrue;
		}
    
    stmt.Close();
	EVTUIDEBUG( "- CEvtDatabase::GetActionId()" );
	
    return id;
    }
    
// ---------------------------------------------------------------------------
// CEvtDatabase::CreateActionL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::CreateActionL( 
								CEvtAction&          aEvtAction )
    {
	EVTUIDEBUG( "+ CEvtDatabase::CreateActionL()" );
    
    RSqlStatement stmt;
	
	// Update Action table.
	RSqlParamWriteStream actionStream;
	CleanupClosePushL( actionStream );
	
	// Prepare the SQL statement to update Action based on Id
	stmt.Prepare( iDb, KInsertAction );
	
	// Set the Action parameter in SQL statement
	TInt paramIndex=stmt.ParameterIndex( KAction );
	if( KNullDesC() == aEvtAction.Action() )
		{
	    stmt.BindNull( paramIndex );
		}
	else
		{
		actionStream.BindText( stmt, paramIndex );
		actionStream.WriteL( aEvtAction.Action() );
		actionStream.CommitL();
		}
	
	// Set the EvtId parameter in SQL statement
	paramIndex=stmt.ParameterIndex( KEvtId );
	stmt.BindInt( paramIndex, aEvtAction.EvtId() );
	
	// Actual Query execution that updates the database.
	User::LeaveIfError( stmt.Exec() );
	
	//Close the stream
	CleanupStack::PopAndDestroy( &actionStream );
	stmt.Close();
	
	//Set the new Action Id
	aEvtAction.SetId( GetActionId( aEvtAction.EvtId() ) );
	
	EVTUIDEBUG( "- CEvtDatabase::CreateActionL()" );

    }
    
// ---------------------------------------------------------------------------
// CEvtDatabase::UpdateActionL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::UpdateActionL( 
                            	CEvtAction&          aEvtAction )
    {
	EVTUIDEBUG( "+ CEvtDatabase::UpdateActionL()" );
    
    RSqlStatement stmt;
	
	// Update Action table.
	RSqlParamWriteStream actionStream;
	CleanupClosePushL( actionStream );
	
	// Prepare the SQL statement to update Action based on Id
	stmt.Prepare( iDb, KUpdateAction );
	
	// Set the Action parameter in SQL statement
	TInt paramIndex=stmt.ParameterIndex( KAction );
	if( KNullDesC() == aEvtAction.Action() )
		{
	    stmt.BindNull( paramIndex );
		}
	else
		{
		actionStream.BindText( stmt, paramIndex );
		actionStream.WriteL( aEvtAction.Action() );
		actionStream.CommitL();
		}
	
	// Set the EvtId parameter in SQL statement
	paramIndex=stmt.ParameterIndex( KEvtId );
	stmt.BindInt( paramIndex, aEvtAction.EvtId() );
	
	// Set the Id parameter in SQL statement
	paramIndex=stmt.ParameterIndex( KId );
	stmt.BindInt64( paramIndex, aEvtAction.Id() );
	
	// Actual Query execution that updates the database.
	User::LeaveIfError( stmt.Exec() );
	
	//Close the stream
	CleanupStack::PopAndDestroy( &actionStream );
	stmt.Close();

	EVTUIDEBUG( "- CEvtDatabase::UpdateActionL()" );
    }
    
// ---------------------------------------------------------------------------
// CEvtDatabase::DeleteActionL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::DeleteActionL( 
                            	TEvtEventId          aEventId )
    {
	EVTUIDEBUG( "+ CEvtDatabase::DeleteActionL()" );
    
	RSqlStatement stmt;
	TInt paramIndex;
	
	// Prepare the SQL statement to Remove Action based on Id
	stmt.Prepare( iDb, KDeleteAction );
	
	// Set the Id parameter in SQL statement
	paramIndex = stmt.ParameterIndex( KEvtId );
	stmt.BindInt( paramIndex, aEventId );
	
	// Execute the Remove SQL statement
	User::LeaveIfError( stmt.Exec() );
	
	stmt.Close();

	EVTUIDEBUG( "- CEvtDatabase::DeleteActionL()" );
    }
    
// ---------------------------------------------------------------------------
// CEvtDatabase::SetObserver
// ---------------------------------------------------------------------------
//
void CEvtDatabase::SetObserver( MEvtStorageDbObserver*      
														aStorageDbObserver )
    {
	EVTUIDEBUG( "+ CEvtDatabase::SetObserver()" );
    // Set the Observer to the database
	iStorageDbObserver = aStorageDbObserver;
	EVTUIDEBUG( "- CEvtDatabase::SetObserver()" );
    } 

// ---------------------------------------------------------------------------
// CEvtDatabase::HandleDbChangedL
// ---------------------------------------------------------------------------
//
void CEvtDatabase::HandleDbChangedL( )
    {
	EVTUIDEBUG( "+ CEvtDatabase::HandleDbChangedL()" );
    
    // If there is an observer to the database, notify it.
	if( iStorageDbObserver )
		{
		iStorageDbObserver->HandleStorageDbChangedL( );
		}
	EVTUIDEBUG( "- CEvtDatabase::HandleDbChangedL()" );
    } 
    
// End of File
