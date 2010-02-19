/*
 * Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:    This file contains class for landmarks editor and viewer
 *
 */

#ifndef CLMKEDITORIMPL_H
#define CLMKEDITORIMPL_H

//  INCLUDES
#include "CLmkFields.h"
#include "MLmkEditorUiBuilder.h"
#include "MLmkLocationObserver.h"
#include <AknForm.h>        // CAknForm
#include <EPos_Landmarks.h> // Lm typedefs, constants etc.
#include <CLmkEditorDlg.h>  // dialog flags
#include <AknInfoPopupNoteController.h>
#include "CLmkLandMarkCategoriesName.h"
#include "clmkmapnavigationinterface.h"
#include "mlmkcentralrepositoryobserver.h"
#include "lmkwaitdlglauncher.h"

// FORWARD DECLARATIONS
class CPosLandmarkDatabase;
class CLmkSender;
class MObjectProvider;
class CPosLmPartialReadParameters;
class CLmkUiUtils;
class CPosLandmark;
class CLmkEditorFieldArray;
class CAknNavigationDecorator;
class CLmkLocationService;
class TCoeHelpContext;
class CLmkCallCmd;
class CLmkGoToURLCmd;
class MLmkEditorField;
class CLmkCentralRepository;

// CLASS DECLARATION
/**
 * CLmkEditorImpl class
 */
class CLmkEditorImpl : public CAknForm,
        public MLmkEditorUiBuilder,
        public MLmkLocationObserver,
        public MLmkListProviderObserver,
        public MLmkCentralRepositoryObserver,
        public MLmkWaitDialogLauncherObserver
    {
public:
    // Constructors and destructor
    /**
     * Edit or create new landmark
     * Create new landmark if aLandmarkId = KPosLmNullItemId
     * Not take ownership CPosLmPartialReadParameters
     * @param aDb
     * @param aSender landmark sender reference
     * @param aAttributes
     * @param aEditorMode
     * @param aLandmarkId
     * @return newly instantiated object
     */
    IMPORT_C static CLmkEditorImpl* NewL(CPosLandmarkDatabase& aDb,
            CLmkSender& aSender, TLmkEditorAttributes aAttributes,
            TLmkEditorMode aEditorMode, TPosLmItemId aLandmarkId);

    /**
     * Edit landmark
     * Not take ownership CPosLandmark or CPosLmPartialReadParameters
     * @param aDb
     * @param aSender landmark sender reference
     * @param aAttributes
     * @param aEditorFlags
     * @param aLandmark
     * @return newly instantiated object
     */
    IMPORT_C static CLmkEditorImpl* NewL(CPosLandmarkDatabase& aDb,
            CLmkSender& aSender, TLmkEditorAttributes aAttributes,
            TLmkEditorMode aEditorMode, CPosLandmark* aLandmark);
    IMPORT_C static CLmkEditorImpl* NewL(CPosLandmarkDatabase& aDb,
            CLmkSender& aSender, TLmkEditorAttributes aAttributes,
            TLmkEditorMode aEditorMode, TPosLmItemId* aLandmarkId,
            CPosLandmark* aLandmark);
    /**
     * Destructor.
     */
    ~CLmkEditorImpl();

public:
    // New functions
    /**
     * Set parent
     * @param aParent
     */
    IMPORT_C void SetMopParent(MObjectProvider* aParent);

    /**
     * Set help context
     * @param aContect
     */
    IMPORT_C void SetHelpContext(TCoeHelpContext aContext);

    /**
     * ExecuteLD
     * @return error code
     */
    IMPORT_C TInt ExecuteLD();
    /**
     * This function when invoked, disables the Map and Navigation related Menu Options
     * This needs to be invoked before ExecuteLD().By default, the Map and Navigation
     * menu options are enabled and are shown.
     *
     */
    IMPORT_C void DisableMapAndNavigationMenuOptions();
    /**
     * This function should be invoked only when,from the landmark
     * view dialog, edit mode dialog and launched and fields are
     * updated. This function updates the corresponding the fields
     * in the landmark viewer dialog.
     */
    void UpdateViewDlgL();

public:
    // From CAknForm
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);
    void ProcessCommandL(TInt aCommandId);
    void HandleControlStateChangeL(TInt aControlId);
    void DoNotSaveFormDataL();
    TInt EditMode();
public:
    //from MLmkWaitDialogLauncherObserver
    void HandleDialogDismissed(TInt aButtonId);

public:
    //from CCoeControl
    void GetHelpContext(TCoeHelpContext& aContext) const;
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

protected:
    //from MLmkLocationObserver
    void NotifyL(const TInt aStatus);
    void NotifyErrorL(TInt aErrorCode);

public:
    //from CEikDialog
    SEikControlInfo CreateCustomControlL(TInt aControlType);
    MEikDialogPageObserver::TFormControlTypes
    ConvertCustomControlTypeToBaseControlType(TInt aControlType) const;

#ifdef RD_SCALABLE_UI_V2
protected:
    // From MEikDialogPageObserver
    void HandleDialogPageEventL(TInt aEventID);
