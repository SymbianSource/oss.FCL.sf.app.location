/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -
*
*/






/*
Change History
22-05-2007	To resolve tool IDO warnings	Komala Nagaraju
*/
#define PRINTTOSCREEN
#define WRITETOLOG


// INCLUDE FILES
//#include "BCAppLmkSelectorContainer.h"
#include "BCAppLmkSelectorEngine.h"
#include <BCAppLmkSelector.rsg>
#include <apgtask.h>
#include <bacline.h>
#include <in_sock.h>
#include <coecntrl.h>
#include <gulicon.h>
#include <eikfrlb.h>
#include <AknIconArray.h>
#include <in_sock.h>
#include <barsread.h>

_LIT(KAppName, "BCAppLmkSelector");


// ================= MEMBER FUNCTIONS =======================

/*void CBCAppLmkSelectorEngine::ExecuteTestL()
    {

    switch (iCurrentTest)
        {
		case 0:
            {
            SetTestCase(_L("CLmkLandmarkSelectorDlg"), _L("CLmkLandmarkSelectorDlg::NewL"));

			CLmkLandmarkSelectorDlg* iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
			delete iLmkSelectDlg;
			iLmkSelectDlg = NULL;
			}
            break;

        case 1:
            {
            SetTestCase(_L("CLmkLandmarkSelectorDlg"), _L("CLmkLandmarkSelectorDlg::SetMopParent"));

			CLmkLandmarkSelectorDlg* iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
			iLmkSelectDlg->SetMopParent(this);
			delete iLmkSelectDlg;
			iLmkSelectDlg = NULL;
			}
            break;

        case 2:
            {
            SetTestCase(_L("CLmkLandmarkSelectorDlg"), _L("CLmkLandmarkSelectorDlg::ExecuteLD"));

			TLmkItemIdDbCombiInfo selItem;
			CLmkLandmarkSelectorDlg* iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
        	TRAPD( result, iLmkSelectDlg->ExecuteLD(selItem));
        	}
            break;

        case 3:
            {
            SetTestCase(_L("CLmkLandmarkSelectorDlg"), _L("CLmkLandmarkSelectorDlg::ExecuteLD#1"));

			RArray<TLmkItemIdDbCombiInfo> selItems;

			CLmkLandmarkSelectorDlg* iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
        	TRAPD( result, iLmkSelectDlg->ExecuteLD(selItems));
        	selItems.Close();
        	}
            break;

        case 4:
            {
            SetTestCase(_L("CLmkCategorySelectorDlg"), _L("CLmkCategorySelectorDlg::NewL"));

			CLmkCategorySelectorDlg* iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL();
			delete iLmkCategoryDlg;
			iLmkCategoryDlg = NULL;
            }
            break;

        case 5:
            {
            SetTestCase(_L("CLmkCategorySelectorDlg"), _L("CLmkCategorySelectorDlg::SetMopParent"));

			CLmkCategorySelectorDlg* iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL();
			iLmkCategoryDlg->SetMopParent(this);
			delete iLmkCategoryDlg;
			iLmkCategoryDlg = NULL;
            }
            break;

        case 6:
            {
            SetTestCase(_L("CLmkCategorySelectorDlg"), _L("CLmkCategorySelectorDlg::ExecuteLD"));

			TLmkItemIdDbCombiInfo selItem;
			CLmkCategorySelectorDlg* iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL();
        	TRAPD( result, iLmkCategoryDlg->ExecuteLD(selItem));
            }
            break;

        case 7:
            {
            SetTestCase(_L("CLmkCategorySelectorDlg"), _L("CLmkCategorySelectorDlg::ExecuteLD#1"));

			RArray<TLmkItemIdDbCombiInfo> selItems;
            CLmkCategorySelectorDlg* iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL();
        	TRAPD( result, iLmkCategoryDlg->ExecuteLD(selItems));
        	selItems.Close();
            }
            break;

		default:
            {
            iAllTestsDone = ETrue;
            }
        }
    }
*/
TTypeUid::Ptr CBCAppLmkSelectorEngine::MopSupplyObject(TTypeUid /*aId*/)
{
}

MObjectProvider* CBCAppLmkSelectorEngine:: MopNext()
{
}

// ---------------------------------------------------------

