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

#include "lasbarcodeviewform.h"
#include "ui_lasbarcodeviewform.h"

//#include "lasbarcodescan.h"
#include "lascameracontrol.h"
#include "lasnewentrydialog.h"
#include <gst/interfaces/xoverlay.h>
#include <gst/app/gstappsink.h>
//#include <zbar.h>
#include <QX11Info>
#include <QTimer>
#include <QMaemo5InformationBox>
#include <QDebug>

// A C-style callback to receive messages from GStreamer:
static gboolean lasGstBusCallback(
  GstBus *bus,
  GstMessage *msg,
  gpointer lasGstObjectPtr)
{
  Q_UNUSED(bus);

  switch (GST_MESSAGE_TYPE(msg))
  {
  case GST_MESSAGE_EOS:
    {
      LasBarcodeViewForm *myForm =
        static_cast<LasBarcodeViewForm *>(lasGstObjectPtr);

      myForm->stopScanning();
    }
    break;

  case GST_MESSAGE_ERROR:
    {
      gchar *debug;
      GError *err;

      gst_message_parse_error(msg, &err, &debug);
      QString errString = "[";
      errString += GST_OBJECT_NAME(msg->src);
      errString += "]: ";
      errString += err->message;
      errString += " ";
      errString += debug;
      qWarning() << errString;

      LasBarcodeViewForm *myForm =
        static_cast<LasBarcodeViewForm *>(lasGstObjectPtr);

      QMaemo5InformationBox::information(myForm, errString);

      myForm->stopScanning();

      g_free(debug);
      g_error_free(err);
    }
    break;

  default:
    break;
  }

  return true; // why even bother with this? :)
}


// Callback for the beep sound:
static gboolean lasBeepCallback(
  GstBus *bus,
  GstMessage *msg,
  gpointer lasGstObjectPtr)
{
  Q_UNUSED(bus);

  switch (GST_MESSAGE_TYPE(msg))
  {
  case GST_MESSAGE_EOS:
    {
      LasBarcodeViewForm *myForm =
        static_cast<LasBarcodeViewForm *>(lasGstObjectPtr);

      myForm->cleanupBeep();
    }
    break;

  case GST_MESSAGE_ERROR:
    {
      gchar *debug;
      GError *err;

      gst_message_parse_error(msg, &err, &debug);
      QString errString = "[";
      errString += GST_OBJECT_NAME(msg->src);
      errString += "]: ";
      errString += err->message;
      errString += " ";
      errString += debug;
      qWarning() << errString;

      LasBarcodeViewForm *myForm =
        static_cast<LasBarcodeViewForm *>(lasGstObjectPtr);

      QMaemo5InformationBox::information(myForm, errString);

      g_free(debug);
      g_error_free(err);
    }
    break;

  default:
    break;
  }

  return true;
}


LasBarcodeViewForm::LasBarcodeViewForm(
  QWidget *parent)
  : QWidget(parent),
    ui(new Ui::LasBarcodeViewForm),
    newEntryDialog(0),
    tee_colorspace1_pad(0),
    tee_queue_pad(0),
    tee(0),
    appsink(0),
    runningElement(0),
    beepElement(0),
    playBeepPref(true),
    gstreamerInUse(false),
    sampleTimer(0),
    ignoreCameraCover(false)
{
  ui->setupUi(this);

  setAttribute(Qt::WA_Maemo5StackedWindow);
  setWindowFlags(windowFlags() | Qt::Window);

  newEntryDialog = new LasNewEntryDialog(this);

  connect(
    newEntryDialog,
    SIGNAL(accepted()),
    this,
    SLOT(marshallNewItem()));

  connect(
    newEntryDialog,
    SIGNAL(rejected()),
    this,
    SLOT(cleanupAfterDialog()));

  connect(
    &dbus,
    SIGNAL(cameraCoverChanged(bool)),
    this,
    SLOT(updateCameraCover(bool)));

  cameraCoverClosed = dbus.currentCameraCoverStatus();

  gst_init(NULL, NULL);
}


LasBarcodeViewForm::~LasBarcodeViewForm()
{
  if (sampleTimer) delete sampleTimer;

  if (beepElement) cleanupBeep();

  if (newEntryDialog) delete newEntryDialog;

  delete ui;
}


