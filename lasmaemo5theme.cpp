//
// lasmaemy5theme.cpp
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

#include "lasmaemo5theme.h"

#include <QMaemo5Style>

LasMaemo5Theme::LasMaemo5Theme()
{
/*
  QColor color;
  QString red, green, blue;

  color = QMaemo5Style::standardColor("DefaultBackgroundColor");

  red.setNum(color.red());
  green.setNum(color.green());
  blue.setNum(color.blue());

  backgroundColor = "rgb(" + red + ", " + green + ", " + blue + ")";

  color = QMaemo5Style::standardColor("DefaultTextColor");

  red.setNum(color.red());
  green.setNum(color.green());
  blue.setNum(color.blue());

  defaultTextColor = "rgb(" + red + ", " + green + ", " + blue + ")";

  color = QMaemo5Style::standardColor("ActiveTextColor");

  red.setNum(color.red());
  green.setNum(color.green());
  blue.setNum(color.blue());

  activeTextColor = "rgb(" + red + ", " + green + ", " + blue + ")";

  color = QMaemo5Style::standardColor("SecondaryTextColor");

  red.setNum(color.red());
  green.setNum(color.green());
  blue.setNum(color.blue());

  secondaryTextColor = "rgb(" + red + ", " + green + ", " + blue + ")";

  color = QMaemo5Style::standardColor("AccentColor1");

  red.setNum(color.red());
  green.setNum(color.green());
  blue.setNum(color.blue());

  accentColor = "rgb(" + red + ", " + green + ", " + blue + ")";

  QFont font = QMaemo5Style::standardFont("SystemFont");

  systemFontFamily = font.family();
*/
}


QColor LasMaemo5Theme::getBackgroundColor() const
{
  return QMaemo5Style::standardColor("DefaultBackgroundColor");
}


QColor LasMaemo5Theme::getDefaultTextColor() const
{
  return QMaemo5Style::standardColor("DefaultTextColor");
}


QColor LasMaemo5Theme::getActiveTextColor() const
{
  return QMaemo5Style::standardColor("ActiveTextColor");
}


QColor LasMaemo5Theme::getSecondaryTextColor() const
{
  return QMaemo5Style::standardColor("SecondaryTextColor");
}


QColor LasMaemo5Theme::getAccentColor() const
{
  return QMaemo5Style::standardColor("AccentColor1");
}


QFont LasMaemo5Theme::getSystemFont() const
{
  return QMaemo5Style::standardFont("SystemFont");
}
