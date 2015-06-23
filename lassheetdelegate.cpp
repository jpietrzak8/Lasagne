//
// lassheetdelegate.cpp
//
// Copyright 2015 by John Pietrzak (jpietrzak8@gmail.com)
//
// This file is part of Lasagne.
//
// Lasagne is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// Lasagne is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Lasagne; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#include "lassheetdelegate.h"
#include "lasroles.h"

#include <QLineEdit>
#include <QCompleter>
#include <QDateTimeEdit>

LasSheetDelegate::LasSheetDelegate(
  QObject *parent)
  : QItemDelegate(parent)
{
}


QWidget *LasSheetDelegate::createEditor(
  QWidget *parent,
  const QStyleOptionViewItem &viewItem,
  const QModelIndex &index) const
{
/*
  if (index.column() == 1)
  {
    QDateTimeEdit *editor = new QDateTimeEdit(parent);
    editor->setDisplayFormat("dd/M/yyyy");
    editor->setCalendarPopup(true);
    return editor;
  }
*/

  QLineEdit *editor = new QLineEdit(parent);

  QStringList allStrings;
  int i = 1;
  int max = index.model()->rowCount();
  while (i < max)
  {
    QString strItem(
      index.model()->data(
        index.sibling(i, index.column()),
        LAS_FORMULAROLE).toString());

    if (!allStrings.contains(strItem))
    {
      allStrings.append(strItem);
    }

    ++i;
  }

  QCompleter *autoComplete = new QCompleter(allStrings);
  editor->setCompleter(autoComplete);

  connect(
    editor,
    SIGNAL(editingFinished()),
    this,
    SLOT(commitAndCloseEditor()));

  return editor;
}


void LasSheetDelegate::commitAndCloseEditor()
{
  QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
  emit commitData(editor);
  emit closeEditor(editor);
}


void LasSheetDelegate::setEditorData(
  QWidget *editor,
  const QModelIndex &index) const
{
  QLineEdit *edit = qobject_cast<QLineEdit *>(editor);

  if (edit)
  {
    edit->setText(index.model()->data(index, LAS_FORMULAROLE).toString());
  }
/*
  else
  {
    QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
    if (dateEditor)
    {
      dateEditor->setDate(
        QDate::fromString(
          index.model()->data(index, LAS_FORMULAROLE).toString(),
          "d/M/yyyy"));
    }
  }
*/
}


void LasSheetDelegate::setModelData(
  QWidget *editor,
  QAbstractItemModel *model,
  const QModelIndex &index) const
{
  QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
  if (edit)
  {
    model->setData(index, edit->text());
  }
/*
  else
  {
    QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
    if (dateEditor)
    {
      model->setData(index, dateEditor->date().toString("dd/M/yyyy"));
    }
  }
*/
}