bool LasBarcodeViewForm::startScanning()
{
  // Check the camera cover:
  if (cameraCoverClosed && !ignoreCameraCover)
  {
    QString errMessage("Camera lens cover is closed");
    QMaemo5InformationBox::information(0, errMessage);
    qWarning() << errMessage;
    return false;
  }

  // Clean up after any existing scan first:
  if (gstreamerInUse) stopScanning();

  GstElement *camsrc = gst_element_factory_make("v4l2src", "camsrc");

  if (!camsrc)
  {
    printError("v4l2src");
    return false;
  }

  // Working with /dev/video0, that should always be right...
  g_object_set(
    G_OBJECT(camsrc),
    "device", "/dev/video0",
    "width", 640,
    "height", 480,
    (char*)NULL);

  GstElement *colorspace1 =
    gst_element_factory_make("ffmpegcolorspace", "colorspace1");

  if (!colorspace1)
  {
    printError("ffmpegcolorspace");
    return false;
  }

  tee = gst_element_factory_make("tee", "tee");

  if (!tee)
  {
    printError("tee");
    return false;
  }

  GstElement *scale = gst_element_factory_make("videoscale", "scale");

  if (!scale)
  {
    printError("videoscale");
    return false;
  }

  GstElement *xvsink = gst_element_factory_make("xvimagesink", "xvsink");

  if (!xvsink)
  {
    printError("xvimagesink");
    return false;
  }

  unsigned long windowId = ui->videoWidget->winId();
  QApplication::syncX();
  gst_x_overlay_set_xwindow_id(
    GST_X_OVERLAY(G_OBJECT(xvsink)),
    windowId);

/*
qDebug() << "x, y, w, h: "
<< ui->videoWidget->x() << ", "
<< ui->videoWidget->y() << ", "
<< ui->videoWidget->width() << ", "
<< ui->videoWidget->height();

  gst_x_overlay_set_window_handle(
    GST_X_OVERLAY(G_OBJECT(xvsink)),
    ui->videoWidget->x(),
    ui->videoWidget->y(),
    ui->videoWidget->width(),
    ui->videoWidget->height());
*/

  gst_element_set_state(xvsink, GST_STATE_READY);

  g_object_set(G_OBJECT(xvsink), "force_aspect_ratio", true, (char*)NULL);

  GstElement *queue = gst_element_factory_make("queue", "queue");

  if (!queue)
  {
    printError("queue");
    return false;
  }

  GstElement *colorspace2 =
    gst_element_factory_make("ffmpegcolorspace", "colorspace2");

  if (!colorspace2)
  {
    printError("ffmpegcolorspace");
    return false;
  }

  // Set colors to Y800:
  GstElement *colorFilter = gst_element_factory_make("capsfilter", "colorFilter");

  if (!colorFilter)
  {
    printError("capsfilter");
    return false;
  }

  GstCaps *caps = gst_caps_new_empty();
  GstStructure *cs = gst_structure_new(
    "video/x-raw-yuv",
    "format", GST_TYPE_FOURCC, GST_MAKE_FOURCC('Y', '8', '0', '0'),
    (char*)NULL);
  gst_caps_append_structure(caps, cs);

  g_object_set(G_OBJECT(colorFilter), "caps", caps, (char*)NULL);
  gst_caps_unref(caps);

  appsink = gst_element_factory_make("appsink", "appsink");

  if (!appsink)
  {
    printError("appsink");
    return false;
  }

  // Only store two images in the buffer.  Could probably get away with
  // just storing one...
  // Also, throw away the oldest buffer every time a new one appears:
  g_object_set(
    G_OBJECT(appsink),
    "max-buffers", 2,
    "drop", true,
    (char*)NULL);

  gst_element_set_state(appsink, GST_STATE_READY);

  GstElement *finalPipe = gst_pipeline_new("finalPipe");

  if (!finalPipe)
  {
    printError("finalPipe");
    return false;
  }

  gst_bin_add_many(
    GST_BIN(finalPipe),
    camsrc,
    tee,
    colorspace1,
    scale,
    xvsink,
    queue,
    colorspace2,
    colorFilter,
    appsink,
    (char*)NULL);

  if (!gst_element_link(camsrc, tee))
  {
    printLinkError("camsrc", "tee");
    return false;
  }

  if (!gst_element_link(colorspace1, scale))
  {
    printLinkError("colorspace1", "scale");
    return false;
  }

  if (!gst_element_link(scale, xvsink))
  {
    printLinkError("scale", "xvsink");
    return false;
  }

  if (!gst_element_link(queue, colorspace2))
  {
    printLinkError("queue", "colorspace2");
    return false;
  }

  if (!gst_element_link(colorspace2, colorFilter))
  {
    printLinkError("colorspace2", "colorFilter");
    return false;
  }

  if (!gst_element_link(colorFilter, appsink))
  {
    printLinkError("colorFilter", "appsink");
    return false;
  }

  // Hooking up the tee requires a little more work:

  tee_colorspace1_pad = gst_element_get_request_pad(tee, "src%d");
  GstPad *colorspace1_pad = gst_element_get_static_pad(colorspace1, "sink");
  tee_queue_pad = gst_element_get_request_pad(tee, "src%d");
  GstPad *queue_pad = gst_element_get_static_pad(queue, "sink");

  if ( gst_pad_link(tee_colorspace1_pad, colorspace1_pad) != GST_PAD_LINK_OK
    || gst_pad_link(tee_queue_pad, queue_pad) != GST_PAD_LINK_OK)
  {
    printLinkError("tee", "tee clients");
    return false;
  }

  gst_object_unref(colorspace1_pad);
  gst_object_unref(queue_pad);

  runningElement = finalPipe;

  GstBus *bus = gst_element_get_bus(GST_ELEMENT(runningElement));
  gst_bus_add_watch(bus, lasGstBusCallback, this);
  gst_object_unref(bus);

  gst_element_set_state(runningElement, GST_STATE_PLAYING);
  gstreamerInUse = true;

  // Set up the sample timer:
  sampleTimer = new QTimer(this);
  connect(sampleTimer, SIGNAL(timeout()), this, SLOT(getSample()));
  sampleTimer->start(500);

  return true;
}


