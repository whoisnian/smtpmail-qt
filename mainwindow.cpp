#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <map>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingdialog.h"
#include "smtpmail.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_import_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择要导入的文件", "/");
    if(fileName == NULL)
        return;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    ui->plainTextEdit_content->setPlainText(file.readAll());
    file.close();
}

void MainWindow::on_action_exit_triggered()
{
    this->close();
}

void MainWindow::on_action_editsetting_triggered()
{
    SettingDialog s;
    s.exec();
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox::about(this, "Author", "nian\nBlog: whoisnian.com\n");
}

void MainWindow::on_pushButton_send_clicked()
{
    if(!QFile::exists("./.config"))
    {
        QMessageBox::information(this, "喵？新来的？", "检测到您是新来的，请完善配置文件。", QMessageBox::Ok, QMessageBox::Ok);
        SettingDialog s;
        s.exec();
        return;
    }
    std::string ERR("");
    if(ui->plainTextEdit_content->toPlainText() == NULL)
        ERR = std::string("邮件内容");
    if(ui->lineEdit_subject->text() == NULL)
        ERR = std::string("邮件主题");
    if(ui->lineEdit_mailto->text() == NULL)
        ERR = std::string("收件人");
    if(ERR != "")
    {
        ERR = std::string("您还没填") + ERR + std::string("呢！");
        QMessageBox::information(this, "喵喵喵？", ERR.c_str(), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    ui->statusBar->showMessage("正在发送邮件......");
    std::map<QString, QString> config;
    QFile file("./.config");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    while(!file.atEnd())
    {
        QString line = file.readLine();
        config[line.split(QRegExp("\\s+"))[0]] = line.split(QRegExp("\\s+"))[1];
    }
    file.close();
    SMTPMAIL email(config[QString("SMTPHOST")].toStdString(), config[QString("SSLPORT")].toULong(), config[QString("USERNAME")].toStdString(), config[QString("PASSWORD")].toStdString(), config[QString("FROM")].toStdString());
    if(ui->radioButton_html->isChecked())
    {
        email.isHtml();
    }
    email.setMailto(ui->lineEdit_mailto->text().toStdString());
    email.setSubject(ui->lineEdit_subject->text().toStdString());
    email.setContent(ui->plainTextEdit_content->toPlainText().toStdString());
    if(email.send())
    {
        ui->statusBar->showMessage("发送成功。", 5000);
    }
    else
    {
        ui->statusBar->showMessage("发送失败。", 5000);
    }
}
