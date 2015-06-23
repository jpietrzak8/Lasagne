//
// lascsvmodel.cpp
//
// Note that the following is modified from the LibQxt original.  As that
// project is now defunct, and I did not need the entire LibQxt functionality
// (just the CSV support), I've gone ahead and customized the code for my
// own needs.  --John Pietrzak
//
/****************************************************************************
** Copyright (c) 2006 - 2011, the LibQxt project.
** See the Qxt AUTHORS file for a list of authors and copyright holders.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the LibQxt project nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** <http://libqxt.org>  <foundation@libqxt.org>
*****************************************************************************/


#include "lascsvmodel.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

/*!
  Creates an empty LasCsvModel with parent \a parent.
  */
LasCsvModel::LasCsvModel(
  QObject *parent)
  : QAbstractTableModel(parent),
    maxColumn(0),
    quoteMode(LasCsvModel::DefaultQuoteMode)
{
}


/*!
  Creates a LasCsvModel with the parent \a parent and content loaded from
  \a file.

  See \a setSource for information on the \a withHeader and \a separator
  properties, or if you need control over the quoting method or codec used
  to parse the file.

  \sa setSource
  */
/*
LasCsvModel::LasCsvModel(
  QIODevice *file,
  QObject *parent,
  bool withHeader,
  QChar separator)
  : QAbstractTableModel(parent),
    maxColumn(0),
    quoteMode(LasCsvModel::DefaultQuoteMode)
{
  setSource(file, withHeader, separator);
}
*/


/*!
  \overload 

  Creates a LasCsvModel with the parent \a parent and content loaded from
  \a file.

  See \a setSource for information on the \a withHeader and \a separator
  properties, or if you need control over the quoting method or codec used
  to parse the file.

  \sa setSource
  */
LasCsvModel::LasCsvModel(
  const QString filename,
  QObject *parent,
  bool withHeader,
  QChar separator)
  : QAbstractTableModel(parent),
    maxColumn(0),
    quoteMode(LasCsvModel::DefaultQuoteMode)
{
  setSource(filename, withHeader, separator);
}


LasCsvModel::~LasCsvModel()
{
}


/*!
    \reimp
 */
int LasCsvModel::rowCount(
  const QModelIndex& parent) const
{
  if (parent.row() != -1 && parent.column() != -1) return 0;
  return csvData.count();
}


/*!
    \reimp
 */
int LasCsvModel::columnCount(
  const QModelIndex& parent) const
{
  if (parent.row() != -1 && parent.column() != -1) return 0;
  return maxColumn;
}


/*!
    \reimp
 */
QVariant LasCsvModel::data(
   const QModelIndex& index,
   int role) const
{
  if (index.parent() != QModelIndex()) return QVariant();

  if ( role == Qt::DisplayRole
    || role == Qt::EditRole
    || role == Qt::UserRole)
  {
    if (index.row() < 0 || index.column() < 0 || index.row() >= rowCount())
            return QVariant();

    const QStringList& row = csvData[index.row()];

    if(index.column() >= row.length())
            return QVariant();

    return row[index.column()];
  }
/*
  else if (role == Qt::TextColorRole)
  {
    if (index.row() == 0)
    {
      return themeSettings.getActiveTextColor();
    }
    else
    {
      return themeSettings.getDefaultTextColor();
    }
  }
*/
  else if (role == Qt::FontRole)
  {
    QFont tempFont = themeSettings.getSystemFont();
    tempFont.setPointSize(24);
    return tempFont;
  }

  return QVariant();
}


/*!
    \reimp
 */
QVariant LasCsvModel::headerData(
  int section,
  Qt::Orientation orientation,
  int role) const
{
  if ( section < header.count()
    && orientation == Qt::Horizontal
    && ( role == Qt::DisplayRole
      || role == Qt::EditRole
      || role == Qt::UserRole))
  {
    return header[section];
  }
  else
  {
    return QAbstractTableModel::headerData(section, orientation, role);
  }
}


/*!
  \overload

  Reads in a CSV file from the provided \a file using \a codec.
  */