#endif //RD_SCALABLE_UI_V2
    
    //from MEikDialogPageObserver
    void LineChangedL(TInt aControlId);

protected:
    // from CAknForm
    void PreLayoutDynInitL();
    void PostLayoutDynInitL();

protected:
    //from CEikDialog
    TBool OkToExitL(TInt aKeyCode);

protected:
    //from MLmkCentralRepositoryObserver
    void HandleCentralRepositoryChangeL();

protected:
    // Constructors and destructor
    /**
     * C++ default constructor.
     * @param aDb
     * @param aSender landmark sender reference
     * @param aEditorMode
     * @return newly instantiated object
     */
    CLmkEditorImpl(CPosLandmarkDatabase& aDb, CLmkSender& aSender,
            TLmkEditorMode aEditorMode);

    /**
     * By default Symbian 2nd phase constructor is private.
     * @param aLandmarkId, negative if create new
     * @param aAttiributes
     */
    void ConstructL(TPosLmItemId aLandmarkId,
            TLmkEditorAttributes aAttributes);

    /**
     * By default Symbian 2nd phase constructor is private.
     * @param aLandmark, Null if create new
     * @param aAttributes
     */
    void ConstructL(CPosLandmark* aLandmark,
            TLmkEditorAttributes aAttributes);

    /**
     * By default Symbian 2nd phase constructor is private.
     * @param aLandmarkId, negative if create new
     * @param aAttiributes
     */
    void ConstructL(TPosLmItemId* aLandmarkId, CPosLandmark* aLandmark,
            TLmkEditorAttributes aAttributes);

    /**
     * Set default things for both other ConstructL() methods.
     */
    void CommonConstructL(TLmkEditorAttributes& aAttributes);

    /**
     * Context menu bar construction
     */
    void ConstructContextMenuL();

    /**
     * Checks Landmarks Coordinate Data Is available or not.
     */
    TBool IsLandmarkDataEmptyL(CPosLandmark* aLandmark);

    /**
     * Check whether Coordinates has to be hidden.
     */
    void CheckHideCoordinateL();

private:
    // from MPbkContactEditorUiBuilder
    CCoeControl* CreateLineL(const TDesC& aCaption, TInt aControlId,
            TInt aControlType);
    CEikCaptionedControl* LineControl(TInt aControlId) const;
    void DeleteControl(TInt aControlId);
    void TryChangeFocusL(TInt aControlId);
    void SetCurrentLineCaptionL(TInt aControlId, const TDesC& aText);
    CCoeControl* Control(TInt aControlId) const;
    void SetEditableL(TBool aState);
    CPosLandmarkDatabase& Database();
    CPosLandmark* GetCurrentEditedLandmarkL();

private:
    // New functions
    void CreateLabelArrayL();
    void CreateLabelL(TInt iResourceId, TInt iPos);
    TBool IsNameFieldEmptyL();
    TBool OkToExitL();
    void SelectCategoriesCmdL();
    void CurrentLocationCmdL();
    void ChangeIconCmdL();
    void CategoriesCmdL();
    void CategoriesCmdFromXmlL();
    void SetCurrentLocationL(const TPosition& aPosition);
    void ChangeContextImageL();
    void AttachAIWMenuInterestL();

    void CreateLabelArrayForNormalModeL();
    void CreateLabelArrayForJapaneseModeL();
    /**
     * Gets the length of text fields
     */
    TInt GetFieldTextLengthL(MLmkEditorField& afield);

    //New Functions to update the view dialog when
    //when the landmark fields are changed during the
    //editing mode

    void UpdateTextEditorControlL(MLmkEditorField* aEditorField,
            CPosLandmark* aLandmark, MLmkFieldData* fieldData,
            TUint aFiledType, TInt aPrevControlId, TInt aPos);
    void UpdateCoordinateEditorControlL(MLmkEditorField* aEditorField,
            MLmkFieldData* fieldData, TReal aValue, TInt aPrevControlId,
            TInt aPos);
    void UpdateCategoryEditorControlL(MLmkEditorField* aEditorField,
            MLmkFieldData* fieldData, TInt aPrevControlId, TInt aPos,
            CPosLandmark& aLandmark);
    void UpdateNumberEditorControlL(MLmkEditorField* aEditorField,
            MLmkFieldData* fieldData, TReal32 aValue, TInt aPrevControlId,
            TInt aPos);

    void UpdateNameEditorControlL(MLmkEditorField* aEditorField,
            CPosLandmark* aLandmark, MLmkFieldData* fieldData);
    void UpdateDesEditorControlL(MLmkEditorField* aEditorField,
            CPosLandmark* aLandmark, MLmkFieldData* fieldData,
            TInt aPrevControlId, TInt aPos);
    
    TBool ShowQueryIfLocationFieldsAlreadyFilledL();

    TBool ValidateContentL();

    void DoNotSaveDataL(const TUint16 aFieldType);

    TBool IsFieldEmptyL(const TUint16 aFieldType);

    TBool CLmkEditorImpl::IsInvalidUriL();

    void LaunchLmkEditorL();

    void DeleteSelfL();
	
    void UpdateCatInfoNotePosition();
	
    TBool IsCategoryField();

    /**
     * Clears the lat/long/pos acc/alt acc fields
     */
    void ClearPositionInfoL();

