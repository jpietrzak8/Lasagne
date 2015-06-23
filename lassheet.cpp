//
// lassheet.cpp
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

#include "lassheet.h"

//#include "lassheetdelegate.h"
//#include "lassheetitem.h"
#include "lasinputdialog.h"
#include "lasroles.h"

#include <QList>
#include <QAction>
#include <QColor>
#include <QColorDialog>
#include <QPainter>
#include <QFontDialog>

#include <QDebug>

//typedef QList<QTableWidgetItem *> LasItemList;

LasSheet::LasSheet(
  QWidget *parent)
  : QTableView(parent),
    colorAction(0),
    fontAction(0),
    firstSeparator(0),
    cell_sumAction(0),
    cell_addAction(0),
    cell_subAction(0),
    cell_mulAction(0),
    cell_divAction(0),
    secondSeparator(0),
    clearAction(0),
    inputDialog(0),
    index(1)
{
  // Customize QTableView for the N900:
  setSelectionMode(QAbstractItemView::SingleSelection);
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setDragDropMode(QAbstractItemView::NoDragDrop);
  setFocusPolicy(Qt::NoFocus);

  // Create an input dialog box:
//  inputDialog = new LasInputDialog();

//  setItemPrototype(item(1, 1));

//  setItemDelegate(new LasSheetDelegate());

//  createActions();
//  setupContents();
//  setupContextMenu();

/*
  connect(
    this,
    SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
    this,
    SLOT(updateColor(QTableWidgetItem*)));
*/

/*
  connect(
    this,
    SIGNAL(cellChanged(int, int)),
    this,
    SLOT(resizeCells(int, int)));
*/

/*
  connect(
    inputDialog,
    SIGNAL(inputDialogResult(QString, QString, QString, QString)),
    this,
    SLOT(acceptDialogValues(QString, QString, QString, QString)));
*/
}


LasSheet::~LasSheet()
{
}


/*
void LasSheet::setColorIcon(
  QPixmap &pix)
{
  colorAction->setIcon(pix);
}
*/


/*
void LasSheet::createActions()
{
  cell_sumAction = new QAction(tr("Sum"), this);
  connect(
    cell_sumAction,
    SIGNAL(triggered()),
    this,
    SLOT(actionSum()));

  cell_addAction = new QAction(tr("&Add"), this);
  cell_addAction->setShortcut(Qt::CTRL | Qt::Key_Plus);
  connect(
    cell_addAction,
    SIGNAL(triggered()),
    this,
    SLOT(actionAdd()));

  cell_subAction = new QAction(tr("&Subtract"), this);
  cell_subAction->setShortcut(Qt::CTRL | Qt::Key_Minus);
  connect(
    cell_subAction,
    SIGNAL(triggered()),
    this,
    SLOT(actionSubtract()));

  cell_mulAction = new QAction(tr("&Multiply"), this);
  cell_mulAction->setShortcut(Qt::CTRL | Qt::Key_multiply);
  connect(
    cell_mulAction,
    SIGNAL(triggered()),
    this,
    SLOT(actionMultiply()));

  cell_divAction = new QAction(tr("&Divide"), this);
  cell_divAction->setShortcut(Qt::CTRL | Qt::Key_division);
  connect(
    cell_divAction,
    SIGNAL(triggered()),
    this,
    SLOT(actionDivide()));

  fontAction = new QAction(tr("Font..."), this);
  fontAction->setShortcut(Qt::CTRL | Qt::Key_F);
  connect(
    fontAction,
    SIGNAL(triggered()),
    this,
    SLOT(selectFont()));

  colorAction = new QAction(QPixmap(16, 16), tr("Background &Color..."), this);
  connect(
    colorAction,
    SIGNAL(triggered()),
    this,
    SLOT(selectColor()));

  clearAction = new QAction(tr("Clear"), this);
  clearAction->setShortcut(Qt::Key_Delete);
  connect(
    clearAction,
    SIGNAL(triggered()),
    this,
    SLOT(clear()));
}
*/


/*
void LasSheet::updateColor(
  QTableWidgetItem *item)
{
  QPixmap pix(16, 16);
  QColor col;

  if (item) col = item->backgroundColor();

  if (!col.isValid()) col = palette().base().color();

  QPainter pt(&pix);
  pt.fillRect(0, 0, 16, 16, col);

  QColor lighter = col.light();
  pt.setPen(lighter);
  QPoint lightFrame[] = {QPoint(0, 15), QPoint(0, 0), QPoint(15, 0)};
  pt.drawPolyline(lightFrame, 3);

  pt.setPen(col.dark());
  QPoint darkFrame[] = {QPoint(1, 15), QPoint(15,15), QPoint(15,1)};
  pt.drawPolyline(darkFrame, 3);

  pt.end();

  setColorIcon(pix);
}
*/


