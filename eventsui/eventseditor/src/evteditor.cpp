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
* Description:   Form class for creating events.
*
*/

#include <evtmgmteditorui.rsg>
#include <avkon.rsg>
#include <avkon.hrh>
#include <eikedwin.h>
#include <eikmenup.h>
#include <eikenv.h>
#include <aknlistquerydialog.h>
#include <StringLoader.h>
#include <eikaufty.h>
#include <AknPopupFieldText.h> 
#include <eikenv.h>
#include <eikspane.h>       // Status pane
#include <akntitle.h>       // CAknTitlePane
#include <aknuniteditor.h> 
#include <e32math.h>
#include <AknIconArray.h>       // Icon Array
#include <evteditor.mbg>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <data_caging_path_literals.hrh>
#include <gulicon.h>            // Gul Icon
#include <eikapp.h>
#include <eikenv.h>
#include <barsread.h>               // For TResourceReader
#include <bautils.h> 
#include <eikserverapp.h>
#include <aknappui.h>
#include <AknUtils.h>
#include <aknnotewrappers.h>
#include <hlplch.h>

#include "evteditor.h"
#include "evtmgmteditorui.hrh"
#include "evtevent.h"
#include "evttoneaction.h"
#include "evtdebug.h"
#include "evteditorconsts.h"
#include "evtmgmtuiringtonecomponent.h"
#include "evtdefs.h"

