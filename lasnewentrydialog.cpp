#include "lasnewentrydialog.h"
#include "ui_lasnewentrydialog.h"

LasNewEntryDialog::LasNewEntryDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LasNewEntryDialog)
{
  ui->setupUi(this);
}

LasNewEntryDialog::~LasNewEntryDialog()
{
  delete ui;
}


void LasNewEntryDialog::setupDialog(
  QString barcode)
{
  ui->barcodeValueLabel->setText(barcode);
  ui->nameEdit->setText("");
  ui->descriptionEdit->setText("");
  ui->quantitySpinBox->setValue(1);
  ui->locationEdit->setText("");
}


QString LasNewEntryDialog::getBarcode()
{
  return ui->barcodeValueLabel->text();
}


QString LasNewEntryDialog::getName()
{
  return ui->nameEdit->text();
}


QString LasNewEntryDialog::getDescription()
{
  return ui->descriptionEdit->text();
}


QString LasNewEntryDialog::getQuantity()
{
  return QString::number(ui->quantitySpinBox->value());
}


QString LasNewEntryDialog::getLocation()
{
  return ui->locationEdit->text();
}
