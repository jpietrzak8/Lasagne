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

#ifndef LASCAMERACONTROL_H
#define LASCAMERACONTROL_H

#include <QObject>

class QTimer;

class LasCameraControl: public QObject
{
  Q_OBJECT

public:
  LasCameraControl();

  ~LasCameraControl();

  void toggleTorch();
  void turnOffTorch();

  void startIncrementingFocus();
  void startDecrementingFocus();

  void startIncrementingExposure();
  void startDecrementingExposure();

  void stopTimer();

private slots:
  void initializeFocus();
  void focusUp();
  void focusDown();
  void exposureUp();
  void exposureDown();

private:
  int fileDescriptor;

  int originalFocusLevel;
  int focusLevel;

  int originalExposureLevel;
  int sunlightExposureLevel;
  int ledLightExposureLevel;

  QTimer *timer;

  bool torchOn;
};

#endif // LASCAMERACONTROL_H