void LasCsvModel::setSource(
  const QString filename,
  bool withHeader,
  QChar separator,
  QTextCodec* codec)
{
  QFile src(filename);

  if (src.exists())
  {
    setSource(&src, withHeader, separator, codec);
  }
  else
  {
    setDefaultSource();
  }
}


/*!
  Reads in a CSV file from the provided \a file using \a codec.

  The value of \a separator will be used to delimit fields, subject to the
  specified \a quoteMode.
  If \a withHeader is set to true, the first line of the file will be used
  to populate the model's horizontal header.
  
  \sa quoteMode
  */
void LasCsvModel::setSource(
  QIODevice *file,
  bool withHeader,
  QChar separator,
  QTextCodec* codec)
{
  bool headerSet = !withHeader;

  if(!file->isOpen())
  {
    file->open(QIODevice::ReadOnly);
  }

  if(withHeader)
  {
    maxColumn = 0;
  }
  else
  {
    maxColumn = header.size();
  }

  csvData.clear();

  QStringList row;
  QString field;
  QChar quote;
  QChar ch, buffer(0);
  bool readCR = false;
  QTextStream stream(file);

  if(codec)
  {
    stream.setCodec(codec);
  }
  else
  {
    stream.setAutoDetectUnicode(true);
  }

  while (!stream.atEnd())
  {
    if (buffer != QChar(0))
    {
      ch = buffer; 
      buffer = QChar(0);
    }
    else
    {
      stream >> ch;
    }

    if (ch == '\n' && readCR) 
    {
      continue;
    }
    else if (ch == '\r')
    {
      readCR = true;
    }
    else
    {
      readCR = false;
    }

    if ( ch != separator
      && ( ch.category() == QChar::Separator_Line
        || ch.category() == QChar::Separator_Paragraph
        || ch.category() == QChar::Other_Control))
    {
      row << field;
      field.clear();
      if(!row.isEmpty())
      {
        if(!headerSet)
        {
          header = row;
          headerSet = true;
        }
        else
        {
          csvData.append(row);
        }

        if (row.length() > maxColumn)
        {
          maxColumn = row.length();
        }
      }

      row.clear();
    }
    else if ( (quoteMode & DoubleQuote && ch == '"')
           || (quoteMode & SingleQuote && ch == '\''))
    {
      quote = ch;

      do
      {
        stream >> ch;
        if ( ch == '\\'
          && quoteMode & BackslashEscape)
        {
          stream >> ch;
        }
        else if (ch == quote)
        {
          if (quoteMode & TwoQuoteEscape)
          {
            stream >> buffer;
            if (buffer == quote)
            {
               buffer = QChar(0);
               field.append(ch);
               continue;
            }
          }
          break;
        }
        field.append(ch);
      } while(!stream.atEnd());
    }
    else if(ch == separator)
    {
      row << field;
      field.clear();
    }
    else
    {
      field.append(ch);
    }
  }

  if (!field.isEmpty()) 
  {
    row << field;
  }

  if (!row.isEmpty())
  {
    if (!headerSet)
    {
       header = row;
    }
    else
    {
      csvData.append(row);
    }
  }

  file->close();
}


void LasCsvModel::setDefaultSource()
{
  // Create the headers:
  header << QString("Barcode");
  header << QString("Name");
  header << QString("Description");
  header << QString("Quantity");
  header << QString("Location");
  maxColumn = 5;
}


/*!
  Sets the horizontal headers of the model to the values provided in \a data.
 */
void LasCsvModel::setHeaderData(
  const QStringList& data)
{
  header = data;
  emit headerDataChanged(Qt::Horizontal, 0, data.count());
}


/*!
  \reimp
  */
bool LasCsvModel::setHeaderData(
  int section,
  Qt::Orientation orientation,
  const QVariant& value,
  int role)
{
  if (orientation != Qt::Horizontal)
  {
    return false;                   // We don't support the vertical header
  }

  if ( role != Qt::DisplayRole
    || role != Qt::EditRole)
  {
    return false; // We don't support any other roles
  }

  if (section < 0)
  {
     return false;                                     // Bogus input
  }

  while (section > header.size())
  {
    header << QString();
  }

  header[section] = value.toString();
  emit headerDataChanged(Qt::Horizontal, section, section);
  return true;
}


