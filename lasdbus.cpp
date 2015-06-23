//
// lasdbus.cpp
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

#include "lasdbus.h"

#include <QDBusInterface>
#include <QDBusMetaType>

#include <iostream>

// Some odd operators required for getting the DBus parameters right:
const QDBusArgument & operator<<(
  QDBusArgument &arg,
  const DBusProperty &change)
{
  arg.beginStructure();
  arg << change.name << change.added << change.removed;
  arg.endStructure();
  return arg;
}


const QDBusArgument & operator>>(
  const QDBusArgument &arg,
  DBusProperty &change)
{
  arg.beginStructure();
  arg >> change.name >> change.added >> change.removed;
  arg.endStructure();
  return arg;
}


// Now, on to the actual LasDBus methods:

LasDBus::LasDBus()
  : halCameraShutter(0),
//    halCameraButtonLaunch(0),
    mce(0)
{
  // Set up the camera shutter interface:

  halCameraShutter = new QDBusInterface(
    "org.freedesktop.Hal",
    "/org/freedesktop/Hal/devices/platform_cam_shutter",
    "org.freedesktop.Hal.Device",
    QDBusConnection::systemBus());

  // Set up the camera button interface:

/*
  halCameraButtonLaunch = new QDBusInterface(
    "org.freedesktop.Hal",
    "/org/freedesktop/Hal/devices/platform_cam_launch",
    "org.freedesktop.Hal.Device",
    QDBusConnection::systemBus());
*/

  // Some annoying QT DBus metatypes:
  qDBusRegisterMetaType<DBusProperty>();
  qDBusRegisterMetaType<QList<DBusProperty> >();

  // Connect any camera cover updates to our cover slot:
  QDBusConnection::systemBus().connect(
    "",
    "/org/freedesktop/Hal/devices/platform_cam_shutter",
    "org.freedesktop.Hal.Device",
    "PropertyModified",
    this,
    SLOT(cameraCoverPropertyModified(int, QList<DBusProperty>)));

  // Connect any camera button updates to our button slot:
/*
  QDBusConnection::systemBus().connect(
    "",
    "/org/freedesktop/Hal/devices/platform_cam_launch",
    "org.freedesktop.Hal.Device",
    "PropertyModified",
    this,
    SLOT(cameraLaunchPropertyModified(int, QList<DBusProperty>)));
*/
}


LasDBus::~LasDBus()
{
  if (halCameraShutter) delete halCameraShutter;
}


bool LasDBus::currentCameraCoverStatus()
{
  QDBusMessage message =
    halCameraShutter->call("GetProperty", "button.state.value");

  return message.arguments().at(0).toBool();
}


/*
void LasDBus::lockScreen()
{
  if (!mce)
  {
    startMCEInterface();
  }

  mce->call("req_tklock_mode_change", "locked");
}
*/


void LasDBus::cameraCoverPropertyModified(
  int count,
  QList<DBusProperty> properties)
{
  Q_UNUSED(count);
  Q_UNUSED(properties);

  QDBusMessage message =
    halCameraShutter->call("GetProperty", "button.state.value");

  // Should check for errors here!

  emit cameraCoverChanged(message.arguments().at(0).toBool());
}


/*
void LasDBus::cameraLaunchPropertyModified(
  int count,
  QList<DBusProperty> properties)
{
  Q_UNUSED(count);
  Q_UNUSED(properties);

  QDBusMessage message =
    halCameraButtonLaunch->call("GetProperty", "button.state.value");

  emit cameraButtonChanged(message.arguments().at(0).toBool());
}
*/


void LasDBus::startMCEInterface()
{
  mce = new QDBusInterface(
    "com.nokia.mce",
    "/com/nokia/mce/request",
    "com.nokia.mce.request",
    QDBusConnection::systemBus());
}
