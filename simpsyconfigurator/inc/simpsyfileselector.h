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
* Description:  Simulation PSY file selector dialog.
*
*/


#ifndef C_SIMPSYFILESELECTOR_H
#define C_SIMPSYFILESELECTOR_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <aknlists.h>

// FORWARD DECLARATIONS
class CSimPsyFileSelectorModel;

/**
 *  Simulation PSY configurator file selector dialog.
 *  
 *  This class provides the extension of single graphic style list box which is
 *  used to display all the simulation psy files.
 * 
 *  @lib simpsyui.exe
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CSimPsyFileSelector ) : public CAknSingleGraphicPopupMenuStyleListBox
    {
public:
    /**
     * Overloaded constructor
     */
    CSimPsyFileSelector();

    /**
     * Second phase of the two phase constructor
     */
    void ConstructL( CCoeControl*              aParent );          

    /**
     * C++ Destructor.
     * Frees all the resources associated with this object.
     */
    virtual ~CSimPsyFileSelector();
    
    /** 
     * Returns the simulation file name corresponding to the
     * selected item.
     */
    TPtrC SimulationFileNameL();
    	    
private: // Inherited from Base classes.
    /**
     * Imherited from CCoeControl.
     */
    void HandleResourceChange( TInt    aType );
    
    /**
     * Updates the list box icons.
     */
    void UpdateIconsL();

private: // data
    /**
     * Pop-up List box model.
     */
    CSimPsyFileSelectorModel*	    iListBoxModel;
    };

#endif // C_LCPOPUPLISTBOX_H
