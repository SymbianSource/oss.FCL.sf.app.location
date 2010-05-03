/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: main.cpp
*
*/

#include <HbApplication>
#include <QTranslator>
#include <QLocale>
#include "locationpickerappwindow.h"
#include "locationpickertypes.h"

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    QTranslator translator;
    QString lang = QLocale::system().name();
    translator.load("lilpicker_" + lang);
    app.installTranslator(&translator);
    
    app.setApplicationName( hbTrId("txt_lint_title_select_location") );
    LocationPickerAppWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
