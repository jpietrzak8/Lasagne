//
// lasdbus.h
//
// Copyright 2015 by John Pietrzak  (jpietrzak8@gmail.com)
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

#ifndef LASDBUS_H
#define LASDBUS_H

#include <QObject>
#include <QList>
#include <QString>
#include <QMetaType>

class QDBusInterface;

// Some annoying nowhere-documented types for use with DBus:
struct DBusProperty
{
  QString name;
  bool added;
  bool removed;
};
Q_DECLARE_METATYPE(DBusProperty)
Q_DECLARE_METATYPE(QList<DBusProperty>)

class LasDBus: public QObject
{
  Q_OBJECT

public:
  LasDBus();
  ~LasDBus();

  bool currentCameraCoverStatus();

//  void lockScreen();

signals:
  void cameraCoverChanged(
    bool coverClosed);

/*
  void cameraButtonChanged(
    bool buttonPressed);
*/

public slots:
  void cameraCoverPropertyModified(
    int count,
    QList<DBusProperty> properties);

/*
  void cameraLaunchPropertyModified(
    int count,
    QList<DBusProperty> properties);
*/

private:
  QDBusInterface *halCameraShutter;
//  QDBusInterface *halCameraButtonLaunch;
  QDBusInterface *mce;

  void startMCEInterface();
};

#endif // LASDBUS_H
