#include "form.h"
#include "ui_form.h"

Form::Form(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
}