// ---------------------------------------------------------------------------
// CEvtEditor::NewL()
// @param[in] aIsEditMode - It sets mode of editor. 
//                          If true, edit mode is set else view mode is set. 
// @param[in] aEvent - The event to be displayed on editor
//                     Empty if new event is going to be created.
// @param[in] aTriggerLocation - Coordinate based location details of 
//                                  the event to be displayed on editor. 
// @param[in] aExitCmdHandler - It handles all commands which may result into closing editor.
// ---------------------------------------------------------------------------
//
CEvtEditor* CEvtEditor ::NewL(TBool                             aIsEditable, 
                             CEvtEvent&                         aEvent, 
                             CEvtMgmtUiEngine&                  aEventEngine,
                             TBool                              aDisplayExitMenuItem
                             )
    {
    CEvtEditor * self = new (ELeave) CEvtEditor(aIsEditable, aEvent, aEventEngine, aDisplayExitMenuItem );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtEditor::~CEvtEditor()
// ---------------------------------------------------------------------------
//
CEvtEditor::~CEvtEditor()
    {  
	EVTUIDEBUG("+ CEvtEditor::~CEvtEditor()");
    // Close the resource loader
    iResourceLoader.Close();
       
    if(iCmdHandler)
        {
        delete iCmdHandler;
        iCmdHandler = NULL;
        }
    
    delete iSysOfMeas;
    iSysOfMeas = NULL;
	EVTUIDEBUG("- CEvtEditor::~CEvtEditor()");
    }

// ---------------------------------------------------------------------------
// CEvtEditor ::ConstructL()
// ---------------------------------------------------------------------------
//
void CEvtEditor ::ConstructL()
    {
	EVTUIDEBUG("+ CEvtEditor::ConstructL()");
    // Open the Resource File
    OpenLocalizedResourceFileL( KEvtEditorResFileName, iResourceLoader );
       
    CAknForm::ConstructL(R_EVTMGMTUI_CREATEEVENTFORM_MENUBAR);    
    iCmdHandler = CEvtMgmtUiEditorCmdHandler::NewL( iEvent, 
                                                    *this,
                                                    iEventEngine );
    
    // Observer for a change in System of Measurement
    iSysOfMeas = CEvtMgmtUiSysOfMeasurement::NewL(*this);
	EVTUIDEBUG("- CEvtEditor::ConstructL()");
    }

// ---------------------------------------------------------------------------
// CEvtEditor ::CEvtEditor()
// ---------------------------------------------------------------------------
//
CEvtEditor::CEvtEditor( TBool                           aIsEditable, 
                        CEvtEvent&                      aEvent,
                        CEvtMgmtUiEngine&               aEventEngine,
                        TBool                           aDisplayExitMenuItem)
    :iIsEditMode( aIsEditable ), 
    iEvent( aEvent ), 
    iResourceLoader( *CCoeEnv::Static()),
    iEventEngine( aEventEngine ),
    iDisplayExitMenuItem( aDisplayExitMenuItem )
    {
    iEventAttributeMask = 0;
    }

// ---------------------------------------------------------------------------
// Derived from CEikDialog
// CEvtEditor ::OkToExitL()
// ---------------------------------------------------------------------------
//
TBool CEvtEditor::OkToExitL( TInt aButtonId )
    {
	EVTUIDEBUG1("+ CEvtEditor::OkToExitL() - %d", aButtonId );
    switch(aButtonId)
        {
        case EAknFormCmdEdit: // when MSK is pressed
            {
            //Handle MSK operation based on focused line.
            HandleMSKL(); 
            //dont close editor
            return EFalse;
            }
        case EAknSoftkeyOptions: //when LSK is pressed
            {
            //display menu specific to editor
            DisplayMenuL();
            //don't close editor
            return EFalse;
            }
        case EAknSoftkeyDone: // when RSK is pressed in edit mode
            {
            return ProcessDoneCommandL();
            }
        default: // close editor in all other cases
            {
            return ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// CEvtEditor::ProcessDoneCommandL()
// It checks if event is empty.
// @ret true if event is empty else false
// ---------------------------------------------------------------------------
//
TBool CEvtEditor::ProcessDoneCommandL()
    {                
    //validate event, if valid event, display 'Event status' popup dialog
    // and perform selected operation.          
    if( IsEmptyEventL() || !iEventAttributeMask )
        {
        return ETrue;
        }                
    else
        {
        //update attribute mask so that only those attributes of event will be modified                    
        iCmdHandler->SetEventAttributeMask( iEventAttributeMask );

        if(EActive == iEvent.EventStatus())
            {
            if(MandatoryFieldsFilled())
                {
                iCmdHandler->HandleEditorCmdL( EEvtEditorCmdActivate );
                }
            else
                {
                HBufC* infoMsg = StringLoader::LoadLC(R_LOCEV_NOTE_MISSED_INFO );
                CAknInformationNote* infoNote = new(ELeave) CAknInformationNote( ETrue );
                infoNote->ExecuteLD( *infoMsg  );
                CleanupStack::PopAndDestroy();
                }
            }
        else
            {
            iCmdHandler->HandleEditorCmdL( EEvtEditorCmdSaveDraft );
            }
            
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CEvtEditor::IsEmptyEventL()
// It checks if event is empty.
// @ret true if event is empty else false
// ---------------------------------------------------------------------------
//
TBool CEvtEditor::IsEmptyEventL()
    {    
    //if eventid is not zero, it means existing event is opened in editor
    //hence it is not empty event.
    if(iEvent.EventId())
        return EFalse;

    // Check for Default Action
    TBool defaultAction(EFalse);
    if( iEvent.HasAction() )
        {            
        CEvtToneAction* tone = CEvtToneAction::NewLC();  
        tone->InternalizeL( iEvent.Action() );  
        CEvtMgmtUiRingtoneComponent* ringtoneComponent = CEvtMgmtUiRingtoneComponent::NewLC( KNullDesC() );
        if( ringtoneComponent->DefaultRingtoneL().Compare( tone->FileName() ) == 0 && 
                !tone->ToneLoop() )
            {
            defaultAction = ETrue;
            }
        CleanupStack::PopAndDestroy( 2 ); // tone, ringtoneComponent
        }
    
    //if eventid is zero, it means new event is about to create.
    //hence check if something is entered by the user.    
    if( 6 == GetNumberOfLinesOnPage(0) && //if only subject,place and repeat are present
        0 == iEvent.Subject().Compare(KNullDesC) && //if subject is empty
        0 == iEvent.Place().Compare(KNullDesC) && // if place is default
        0 == iEvent.Repeat() && // if repeat is OFF
        EDraft == iEvent.EventStatus() &&
        defaultAction ) // if Status is Draft
        {       
        return ETrue; 
        }  
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CEvtEditor ::LoadFormValuesFromDataL()
// Called when the form is  executed and when the user chooses to discard changes
// in QuerySaveChangesL (via  DoNotSaveFormDataL).
// ---------------------------------------------------------------------------
// 
void CEvtEditor::LoadFormValuesFromDataL()
    {
	EVTUIDEBUG("+ CEvtEditor::LoadFormValuesFromDataL()");
    if(iEvent.EventId()==0) //if create event
        {
        CEikEdwin* placeEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdPlace));            
            if(placeEditor)
                {   
                if( iEvent.Place().Compare(KNullDesC) == 0 )  
                    {
                    HBufC* define = StringLoader::LoadLC(R_EVTUI_CREATENEW_FORM_DEFINED);   
                    placeEditor->SetTextL(define);
                    CleanupStack::PopAndDestroy(define);
                    }               
                } 
            
        CAknPopupFieldText* repeatPopupFieldText = (CAknPopupFieldText*)Control( EEvtMgmtUiDlgCIdRepeat );
            if(repeatPopupFieldText)
                {  
                repeatPopupFieldText->SetCurrentValueIndex(1);      
                }

        // Set the Default Ringtone
        CEvtMgmtUiRingtoneComponent* ringtoneComponent = CEvtMgmtUiRingtoneComponent::NewLC( KNullDesC() );
        if( ringtoneComponent->DefaultRingtoneL().Compare( KNullDesC ) == 0 )
            {
            iEvent.SetActionL( NULL );
            }
        else
            {
            CEvtToneAction* tone = CEvtToneAction::NewLC();
            tone->SetFileNameL( ringtoneComponent->Ringtone() );
            tone->SetToneLoop( EFalse );  
      
            CEvtAction* action = CEvtAction::NewLC();
            tone->ExternalizeL( *action );
            iEvent.SetActionL( action ); // ownership is transferred.
            CleanupStack::Pop( action ); // action  

            CleanupStack::PopAndDestroy( ); // tone        
            }
        CleanupStack::PopAndDestroy( ); // ringtoneComponent   
        
        SetRingtoneL();
        
        // Set Default Status
        iEvent.SetEventStatus(EDraft);
        SetStatusL();
        
        return;
        }
    
   //assign initial values
   	CEikEdwin* subjectEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdSubject));
    if(subjectEditor )
        {        
		if( iEvent.Subject().Compare(KNullDesC) != 0 )
			{
			HBufC* subject = iEvent.Subject().AllocLC();   
			subjectEditor->SetTextL( subject );
			CleanupStack::PopAndDestroy(subject);
			}
        }
   
    CEikEdwin* placeEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdPlace));            
    if(placeEditor)
        {   
        if( iEvent.Place().Compare(KNullDesC) == 0 )  
	        {
			HBufC* define = StringLoader::LoadLC(R_EVTUI_CREATENEW_FORM_DEFINED);   
			placeEditor->SetTextL(define);
			CleanupStack::PopAndDestroy(define);
	        }
	    else
		    {
			HBufC* place = iEvent.Place().AllocLC();   
			placeEditor->SetTextL(place);
			CleanupStack::PopAndDestroy(place);
            EVTUIDEBUG(" Setting Radius");
            InsertRadiusDlgLineL();
		    }
	    }     
    
    CAknPopupFieldText* repeatPopupFieldText = (CAknPopupFieldText*)Control( EEvtMgmtUiDlgCIdRepeat );
    if(repeatPopupFieldText)
        {    
        if(iEvent.Repeat())
            repeatPopupFieldText->SetCurrentValueIndex(0);
         else
             repeatPopupFieldText->SetCurrentValueIndex(1);      
        }
    
    // Set Ringtone
    SetRingtoneL();
    
    // Set Status
    SetStatusL();
       
    if( iEvent.Description().Compare(KNullDesC)!=0 )
        {
        InsertDesDlgLineL();
        SetDescL();
        }
	EVTUIDEBUG("- CEvtEditor::LoadFormValuesFromDataL()");
    }
 
// ---------------------------------------------------------------------------
// Derived from CEikDialog
// CEvtEditor ::PreLayoutDynInitL()
// ---------------------------------------------------------------------------
// 
void CEvtEditor::PreLayoutDynInitL()
    {
	EVTUIDEBUG("+ CEvtEditor::PreLayoutDynInitL()");
    CAknForm::PreLayoutDynInitL();
    
    //Load existing event if it is opened in viewe mode  
    LoadFormValuesFromDataL();
	EVTUIDEBUG("- CEvtEditor::PreLayoutDynInitL()");
    }
 
// ---------------------------------------------------------------------------
// Derived from CEikDialog
// CEvtEditor ::PostLayoutDynInitL()
// ---------------------------------------------------------------------------
// 
void CEvtEditor::PostLayoutDynInitL()
    {
	EVTUIDEBUG("+ CEvtEditor::PostLayoutDynInitL()");
   CAknForm::PostLayoutDynInitL();
   
     if(iEvent.Subject().Compare(KNullDesC) == 0)
        {
        if(iEvent.EventId())
            {
            TPtrC ptr = KNullDesC();
            MakeTitleL(ptr);  
            }
        else
            {
            HBufC* navititle = StringLoader::LoadLC( R_EVTUI_CREATENEW_FORM_DEFAULTNAVITITLE);
            MakeTitleL(*navititle);  
            CleanupStack::PopAndDestroy(navititle);
            }
        }
    else
        {
        TPtrC ptr = iEvent.Subject();
        MakeTitleL( ptr);
        }
            
    ChangeRSKCaptionL();
    SetEditableL(iIsEditMode);
	EVTUIDEBUG("- CEvtEditor::PostLayoutDynInitL()");
    }
// ---------------------------------------------------------------------------
// Derived from CAknForm
// CEvtEditor ::DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//  
void CEvtEditor ::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    CAknForm::DynInitMenuPaneL(aResourceId,aMenuPane);

    if (aResourceId == R_AVKON_FORM_MENUPANE)
        {  
        
        // Disable all the unwanted AKnForm's Default Menu Items 
        aMenuPane->SetItemDimmed(EAknFormCmdLabel, ETrue);
        aMenuPane->SetItemDimmed(EAknFormCmdAdd, ETrue);
        aMenuPane->SetItemDimmed(EAknFormCmdDelete, ETrue);
        aMenuPane->SetItemDimmed(EAknFormCmdSave, ETrue);
        
        if(iDisplayExitMenuItem)
            aMenuPane->SetItemDimmed(EAknCmdExit, EFalse);
        else
            aMenuPane->SetItemDimmed(EAknCmdExit, ETrue);
        
        if( IsEditable() )
	        {
	        // "Edit" present only in Viewer mode
	        aMenuPane->SetItemDimmed(EAknFormCmdEdit, ETrue);
	        
	        // "Discard Changes" has to be present only in Edit mode
	        // and if some changes are made
	        if( IsEmptyEventL() || !iEventAttributeMask || !iEvent.EventId())
	            aMenuPane->SetItemDimmed(EEvtEditorCmdDiscard, ETrue);
	        else
                aMenuPane->SetItemDimmed(EEvtEditorCmdDiscard, EFalse);
            
            // Initially display all the Editor specific Menu items which 
            // are dimmed based on focus and context
			aMenuPane->SetItemDimmed(EEvtEditorCmdAddDesc, ETrue);
			aMenuPane->SetItemDimmed(EEvtEditorCmdRemoveDesc, ETrue);
			aMenuPane->SetItemDimmed(EEvtEditorCmdSetPlace, ETrue);
			aMenuPane->SetItemDimmed(EEvtEditorCmdEditDesc, ETrue);

			if(NULL == ControlOrNull(EEvtMgmtUiDlgCIdAssignTone))
			    aMenuPane->SetItemDimmed(EEvtEditorCmdAssignTone, EFalse);
			else
			    aMenuPane->SetItemDimmed(EEvtEditorCmdAssignTone, ETrue);
	        
	        // "Add Description" / "Remove Description" is displayed based
	        // on Decsription control's existance in Form
	        if( NULL == ControlOrNull(EEvtMgmtUiDlgCIdDesc) )
	            {
	            aMenuPane->SetItemDimmed(EEvtEditorCmdAddDesc, EFalse);
	        	aMenuPane->SetItemDimmed(EEvtEditorCmdRemoveDesc, ETrue);
	            }
	        else
	            {
	            aMenuPane->SetItemDimmed(EEvtEditorCmdAddDesc, ETrue);
	        	aMenuPane->SetItemDimmed(EEvtEditorCmdRemoveDesc, EFalse);
	            }
            
	        // Menu items whose existance depends on the focussed control
	        switch( IdOfFocusControl() )
	            {
	            case EEvtMgmtUiDlgCIdPlace:
	                {
	                aMenuPane->SetItemDimmed(EEvtEditorCmdSetPlace, EFalse);
	                break;
	                }
	            case EEvtMgmtUiDlgCIdDesc:            
	                {
	                aMenuPane->SetItemDimmed(EEvtEditorCmdEditDesc, EFalse);
	                break;
	                }    
	            case EEvtMgmtUiDlgCIdAssignTone:              
	                {
	                aMenuPane->SetItemDimmed(EAknFormCmdEdit, ETrue);
	                aMenuPane->SetItemDimmed(EEvtEditorCmdAssignTone, EFalse);
	                break;
	                } 
	            case EEvtMgmtUiDlgCIdRepeat:
	            case EEvtMgmtUiDlgCIdAudioLoop:   
              case EEvtMgmtUiDlgCIdStatus:            
	                {
	        				aMenuPane->SetItemDimmed(EAknFormCmdEdit, EFalse);
        					aMenuPane->SetItemTextL(EAknFormCmdEdit,R_EVTUI_CREATENEW_FORM_CMDCHANGE);
	                break; 
	                }
	            default:
	                {
	                break;
	                }                
	                   
	            }//switch
 
            // Activate and Draft are absent in Edit mode
            aMenuPane->SetItemDimmed(EEvtEditorCmdActivate, ETrue);
            aMenuPane->SetItemDimmed(EEvtEditorCmdSaveDraft, ETrue);
	            
	        // Editor/Viewer specific menu items    
    		aMenuPane->SetItemDimmed(EEvtEditorCmdNavigateToPlace, ETrue);
    		aMenuPane->SetItemDimmed(EEvtEditorCmdShowOnMap, ETrue);
    		aMenuPane->SetItemDimmed(EEvtMgmtUiEventUiPosSettings, EFalse);
	        }
	    else   // View Mode
	        {
	        // "Edit" present only in Viewer mode
	        aMenuPane->SetItemDimmed(EAknFormCmdEdit, EFalse);
            
            // "Discard Changes" has to be absent only in View mode
            aMenuPane->SetItemDimmed(EEvtEditorCmdDiscard, ETrue);
            
            // Dim all the Editor specific Menu items.
			aMenuPane->SetItemDimmed(EEvtEditorCmdAddDesc, ETrue);
			aMenuPane->SetItemDimmed(EEvtEditorCmdRemoveDesc, ETrue);
			aMenuPane->SetItemDimmed(EEvtEditorCmdSetPlace, ETrue);
			aMenuPane->SetItemDimmed(EEvtEditorCmdEditDesc, ETrue);
			aMenuPane->SetItemDimmed(EEvtEditorCmdAssignTone, ETrue);
            
            // Menu items that depend on Event's status
    		switch( iEvent.EventStatus() )
    			{
    			case EActive:
    			    {
            		aMenuPane->SetItemDimmed(EEvtEditorCmdActivate, ETrue);
            		aMenuPane->SetItemDimmed(EEvtEditorCmdSaveDraft, EFalse);
    				break;
    			    }
    			case EDraft:
    			    {
    			    if(MandatoryFieldsFilled())
    			        aMenuPane->SetItemDimmed(EEvtEditorCmdActivate, EFalse);
    			    else
    			        aMenuPane->SetItemDimmed(EEvtEditorCmdActivate, ETrue);
            		aMenuPane->SetItemDimmed(EEvtEditorCmdSaveDraft, ETrue);
    				break;
    			    }
                case ECompleted:
                    {
                    aMenuPane->SetItemDimmed(EEvtEditorCmdActivate, EFalse);
                    aMenuPane->SetItemDimmed(EEvtEditorCmdSaveDraft, EFalse);
                    break;
                    }
    			default:
    				break;
    			}
    			
	        // if location is defined
    		if( Math::IsNaN( iEvent.Location().Latitude() ) || Math::IsNaN( iEvent.Location().Latitude() )
    			|| iEvent.Location().Latitude() == 0 || iEvent.Location().Latitude() == 0 ) 
    		    {
        		aMenuPane->SetItemDimmed(EEvtEditorCmdNavigateToPlace, ETrue);
        		aMenuPane->SetItemDimmed(EEvtEditorCmdShowOnMap, ETrue);
    		    }
    		else
    		    {
    		    aMenuPane->SetItemDimmed(EEvtEditorCmdNavigateToPlace, EFalse);
                aMenuPane->SetItemDimmed(EEvtEditorCmdShowOnMap, EFalse);
                 }
    		aMenuPane->SetItemDimmed(EEvtMgmtUiEventUiPosSettings, ETrue);
	        }

        }//if
    }