/*
void LasSheet::selectColor()
{
  QTableWidgetItem *item = currentItem();

  QColor col = item ? item->backgroundColor() : palette().base().color();
  col = QColorDialog::getColor(col, this);

  if (!col.isValid()) return;

  LasItemList selected = selectedItems();
  if (selected.count() == 0) return;

  LasItemList::iterator i = selected.begin();
  while (i != selected.end())
  {
    if (*i) (*i)->setBackgroundColor(col);
    ++i;
  }

  updateColor(currentItem());
}
*/


/*
void LasSheet::selectFont()
{
  LasItemList selected = selectedItems();
  if (selected.count() == 0) return;

  bool ok = false;
  QFont fnt = QFontDialog::getFont(&ok, font(), this);
  if (!ok) return;

  LasItemList::iterator i = selected.begin();
  while (i != selected.end())
  {
    if (*i) (*i)->setFont(fnt);
    ++i;
  }
}
*/


/*
void LasSheet::actionSum()
{
  int col_first = 0;
  int col_last = 0;
  int col_cur = 0;
  int row_first = 0;
  int row_last = 0;
  int row_cur = 0;

  LasItemList selected = selectedItems();

  if (!selected.isEmpty())
  {
    QTableWidgetItem *first = selected.first();
    QTableWidgetItem *last = selected.last();
    col_first = column(first);
    col_last = column(last);
    row_first = row(first);
    row_last = row(last);
  }

  QTableWidgetItem *current = currentItem();

  if (current)
  {
    col_cur = column(current);
    row_cur = row(current);
  }

  QString cell1 = encodePos(col_first, row_first);
  QString cell2 = encodePos(col_last, row_last);
  QString out = encodePos(col_cur, row_cur);

  inputDialog->setupFields(
    columnCount(),
    rowCount(),
    tr("Sum cells"),
    tr("First cell:"),
    col_first,
    row_first,
    tr("Last cell:"),
    col_last,
    row_last,
    QString(QChar(0x03a3)),
    tr("Output to:"),
    col_cur,
    row_cur);

  inputDialog->exec();
}


void LasSheet::actionMath_helper(
  QString title,
  QString op)
{
  QString cell1 = "A1";
  QString cell2 = "A2";
  QString out = "A3";

  QTableWidgetItem *current = currentItem();
  if (current)
  {
    out = encodePos(currentColumn(), currentRow());
  }

  int c1Col, c1Row, c2Col, c2Row, outCol, outRow;
  decodePos(cell1, c1Col, c1Row);
  decodePos(cell2, c2Col, c2Row);
  decodePos(out, outCol, outRow);

  inputDialog->setupFields(
    columnCount(),
    rowCount(),
    title,
    tr("Cell 1"),
    c1Col,
    c1Row,
    tr("Cell 2"),
    c2Col,
    c2Row,
    op,
    tr("Output to:"),
    outCol,
    outRow);

  inputDialog->exec();
}


void LasSheet::acceptDialogValues(
  QString op,
  QString cell1,
  QString cell2,
  QString outCell)
{
  int col, row;
  decodePos(outCell, col, row);
  item(row, col)->setText(op + " " + cell1 + " " + cell2);
}


void LasSheet::actionAdd()
{
  actionMath_helper(tr("Addition"), "+");
}


void LasSheet::actionSubtract()
{
  actionMath_helper(tr("Subtraction"), "-");
}


void LasSheet::actionMultiply()
{
  actionMath_helper(tr("Multiplication"), "*");
}


void LasSheet::actionDivide()
{
  actionMath_helper(tr("Division"), "/");
}
*/


/*
void LasSheet::clear()
{
  LasItemList selected = selectedItems();

  LasItemList::iterator i = selected.begin();
  while(i != selected.end())
  {
    (*i)->setText("");
    ++i;
  }
}
*/


/*
void LasSheet::setupContextMenu()
{
  addAction(cell_addAction);
  addAction(cell_subAction);
  addAction(cell_mulAction);
  addAction(cell_divAction);
  addAction(cell_sumAction);
  addAction(firstSeparator);
  addAction(colorAction);
  addAction(fontAction);
  addAction(secondSeparator);
  addAction(clearAction);
  setContextMenuPolicy(Qt::ActionsContextMenu);
}
*/


