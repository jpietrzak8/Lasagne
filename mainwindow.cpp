//
// mainwindow.cpp
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "lassheet.h"
#include "lassheetdelegate.h"
//#include "lassheetitem.h"
#include "lasbarcodeviewform.h"
#include "lasoptionsdialog.h"
#include "lasroles.h"
#include "lascsvmodel.h"
#include "lasaboutform.h"
#include "lasdocumentationform.h"
#include "laspreferencesform.h"

#include <QtCore/QCoreApplication>
#include <QList>
//#include <QStatusBar>
#include <QFileDialog>
#include <QMaemo5InformationBox>
#include <QSettings>
//#include <QModelIndex>

#include <QDebug>

MainWindow::MainWindow(
  QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    aboutForm(0),
    documentationForm(0),
    preferencesForm(0),
    toolBar(0),
    formulaInput(0),
    cellLabel(0),
    optionsButton(0),
    scanButton(0),
    csvModel(0),
    table(0),
    barcodeViewForm(0),
    optionsDialog(0)
{
  ui->setupUi(this);
  setAttribute(Qt::WA_Maemo5StackedWindow);

  barcodeViewForm = new LasBarcodeViewForm(this);
  optionsDialog = new LasOptionsDialog(this);
  preferencesForm = new LasPreferencesForm(this);

  toolBar = new QToolBar();
  addToolBar(toolBar);

  formulaInput = new QLineEdit();

  cellLabel = new QLabel(toolBar);
  cellLabel->setMinimumSize(80, 0);

  optionsButton = new QPushButton();
  optionsButton->setIcon(QIcon(":/icons/dash.svg"));

  scanButton = new QPushButton();
  scanButton->setIcon(QIcon(":/icons/plus.svg"));

  toolBar->addWidget(scanButton);
  toolBar->addWidget(cellLabel);
  toolBar->addWidget(formulaInput);
  toolBar->addWidget(optionsButton);

  table = new LasSheet(this);
  ui->centralLayout->addWidget(table);

  QSettings settings("pietrzak.org", "Lasagne");

  if (settings.contains("currentFilename"))
  {
    currentFilename = settings.value("currentFilename").toString();
  }
  else
  {
    currentFilename = "/home/user/MyDocs/.documents/Lasagne.csv";
  }
  
  csvModel = new LasCsvModel(currentFilename, this, true);

  table->setModel(csvModel);

  table->resizeAllCells();

//  statusBar();
/*
  connect(
    table,
    SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
    this,
    SLOT(updateStatus(QTableWidgetItem*)));
*/

  connect(
    table->selectionModel(),
    SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
    this,
    SLOT(updateToolbar(const QModelIndex &)));

/*
  connect(
    table,
    SIGNAL(itemChanged(QTableWidgetItem*)),
    this,
    SLOT(updateStatus(QTableWidgetItem*)));
*/

  connect(
    formulaInput,
    SIGNAL(returnPressed()),
    this,
    SLOT(returnPressed()));

/*
  connect(
    table,
    SIGNAL(itemChanged(QTableWidgetItem*)),
    this,
    SLOT(updateLineEdit(QTableWidgetItem*)));
*/

//  table->updateColor(0);

  connect(
    optionsButton,
    SIGNAL(clicked()),
    optionsDialog,
    SLOT(exec()));

  connect(
    scanButton,
    SIGNAL(clicked()),
    this,
    SLOT(displayBarcodeForm()));

  connect(
    barcodeViewForm,
    SIGNAL(barcodeFound(QString, QString, QString, QString, QString)),
    this,
    SLOT(addNewRow(QString, QString, QString, QString, QString)));

/*
  connect(
    optionsDialog,
    SIGNAL(editFormula()),
    this,
    SLOT(something()));
*/

  connect(
    optionsDialog,
    SIGNAL(clearCell()),
    this,
    SLOT(clearCurrentCell()));

  connect(
    optionsDialog,
    SIGNAL(removeRow()),
    this,
    SLOT(removeCurrentRow()));

  connect(
    optionsDialog,
    SIGNAL(loadFile()),
    this,
    SLOT(load()));

  connect(
    optionsDialog,
    SIGNAL(saveFile()),
    this,
    SLOT(save()));

  connect(
    optionsDialog,
    SIGNAL(saveFileAs()),
    this,
    SLOT(saveAs()));

  // Preferences form:
  connect(
    preferencesForm,
    SIGNAL(beepPreference(bool)),
    barcodeViewForm,
    SLOT(setBeepPref(bool)));

  // Initialize beep preference:
  barcodeViewForm->setBeepPref(preferencesForm->getBeepPref());

  connect(
    preferencesForm,
    SIGNAL(ignoreLensCover(bool)),
    barcodeViewForm,
    SLOT(updateIgnoreCover(bool)));

  barcodeViewForm->updateIgnoreCover(preferencesForm->getIgnoreCover());
}


MainWindow::~MainWindow()
{
  QSettings settings("pietrzak.org", "Lasagne");

  if (!currentFilename.isEmpty())
  {
    csvModel->toCSV(currentFilename, true);
    settings.setValue("currentFilename", currentFilename);
  }
  
  if (scanButton) delete scanButton;
  if (optionsButton) delete optionsButton;
  if (cellLabel) delete cellLabel;
  if (formulaInput) delete formulaInput;
  if (table) delete table;
  if (barcodeViewForm) delete barcodeViewForm;
  if (preferencesForm) delete preferencesForm;
  if (documentationForm) delete documentationForm;
  if (aboutForm) delete aboutForm;
  delete ui;
}


void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}


