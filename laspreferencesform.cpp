//
// laspreferencesform.cpp
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

#include "laspreferencesform.h"
#include "ui_laspreferencesform.h"

#include <QSettings>

LasPreferencesForm::LasPreferencesForm(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::LasPreferencesForm)
{
  ui->setupUi(this);

  setAttribute(Qt::WA_Maemo5StackedWindow);
  setWindowFlags(windowFlags() | Qt::Window);

  QSettings settings("pietrzak.org", "Lasagne");

  if (settings.contains("BeepForEachBarcode"))
  {
    ui->beepBox->setChecked(settings.value("BeepForEachBarcode").toBool());
  }

  if (settings.contains("IgnoreLensCover"))
  {
    ui->lensCoverBox->setChecked(settings.value("IgnoreLensCover").toBool());
  }

  connect(
    ui->beepBox,
    SIGNAL(toggled(bool)),
    this,
    SIGNAL(beepPreference(bool)));

  connect(
    ui->lensCoverBox,
    SIGNAL(toggled(bool)),
    this,
    SIGNAL(ignoreLensCover(bool)));
}

LasPreferencesForm::~LasPreferencesForm()
{
  QSettings settings("pietrzak.org", "Lasagne");

  settings.setValue("BeepForEachBarcode", ui->beepBox->isChecked());
  settings.setValue("IgnoreLensCover", ui->lensCoverBox->isChecked());

  delete ui;
}


bool LasPreferencesForm::getBeepPref()
{
  return ui->beepBox->isChecked();
}


bool LasPreferencesForm::getIgnoreCover()
{
  return ui->lensCoverBox->isChecked();
}