// -----------------------------------------------------------------------------
// Derived from CEikDialog
// CEvtEditor::HandleControlStateChangeL()
// Called when focus is moved from the specified line.
// -----------------------------------------------------------------------------
//
void CEvtEditor::HandleControlStateChangeL(TInt aControlId)
    {
	EVTUIDEBUG1("+ CEvtEditor::PostLayoutDynInitL() - %d", aControlId );
    switch(aControlId)
        {
        case EEvtMgmtUiDlgCIdSubject:
            {            
            iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeSubject;
            
            //Retrieve subject text 
            CEikEdwin* subjectEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdSubject));            
            if(subjectEditor)
                {                       
                HBufC* subject = subjectEditor->GetTextInHBufL();
                if( subject )
	                {
	                CleanupStack::PushL(subject);
	                iEvent.SetSubjectL(*subject);
                	MakeTitleL(*subject); 
	                CleanupStack::PopAndDestroy(subject);
	                }
	            else
	                {
	                iEvent.SetSubjectL(KNullDesC);
	                HBufC* navititle = StringLoader::LoadLC( R_EVTUI_CREATENEW_FORM_DEFAULTNAVITITLE);
	                MakeTitleL(*navititle);             
	                CleanupStack::PopAndDestroy(navititle);	
	                }
                }              
            break;
            } 
        case EEvtMgmtUiDlgCIdRadius:
            {
            iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeRadius;
            CAknUnitEditor* radiusEd =
                 (CAknUnitEditor*)Control( EEvtMgmtUiDlgCIdRadius );
            if(radiusEd)
                {
                //convert radius in km unit
                TReal val(0);
        		if( EAknUnitEditorMile == radiusEd->Unit() )
        			{        			
        			val = radiusEd->Value();
        			val /= KUnitConversion;        		       			       			
        			}
        		if( EAknUnitEditorKilometer ==radiusEd->Unit() )
        		    val = radiusEd->Value();
        		
        		//validate radius value
        		if( val<KMinRadiusInKm ) //if less than minima, set to minima
        		    iEvent.SetRadius( KMinRadiusInKm );
        		else if( val>KMaxRadiusInKm ) //if greater than maxima, set to maxima
        		    iEvent.SetRadius( KMaxRadiusInKm );
        		else
        		    iEvent.SetRadius( val );
                }
            break;
            } 
        case EEvtMgmtUiDlgCIdRepeat:
           {
           iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeRepeat;
           CAknPopupFieldText* repeatPopupFieldText =
                 (CAknPopupFieldText*)Control( EEvtMgmtUiDlgCIdRepeat );
           if(repeatPopupFieldText)
               {
               if(repeatPopupFieldText->CurrentValueIndex() == 0)               
                   iEvent.SetRepeat( ETrue );
               else
                   iEvent.SetRepeat( EFalse );   
               }
           break;
           }
        case EEvtMgmtUiDlgCIdAudioLoop:
           {
           iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeAudioLoop;                 
           CAknPopupFieldText* audioLoopPopupFieldText =
                 (CAknPopupFieldText*)Control( EEvtMgmtUiDlgCIdAudioLoop );
           if(audioLoopPopupFieldText)
               {
               CEvtToneAction* tone = CEvtToneAction::NewLC();
               tone->InternalizeL( iEvent.Action() );
               if( audioLoopPopupFieldText->CurrentValueIndex() == 0)
                   tone->SetToneLoop( EFalse );
               else
                   tone->SetToneLoop( ETrue );   
               
                CEvtAction* action = CEvtAction::NewLC();
                tone->ExternalizeL( *action );
                iEvent.SetActionL( action ); //ownership is transferred.
                CleanupStack::Pop( action );
                CleanupStack::PopAndDestroy( tone );
               }
           break;
           }    
        case EEvtMgmtUiDlgCIdStatus:
           {
           iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeStatus;                 
           CAknPopupFieldText* statusPopupFieldText =
                 (CAknPopupFieldText*)Control( EEvtMgmtUiDlgCIdStatus );
           if(statusPopupFieldText)
               {
               switch(statusPopupFieldText->CurrentValueIndex())
                   {
                   case 0:
                       iEvent.SetEventStatus(EActive);
                       break;
                   case 1:
                       iEvent.SetEventStatus(EDraft);
                       break;
                   case 2:
                       iEvent.SetEventStatus(ECompleted);
                       break;
                   default:
                       iEvent.SetEventStatus(EDraft);
                       break;
                   }
               }
           break;
           }                
        }
	EVTUIDEBUG("- CEvtEditor::PostLayoutDynInitL()");
    }


