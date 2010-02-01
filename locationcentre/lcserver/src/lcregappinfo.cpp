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
* Description:  Registration information for a Location based Application.
*
*/



// SYSTEM INCLUDES
#include <bautils.h>
#include <f32file.h>

// USER INCLUDES
#include "lcregappinfo.h"
#include "lcregappnamelangid.h"
// CONST DEFINATION


// CLASS DEFINATION
/**
 *  Maximum length of a service name can be
 */
const TInt KAppNameMaxLength = 50;

// ----- Member funtions for CLcRegAppInfo ---------------------------------

// ---------------------------------------------------------------------------
// CLcRegAppInfo::CLcRegAppInfo
// ---------------------------------------------------------------------------
//
CLcRegAppInfo::CLcRegAppInfo( )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.Initiallize all the variable here
    iFilePresenceStatus = ETrue;
    }
         
// ---------------------------------------------------------------------------
// CLcRegAppInfo::~CLcRegAppInfo
// ---------------------------------------------------------------------------
//
CLcRegAppInfo::~CLcRegAppInfo()
    {
    // C++ Destructor. Free all resources associated with this class.
    iAppNameArray.ResetAndDestroy();
    iAppNameArray.Close();
    iAppLangArray.Reset();
    iAppLangArray.Close();
    iFileName.Delete( 0,iFileName.Length() );
    }
        
// ---------------------------------------------------------------------------
// CLcRegAppInfo* CLcRegAppInfo::NewL
// ---------------------------------------------------------------------------
//
CLcRegAppInfo* CLcRegAppInfo::NewL()
    {
    CLcRegAppInfo* self = new ( ELeave )CLcRegAppInfo();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcRegAppInfo::SetAppNameL
// ---------------------------------------------------------------------------
//
void CLcRegAppInfo::SetAppNameL( const TDesC& aName )
	{
	HBufC* nameBuf = NULL;
	if( aName.Length() > KAppNameMaxLength )
	    {
	    nameBuf = HBufC::NewL( KAppNameMaxLength );	
	    nameBuf->Des().Copy( aName.Left( KAppNameMaxLength ));    
	    }
	else
	    {
	    nameBuf = HBufC::NewL( aName.Length() );
	    nameBuf->Des().Copy( aName );
	    }
    User::LeaveIfError( iAppNameArray.Append( nameBuf ) );	
	}

// ---------------------------------------------------------------------------
// TInt CLcRegAppInfo::SetNameLanguage
// ---------------------------------------------------------------------------
//
TInt CLcRegAppInfo::SetNameLanguage( const TDesC& aNameLang )
	{
	for (TInt i=0; i< NUMLANGOPTIONS; i++)
		{
		if ( !KLangOptions[i].iName.Compare( aNameLang ) )
			{
			return iAppLangArray.Append( ( TUint32 )KLangOptions[i].iNumber );
			}
		}
	return KErrNotFound;		
	}

// ---------------------------------------------------------------------------
// void CLcRegAppInfo::SetAppLangSpecificNameL
// ---------------------------------------------------------------------------
//
void CLcRegAppInfo::SetAppLangSpecificNameL( RFs& aFs )
	{
	TLanguage langcode = User::Language(); //current language
	TInt index = iAppLangArray.Find( ( TUint32 )langcode ); 
	if( index != KErrNotFound )
		{
		if( index < iAppNameArray.Count() )
			{
			SetNameL( *iAppNameArray[index] );	
			}
		}
	else
		{
		RArray<TLanguage> langs;
		CleanupClosePushL( langs );
	   	BaflUtils::GetDowngradePathL( aFs,langcode,langs );
	    for(TInt i = 0; i < iAppLangArray.Count(); ++i)
	    	{
	    	index = langs.Find( ( TLanguage )iAppLangArray[i] );
			if( index != KErrNotFound )
				{
				break;					
				}
			}
		if( index == KErrNotFound )			
			{
			index = langs.Count() - 1;	
			}
		if( index >= 0 && index < iAppNameArray.Count()  )
			{
			SetNameL( *iAppNameArray[index] );	
			}
		CleanupStack::PopAndDestroy( &langs );
		}
    iAppNameArray.ResetAndDestroy();
    iAppNameArray.Close();
    iAppLangArray.Reset();
    iAppLangArray.Close();
	}

// ---------------------------------------------------------------------------
// void CLcRegAppInfo::SetFileNameL
// ---------------------------------------------------------------------------
//
void CLcRegAppInfo::SetFileNameL( const TDesC& aFile )
	{
	if( aFile.Length() <= KMaxFileName )
		{
		iFileName.Copy( aFile );		
		}
	else
		{
		User::Leave( KErrBadName );	
		}		
	}	
	
// ---------------------------------------------------------------------------
// TFileName CLcRegAppInfo::FileName
// ---------------------------------------------------------------------------
//
TFileName CLcRegAppInfo::FileName()
	{
	return iFileName;	
	}	

// ---------------------------------------------------------------------------
// void CLcRegAppInfo::IsAppPresent
// ---------------------------------------------------------------------------
//
TBool CLcRegAppInfo::IsAppPresent( )
	{
	return iFilePresenceStatus;	
	}	
	
// ---------------------------------------------------------------------------
// void CLcRegAppInfo::SetFilePresenceStatus
// ---------------------------------------------------------------------------
//
void CLcRegAppInfo::SetFilePresenceStatus( TBool aPresenceStatus )
	{
	iFilePresenceStatus = aPresenceStatus;	
	}	
	
// ---------------------------------------------------------------------------
// void CLcRegAppInfo::SetMmcId
// ---------------------------------------------------------------------------
//
void CLcRegAppInfo::SetMmcId( TUint aUid )
	{
	iMmcId = aUid;	
	}	
	
// ---------------------------------------------------------------------------
// TFileName CLcRegAppInfo::MmcId
// ---------------------------------------------------------------------------
//
TUint CLcRegAppInfo::MmcId()
	{
	return iMmcId;	
	}	

// ---------------------------------------------------------------------------
// void CLcRegAppInfo::SetArrayIndex
// ---------------------------------------------------------------------------
//
void CLcRegAppInfo::SetArrayIndex( TInt aIndex )
	{
	iIndex = aIndex;	
	}	
	
// ---------------------------------------------------------------------------
// TFileName CLcRegAppInfo::Index
// ---------------------------------------------------------------------------
//
TInt CLcRegAppInfo::Index()
	{
	return iIndex;	
	}	

// ---------------------------------------------------------------------------
// TBool CLcRegAppInfo::IsAppNamePresent
// ---------------------------------------------------------------------------
//
TBool CLcRegAppInfo::IsAppNamePresent()	
    {
    if( iAppNameArray.Count() == 0 &&
        iAppLangArray.Count() == 0 )
        {
        return EFalse;
        }
    return ETrue;        
    }
//End of file