/*
void LasSheet::setupContents()
{
  // Set the size of the table:
  setColumnCount(3);
  setRowCount(1);

  // Set up the horizontal header values:
  int c = 0;
  while (c < 3)
  {
    QString character(QChar('A' + c));
    setHorizontalHeaderItem(c, new QTableWidgetItem(character));
    ++c;
  }

  // Set the font size:
  QFont tempFont = font();
  tempFont.setPointSize(24);
  setFont(tempFont);

  // Modify the font for the headers:
  tempFont.setBold(true);

  // column 0
  setItem(0, 0, new LasSheetItem("Barcode", this));
  item(0, 0)->setFont(tempFont);
  resizeColumnToContents(0);

  // column 1
  setItem(0, 1, new LasSheetItem("Name", this));
  item(0, 1)->setFont(tempFont);
  resizeColumnToContents(1);

  // column 2
  setItem(0, 2, new LasSheetItem("Category", this));
  item(0, 2)->setFont(tempFont);
  resizeColumnToContents(2);
  resizeRowToContents(0);
}
*/


/*
QVariant LasSheet::computeFormula(
  const LasSheetItem *lsi)
{
  QString formula = lsi->data(LAS_FORMULAROLE).toString();

  QStringList list = formula.split(' ');
  if (list.isEmpty())
  {
//qDebug() << "computeFormula returing " << formula;
    return formula;
  }

  QString op = list.value(0).toLower();

  int firstCol = -1;
  int firstRow = -1;
  int secondCol = -1;
  int secondRow = -1;

  if (list.count() > 1)
  {
    decodePos(list.value(1), firstCol, firstRow);

    if (list.count() > 2)
    {
      decodePos(list.value(2), secondCol, secondRow);
    }
  }

  const QTableWidgetItem *start = item(firstRow, firstCol);
  const QTableWidgetItem *end = item(secondRow, secondCol);

  int firstVal = start ? start->text().toInt() : 0;
  int secondVal = end ? end->text().toInt() : 0;

  QVariant result;
  if (op == "sum")
  {
    int sum = 0;
    int c = firstCol;
    int r = firstRow;
    while (c <= secondCol)
    {
      while (r <= secondRow)
      {
        const QTableWidgetItem *tableItem = item(r, c);
        if (tableItem && tableItem != dynamic_cast<const QTableWidgetItem *>(lsi))
        {
          sum += tableItem->text().toInt();
        }
        ++r;
      }
      ++c;
    }

    return sum;
  }
  else if (op == "+")
  {
    return (firstVal + secondVal);
  }
  else if (op == "-")
  {
    return (firstVal - secondVal);
  }
  else if (op == "*")
  {
    return (firstVal * secondVal);
  }
  else if (op == "/")
  {
    if (secondVal == 0)
    {
      return QString("nan");
    }
    else
    {
      return (firstVal / secondVal);
    }
  }
  else if (op == "=")
  {
    if (start) return start->text();
  }

  return formula;
}
*/


QString LasSheet::encodePos(
  int col,
  int row)
{
  return QString(col + 'A') + QString::number(row + 1);
}


void LasSheet::decodePos(
  QString pos,
  int &col,
  int &row)
{
  if (pos.isEmpty())
  {
    col = -1;
    row = -1;
  }
  else
  {
    col = pos.at(0).toLatin1() - 'A';
    row = pos.right(pos.size() - 1).toInt() - 1;
  }
}


void LasSheet::resizeCells(
  int column)
{
  resizeColumnToContents(column);
  // Will we need to resize the row as well?
}


void LasSheet::resizeAllCells()
{
  resizeColumnsToContents();
}


/*
QColor LasSheet::getColor(
  LasColorName color)
{
  switch (color)
  {
  case Background_Color:
    return themeSettings.getBackgroundColor();
    break;

  case Title_Color:
    return themeSettings.getActiveTextColor();

  case Text_Color:
    return themeSettings.getDefaultTextColor();
    break;

  default:
    qWarning() << "Unknown Color Value: " << color;
    return themeSettings.getDefaultTextColor();
    break;
  }

  // This line should never be reached:
  return themeSettings.getBackgroundColor();
}
*/


/*
QColor LasSheet::getTitleColor()
{
  return themeSettings.getActiveTextColor();
}
*/


/*
void LasSheet::addNewRow(
  QString barcode)
{
  // Add a row to the table:
  setRowCount(index + 1);

  // Insert the barcode:
  setCurrentCell(index, 0);
  setItem(index, 0, new LasSheetItem(barcode, this));
  viewport()->update();

  // Prepare for the next row:
  ++index;
}
*/


/*
void LasSheet::clearCurrentCell()
{
  int row = currentRow();

  // Don't clear the top row cells:
  if (row != 0)
  {
    removeCellWidget(row, currentColumn());
  }
}


void LasSheet::removeCurrentRow()
{
  int row = currentRow();

  // Don't remove the top row:
  if (row != 0)
  {
    removeRow(row);
  }
}
*/