/*
void MainWindow::updateStatus(
  QTableWidgetItem *item)
{
  // Sanity check:
  if (!item) return;

  if (item == table->currentItem())
  {
    statusBar()->showMessage(
      item->data(Qt::StatusTipRole).toString(),
      1000);

    QString labelText = tr("Cell");
    labelText += ": (";
    labelText += table->encodePos(table->column(item), table->row(item));
    labelText += ")";
    cellLabel->setText(labelText);
  }
}
*/


void MainWindow::updateToolbar(
  const QModelIndex &index)
{
  QString labelText = tr("Cell");
  labelText += ": (";
  labelText += table->encodePos(index.column(), index.row());
  labelText += ")";
  cellLabel->setText(labelText);

/*
  QTableWidgetItem *qtwi = table->item(row, column);

  if (qtwi)
  {
    formulaInput->setText(qtwi->data(LAS_FORMULAROLE).toString());
  }
  else
  {
    formulaInput->clear();
  }
*/

  formulaInput->setText(csvModel->data(index, Qt::DisplayRole).toString());

  currentIndex = index;
}


void MainWindow::returnPressed()
{
  csvModel->setData(currentIndex, QVariant(formulaInput->text()), Qt::DisplayRole);
//  csvModel->setData(index, formulaInput->text(), Qt::UserRole);

//  table->viewport()->update();
  table->resizeAllCells();
}


/*
void MainWindow::selectColor()
{
  QTableWidgetItem *item = table->currentItem();

  QColor col = item ? item->backgroundColor() : table->palette().base.color();
  col = QColorDialog::getColor(col, this);

  if (!col.isValid()) return;

  LasItemList selected = table->selectedItems();
  if (selected.count() == 0) return;

  LasItemList::iterator i = selected.begin();
  while (i != selected.end())
  {
    if (*i) (*i)->setBackgroundColor(col);
    ++i;
  }

  table->updateColor(table->currentItem());
}
*/


/*
void MainWindow::selectFont()
{
  LasItemList selected = table.selectedItems();
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
bool MainWindow::runInputDialog(
  QString title,
  QString c1Text,
  QString c2Text,
  QString opText,
  QString outText,
  QString &cell1,
  QString &cell2,
  QString &outCell)
{
  QStringList rows, cols;

  int i = 0;
  while (i < table->columnCount())
  {
    cols << QChar('A' + i);
    ++i;
  }

  i = 0;
  while (i < table->rowCount())
  {
    rows << QString::number(1 + i);
    ++i;
  }

  if (!inputDialog)
  {
    inputDialog = new LasInputDialog(this);
  }

  inputDialog.setWindowTitle(title);

  cell1Label.setText(c1Text);

  int c1Col, c1Row;
  decode_pos(cell1, c1Col, c1Row);
  c1ColComboBox.clear();
  c1ColComboBox.addItems(cols);
  c1ColComboBox.setCurrentIndex(c1Col);
  c1RowComboBox.clear();
  c1RowComboBox.addItems(rows);
  c1RowComboBox.setCurrentIndex(c1Row);

  operatorLabel.setText(opText);

  cell2Label.setText(c2Text);

  int c2Col, c2Row;
  decodePos(cell2, c2Col, c2Row);
  c2ColComboBox.clear();
  c2ColComboBox.addItems(cols);
  c2ColComboBox.setCurrentIndex(c2Col);
  c2RowComboBox.clear();
  c2RowComboBox.addItems(rows);
  c2RowComboBox.setCurrentIndex(c2Row);

  outLabel.setText(outText);

  int outCol, outRow;
  decodePos(outCell, outCol, outRow);
  outColComboBox.clear();
  outColComboBox.addItems(cols);
  outColComboBox.setCurrentIndex(outCol);
  outRowComboBox.clear();
  outRowComboBox.addItems(rows);
  outRowComboBox.setCurrentIndex(outRow);

  if (inputDialog.exec())
  {
    cell1 = c1ColComboBox.currentText + c1RowComboBox.currentText();
    cell2 = c2ColComboBox.currentText + c2RowComboBox.currentText();
    outCell = outColComboBox.currentText + outRowComboBox.currentText();
    return true;
  }

  return false;
}
*/


