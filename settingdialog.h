#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

private slots:
    void on_buttonBox_accepted(void);               //保存文件
    void on_buttonBox_rejected(void);               //取消修改

private:
    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