/*!
    \reimp
 */
bool LasCsvModel::setData(
  const QModelIndex& index,
  const QVariant& data,
  int role)
{
  if (index.parent() != QModelIndex()) return false;

  if ( role == Qt::DisplayRole
    || role == Qt::EditRole
    || role == Qt::UserRole)
  {
    if ( index.row() >= rowCount()
      || index.column() >= columnCount()
      || index.row() < 0 || index.column() < 0)
    {
       return false;
    }

    QStringList& row = csvData[index.row()];

    while (row.length() <= index.column())
    {
      row << QString();
    }

    row[index.column()] = data.toString();
    emit dataChanged(index, index);
    return true;
  }

  return false;
}


/*!
    \reimp
 */
bool LasCsvModel::insertRow(
  int row,
  const QModelIndex& parent)
{
  return insertRows(row, 1, parent);
}


/*!
    \reimp
 */
bool LasCsvModel::insertRows(
  int row,
  int count,
  const QModelIndex& parent)
{
  if (parent != QModelIndex() || row < 0) return false;

  emit beginInsertRows(parent, row, row + count - 1);

  if (row >= rowCount())
  {
    for (int i = 0; i < count; i++)
      csvData << QStringList();
  }
  else
  {
    for(int i = 0; i < count; i++)
      csvData.insert(row, QStringList());
  }

  emit endInsertRows();

  return true;
}


/*!
    \reimp
 */
bool LasCsvModel::removeRow(
  int row,
  const QModelIndex& parent)
{
  return removeRows(row, 1, parent);
}


/*!
    \reimp
 */
bool LasCsvModel::removeRows(
  int row,
  int count,
  const QModelIndex& parent)
{
  if (parent != QModelIndex() || row < 0) return false;

  if (row >= rowCount()) return false;

  if (row + count >= rowCount()) count = rowCount() - row;

  emit beginRemoveRows(parent, row, row + count);

  for (int i = 0;i < count;i++)
    csvData.removeAt(row);

  emit endRemoveRows();

  return true;
}


/*!
    \reimp
 */
bool LasCsvModel::insertColumn(
  int col,
  const QModelIndex& parent)
{
  return insertColumns(col, 1, parent);
}


/*!
    \reimp
 */
bool LasCsvModel::insertColumns(
  int col,
  int count,
  const QModelIndex& parent)
{
  if (parent != QModelIndex() || col < 0) return false;

  emit beginInsertColumns(parent, col, col + count - 1);

  for (int i = 0; i < rowCount(); i++)
  {
    QStringList& row = csvData[i];

    while(col >= row.length()) row.append(QString());

    for (int j = 0; j < count; j++)
    {
      row.insert(col, QString());
    }
  }

  for (int i = 0; i < count ;i++)
    header.insert(col, QString());

  maxColumn += count;

  emit endInsertColumns();

  return true;
}


/*!
    \reimp
 */
bool LasCsvModel::removeColumn(
  int col,
  const QModelIndex& parent)
{
  return removeColumns(col, 1, parent);
}


/*!
    \reimp
 */
bool LasCsvModel::removeColumns(
  int col,
  int count,
  const QModelIndex& parent)
{
  if (parent != QModelIndex() || col < 0) return false;

  if (col >= columnCount()) return false;

  if (col + count >= columnCount()) count = columnCount() - col;

  emit beginRemoveColumns(parent, col, col + count);

  QString before, after;

  for (int i = 0; i < rowCount(); i++)
  {
    for (int j = 0; j < count; j++)
    {
      csvData[i].removeAt(col);
    }
  }

  for (int i = 0; i < count; i++)
    header.removeAt(col);

  emit endRemoveColumns();

  return true;
}


