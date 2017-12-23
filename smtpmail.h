#ifndef SMTPMAIL_H
#define SMTPMAIL_H

#include <string>

class SMTPMAIL
{
public:
    SMTPMAIL(std::string smtphost, unsigned long sslport, std::string username, std::string password, std::string from);
    void setMailto(std::string mailto);
    void setSubject(std::string subject);
    void setContent(std::string content);
    void isHtml(void);
    bool send(void);

private:
    std::string LOCALHOST;      //本地主机名
    std::string SMTPHOST;       //SMTP服务器地址
    unsigned long SSLPORT;      //SMTP服务器SSL端口
    std::string USERNAME;       //用户名
    std::string PASSWORD;       //密码
    std::string FROM;           //发件人邮箱
    std::string MAILTO;         //收件人邮箱
    std::string CHARSET;        //邮件中文编码：UTF-8 GB2312
    std::string TYPE;           //邮件格式：text/html text/plain
    std::string SUBJECT;        //邮件主题
    std::string CONTENT;        //邮件内容
};

#endif // SMTPMAIL_H
