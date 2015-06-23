//
// lascameracontrol.cpp
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

#include "lascameracontrol.h"
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>


LasCameraControl::LasCameraControl()
  : fileDescriptor(-1),
    originalFocusLevel(0),
    focusLevel(0),
    originalExposureLevel(200000),
    sunlightExposureLevel(200000),
    ledLightExposureLevel(6000),
    timer(0),
    torchOn(false)
{
  // First, need to ensure that the camera is not running:
  QProcess process;
  process.execute("/usr/sbin/dsmetool -k /usr/bin/camera-ui");

  fileDescriptor = open("/dev/video0", O_RDWR, 0);

  if (fileDescriptor == -1)
  {
    QString err("Failed to connect to /dev/video0.\nError is ");
    err += strerror(errno);
//    QMaemo5InformationBox::information(this, err);
    qWarning() << err;
  }
  else
  {
    struct v4l2_control ctrl;
    ctrl.id = V4L2_CID_FOCUS_ABSOLUTE;

    if (ioctl(fileDescriptor, VIDIOC_G_CTRL, &ctrl) == -1)
    {
      qWarning() << "Unable to query focus, err: " << strerror(errno);
    }
    else
    {
      originalFocusLevel = ctrl.value;
      focusLevel = ctrl.value;
      qWarning() << "Focus value: " << ctrl.value;
    }

    ctrl.id = V4L2_CID_EXPOSURE;

    if (ioctl(fileDescriptor, VIDIOC_G_CTRL, &ctrl) == -1)
    {
      qWarning() << "Unable to get exposure, err: "
        << strerror(errno);
    }
    else
    {
      originalExposureLevel = ctrl.value;
      qWarning() << "Exposure value: " << ctrl.value;
    }

    // Gently initialize the focus level:
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(initializeFocus()));
    timer->start(50);
  }
}


LasCameraControl::~LasCameraControl()
{
  if (torchOn) toggleTorch();

  // Reset the focus:
  timespec sleeptime;
  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = 50000000;
  timespec remainingtime;
  if (focusLevel > originalFocusLevel)
  {
    while (focusLevel > originalFocusLevel)
    {
      focusDown();
      if (nanosleep(&sleeptime, &remainingtime) == -1)
      {
        qWarning() << "Failed to sleep, error: " << strerror(errno);
      }
    }
  }
  else
  {
    while (focusLevel < originalFocusLevel)
    {
      focusUp();
      if (nanosleep(&sleeptime, &remainingtime) == -1)
      {
        qWarning() << "Failed to sleep, error: " << strerror(errno);
      }
    }
  }

  if (fileDescriptor >= 0)
  {
    close(fileDescriptor);
  }

  if (timer) delete timer;

  // Restart the camera:
  QProcess process;
  process.execute("/usr/sbin/dsmetool -t /usr/bin/camera-ui");
}


void LasCameraControl::toggleTorch()
{
  struct v4l2_control ctrl;

  // Sanity check:
  if (fileDescriptor == -1)
  {
    // An error message should have already been shown...
    return;
  }

  ctrl.id = V4L2_CID_TORCH_INTENSITY;

  if (torchOn)
  {
    ctrl.value = 0;
    torchOn = false;
  }
  else
  {
    ctrl.value = 1;
    torchOn = true;
  }

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString err("Failed to set torch intensity to ");
    err += ctrl.value;
    err += "\nError is ";
    err += strerror(errno);
//    QMaemo5InformationBox::information(this, err);
    qWarning() << err;
  }

  ctrl.id = V4L2_CID_EXPOSURE;
  if (torchOn)
  {
    ctrl.value = ledLightExposureLevel;
  }
  else
  {
    ctrl.value = sunlightExposureLevel;
  }

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    qWarning() << "Unable to set exposure to " << ctrl.value;
    qWarning() << ", error is: " << strerror(errno);
  }
}


void LasCameraControl::turnOffTorch()
{
  struct v4l2_control ctrl;

  // Sanity check:
  if (fileDescriptor == -1)
  {
    // An error message should have already been shown...
    return;
  }

  ctrl.id = V4L2_CID_TORCH_INTENSITY;

  ctrl.value = 0;
  torchOn = false;

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString err("Failed to set torch intensity to ");
    err += ctrl.value;
    err += "\nError is ";
    err += strerror(errno);
//    QMaemo5InformationBox::information(this, err);
    qWarning() << err;
  }

  ctrl.id = V4L2_CID_EXPOSURE;
  ctrl.value = sunlightExposureLevel;

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    qWarning() << "Unable to set exposure to " << ctrl.value;
    qWarning() << ", error is: " << strerror(errno);
  }
}


void LasCameraControl::startIncrementingFocus()
{
  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_FOCUS_ABSOLUTE;

  if (ioctl(fileDescriptor, VIDIOC_G_CTRL, &ctrl) == -1)
  {
    // Don't try to move the lens if you don't know the current focus level!
    qWarning() << "Unable to query focus, err: " << strerror(errno);
    return;
  }
  else
  {
    focusLevel = ctrl.value;
  }

  if (timer) delete timer;
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(focusUp()));
  timer->start(100);
}


void LasCameraControl::startDecrementingFocus()
{
  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_FOCUS_ABSOLUTE;

  if (ioctl(fileDescriptor, VIDIOC_G_CTRL, &ctrl) == -1)
  {
    // Don't try to move the lens if you don't know the current focus level!
    qWarning() << "Unable to query focus, err: " << strerror(errno);
    return;  // Should return error code here
  }
  else
  {
    focusLevel = ctrl.value;
  }

  if (timer) delete timer;
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(focusDown()));
  timer->start(100);
}


