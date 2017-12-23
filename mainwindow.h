#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_import_triggered(void);          //从文件导入邮件内容
    void on_action_exit_triggered(void);            //退出
    void on_action_editsetting_triggered(void);     //编辑SMTP设置
    void on_action_about_triggered(void);           //查看关于
    void on_pushButton_send_clicked(void);          //发送邮件

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
