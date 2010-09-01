/*
 * Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:    Class for Landmarks Package Viewer 
 *
 */

#ifndef CLMKPACKAGEEDITORIMPL_H
#define CLMKPACKAGEEDITORIMPL_H

//  INCLUDES
#include "CLmkEditorImpl.h"
#include "MLmkAOOperationObserver.h"

// FORWARD DECLARATIONS
class CLmkParser;
class CLmkLandMarkCategoriesName;
class CAknWaitDialog;

// CLASS DECLARATION
/**
 * CLmkPackageEditorImpl specific class for viewing/editing a landmark
 * which exists in the received landmark package.
 */
class CLmkPackageEditorImpl : public CLmkEditorImpl,
        public MLmkAOOperationObserver
    {
public:
    // Constructors and destructor
    /**
     * View/edit landmark. User must ensure that aItemIndex is
     * valid index for a parsed item.
     * @param aDb
     * @param aParser
     * @param aSender landmark sender reference
     * @param aItemIndex index of item to show in editor
     * @return newly instantiated object
     */
    IMPORT_C static CLmkPackageEditorImpl* NewL(CPosLandmarkDatabase& aDb,
            CLmkParser& aParser, CLmkSender& aSender, TInt aItemIndex);

    /**
     * Destructor.
     */
    ~CLmkPackageEditorImpl();

public:
    //from MLmkAOOperationObserver
    void HandleOperationL(TOperationTypes aType, TReal32 aProgress,
            TInt aStatus);

protected:
    // From CAknForm
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);
    void ProcessCommandL(TInt aCommandId);

private:
    // New functions
    void SaveLandmarkCmdL();
    /**
     * Sends the current displayed landmark
     */
    void SendLandmarkCmdL(TInt aCommandId);
    void DimmMenuItemsL(CEikMenuPane* aMenuPane);

private:
    // Constructors and destructor
    /**
     * C++ default constructor.
     * @param aDb
     * @param aParser
     * @param aSender landmark sender reference
     * @param aItemIndex index of item to show in editor
     * @return newly instantiated object
     */
    CLmkPackageEditorImpl(CPosLandmarkDatabase& aDb, CLmkParser& aParser,
            CLmkSender& aSender, TInt aItemIndex);

    /**
     * By default Symbian 2nd phase constructor is private.

     */
    void ConstructL();
    
private:
    //from CLmkEditorImpl
    void UpdateMskOnArrowMoveForViewerL();
    void EnableMskMenuL();
    void PostLayoutDynInitL();
    
#ifdef RD_SCALABLE_UI_V2
protected:
    // From MEikDialogPageObserver
    void HandleDialogPageEventL(TInt aEventID);
#endif //RD_SCALABLE_UI_V2
    
private:
    // Data
    ///Ref: landmark package parser
    CLmkParser& iParser;

    // edited item's index within parser
    TInt iItemIndex;

    ///Owns: parsed landmark
    CPosLandmark* iLandmark;
    CLmkLandMarkCategoriesName* iCategoryNameArray;

    // Owns.
    CAknWaitDialog* iWaitNote;
    };

#endif      // CLMKPACKAGEEDITORIMPL_H

// End of File