/*
void MainWindow::actionSum()
{
  int col_first = 0;
  int col_last = 0;
  int col_cur = 0;
  int row_first = 0;
  int row_last = 0;
  int row_cur = 0;

  LasItemList selected = table->selectedItems();

  if (!selected.isEmpty())
  {
    QTableWidgetItem *first = selected.first();
    QTableWidgetItem *last = selected.last();
    col_first = table->column(first);
    col_last = table->column(last);
    row_first = table->row(first);
    row_last = table->row(last);
  }

  QTableWidgetItem *current = table->currentItem();

  if (current)
  {
    col_cur = table->column(current);
    row_cur = table->row(current);
  }

  QString cell1 = encodePos(col_first, row_first);
  QString cell2 = encodePos(col_last, row_last);
  QString out = encodePos(col_cur, row_cur);

  if (runInputDialog(
    tr("Sum cells"),
    tr("First cell:"),
    tr("Last cell:"),
    QString(QChar(0x03a3)),
    tr("Output to:"),
    cell1,
    cell2,
    out))
  {
    int col, row;
    decodePos(out, col, row);
    table->item(row, col)->setText(tr("sum ") + cell1 + " " + cell2);
  }
}


void MainWindow::actionMath_helper(
  QString title,
  QString op)
{
  QString cell1 = "C1";
  QString cell2 = "C2";
  QString out = "C3";

  QTableWidgetItem *current = table->currentItem();
  if (current)
  {
    out = encodePos(table->currentColumn(), table->currentRow());
  }

  if (runInputDialog(
    title,
    tr("Cell 1"),
    tr("Cell 2"),
    op,
    tr("Output to:"),
    cell1,
    cell2,
    out))
  {
    int col, row;
    decodePos(out, col, row);
    table->item(row, col)->setText(op + " " + cell1 + " " + cell2);
  }
}


void MainWindow::actionAdd()
{
  actionMath_helper(tr("Addition"), "+");
}


void MainWindow::actionSubtract()
{
  actionMath_helper(tr("Subtraction"), "-");
}


void MainWindow::actionMultiply()
{
  actionMath_helper(tr("Multiplication"), "*");
}


void MainWindow::actionDivide()
{
  actionMath_helper(tr("Division"), "/");
}


void MainWindow::clear()
{
  LasItemList selected = table->selectedItems();

  LasItemList::iterator i = selected.begin();
  while(i != selected.end())
  {
    (*i)->setText("");
    ++i;
  }
}
*/


/*
void MainWindow::setupContextMenu()
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
void MainWindow::setupContents()
{
  QColor titleBackground(Qt::lightGray);
  QFont titleFont = table->font();
  titleFont.setBold(true);

  // column 0
  table->setItem(0, 0, new LasSheetItem("ID number"));
  table->item(0, 0)->setBackgroundColor(titleBackground);
  table->item(0, 0)->setFont(titleFont);

  // column 1
  table->setItem(0, 1, new LasSheetItem("Name"));
  table->item(0, 1)->setBackgroundColor(titleBackground);
  table->item(0, 1)->setFont(titleFont);
}
*/

void MainWindow::displayBarcodeForm()
{
  if (barcodeViewForm->startScanning())
  {
    // Set auto orientation to false:
    setAttribute(static_cast<Qt::WidgetAttribute>(130), false);

    // Set landscape orientation to true:
    setAttribute(static_cast<Qt::WidgetAttribute>(129), false);

    barcodeViewForm->show();
  }
}


void MainWindow::clearCurrentCell()
{
  csvModel->setText(
    currentIndex.row(),
    currentIndex.column(),
    "");
}


void MainWindow::removeCurrentRow()
{
  csvModel->removeRow(
    currentIndex.row());
}


void MainWindow::load()
{
  QString filename = QFileDialog::getOpenFileName(
    this, "Select Spreadsheet File");

  if (filename.isEmpty()) return;

  currentFilename = filename;

  delete csvModel;

  csvModel = new LasCsvModel(currentFilename, this, true);

  table->setModel(csvModel);

  table->resizeAllCells();

  connect(
    table->selectionModel(),
    SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
    this,
    SLOT(updateToolbar(const QModelIndex &)));
}


void MainWindow::save()
{
  csvModel->toCSV(currentFilename, true);
}


void MainWindow::saveAs()
{
  QString filename = QFileDialog::getSaveFileName(
    this, "Select Spreadsheet File");

  if (filename.isEmpty()) return;

  currentFilename = filename;

  csvModel->toCSV(currentFilename);
}


void MainWindow::addNewRow(
  QString barcode,
  QString name,
  QString description,
  QString quantity,
  QString location)
{
  // Add a new row:
  int row = csvModel->rowCount();
  csvModel->insertRow(row);

  // Insert the data:
  csvModel->setText(row, 0, barcode);
  csvModel->setText(row, 1, name);
  csvModel->setText(row, 2, description);
  csvModel->setText(row, 3, quantity);
  csvModel->setText(row, 4, location);

  table->resizeAllCells();
}


void MainWindow::on_actionPreferences_triggered()
{
  preferencesForm->show();
}


void MainWindow::on_actionDocumentation_triggered()
{
  if (!documentationForm)
  {
    documentationForm = new LasDocumentationForm(this);
  }

  documentationForm->show();
}


void MainWindow::on_actionAbout_triggered()
{
  if (!aboutForm)
  {
    aboutForm = new LasAboutForm(this);
  }

  aboutForm->show();
}