// -----------------------------------------------------------------------------
// CEvtEditor::ProcessCommandL()
// It handles command events
// -----------------------------------------------------------------------------
//
void CEvtEditor::ProcessCommandL(TInt  aCommandId)
    {      
	EVTUIDEBUG1("+ CEvtEditor::ProcessCommandL() - %d", aCommandId );         
    CAknForm::ProcessCommandL(aCommandId); //to hide menu  
 
    
    switch(aCommandId)
        {          
        case EEvtEditorCmdAddDescNew:
        case EEvtEditorCmdAddDescFromExisting:            
        case EEvtEditorCmdEditDesc:
        case EEvtEditorCmdAssignTone:
        case EEvtEditorCmdShowOnMap:
        case EEvtEditorCmdNavigateToPlace:
        case EEvtMgmtUiEventUiPosSettings:
        case EEvtEditorCmdSetPlace:
            {      
            iCmdHandler->HandleEditorCmdL( aCommandId );
            break;
            }
        case EEvtEditorCmdRemoveDesc:
            {
            RemoveDesDlgLineL();
            iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeDesc;
            DrawDeferred();
            break;
            }
        case EEvtEditorCmdDelete:    
            {
            if( IsEmptyEventL() )
                TryExitL( EAknSoftkeyBack );
            else if( DisplayConfirmationQueryDlgL(R_EVTUI_CREATENEW_FORM_CONFIRMATION_DELETEEVENT) )
                {
                if( !iEvent.EventId() )
                    TryExitL( EAknSoftkeyBack );
                else
                    iCmdHandler->HandleEditorCmdL( aCommandId );
                }
            break;
            }
        case EEvtEditorCmdActivate:
            {         
            if(IsEditable())
                ProcessDoneCommandL();
            else
                {            
                iEvent.SetEventStatus(EActive);
                iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeStatus;          
                iCmdHandler->SetEventAttributeMask( iEventAttributeMask );
                iCmdHandler->HandleEditorCmdL( aCommandId );
                }
            break;
            }    
        case EEvtEditorCmdSaveDraft:
            {         
            if(IsEditable())
                ProcessDoneCommandL();
            else
                {            
                iEvent.SetEventStatus(EDraft);
                iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeStatus;          
                iCmdHandler->SetEventAttributeMask( iEventAttributeMask );
                iCmdHandler->HandleEditorCmdL( aCommandId );
                }
            break;
            }         
        case EAknCmdExit:
            {
            // The User has exited the application using the Exit option from
            //.Options menu.  But, thats possible only if we exit the application
            // with EAknCmdExit.
            // A bit of convoluted logic but needed because we are using the
            // same components for the Settings UI Server as well as LC.
            
           	CEikAppServer* server = iEikonEnv->AppServer();
			if ( server )
				{
				server->NotifyServerExit( EAknCmdExit );
				}
		    CAknAppUi* appUi = static_cast< CAknAppUi* >( iEikonEnv->AppUi());
		    appUi->RunAppShutter();            
            break;
            }
        case EAknFormCmdEdit:
            {
            HandleMSKL(); 
            if( !iIsEditMode )
                {
                iIsEditMode = ETrue;
                }               
            DrawDeferred();             
            break;
            }
        case EEvtEditorCmdDiscard:
            if( !DisplayConfirmationQueryDlgL(R_LOCEV_QUERY_DISCARD_CHANGES) )
                break;
        case EAknSoftkeyBack:
            {
            TryExitL( EAknSoftkeyBack );
            break;
            }
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                static_cast<CAknAppUi*>(
                    iCoeEnv->AppUi() )->AppHelpContextL() );
            break;
            }
        default:
            {
            //panic code
            break;
            }
        }  
	EVTUIDEBUG("- CEvtEditor::ProcessCommandL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::SetMopParent
// -----------------------------------------------------------------------------
//
void CEvtEditor::SetMopParent(
    MObjectProvider* aParent )
    {        
    CCoeControl::SetMopParent( aParent );
    }

// -----------------------------------------------------------------------------
// CEvtEditor::SetHelpContext
// -----------------------------------------------------------------------------
//
void CEvtEditor::SetHelpContext( TCoeHelpContext aContext )
    {    
    iHelpContext = aContext;
    }

// -----------------------------------------------------------------------------
// CEvtEditor::ExecuteL
// -----------------------------------------------------------------------------
//
TInt CEvtEditor::ExecuteLD()
    {    
	return CAknForm::ExecuteLD( R_EVTMGMTUI_CREATEEVENT_FORM_DIALOG );    	
    }

// -----------------------------------------------------------------------------
// CEvtEditor::GetHelpContext
// -----------------------------------------------------------------------------
//
void CEvtEditor::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    if ( iHelpContext.iContext.Length() > 0 )
        {
        aContext.iContext = iHelpContext.iContext;
        aContext.iMajor = iHelpContext.iMajor;
        }
    }
    
