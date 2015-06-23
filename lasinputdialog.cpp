//
// lasinputdialog.cpp
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

#include "lasinputdialog.h"
#include "ui_lasinputdialog.h"

LasInputDialog::LasInputDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LasInputDialog)
{
  ui->setupUi(this);
}

LasInputDialog::~LasInputDialog()
{
  delete ui;
}


void LasInputDialog::setupFields(
  int columnCount,
  int rowCount,
  QString title,
  QString c1Text,
  int c1Col,
  int c1Row,
  QString c2Text,
  int c2Col,
  int c2Row,
  QString opText,
  QString outText,
  int outCol,
  int outRow)
{
  QStringList cols, rows;

  int i = 0;
  while (i < columnCount)
  {
    cols << QChar('A' + i);
    ++i;
  }

  i = 0;
  while (i < rowCount)
  {
    rows << QString::number(1 + i);
    ++i;
  }

  setWindowTitle(title);

  ui->cell1Label->setText(c1Text);

  ui->c1ColComboBox->clear();
  ui->c1ColComboBox->addItems(cols);
  ui->c1ColComboBox->setCurrentIndex(c1Col);

  ui->c1RowComboBox->clear();
  ui->c1RowComboBox->addItems(rows);
  ui->c1RowComboBox->setCurrentIndex(c1Row);

  ui->operatorLabel->setText(opText);

  ui->cell2Label->setText(c2Text);

  ui->c2ColComboBox->clear();
  ui->c2ColComboBox->addItems(cols);
  ui->c2ColComboBox->setCurrentIndex(c2Col);

  ui->c2RowComboBox->clear();
  ui->c2RowComboBox->addItems(rows);
  ui->c2RowComboBox->setCurrentIndex(c2Row);

  ui->outLabel->setText(outText);

  ui->outColComboBox->clear();
  ui->outColComboBox->addItems(cols);
  ui->outColComboBox->setCurrentIndex(outCol);

  ui->outRowComboBox->clear();
  ui->outRowComboBox->addItems(rows);
  ui->outRowComboBox->setCurrentIndex(outRow);
}


void LasInputDialog::accept()
{
  QString cell1 =
    ui->c1ColComboBox->currentText() + ui->c1RowComboBox->currentText();

  QString cell2 =
    ui->c2ColComboBox->currentText() + ui->c2RowComboBox->currentText();

  QString outCell =
    ui->outColComboBox->currentText() + ui->outRowComboBox->currentText();

  emit inputDialogResult(ui->operatorLabel->text(), cell1, cell2, outCell);

  QDialog::accept();
}
