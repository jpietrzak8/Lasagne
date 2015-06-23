//
// lasdocumentationform.h
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

#ifndef LASDOCUMENTATIONFORM_H
#define LASDOCUMENTATIONFORM_H

#include <QWidget>

namespace Ui {
class LasDocumentationForm;
}

class LasDocumentationForm : public QWidget
{
  Q_OBJECT
  
public:
  explicit LasDocumentationForm(QWidget *parent = 0);
  ~LasDocumentationForm();
  
private:
  Ui::LasDocumentationForm *ui;
};

#endif // LASDOCUMENTATIONFORM_H
