#include "lasoptionsdialog.h"
#include "ui_lasoptionsdialog.h"

LasOptionsDialog::LasOptionsDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LasOptionsDialog)
{
  ui->setupUi(this);
}

LasOptionsDialog::~LasOptionsDialog()
{
  delete ui;
}

/*
void LasOptionsDialog::on_formulaButton_clicked()
{
  emit editFormula();
  accept();
}
*/

void LasOptionsDialog::on_clearCellButton_clicked()
{
  emit clearCell();
  accept();
}

void LasOptionsDialog::on_removeRowButton_clicked()
{
  emit removeRow();
  accept();
}

void LasOptionsDialog::on_loadButton_clicked()
{
  emit loadFile();
  accept();
}

void LasOptionsDialog::on_saveButton_clicked()
{
  emit saveFile();
  accept();
}

void LasOptionsDialog::on_saveAsButton_clicked()
{
  emit saveFileAs();
  accept();
}
