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
* Description: LocationPickerProxyModel implementation
*
*/

#include <QStandardItemModel>
#include "locationpickerproxymodel.h"

// ----------------------------------------------------
// LocationPickerProxyModel::LocationPickerProxyModel()
// ----------------------------------------------------

LocationPickerProxyModel::LocationPickerProxyModel(Qt::Orientations aOrientation , QObject *parent)
     :QSortFilterProxyModel(parent),
     mOrientation( aOrientation )
{
}

// ----------------------------------------------------
// LocationPickerProxyModel::lessThan()
// ----------------------------------------------------
 bool LocationPickerProxyModel::lessThan(const QModelIndex &left,
                                      const QModelIndex &right) const
{
     // get left and right items data and implement sort logic
     // return true if left is less than right
	 QVariant leftData = sourceModel()->data(left);
     QVariant rightData = sourceModel()->data(right);

    if( mOrientation == Qt::Vertical )
    {
        QStringList leftStringList = leftData.toStringList();

        QStringList rightStringList = rightData.toStringList();

        return QString::compare( QString( leftStringList[0] + " " + leftStringList[1] ),
                                            QString( rightStringList[0] + " " + rightStringList[1] ), Qt::CaseInsensitive ) < 0;
    }
    else
    {
        QString leftString = leftData.toString();

        QString rightString = rightData.toString();

        return QString::compare( leftString, rightString, Qt::CaseInsensitive ) < 0;
    }
}

 // ----------------------------------------------------
// LocationPickerProxyModel::filterAcceptsRow()
// ----------------------------------------------------

bool LocationPickerProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    if( mOrientation == Qt::Vertical)
    {
    // implement logic for search.
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);

    QStringList stringList = sourceModel()->data(index0).toStringList();
    QString fullString = " " + stringList[0] + " " + stringList[1];

    return (fullString.contains(mSearchText, Qt::CaseInsensitive));
    }
    else
    return true;
}

 // ----------------------------------------------------
// LocationPickerProxyModel::filterParameterChanged()
// ----------------------------------------------------
void LocationPickerProxyModel::filterParameterChanged(QString aSearchText)
{
    mSearchText = " " + aSearchText;
}
