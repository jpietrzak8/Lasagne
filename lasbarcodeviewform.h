//
// lasbarcodeviewform.cpp
//
// Copyright 2015 by John Pietrzak (jpietrzak8@gmail.com)
//
// This file is part of Lasagne.
//
// Lasagne is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
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

#ifndef LASBARCODEVIEWFORM_H
#define LASBARCODEVIEWFORM_H

#include <QWidget>

#include <gst/gst.h>
#include <zbar.h>

#include "lascameracontrol.h"
#include "lasdbus.h"

class QCloseEvent;
class QTimer;
class LasNewEntryDialog;

namespace Ui {
class LasBarcodeViewForm;
}


class LasBarcodeViewForm : public QWidget
{
  Q_OBJECT
  
public:
  explicit LasBarcodeViewForm(QWidget *parent = 0);
  ~LasBarcodeViewForm();

  bool startScanning();

  void stopScanning();

  void playBeep();

  void cleanupBeep();

public slots:
  void setBeepPref(
    bool pref);

  void updateIgnoreCover(
    bool pref);

signals:
  void barcodeFound(
    QString barcode,
    QString name,
    QString description,
    QString quantity,
    QString location);

protected:
  void closeEvent(
    QCloseEvent *event);
  
private slots:
  void on_lightButton_clicked();

  void on_focusUpButton_pressed();
  void on_focusUpButton_released();
  void on_focusDownButton_pressed();
  void on_focusDownButton_released();

  void on_expUpButton_pressed();
  void on_expUpButton_released();
  void on_expDownButton_pressed();
  void on_expDownButton_released();
  void on_closeButton_clicked();

  void getSample();

  void marshallNewItem();
  void cleanupAfterDialog();

  void updateCameraCover(bool value);

private:
  void printError(
    QString gstName);

  void printLinkError(
    QString firstItem,
    QString secondItem);

  Ui::LasBarcodeViewForm *ui;

  LasNewEntryDialog *newEntryDialog;

  GstPad *tee_colorspace1_pad;
  GstPad *tee_queue_pad;
  GstElement *tee;
  GstElement *appsink;

  GstElement *runningElement;

  GstElement *beepElement;
  bool playBeepPref;

  bool gstreamerInUse;

  LasCameraControl camera;
  zbar::ImageScanner scanner;

  QTimer *sampleTimer;

  LasDBus dbus;
  bool cameraCoverClosed;
  bool ignoreCameraCover;
};

#endif // LASBARCODEVIEWFORM_H
