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







#ifndef BCAPPLMKSELECTORENGINE_H
#define BCAPPLMKSELECTORENGINE_H

// INCLUDES
//#include "BCAppLmkSelectorContainer.h"
#include <coecntrl.h>
#include <coeccntx.h>
#include <eikenv.h>
#include <CSatelliteInfoUi.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <CLmkEditorDlg.h>
#include <coemop.h>
#include <CLmkLandmarkSelectorDlg.h>
#include <CLmkCategorySelectorDlg.h>
#include <TLmkItemIdDbCombiInfo.h>

// FORWARD DECLARATIONS



// CLASS DECLARATION

/*
CBCAppLmkSelectorEngine
*/

class CBCAppLmkSelectorEngine : public CBase, public MObjectProvider
	{
public:
    static CBCAppLmkSelectorEngine* NewL();
    /*~CBCAppLmkSelectorEngine();
    void RunL();
    void DoCancel();

    void Init();
    void IssueTest();

	void ExecuteTestCase();
    void SetTestCaseNumber(const TDesC8 &aNumber);
public:
	TBuf<15> iText;*/

protected:
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    MObjectProvider* MopNext();

private:
    CBCAppLmkSelectorEngine();
   /* void ConstructL(CBCAppLmkSelectorContainer* aContainer);

    void ExecuteTestL();

    void SetTestCase(const TDesC& aHeaderName, const TDesC& aFunctionName);
    void Print(const TDesC& aDes);
    void Print(const TDesC8& aDes);
    void Print(TInt aInt);
	void CreateNewLandmark();
private:
    RTimer iTimer;

    CEikonEnv* iEnv;
    CBCAppLmkSelectorContainer* iContainer;

    TInt iCurrentTest;
    TBool iWarningIssued;
    TBool iExecuteAllTests;
    TBool iAllTestsDone;

    TBuf<64> iCurrentHeaderName;
    TBuf<128> iCurrentFunctionName;

	RWsSession iWs;
	CActiveSchedulerWait iWait;*/
	};

#endif

// End of File
