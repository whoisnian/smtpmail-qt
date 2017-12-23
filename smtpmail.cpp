#include <cstdio>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <QDebug>
#include "smtpmail.h"

std::string base64_encode(std::string str)
{
    int len, k, i;
    char ori[300], res[500];
    strcpy(ori, str.c_str());
    k = len = strlen(ori);

    if(len % 3 != 0)
        k = (len / 3 + 1) * 3;

    for(i = 0;i * 3 < k;i++)
    {
        res[i*4] = (ori[i*3] & 0xfc) >> 2;
        res[i*4+1] = ((ori[i*3] & 0x03) << 4) + ((ori[i*3+1] & 0xf0) >> 4);
        res[i*4+2] = ((ori[i*3+1] & 0x0f) << 2) + ((ori[i*3+2] & 0xc0) >> 6);
        res[i*4+3] = ori[i*3+2] & 0x3f;
    }

    for(i = 0;i < (len + 2) / 3 * 4;i++)
    {
        if(i < (len + 2) / 3 + len)
        {
            if(res[i] >= 0&&res[i] <= 25)
                res[i] += 65;
            else if(res[i] >= 26&& res[i] <= 51)
                res[i] += 71;
            else if(res[i] >= 52&&res[i] <= 61)
                res[i] -= 4;
            else if(res[i] == 62)
                res[i] = 43;
            else if(res[i] == 63)
                res[i] = 47;
        }
        else
            res[i] = '=';
    }
    res[i] = '\0';
    return std::string(res);
}

SMTPMAIL::SMTPMAIL(std::string smtphost, unsigned long sslport, std::string username, std::string password, std::string from)
{
    char localhostname[100];
    LOCALHOST = std::string("localhost");
    if(!gethostname(localhostname, sizeof(localhostname)))
    {
        LOCALHOST = std::string(localhostname);
    }
    SMTPHOST = smtphost;
    SSLPORT = sslport;
    USERNAME = username;
    PASSWORD = password;
    FROM = from;
    CHARSET = std::string("UTF-8");
    TYPE = std::string("text/plain");
}

void SMTPMAIL::setMailto(std::string mailto)
{
    MAILTO = mailto;
}

void SMTPMAIL::setSubject(std::string subject)
{
    SUBJECT = subject;
}

void SMTPMAIL::setContent(std::string content)
{
    CONTENT = content;
}

void SMTPMAIL::isHtml()
{
    TYPE = std::string("text/html");
}

bool SMTPMAIL::send()
{
    char buf[1000];

    //获取主机名对应IP地址
    struct hostent *host;
    char ip[30];
    host = gethostbyname(SMTPHOST.c_str());
    inet_ntop(host->h_addrtype, *(host->h_addr_list), ip, sizeof(ip));
    qDebug() << "Connecting to " << SMTPHOST.c_str() << "(" << ip << ")...\n\n";

    //创建服务器端socket，地址族为AF_INET(IPv4)，传输方式为TCP
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //初始化IP为输入的IP，端口为已设置的port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SSLPORT);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    //客户端连接服务器
    connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    //初始化SSL_CTX对象
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    //加载SSL有关内容
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    //绑定SSL到socket
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server_socket);
    SSL_connect(ssl);

    int len;
    memset(buf, 0, sizeof(buf));

    //获取服务器状态
    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    qDebug() << buf;        // 220

    //发送本地主机身份
    sprintf(buf, "EHLO %s\r\n", LOCALHOST.c_str());
    SSL_write(ssl, buf, strlen(buf));

    for(int i = 0;i < 3;i++)
    {
        len = SSL_read(ssl, buf, sizeof(buf));
        buf[len] = '\0';
        qDebug() << buf;    // 250
    }

    //发送登录请求
    strcpy(buf, "AUTH LOGIN\r\n");
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    qDebug() << buf;        // 334

    //发送Base64编码的用户名
    std::string username = base64_encode(USERNAME) + std::string("\r\n");
    qDebug() << username.c_str() << username.size();
    SSL_write(ssl, username.c_str(), username.size());

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    qDebug() << buf;        // 334

    //发送Base64编码的密码
    std::string password = base64_encode(PASSWORD) + std::string("\r\n");
    SSL_write(ssl, password.c_str(), password.size());

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    qDebug() << buf;        // 235

    //发送发件人邮箱
    sprintf(buf, "MAIL FROM:<%s>\r\n", FROM.c_str());
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    qDebug() << buf;        // 250

    //发送收件人邮箱
    sprintf(buf, "RCPT TO:<%s>\r\n", MAILTO.c_str());
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    qDebug() << buf;        // 250

    //准备发送数据
    strcpy(buf, "DATA\r\n");
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    qDebug() << buf;        // 354

    //获取当前时间
    time_t Time;
    char time_of_now[50];
    time(&Time);
    strftime(time_of_now, 50, "%a, %d %b %Y %H:%I:%S %z", localtime(&Time));

    //对邮件主题进行编码
    std::string subject = base64_encode(SUBJECT);

    //发送数据
    std::string data = std::string("Date: ") + std::string(time_of_now)
                     + std::string("\r\nTo: ") + MAILTO
                     + std::string("\r\nFrom: ") + FROM
                     + std::string("\r\nSubject: =?") + CHARSET + std::string("?B?") + subject
                     + std::string("?=\r\nContent-Type: ") + TYPE
                     + std::string("; charset=") + CHARSET
                     + std::string("\r\n\r\n") + CONTENT
                     + std::string("\r\n\r\n.\r\n");
    SSL_write(ssl, data.c_str(), data.size());

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    qDebug() << buf;        // 250
    bool res = false;
    if(buf[0] == '2'&&buf[1] == '5'&&buf[2] == '0')
        res = true;

    //结束会话
    strcpy(buf, "QUIT\r\n");
    SSL_write(ssl, buf, strlen(buf));

    len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    qDebug() << buf;        // DONE

    //关闭SSL和socket
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(server_socket);
    return res;
}
