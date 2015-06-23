#ifndef LASOPTIONSDIALOG_H
#define LASOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class LasOptionsDialog;
}

class LasOptionsDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit LasOptionsDialog(QWidget *parent = 0);
  ~LasOptionsDialog();

signals:
//  void editFormula();
  void clearCell();
  void removeRow();
  void loadFile();
  void saveFile();
  void saveFileAs();
  
private slots:
//  void on_formulaButton_clicked();
  void on_clearCellButton_clicked();
  void on_removeRowButton_clicked();
  void on_loadButton_clicked();
  void on_saveButton_clicked();
  void on_saveAsButton_clicked();

private:
  Ui::LasOptionsDialog *ui;
};

#endif // LASOPTIONSDIALOG_H