void LasBarcodeViewForm::closeEvent(
  QCloseEvent *event)
{
  stopScanning();

  // For some reason, torch is automatically turned off when v4l2src element
  // closes.  However, can't guarantee that (undocumented) feature, so
  // manually turn it off as well (shouldn't hurt):
  camera.turnOffTorch();

  QWidget::closeEvent(event);

  // Set landscape orientation to false:
  parentWidget()->setAttribute(static_cast<Qt::WidgetAttribute>(129), false);
  // Set auto orientation to true:
  parentWidget()->setAttribute(static_cast<Qt::WidgetAttribute>(130), true);
}


void LasBarcodeViewForm::stopScanning()
{
  if (sampleTimer)
  {
    delete sampleTimer;
    sampleTimer = 0;
  }

  if (!runningElement) return;

  gst_element_set_state(runningElement, GST_STATE_NULL);

  gst_object_unref(GST_OBJECT(runningElement));

  runningElement = 0;

  gstreamerInUse = false;
}


void LasBarcodeViewForm::printError(
  QString gstName)
{
  QString err("Unable to create GStreamer element '");
  err += gstName;
  err += "'";
  QMaemo5InformationBox::information(this, err);
  qWarning() << err;
}


void LasBarcodeViewForm::printLinkError(
  QString firstItem,
  QString secondItem)
{
  QString err("Unable to link '");
  err += firstItem;
  err += "' to '";
  err += secondItem;
  err += "'";
  QMaemo5InformationBox::information(this, err);
  qWarning() << err;
}


void LasBarcodeViewForm::on_lightButton_clicked()
{
  camera.toggleTorch();
}


void LasBarcodeViewForm::on_focusUpButton_pressed()
{
  camera.startIncrementingFocus();
}

void LasBarcodeViewForm::on_focusUpButton_released()
{
  camera.stopTimer();
}

void LasBarcodeViewForm::on_focusDownButton_pressed()
{
  camera.startDecrementingFocus();
}

void LasBarcodeViewForm::on_focusDownButton_released()
{
  camera.stopTimer();
}

void LasBarcodeViewForm::on_expUpButton_pressed()
{
  camera.startIncrementingExposure();
}

