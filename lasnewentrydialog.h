#ifndef LASNEWENTRYDIALOG_H
#define LASNEWENTRYDIALOG_H

#include <QDialog>

namespace Ui {
class LasNewEntryDialog;
}

class LasNewEntryDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit LasNewEntryDialog(QWidget *parent = 0);
  ~LasNewEntryDialog();

  void setupDialog(
    QString barcode);

  QString getBarcode();
  QString getName();
  QString getDescription();
  QString getQuantity();
  QString getLocation();
  
private:
  Ui::LasNewEntryDialog *ui;
};

#endif // LASNEWENTRYDIALOG_H