void LasCameraControl::startIncrementingExposure()
{
/*
  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_EXPOSURE;

  if (ioctl(fileDescriptor, VIDIOC_G_CTRL, &ctrl) == -1)
  {
    qWarning() << "Unable to query exposure, err: " << strerror(errno);
    return;
  }
  else
  {
    exposureLevel = ctrl.value;
qDebug() << "exposure: " << ctrl.value;
  }
*/

  if (timer) delete timer;
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(exposureUp()));
  timer->start(100);
}


void LasCameraControl::startDecrementingExposure()
{
/*
  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_EXPOSURE;

  if (ioctl(fileDescriptor, VIDIOC_G_CTRL, &ctrl) == -1)
  {
    qWarning() << "Unable to query exposure, err: " << strerror(errno);
    return;
  }
  else
  {
    exposureLevel = ctrl.value;
qDebug() << "exposure: " << ctrl.value;
  }
*/

  if (timer) delete timer;
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(exposureDown()));
  timer->start(100);
}


void LasCameraControl::stopTimer()
{
  if (timer)
  {
    delete timer;
    timer = 0;
  }
}


void LasCameraControl::initializeFocus()
{
  // sanity check:
  if (fileDescriptor == -1)
  {
    return;
  }

  // We'll reach the end when we've reached 700:
  if (focusLevel >= 700)
  {
    // Stop the timer and return:
    // (Could probably do this in a cleaner way...)
    if (timer)
    {
      delete timer;
      timer = 0;
    }
    return;
  }

  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_FOCUS_ABSOLUTE;

  focusLevel += 10;
  ctrl.value = focusLevel;

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    qWarning() << "Unable to set focus to: " << ctrl.value
      << ", err: " << strerror(errno);
    return;
  }
}


void LasCameraControl::focusUp()
{
  // sanity check:
  if (fileDescriptor == -1)
  {
    return;
  }

  // Don't bother if we've reached the end:
  if (focusLevel == 1023)
  {
    return;
  }

  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_FOCUS_ABSOLUTE;

  focusLevel += 10;
  if (focusLevel > 1023)
  {
    focusLevel = 1023;
  }

  ctrl.value = focusLevel;

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString err("Unable to set focus to ");
    err += ctrl.value;
    err += "\nError is ";
    err += strerror(errno);
//    QMaemo5InformationBox::information(this, err);
    qWarning() << err;
  }
qDebug() << "focus level: " << ctrl.value;
}


void LasCameraControl::focusDown()
{
  // sanity check:
  if (fileDescriptor == -1)
  {
    return;
  }

  // Don't bother if we've reached the end:
  if (focusLevel == 0)
  {
    return;
  }

  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_FOCUS_ABSOLUTE;

  focusLevel -= 10;
  if (focusLevel < 0)
  {
    focusLevel = 0;
  }

  ctrl.value = focusLevel;

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString err("Unable to set focus to ");
    err += ctrl.value;
    err += "\nError is ";
    err += strerror(errno);
//    QMaemo5InformationBox::information(this, err);
    qWarning() << err;
  }
qDebug() << "Focus level: " << ctrl.value;
}


void LasCameraControl::exposureUp()
{
  // sanity check:
  if (fileDescriptor == -1)
  {
    return;
  }

  int exposureLevel;

  if (torchOn)
  {
    exposureLevel = ledLightExposureLevel;
  }
  else
  {
    exposureLevel = sunlightExposureLevel;
  }

  // Stop if we've reached the end.
  // Not sure about the units here...
  if (exposureLevel == 65535) 
  {
    return;
  }

  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_EXPOSURE;

  exposureLevel += 500;
  if (exposureLevel > 65535)
  {
    exposureLevel = 65535;
  }

  ctrl.value = exposureLevel;

qDebug() << "Setting exposure to " << ctrl.value;
  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString err("Unable to set exposure to ");
    err += ctrl.value;
    err += "\nError is ";
    err += strerror(errno);
//    QMaemo5InformationBox::information(this, err);
    qWarning() << err;
  }
  else
  {
    // Store the new level:
    if (torchOn)
    {
      ledLightExposureLevel = exposureLevel;
    }
    else
    {
      sunlightExposureLevel = exposureLevel;
    }
  }
}


void LasCameraControl::exposureDown()
{
  // sanity check:
  if (fileDescriptor == -1)
  {
    return;
  }

  int exposureLevel;

  if (torchOn)
  {
    exposureLevel = ledLightExposureLevel;
  }
  else
  {
    exposureLevel = sunlightExposureLevel;
  }

  // Stop if we've reached the end.
  if (exposureLevel == 1) 
  {
    return;
  }

  struct v4l2_control ctrl;
  ctrl.id = V4L2_CID_EXPOSURE;

  exposureLevel -= 500;
  if (exposureLevel < 1)
  {
    exposureLevel = 1;
  }

  ctrl.value = exposureLevel;

qDebug() << "Setting exposure to " << ctrl.value;
  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString err("Unable to set exposure to ");
    err += ctrl.value;
    err += "\nError is ";
    err += strerror(errno);
//    QMaemo5InformationBox::information(this, err);
    qWarning() << err;
  }
  else
  {
    // Store the new level:
    if (torchOn)
    {
      ledLightExposureLevel = exposureLevel;
    }
    else
    {
      sunlightExposureLevel = exposureLevel;
    }
  }
}
