//
// lassheet.h
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

#ifndef LASSHEET_H
#define LASSHEET_H

#include <QTableView>

//#include "lasmaemo5theme.h"

enum LasColorName
{
  Text_Color,
  Title_Color,
  Background_Color
};

//class LasSheetItem;
class LasInputDialog;

class LasSheet: public QTableView
{
  Q_OBJECT

public:
  LasSheet(QWidget *parent);

  virtual ~LasSheet();

//  QVariant computeFormula(
//    const LasSheetItem *item);

  QString encodePos(
    int col,
    int row);

  void decodePos(
    QString pos,
    int &col,
    int &row);

/*
  void setColorIcon(
    QPixmap &pix);
*/

/*
  QColor getColor(
    LasColorName color);
*/

//  QColor getTitleColor();

  void resizeCells(
    int column);

  void resizeAllCells();

public slots:
//  void updateColor(QTableWidgetItem *item);
//  void selectColor();
//  void selectFont();
//  void clear();

/*
  void actionSum();
  void actionSubtract();
  void actionAdd();
  void actionMultiply();
  void actionDivide();
*/

//  void addNewRow(
//    QString barcode);

protected:
//  void setupContextMenu();
//  void setupContents();

  void createActions();

/*
  void actionMath_helper(
    QString title,
    QString op);
*/

  bool runInputDialog(
    QString title,
    QString c1Text,
    QString c2Text,
    QString opText,
    QString outText,
    QString &cell1,
    QString &cell2,
    QString &outCell);

private slots:
/*
  void acceptDialogValues(
    QString op,
    QString cell1,
    QString cell2,
    QString outCell);
*/

private:
  QAction *colorAction;
  QAction *fontAction;
  QAction *firstSeparator;
  QAction *cell_sumAction;
  QAction *cell_addAction;
  QAction *cell_subAction;
  QAction *cell_mulAction;
  QAction *cell_divAction;
  QAction *secondSeparator;
  QAction *clearAction;

  LasInputDialog *inputDialog;

//  LasMaemo5Theme themeSettings;

  int index;
};

#endif // LASSHEET_H