CBCAppLmkSelectorEngine* CBCAppLmkSelectorEngine::NewL(/*CBCAppLmkSelectorContainer* aContainer*/)
    {
    CBCAppLmkSelectorEngine* self = new(ELeave) CBCAppLmkSelectorEngine;
    CleanupStack::PushL(self);
    //self->ConstructL(/*aContainer*/);
    //self->CreateNewLandmark();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------

/*CBCAppLmkSelectorEngine::CBCAppLmkSelectorEngine() : CActive(0)
    {
    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::IssueTest()
    {
    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("active"), 1));

    iTimer.After(iStatus, 50000);
    SetActive();
    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::RunL()
    {

    // Execute the test case
    TRAPD(err, ExecuteTestL());

    if (iExecuteAllTests)
        {
        if (iAllTestsDone)
            {
            // All tests are done  (don't have to exit the program, just print a text)
            Print(_L("All tests done.\n"));
            }
        else
            {
            // We have more tests to be run, move to the next one
            Print(iCurrentFunctionName);
            Print(_L("\n"));
            iCurrentTest++;
            IssueTest();
            }
        }
    else
        {
        if (iAllTestsDone)
            {
            // Signal the main application that there are no more tests to be run
            Print(_L("All tests done.\n"));
            User::Exit(7777);
            }
        else
            {
            // Only one test case executed, exit normally
            Print(iCurrentFunctionName);
            Print(_L("\n"));
            User::Exit(0);
            }
        }

    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::DoCancel()
    {
    iTimer.Cancel();
    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::Init()
    {
    iCurrentTest = 0;
    iWarningIssued = EFalse;
    iExecuteAllTests = EFalse;
    iAllTestsDone = EFalse;
	iText.Append(_L("Setting Title"));
    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::Print(const TDesC& aDes)
    {
    #ifdef PRINTTOSCREEN
	iContainer->PrintNotify(aDes, 0);
    #endif
    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::Print(const TDesC8& aDes)
    {
    #ifdef PRINTTOSCREEN
	iContainer->PrintNotify(aDes, 0);
    #endif
    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::Print(TInt aInt)
    {
    #ifdef PRINTTOSCREEN
	iContainer->PrintNotify(aInt);
    #endif
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
*/
/*void CBCAppLmkSelectorEngine::ConstructL(CBCAppLmkSelectorContainer* aContainer)
    {
    iEnv = CEikonEnv::Static();
    TInt err = iWs.Connect();
    err = err;
    User::LeaveIfError(iTimer.CreateLocal());
    //iContainer = aContainer;

    Init();
    CActiveScheduler::Add(this);


    // check if BCAppMain is running
    TApaTaskList taskList(iWs);
    TApaTask thisTask = taskList.FindApp(TUid::Uid(0x101FD6FF));

    if (thisTask.Exists())
        {
    	// ask the BCAppMain to send the test case number
    	thisTask.SendMessage(TUid::Uid( KUidApaMessageSwitchOpenFileValue ), _L8("SendNewTestCaseNumber"));
        iExecuteAllTests = EFalse;
        }
    else
        {
        // BCAppMain not running, execute all cases immediately
        iExecuteAllTests = ETrue;
        IssueTest();
        }
    }
*/
// ---------------------------------------------------------

/*CBCAppLmkSelectorEngine::~CBCAppLmkSelectorEngine()
    {
    Cancel();
    iWs.Close();
    iTimer.Close();
    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::SetTestCase(const TDesC& aHeaderName, const TDesC& aFunctionName)
    {
    // copy names to our variables
    iCurrentHeaderName.Copy(aHeaderName);
    iCurrentFunctionName.Copy(aFunctionName);

    // send this information to BCAppMain
    TApaTaskList taskList(iWs);
    TApaTask thisTask = taskList.FindApp(TUid::Uid(0x101FD6FF));

    if (thisTask.Exists())
        {
        TBuf8<400> msg;
        msg.Copy(iCurrentHeaderName);
        msg.Append(_L8("@"));
        msg.Append(iCurrentFunctionName);
        thisTask.SendMessage(TUid::Uid( KUidApaMessageSwitchOpenFileValue ), msg);

        // now start waiting till BCAppMain has acknowledged our message
        iWait.Start();
        }

    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::ExecuteTestCase()
    {
    // stop waiting
    iWait.AsyncStop();
    }

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::SetTestCaseNumber(const TDesC8 &aNumber)
    {
    // set the test case number
    TLex8 msgConv(aNumber);
    msgConv.Val(iCurrentTest);

    // start the active object
    IssueTest();
    }

// ---------------------------------------------------------

// ---------------------------------------------------------

void CBCAppLmkSelectorEngine::CreateNewLandmark()
    {

  TRAP_IGNORE(
	CPosLandmarkDatabase* dBase = CPosLandmarkDatabase::OpenL();
	CleanupStack::PushL( dBase );
	if (dBase->IsInitializingNeeded())
		{
	ExecuteAndDeleteLD(dBase->InitializeL()); // Synchronous since no argument
		}

	CPosLmItemIterator*  iterator = dBase->LandmarkIteratorL();
    CleanupStack::PushL(iterator);


    if( iterator->NumOfItemsL() == 0 )
	    {
	    CPosLandmark* landmark = CPosLandmark::NewL();
		CleanupStack::PushL( landmark );

		landmark->SetLandmarkNameL(_L("Nokia"));

		dBase->AddLandmarkL(*landmark);
		CleanupStack::PopAndDestroy();//landmark
	    }

 	CleanupStack::PopAndDestroy(2);	//iterator,dBase
 	)
	}
*/

// End of File
