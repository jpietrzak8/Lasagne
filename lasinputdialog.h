//
// lasinputdialog.h
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

#ifndef LASINPUTDIALOG_H
#define LASINPUTDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class LasInputDialog;
}


class LasInputDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit LasInputDialog(QWidget *parent = 0);
  ~LasInputDialog();

  void setupFields(
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
    int outRow);

signals:
  void inputDialogResult(
    QString op,
    QString cell1,
    QString cell2,
    QString outCell);

public slots:
  void accept();
  
private:
  Ui::LasInputDialog *ui;
};

#endif // LASINPUTDIALOG_H