static QString las_addCsvQuotes(
  LasCsvModel::QuoteMode mode,
  QString field)
{
  bool addDoubleQuotes =
    (mode & LasCsvModel::DoubleQuote)
    && field.contains('"');

  bool addSingleQuotes =
    (mode & LasCsvModel::SingleQuote)
    && field.contains('\'');

  bool quoteField =
    (mode & LasCsvModel::AlwaysQuoteOutput)
    || addDoubleQuotes
    || addSingleQuotes;

  if ( quoteField
    && !addDoubleQuotes
    && !addSingleQuotes)
  {
    if (mode & LasCsvModel::DoubleQuote)
    {
      addDoubleQuotes = true;
    }
    else if (mode & LasCsvModel::SingleQuote)
    {
      addSingleQuotes = true;
    }
  } 

  if (mode & LasCsvModel::BackslashEscape)
  {
    if (addDoubleQuotes) 
      return '"' + field.replace("\\", "\\\\").replace("\"", "\\\"") + '"';

    if (addSingleQuotes)
      return '\'' + field.replace("\\", "\\\\").replace("'", "\\'") + '\'';
  }
  else
  {
    if (addDoubleQuotes) 
      return '"' + field.replace("\"", "\"\"") + '"';

    if (addSingleQuotes)
      return '\'' + field.replace("'", "''") + '\'';
  }

  return field;
}


/*!
  Outputs the content of the model as a CSV file to the device \a dest
  using \a codec.

  Fields in the output file will be separated by \a separator. Set \a
  withHeader to true to output a row of headers at the top of the file.
 */ 
void LasCsvModel::toCSV(
  QIODevice* dest,
  bool withHeader,
  QChar separator,
  QTextCodec* codec) const
{
  int row, col, rows, cols;

  rows = rowCount();
  cols = columnCount();
  QString data;
  if (!dest->isOpen())
  {
    dest->open(QIODevice::WriteOnly | QIODevice::Truncate);
  }

  QTextStream stream(dest);
  if (codec) stream.setCodec(codec);

  if (withHeader)
  {
    data = "";
    for (col = 0; col < cols; ++col)
    {
      if (col > 0) data += separator;

      data += las_addCsvQuotes(quoteMode, header.at(col)); 
    }

    stream << data << endl;
  }

  for (row = 0; row < rows; ++row)
  {
    const QStringList& rowData = csvData[row];
    data = "";

    for (col = 0; col < cols; ++col)
    {
      if (col > 0) data += separator;

      if (col < rowData.length())
      {
        data += las_addCsvQuotes(quoteMode, rowData.at(col)); 
      }
      else
      {
        data += las_addCsvQuotes(quoteMode, QString());; 
      }
    }

    stream << data << endl;
  }

  stream << flush;
  dest->close();
}


/*!
  \overload

  Outputs the content of the model as a CSV file to the file specified
  by \a filename using \a codec.

  Fields in the output file will be separated by \a separator. Set
  \a withHeader to true to output a row of headers at the top of the file.
 */ 
void LasCsvModel::toCSV(
  const QString filename,
  bool withHeader,
  QChar separator,
  QTextCodec* codec) const
{
  QFile dest(filename);
  toCSV(&dest, withHeader, separator, codec);
}


/*!
    \reimp
 */
Qt::ItemFlags LasCsvModel::flags(
  const QModelIndex& index) const
{
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}


/*!
  Sets the content of the cell at row \a row and column \a column to \a value.
  
  \sa text
  */
void LasCsvModel::setText(
  int row,
  int column,
  const QString& value)
{
  setData(index(row, column), value);
}


/*!
  Fetches the content of the cell at row \a row and column \a column.
  
  \sa setText
  */
QString LasCsvModel::text(
  int row,
  int column) const
{
  return data(index(row, column)).toString();
}


/*!
  Sets the content of the header for column \a column to \a value.
  
  \sa headerText
  */
void LasCsvModel::setHeaderText(
  int column,
  const QString& value)
{
  setHeaderData(column, Qt::Horizontal, value);
}


/*!
  Fetches the content of the cell at row \a row and column \a column.
  
  \sa setText
  */
QString LasCsvModel::headerText(
  int column) const
{
  return headerData(column, Qt::Horizontal).toString();
}