void LasBarcodeViewForm::on_expUpButton_released()
{
  camera.stopTimer();
}

void LasBarcodeViewForm::on_expDownButton_pressed()
{
  camera.startDecrementingExposure();
}

void LasBarcodeViewForm::on_expDownButton_released()
{
  camera.stopTimer();
}


static int blah = 0;
void LasBarcodeViewForm::getSample()
{
qDebug() << "Getting Sample " << ++blah;
  GstBuffer *buffer = gst_app_sink_pull_buffer((GstAppSink *)appsink);

  zbar::Image image(640, 480, "Y800", buffer->data, 640 * 480);

  int scanResults = scanner.scan(image);

  if (scanResults < 0)
  {
    qWarning() << "Zbar scanner had an error.";
    return;
  }
  else if (scanResults == 0)
  {
    // No symbols found.
    return;
  }

  zbar::Image::SymbolIterator symbol = image.symbol_begin();

  if (symbol == image.symbol_end())
  {
    // No symbols found.
    return;
  }

  // Make a sound:
  playBeep();

  // Run the new entry dialog:
  newEntryDialog->setupDialog(QString::fromStdString(symbol->get_data()));
  newEntryDialog->exec();

  // Pause the video and stop the timer:
//  gst_element_set_state(runningElement, GST_STATE_PAUSED);
//  sampleTimer->stop();
}


void LasBarcodeViewForm::playBeep()
{
  if (!playBeepPref)
  {
    // User doesn't want a beep.
    return;
  }

  if (beepElement)
  {
    // Don't step on an already running beep.
    return;
  }

  beepElement = gst_element_factory_make("playbin2", "beepElement");

  if (!beepElement)
  {
    printError("playbin2");
    return;
  }

  QString beepFilename = "file:///opt/lasagne/audio/beep.wav"; // work on this
  QByteArray ba = beepFilename.toAscii();
  g_object_set(
    G_OBJECT(beepElement),
    "uri",
    ba.data(),
    NULL);

  gst_element_set_state(beepElement, GST_STATE_PLAYING);

  GstBus *bus = gst_element_get_bus (GST_ELEMENT(beepElement));
  gst_bus_add_watch(bus, lasBeepCallback, this);
  gst_object_unref(bus);
}


void LasBarcodeViewForm::cleanupBeep()
{
  if (!beepElement) return;

  gst_element_set_state(beepElement, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT(beepElement));
  beepElement = 0;
}


void LasBarcodeViewForm::on_closeButton_clicked()
{
  // Close this window:
  close();
}


void LasBarcodeViewForm::marshallNewItem()
{
  emit barcodeFound(
    newEntryDialog->getBarcode(),
    newEntryDialog->getName(),
    newEntryDialog->getDescription(),
    newEntryDialog->getQuantity(),
    newEntryDialog->getLocation());

  cleanupAfterDialog();
}


void LasBarcodeViewForm::cleanupAfterDialog()
{
  // Restart the scanning:
//  gst_element_set_state(runningElement, GST_STATE_PLAYING);
//  sampleTimer->start();
}


void LasBarcodeViewForm::updateCameraCover(
  bool coverStatus)
{
  cameraCoverClosed = coverStatus;

  // If we're currently ignoring this value, don't worry about it:
  if (ignoreCameraCover) return;

  if (cameraCoverClosed)
  {
    // Need to turn off LED, and pause video:
    camera.turnOffTorch();

    if (runningElement)
    {
//      gst_element_set_state(runningElement, GST_STATE_PAUSED);
    }

    if (sampleTimer)
    {
//      sampleTimer->stop();
    }
  }
  else
  {
    // If the video isn't already blocked by the dialog box, restart it:
    if (!newEntryDialog->isVisible())
    {
      // Video can be restarted, if it was off:
      if (runningElement)
      {
//        gst_element_set_state(runningElement, GST_STATE_PLAYING);
      }

      if (sampleTimer)
      {
//        sampleTimer->start();
      }
    }
  }
}


void LasBarcodeViewForm::updateIgnoreCover(
  bool value)
{
  ignoreCameraCover = value;
}


void LasBarcodeViewForm::setBeepPref(
  bool pref)
{
  playBeepPref = pref;
}