// -----------------------------------------------------------------------------
// CEvtEditor::DisplayConfirmationQueryDlg()
// It shows confirmation query dialog
// -----------------------------------------------------------------------------
//
TBool CEvtEditor::DisplayConfirmationQueryDlgL( TInt aResourceId )
    {
    HBufC* msg = StringLoader::LoadLC(aResourceId);
    TBool response= EFalse;
    CAknQueryDialog* confirmationQuery = CAknQueryDialog::NewL();     
    response = confirmationQuery->ExecuteLD(R_EVTUI_CONFIRMATION_QUERY, *msg);         
    CleanupStack::PopAndDestroy(msg);
    return response;
    }

// -----------------------------------------------------------------------------
// CEvtEditor::ChangeRSKCaptionL()
// It changes caption of RSK key
// -----------------------------------------------------------------------------
//
void CEvtEditor::ChangeRSKCaptionL()
    {
	EVTUIDEBUG("+ CEvtEditor::ChangeRSKCaptionL()");
    //if edit mode then RSK =Done otherwise Back
    CEikButtonGroupContainer* cba   = CEikButtonGroupContainer::Current();
    CleanupStack::PushL( cba );
    if(iIsEditMode)
        cba->SetCommandSetL ( R_EVTMGMTUI_CREATEEVENT_CBA_NOMSK ); 
    else
        cba->SetCommandSetL(R_EVTMGMTUI_CREATEEVENT_CBA_BACK);         
    cba->DrawDeferred();    
    CleanupStack::Pop();//cba  
	EVTUIDEBUG("- CEvtEditor::ChangeRSKCaptionL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::ChangeMSKCaptionL()
// It changes caption of MSK key
// @param aControlId of current focused line
// -----------------------------------------------------------------------------
//
void CEvtEditor::ChangeMSKCaptionL(TInt  aControlId)
    {
	EVTUIDEBUG("+ CEvtEditor::ChangeMSKCaptionL()");
    CEikButtonGroupContainer* cba   = CEikButtonGroupContainer::Current();
    CleanupStack::PushL( cba );
         
    //attribute specific cba
    switch(aControlId)
        {
        case EEvtMgmtUiDlgCIdSubject:
        case EEvtMgmtUiDlgCIdRadius:
            {//Options Done
            cba->SetCommandSetL(R_EVTMGMTUI_CREATEEVENT_CBA_NOMSK);
            break;
            }
        case EEvtMgmtUiDlgCIdRepeat:
        case EEvtMgmtUiDlgCIdAssignTone:
        case EEvtMgmtUiDlgCIdAudioLoop:
        case EEvtMgmtUiDlgCIdStatus:
            { //Options Change Done
            cba->SetCommandSetL(R_EVTMGMTUI_CREATEEVENT_CBA);                
            break;
            }
        case EEvtMgmtUiDlgCIdDesc:
            {//Options Edit Done
            cba->SetCommandSetL(R_EVTMGMTUI_CREATEEVENT_CBA_EDIT);
            break;
            }
        case EEvtMgmtUiDlgCIdPlace:
            {
            if(iEvent.Place().Compare(KNullDesC)==0)//Options Define Done
                cba->SetCommandSetL(R_EVTMGMTUI_CREATEEVENT_CBA_DEFINE);
            else //Options Change Done
                cba->SetCommandSetL(R_EVTMGMTUI_CREATEEVENT_CBA);
            break;
            }
        default:
            break;
        }
    cba->DrawDeferred();    
    CleanupStack::Pop();//cba    
	EVTUIDEBUG("- CEvtEditor::ChangeMSKCaptionL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::HandleMSKL()
// It performs the required action when MSK is pressed
// -----------------------------------------------------------------------------
//
void CEvtEditor::HandleMSKL()
    {
	EVTUIDEBUG("+ CEvtEditor::HandleMSKL()");
    if( !iIsEditMode )
        {
        iIsEditMode = ETrue;
        SetEditableL(ETrue);  
        ChangeRSKCaptionL();
        ChangeMSKCaptionL( IdOfFocusControl() ); 
        return;    
        }
    
    if(IsEditable())
        {
        //attribute specific action
        switch( IdOfFocusControl() )
            {
            case EEvtMgmtUiDlgCIdSubject:
                {
                CEikEdwin* subjectEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdSubject));
                if(subjectEditor )
                    {    
                    subjectEditor->SetCursorPosL(iEvent.Subject().Length(),EFalse);
                    subjectEditor->DrawDeferred();
                    }
                break;
                }
            case EEvtMgmtUiDlgCIdPlace:
                {
                ProcessCommandL(EEvtEditorCmdSetPlace);
                break;
                }
            case EEvtMgmtUiDlgCIdDesc:
                {
                ProcessCommandL(EEvtEditorCmdEditDesc);
                break;
                }
            case EEvtMgmtUiDlgCIdAssignTone:
                {
                ProcessCommandL(EEvtEditorCmdAssignTone);
                break;
                }
            case EEvtMgmtUiDlgCIdRepeat:    
                {
                //ProcessCommandL(EEvtMgmtUiDlgCIdRepeat);
                TogglePopupFieldControlL( EEvtMgmtUiDlgCIdRepeat );
                break;
                }
            case EEvtMgmtUiDlgCIdAudioLoop:
                {              
                //ProcessCommandL(EEvtMgmtUiDlgCIdAudioLoop);
               TogglePopupFieldControlL( EEvtMgmtUiDlgCIdAudioLoop );
                break;
                }  
            case EEvtMgmtUiDlgCIdStatus:
                {      
                CheckStatusL();
                CAknPopupFieldText* statusEditor = static_cast <CAknPopupFieldText*> ( ControlOrNull(EEvtMgmtUiDlgCIdStatus) );
                     if (statusEditor)
                         {  
                         TKeyEvent keyEvent;
                         keyEvent.iCode = EKeyEnter;
                         keyEvent.iModifiers = EModifierPureKeycode;
                         statusEditor->OfferKeyEventL( keyEvent, EEventKey ); 
                         }
                break;
                }                  
            default:
                {  
                //error panic
              break;
                }
            }
        }
	EVTUIDEBUG("- CEvtEditor::HandleMSKL()");
    }
    
// -----------------------------------------------------------------------------
// CEvtEditor::InsertDesDlgLineL()
// It inserts dialog line for descriptor
// -----------------------------------------------------------------------------
//
void CEvtEditor::InsertDesDlgLineL()
    {
	EVTUIDEBUG("+ CEvtEditor::InsertDesDlgLineL()");
    TInt lineIndex;
    if( !ControlOrNull( EEvtMgmtUiDlgCIdRadius ))
        {
        lineIndex = FindLineIndex(* ControlOrNull(EEvtMgmtUiDlgCIdPlace)) + 1;
        }
    else
        {
        lineIndex = FindLineIndex(* ControlOrNull(EEvtMgmtUiDlgCIdRadius))  + 1;
        }
   InsertLineL(lineIndex,R_EVTUI_CREATENEWFORM_DLGLINE_DESC);
   UpdatePageL(ETrue);
	EVTUIDEBUG("- CEvtEditor::InsertDesDlgLineL()");
   }

// -----------------------------------------------------------------------------
// CEvtEditor::RemoveDesDlgLineL()
// It removes dialog line for descriptor
// -----------------------------------------------------------------------------
//
void CEvtEditor::RemoveDesDlgLineL()
    {      
	EVTUIDEBUG("+ CEvtEditor::RemoveDesDlgLineL()");
    if( ControlOrNull(EEvtMgmtUiDlgCIdDesc))
      {
      DeleteLine(EEvtMgmtUiDlgCIdDesc,ETrue);
      iEvent.SetDescriptionL(KNullDesC);
      }               
	EVTUIDEBUG("- CEvtEditor::RemoveDesDlgLineL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::SetDescL()
// It sets text to descriptor edwin
// -----------------------------------------------------------------------------
//
void CEvtEditor::SetDescL()
    {
	EVTUIDEBUG("+ CEvtEditor::SetDescL()");
    CEikEdwin* descEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdDesc));
     if (descEditor)
         {
         TPtrC desc = iEvent.Description();
         descEditor->SetTextL(&desc);  
         UpdatePageL(ETrue);
         }
	EVTUIDEBUG("- CEvtEditor::SetDescL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::SetRadiusL()
// It sets radius to radius editor
// -----------------------------------------------------------------------------
//
void CEvtEditor::SetRadiusL()
    {
	EVTUIDEBUG("+ CEvtEditor::SetRadiusL()");
    CAknUnitEditor* radiusEd =
                      (CAknUnitEditor*)Control( EEvtMgmtUiDlgCIdRadius );
     if(radiusEd)
         {
         NotifySOMChangeL(); // to set unit value
         }
	EVTUIDEBUG("- CEvtEditor::SetRadiusL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::CheckStatusL()
// It Checks for the status and dynamically sets the popup list
// -----------------------------------------------------------------------------
//
void CEvtEditor::CheckStatusL()
    {
    CAknPopupFieldText* statusEditor = static_cast <CAknPopupFieldText*> ( ControlOrNull(EEvtMgmtUiDlgCIdStatus) );
    if( statusEditor && statusEditor->MdcArray()->MdcaCount() == 3 )
        {
        MDesC16Array* statusArray = (MDesC16Array*) (statusEditor->MdcArray());
        CAknQueryValueTextArray* aTextArray = static_cast <CAknQueryValueTextArray*> (statusArray);
        CDesCArray* arr =  aTextArray->Array();      // Delete the previous Array
        arr->Reset();
        arr->Compress();
        HBufC* strActive = StringLoader::LoadLC( R_LOCEV_LIST_VALUE_ACTIVE );
        HBufC* strDraft = StringLoader::LoadLC( R_LOCEV_LIST_VALUE_DRAFT );
        arr->AppendL( *strActive );
        arr->AppendL( *strDraft );
        CleanupStack::PopAndDestroy( strDraft );
        CleanupStack::PopAndDestroy( strActive );   
        
        if(EActive == iEvent.EventStatus())
            statusEditor->SetCurrentValueIndex (0);
        else
            statusEditor->SetCurrentValueIndex (1);
            
        statusEditor->SetFocus(ETrue);
        }
    }

// -----------------------------------------------------------------------------
// CEvtEditor::SetStatusL()
// It sets Status of the event
// -----------------------------------------------------------------------------
//
void CEvtEditor::SetStatusL()
    {
    EVTUIDEBUG("+ CEvtEditor::SetStatusL()");
    //Set Status of the Event
    CAknPopupFieldText* statusEditor = static_cast <CAknPopupFieldText*> ( ControlOrNull(EEvtMgmtUiDlgCIdStatus) );
    if(statusEditor)
        {
        switch(iEvent.EventStatus())
            {
            case EActive:
                statusEditor->SetCurrentValueIndex(0);
                break;
            case EDraft:
                statusEditor->SetCurrentValueIndex(1);
                break;
            case ECompleted:
                statusEditor->SetCurrentValueIndex(2);
                break;
            default:
                statusEditor->SetCurrentValueIndex(0);
                break;
            }
       }
    EVTUIDEBUG("- CEvtEditor::SetStatusL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::SetPlaceL()
// It sets text to place edwin
// -----------------------------------------------------------------------------
//
void CEvtEditor::SetPlaceL()
    {
	EVTUIDEBUG("+ CEvtEditor::SetPlaceL()");
    //Set place dialog line to new place value
    CEikEdwin* placeEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdPlace));
    if (placeEditor)
       { 
       HBufC* place = iEvent.Place().AllocLC();   
       if(place->Compare(KNullDesC)!=0)
           {
           iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributePlace;
           placeEditor->SetTextL(place);   
           placeEditor->DrawDeferred();   
           InsertRadiusDlgLineL();
		   DrawNow();
           } 
       CleanupStack::PopAndDestroy(place);       
       }
	EVTUIDEBUG("- CEvtEditor::SetPlaceL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::SetRingtoneL()
// It sets text to ringtone edwin
// -----------------------------------------------------------------------------
//
void CEvtEditor::SetRingtoneL()
    {
	EVTUIDEBUG("+ CEvtEditor::SetRingtoneL()");
    CEikEdwin* ringtoneEditor = static_cast <CEikEdwin*> ( ControlOrNull(EEvtMgmtUiDlgCIdAssignTone) );
          
    if ( ringtoneEditor )
         {
         if( iEvent.HasAction() )
             {            
             CEvtToneAction* tone = CEvtToneAction::NewLC();  
             tone->InternalizeL( iEvent.Action() );      
             TParsePtrC parse( tone->FileName() );
             TPtrC filename = parse.NameAndExt();
             ringtoneEditor->SetTextL( &filename ); 
             
             // Insert Loop
             InsertAudioLoopDlgLinesL();
             CAknPopupFieldText* audioLoopEditor = static_cast <CAknPopupFieldText*> ( ControlOrNull(EEvtMgmtUiDlgCIdAudioLoop) );
             if(audioLoopEditor)
                 {
                 if(tone->ToneLoop())
                     audioLoopEditor->SetCurrentValueIndex(1);
                 else
                     audioLoopEditor->SetCurrentValueIndex(0);
                 }
             CleanupStack::PopAndDestroy( tone );
             }
         else
             {
             HBufC* strOff = StringLoader::LoadLC( R_EVTUI_RINGTONESELECT_OFF );
             ringtoneEditor->SetTextL( strOff ); 
             CleanupStack::PopAndDestroy( strOff );
             
             // Remove Loop
             RemoveAudioLoopDlgLineL();
             }
         UpdatePageL(ETrue);
         }
    Debug(_L("CEvtEditor::SetRingtoneL()"));
      if( iEvent.HasAction() )
          {        
          CEvtToneAction* tone = CEvtToneAction::NewLC();
          TFileName debug;
          tone->InternalizeL( iEvent.Action() );
          debug.Copy(_L("Ringtone: "));
          debug.Append(tone->FileName());
          Debug(debug);
          debug.Copy(_L("Audio Loop: "));
          debug.AppendNum(tone->ToneLoop());
          Debug(debug);            
          CleanupStack::PopAndDestroy(tone);
          }
	EVTUIDEBUG("- CEvtEditor::SetRingtoneL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::MandatoryFieldsFilled()
// It checks if all mandatory fields are filled
// @return ETrue if all mandatory fields are filled
//         KFalse otherwise
// -----------------------------------------------------------------------------
//
TBool CEvtEditor::MandatoryFieldsFilled()
    {   
    return (iEvent.Subject().Compare(KNullDesC)!=0 && iEvent.Place().Compare(KNullDesC)!=0);
    }

// -----------------------------------------------------------------------------
// CEvtEditor::LineChangedL()
// Derived from CAknDialog
// -----------------------------------------------------------------------------
//
void CEvtEditor::LineChangedL(TInt  aControlId)
    {
	EVTUIDEBUG("+ CEvtEditor::LineChangedL()");
    if(iIsEditMode)
        ChangeMSKCaptionL(aControlId); 
	EVTUIDEBUG("- CEvtEditor::LineChangedL()");
    }

// -----------------------------------------------------------------------------
// CEvtEditor::InsertRadiusDlgLineL()
// It inserts dialog line for radius
// -----------------------------------------------------------------------------
//
void CEvtEditor::InsertRadiusDlgLineL()
    {
	EVTUIDEBUG("+ CEvtEditor::InsertRadiusDlgLineL()");
    if( ControlOrNull(EEvtMgmtUiDlgCIdRadius)==NULL )
        {
        TInt lineIndex;   
        lineIndex = FindLineIndex(* ControlOrNull(EEvtMgmtUiDlgCIdPlace))+1;
        InsertLineL(lineIndex,R_EVTUI_CREATENEWFORM_DLGLINE_RADIUS);
		SetRadiusL();
        }
	EVTUIDEBUG("- CEvtEditor::InsertRadiusDlgLineL()");
	}

// -----------------------------------------------------------------------------
// CEvtEditor::RemoveRadiusDlgLineL()
// It remove dialog line for radius
// -----------------------------------------------------------------------------
//
void CEvtEditor::RemoveRadiusDlgLineL()
    {  
	EVTUIDEBUG("+ CEvtEditor::RemoveRadiusDlgLineL()");
    if(ControlOrNull(EEvtMgmtUiDlgCIdRadius)!=NULL)
          {
          DeleteLine(EEvtMgmtUiDlgCIdRadius,ETrue); 
          }  
	EVTUIDEBUG("- CEvtEditor::RemoveRadiusDlgLineL()");  
    }

// -----------------------------------------------------------------------------
// CEvtEditor::RemoveAudioLoopDlgLineL()
// It removes dialog line for ringtone and audio-loop
// -----------------------------------------------------------------------------
//
void CEvtEditor::RemoveAudioLoopDlgLineL()
    {  
	EVTUIDEBUG("+ CEvtEditor::RemoveAudioLoopDlgLineL()");    
    if(ControlOrNull(EEvtMgmtUiDlgCIdAudioLoop)!=NULL)
      {
      DeleteLine(EEvtMgmtUiDlgCIdAudioLoop,ETrue); 
      }  
	EVTUIDEBUG("- CEvtEditor::RemoveAudioLoopDlgLineL()");             
    }
// -----------------------------------------------------------------------------
// CEvtEditor::InsertAudioLoopDlgLinesL()
// It inserts dialog line for ringtone and audio-loop
// -----------------------------------------------------------------------------
//
void CEvtEditor::InsertAudioLoopDlgLinesL()
    {
	EVTUIDEBUG("+ CEvtEditor::InsertAudioLoopDlgLinesL()"); 
    if( ControlOrNull(EEvtMgmtUiDlgCIdAudioLoop)==NULL )
        {
        TInt lastLineIndex = FindLineIndex(* ControlOrNull(EEvtMgmtUiDlgCIdAssignTone));
        InsertLineL(lastLineIndex+1,R_EVTUI_CREATENEWFORM_DLGLINE_AUDIOLOOP);
        UpdatePageL(ETrue);
        }
	EVTUIDEBUG("- CEvtEditor::InsertAudioLoopDlgLinesL()"); 
    }

// -----------------------------------------------------------------------------
// CEvtEditor::NotifyEditorL()
// Inherited from MEvtMgmtUiEditorObserver
// -----------------------------------------------------------------------------
//
void CEvtEditor::NotifyEditorL(TInt aCommand, TInt aErrorCode)
    {
	EVTUIDEBUG1("+ CEvtEditor::NotifyEditorL() - %d", aErrorCode ); 
    if(aErrorCode!=KErrNone && aErrorCode!=KErrCancel&& aErrorCode!=KErrNotFound)
        return;
    
    switch(aCommand)
        {              
        case EEvtEditorCmdSetPlace:
            {        
            if( aErrorCode==KErrNone )              
            	SetPlaceL();
            ChangeMSKCaptionL(EEvtMgmtUiDlgCIdPlace);          
            break;
            }
        case EEvtEditorCmdAddDescNew:
        case EEvtEditorCmdAddDescFromExisting: 
        case EEvtEditorCmdEditDesc:
            {
            if( aErrorCode == KErrNone )
                {
                if(!ControlOrNull(EEvtMgmtUiDlgCIdDesc))
                    InsertDesDlgLineL();
                TryChangeFocusToL( EEvtMgmtUiDlgCIdDesc ); 
                SetDescL();
                }
             else
             	{
				RemoveDesDlgLineL();
				DrawNow();
             	}     
            iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeDesc;             
            break;
            }
        case EEvtEditorCmdAssignTone:
            {     
            if (  aErrorCode == KErrNone  ||  aErrorCode == KErrNotFound )
                {
                // Ring tone assignment was successful
                SetRingtoneL();
                TryChangeFocusToL( EEvtMgmtUiDlgCIdAssignTone );   
                iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributeTone;
                }        
            break;
            }
        case EEvtEditorCmdDelete:        
        case EEvtEditorCmdActivate:
        case EEvtEditorCmdSaveDraft:
            {  
            if ( !aErrorCode )
                {
                TryExitL( EEvtEditorCmdCloseDlg );
                }
            break;
            }      
        default:
            {
            break;
            }
        }
	EVTUIDEBUG("- CEvtEditor::NotifyEditorL()");
    }
    
// -----------------------------------------------------------------------------
// CEvtEditor::NotifySOMChangeL()
// Inherited from MEvtMgmtUiSOMObserver
// -----------------------------------------------------------------------------
//
void CEvtEditor::NotifySOMChangeL( )
	{
	EVTUIDEBUG("+ CEvtEditor::NotifySOMChangeL()");
    if( ControlOrNull(EEvtMgmtUiDlgCIdRadius) != NULL )
        {
        CAknUnitEditor* radiusEd =
             (CAknUnitEditor*)Control( EEvtMgmtUiDlgCIdRadius );
             
        //convert radius in km unit
        TReal val(0);
        switch( iSysOfMeas->GetCurrentSysofMeasL() )
        	{
        	case ELocImperial: // Mile
        	    {
                val = iEvent.Radius();
                val *= KUnitConversion;
                TReal setVal;
                Math::Round(setVal, val, 2);
                radiusEd->SetMinimumAndMaximum( KMinRadiusInKm*KUnitConversion, KMaxRadiusInKm*KUnitConversion );
                radiusEd->SetValue( setVal );
                radiusEd->SetUnitL( EAknUnitEditorMile );      
        		break;
        	    }
        	case ELocMetric:   // KiloMetre
                {
                val = iEvent.Radius();
                TReal setVal;
                Math::Round(setVal, val, 2);
                radiusEd->SetMinimumAndMaximum( KMinRadiusInKm, KMaxRadiusInKm );
                radiusEd->SetValue( setVal );
                radiusEd->SetUnitL( EAknUnitEditorKilometer );      
                break;  			
                }
        	default: 
        		break;
        	}
        UpdatePageL(ETrue);
        }
	EVTUIDEBUG("- CEvtEditor::NotifySOMChangeL()");
	}

// -----------------------------------------------------------------------------
// CEvtEditor::OfferKeyEventL()
// Inherited from CAknForm
// -----------------------------------------------------------------------------
//
TKeyResponse CEvtEditor::OfferKeyEventL (const TKeyEvent &aKeyEvent, TEventCode aType) 
    {
    // Handle the "C" key in View mode.
    if(!IsEditable() && aType == EEventKeyUp)
        {
        switch ( aKeyEvent.iScanCode )
            {
            case EStdKeyBackspace:
            case EStdKeyDelete:
                {
                ProcessCommandL( EEvtEditorCmdDelete );  
                return EKeyWasConsumed;               
                }
            default:
                {
                break;  
                }
            }
        }  
    
    // Handle the Control Specific key events.
    if( IdOfFocusControl() == EEvtMgmtUiDlgCIdPlace && aType == EEventKey )
        {
        switch(aKeyEvent.iCode)
            {
            case EKeyDownArrow:
                {
                if( ControlOrNull(EEvtMgmtUiDlgCIdRadius) )
                    TryChangeFocusToL( EEvtMgmtUiDlgCIdRadius );  
                else if( ControlOrNull(EEvtMgmtUiDlgCIdDesc) )
                    TryChangeFocusToL( EEvtMgmtUiDlgCIdDesc );  
                else if( ControlOrNull(EEvtMgmtUiDlgCIdRepeat) )
                    TryChangeFocusToL( EEvtMgmtUiDlgCIdRepeat );  
                return EKeyWasConsumed;
                }
            default:
                break;
            }
        }
    if( IdOfFocusControl() == EEvtMgmtUiDlgCIdStatus && aType == EEventKey )
        {
        switch(aKeyEvent.iCode)
            {
            case EKeyLeftArrow:
            case EKeyRightArrow:
            case EKeyOK:
            case EKeyEnter:
                {
                CheckStatusL();
                }
            default:
                break;
            }
        }
    return CAknForm::OfferKeyEventL( aKeyEvent, aType );  
    }

// -----------------------------------------------------------------------------
// CEvtEditor::HandlePointerEventL()
// Inherited from CAknForm
// -----------------------------------------------------------------------------
//
void CEvtEditor::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {    
	EVTUIDEBUG("+ CEvtEditor::HandlePointerEventL()");

	/* As default HandlePointerEventL() calls control's HandlePointerEventL()
	 * We have to modify status's array before CAknForm::HandlePointerEventL() is called.
	 */
	if(aPointerEvent.iType == TPointerEvent::EButton1Up )
	   {
        CAknPopupFieldText* statusEditor = static_cast <CAknPopupFieldText*> ( ControlOrNull(EEvtMgmtUiDlgCIdStatus) );
        if (statusEditor)
            {
            if( statusEditor->Rect().Contains(aPointerEvent.iPosition) ) 
                CheckStatusL();
            }
	    }
	                
	EVTUIDEBUG1("+ b4 HandlePointerEventL- %d", aPointerEvent.iType);
	CAknForm::HandlePointerEventL( aPointerEvent );  
	EVTUIDEBUG1("+ after HandlePointerEventL- %d", aPointerEvent.iType);
	if(aPointerEvent.iType == TPointerEvent::EButton1Up )
	   {
        if( !IsEditable() )
	        {
	        iIsEditMode = ETrue;
	        SetEditableL(ETrue);
	        ChangeRSKCaptionL();
	        ChangeMSKCaptionL( IdOfFocusControl() ); 			
			return;
	        }	  
      
        switch ( IdOfFocusControl() )
           {
           case EEvtMgmtUiDlgCIdPlace:
               {
               CEikEdwin* placeEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdPlace));
                if (placeEditor)
                    {
                    if( placeEditor->Rect().Contains(aPointerEvent.iPosition) ) 
                        iCmdHandler->HandleEditorCmdL( EEvtEditorCmdSetPlace );
                    }
               break;
               }
           case EEvtMgmtUiDlgCIdDesc:
               {
               CEikEdwin* descEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdDesc));
                if (descEditor)
                    {
                    if( descEditor->Rect().Contains(aPointerEvent.iPosition) ) 
                        iCmdHandler->HandleEditorCmdL( EEvtEditorCmdEditDesc );
                    }
               break;
               }
           case EEvtMgmtUiDlgCIdAssignTone:
               {
               CEikEdwin* toneEditor = static_cast <CEikEdwin*> (ControlOrNull(EEvtMgmtUiDlgCIdAssignTone));
                if (toneEditor)
                    {
                    if( toneEditor->Rect().Contains(aPointerEvent.iPosition) ) 
                        iCmdHandler->HandleEditorCmdL( EEvtEditorCmdAssignTone );
                    }
               break;
               }         
           default:
               {
               break;
               }
           }
       }
    
	EVTUIDEBUG("- CEvtEditor::HandlePointerEventL()");
    }
// ---------------------------------------------------------------------------
// CEvtEditor::MakeTitleL()
// Set the Title Text
// ---------------------------------------------------------------------------
//
void CEvtEditor::MakeTitleL( TDesC& aText )
    {
	EVTUIDEBUG("+ CEvtEditor::MakeTitleL()");
    
    // Obtain the title from the Status Pane
    CAknTitlePane* title = static_cast<CAknTitlePane*>( iEikonEnv->AppUiFactory()->
    	StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
       
    // Set the Title's buffer    
    HBufC* buf = aText.AllocL();
    title->SetText( buf ); // Takes ownership of buf
    title->DrawNow();
	EVTUIDEBUG("- CEvtEditor::MakeTitleL()");
    }     

// ---------------------------------------------------------------------------
// CEvtEditor::TogglePopupFieldControlL()
// it toggles  value of popupfield control
// ---------------------------------------------------------------------------
//
void CEvtEditor::TogglePopupFieldControlL( TInt aControlId )
    {
	EVTUIDEBUG1("+ CEvtEditor::TogglePopupFieldControlL() - %d", aControlId );
    CAknPopupFieldText* audioLoopPopupFieldText =
                    (CAknPopupFieldText*)Control( aControlId );
      if(audioLoopPopupFieldText)
          {          
          if( audioLoopPopupFieldText->CurrentValueIndex() == 0)
              audioLoopPopupFieldText->SetCurrentValueIndex(1);
          else
              audioLoopPopupFieldText->SetCurrentValueIndex(0);    
                    
          HandleControlStateChangeL( aControlId );
          
          UpdatePageL(ETrue);
          }
	EVTUIDEBUG("- CEvtEditor::TogglePopupFieldControlL()");
    }
    
// ---------------------------------------------------------------------------
// void CEvtMgmtUiModel::OpenLocalizedResourceFileL
// Opens the nearest localized resourcefile using aResourceLoader. 
// aResourceLoader should be opened only once before closing it. 
// Otherwise it will raise a panic. Therefore it is better to have 
// multiple resource loaders.
// 
// @param aResourceFileName Drive and name of resource file in format 
//                          <path>:<rsc_file_name>
// @param aResourceLoader Resource loader for the resource. The loader 
//                        must not be opened allready or it will raise 
//                        a panic. It is caller's responsibility to 
//                        close the resource loader after using the 
//                        resource.
// ---------------------------------------------------------------------------
//  
void CEvtEditor::OpenLocalizedResourceFileL( 
                            const TDesC&               aResourceFileName, 
                                  RConeResourceLoader& aResourceLoader )
    {    
    TFileName* tmpName = new(ELeave) TFileName;
    CleanupStack::PushL( tmpName );
    
    // Append the Resource Files Directory
    tmpName->Append( KDC_RESOURCE_FILES_DIR );
    
    // Append the Ressource File Name
    tmpName->Append( aResourceFileName );
    
    // Obtain the drive where the DLL is installed
    TFileName* dllDrive = new (ELeave) TFileName;
    CleanupStack::PushL( dllDrive );
    Dll::FileName( *dllDrive );
    
    // Obtain the Complete path for the Resource File
    TParse parse;
    User::LeaveIfError( parse.Set( *dllDrive, NULL, NULL ) );
    User::LeaveIfError( parse.Set( parse.Drive(), tmpName, NULL ) );
    TFileName* fileName = new(ELeave) TFileName;
    CleanupStack::PushL( fileName );
    fileName->Append( parse.FullName() );
    
    // Get language of resource file
    BaflUtils::NearestLanguageFile( CEikonEnv::Static()->FsSession(), *fileName );
    
    // Open resource file    
    aResourceLoader.OpenL( *fileName );
    
    // Pop all the fields
    CleanupStack::PopAndDestroy( fileName );
    CleanupStack::PopAndDestroy( dllDrive );
    CleanupStack::PopAndDestroy( tmpName );    
    }

//End of file