protected:
    /**
     * Checks if position fields do not have value
     */
    TBool ArePositionFieldEmptyL();

private:
#ifdef RD_SCALABLE_UI_V2
    /**
     * Handles touch click command , only for editor
     */
    void HandleEditorCommandL();
#endif //RD_SCALABLE_UI_V2
    MLmkEditorField* InsertLineL(MLmkFieldData* aFieldData,
            TInt aPrevControlId);
    void DeleteLineL(MLmkEditorField* aEditorField, TInt aCtrlId, TInt aPos);
    MLmkEditorField* AddOrDeleteLineL(MLmkEditorField* aEditorField,
            MLmkFieldData* aFieldData, TInt aPrevControlId, TInt aCtrlId,
            TInt aPos);
    static TInt MessageQueryCallBack(TAny* aPtr);

private:
    //from MLmkListProviderObserver
    void HandleListProviderEvent(TLmkListProviderEventType aEvent);

    void HandleListProviderError(TInt aError);

protected:
    //overriden virtual from CCoeControl
    virtual void ActivateL();
	
public:
    // Added for category name support
    void SetCategoryName(const TDesC& aCategoryName);
    //This function is called when moving from view mode to
    //edit mode, which field to be focussed/
    void SetFieldTypeToFocusInEditMode(TUint aFieldType);

    /**
     * This function checks if the editing dialog is launched
     * from the lmk viewer dialog
     */
    TBool IsDlgEditing();

    /**
     * This function sets the mode of the editor dialog as
     * editing, when launched from lmk viewer dialog
     * This is to avoid crash in ActivateL
     */
    void SetDlgEditing(TBool aEditing);
    // For Landscape Support
    void HandleResourceChange(TInt aType);
	
protected:
    //for MSK
    virtual void UpdateMskOnArrowMoveForViewerL();
    void AddMskCommandL(TInt aResourceId, TLmkAppCmdId aCommandId);
    void DimmMiddleSoftKey();

protected:
    // Data
    /// Reference to database
    CPosLandmarkDatabase& iDb;

    /// Ref: landmark sender
    CLmkSender& iSender;

    /// Ref: CPosLmPartialReadParameters
    CPosLmPartialReadParameters* iPartialLmk;

    /// TInt
    TInt iResourceOffset;

    /// Own: landmark UI utils
    CLmkUiUtils* iLmkUiUtils;

    /// Own:
    CLmkFields* iLmkFields;

    /// Own:
    RPointerArray<HBufC> iLabelArray;

    /// Own:
    CLmkEditorFieldArray* iEditorFieldArray;

    TLmkEditorMode iEditorMode;

    ///Own:
    CLmkLocationService* iLocationService;

    TCoeHelpContext iHelpContext;
    TBool iCalegorySelectionFlag;
    RArray<TCategoryName> iCategoryNames;

    ///Ref:
    // Used to return the created Landmark Id Value.
    // Don't allocate or delete memory or set to NULL;
    // The variable might be a local stack variable.
    // If no landmarks is created then set it to -1 or if successful
    // then set it to the Landmark Id value.
    TPosLmItemId* iLandmarkItemId;

    /// Own: context menu bar
    CEikMenuBar* iContextMenuBar;
    CLmkGoToURLCmd* iLmkGoToURLCmd;
    CLmkCallCmd* iLmkCallCmd;
    // Flag to determine if Map and Navigation options needs to be shown
    TBool iMapNavFeature;
    // Current Input language. (Used for Japanese)
    TBool iJapaneseInputMode;
    CPosLandmark* iLandmark;

    // Own: Pointer to map and navigation interface
    CLmkMapNavigationInterface* iMapNavInterface;
    // Flag which indicates receive state of landmark editor dialog
    // Set to true for receive mode
    TBool iIsLmkReceiveMode;
    TLmkAppCmdId iMskCmdId;
    TInt iCurrentResourceId;

    // It will be ETrue when coordinates has to be hidden.
    TBool iIsHideCoordinate;

    // It will be ETrue when help has to be hidden.
    TBool iIsHideHelp;
    
private:
    // added data members
    // holds the field type of the current focussed item in the dialog
    TUint iFieldTypeToFocus;
    // Set true when editor dialog is launched from view dialog.
    TBool iIsEditing;

    TBool iBackspaceStartForWebAddressField;

    TBool iResetWebAddrSize;

    CAknInfoPopupNoteController* iCatInfoPopupNote;
    // Own:
    CLmkListProviderBase* iListProvider; 
    
    CLmkEditorImpl* iEditor;

    CLmkCentralRepository* iRepository;

    /**
     * Wait dialog launcher
     * Owns
     */
    CLmkWaitDialogLauncher* iWaitDialogLauncher;

    TBool iIsEndKey;

    TBool iIsEditableField;

    // To check the ownership of iLandmark
    TBool iIsLmOwned;
    // To Check category selector launched or not
    TBool iIsCategorySelctorOpen;
    };
#endif      // CLMKEDITORIMPL_H

// End of File
