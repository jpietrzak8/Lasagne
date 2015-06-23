//
// mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <QToolBar>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QModelIndex>

namespace Ui {
    class MainWindow;
}

//class QTableWidgetItem;
class LasSheet;
class LasBarcodeViewForm;
class LasOptionsDialog;
class LasCsvModel;
//class QModelIndex;
class LasAboutForm;
class LasDocumentationForm;
class LasPreferencesForm;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  enum ScreenOrientation {
    ScreenOrientationLockPortrait,
    ScreenOrientationLockLandscape,
    ScreenOrientationAuto
  };

  explicit MainWindow(QWidget *parent = 0);

  virtual ~MainWindow();

  // Note that this will only have an effect on Symbian and Fremantle.
  void setOrientation(ScreenOrientation orientation);

  void showExpanded();

public slots:
//  void updateStatus(QTableWidgetItem *item);
//  void updateLineEdit(QTableWidgetItem *item);
  void returnPressed();

  void updateToolbar(
    const QModelIndex &index);

private slots:
  void displayBarcodeForm();
  void clearCurrentCell();
  void removeCurrentRow();
  void load();
  void save();
  void saveAs();

  void addNewRow(
    QString barcode,
    QString name,
    QString description,
    QString quantity,
    QString location);

  void on_actionPreferences_triggered();
  void on_actionDocumentation_triggered();
  void on_actionAbout_triggered();

private:
  Ui::MainWindow *ui;

  LasAboutForm *aboutForm;
  LasDocumentationForm *documentationForm;
  LasPreferencesForm *preferencesForm;

  QToolBar *toolBar;
  QLineEdit *formulaInput;
  QLabel *cellLabel;
  QPushButton *optionsButton;
  QPushButton *scanButton;

  QString currentFilename;
  LasCsvModel *csvModel;

  LasSheet *table;
  LasBarcodeViewForm *barcodeViewForm;

  LasOptionsDialog *optionsDialog;
  QModelIndex currentIndex;
};

#endif // MAINWINDOW_H
