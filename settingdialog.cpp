#include <QFile>
#include <map>
#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    std::map<QString, QString> config;
    if(QFile::exists("./.config"))
    {
        QFile file("./.config");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        while(!file.atEnd())
        {
            QString line = file.readLine();
            config[line.split(QRegExp("\\s+"))[0]] = line.split(QRegExp("\\s+"))[1];
        }
        file.close();
    }
    ui->lineEdit_smtphost->setText(config[QString("SMTPHOST")]);
    ui->lineEdit_sslport->setText(config[QString("SSLPORT")]);
    ui->lineEdit_username->setText(config[QString("USERNAME")]);
    ui->lineEdit_password->setText(config[QString("PASSWORD")]);
    ui->lineEdit_from->setText(config[QString("FROM")]);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::on_buttonBox_accepted()
{
    QFile file("./.config");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    file.write("SMTPHOST ", 9);
    file.write(ui->lineEdit_smtphost->text().toLatin1().data(), ui->lineEdit_smtphost->text().size());
    file.write("\nSSLPORT ", 9);
    file.write(ui->lineEdit_sslport->text().toLatin1().data(), ui->lineEdit_sslport->text().size());
    file.write("\nUSERNAME ", 10);
    file.write(ui->lineEdit_username->text().toLatin1().data(), ui->lineEdit_username->text().size());
    file.write("\nPASSWORD ", 10);
    file.write(ui->lineEdit_password->text().toLatin1().data(), ui->lineEdit_password->text().size());
    file.write("\nFROM ", 6);
    file.write(ui->lineEdit_from->text().toLatin1().data(), ui->lineEdit_from->text().size());
    file.close();
    this->close();
}

void SettingDialog::on_buttonBox_rejected()
{
    this->close();
}
