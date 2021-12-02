#ifndef MYHELPER_H
#define MYHELPER_H

#include <QtCore>
#include <QHostInfo>
#include <QNetworkInterface>

class myHelper : public QObject
{
public:
    myHelper();

    // 字节数组转16进制字符串
    static void ByteToHexString(QString &str, QByteArray &ba)
    {
        // str= ba.toHex();//直接转换中间没有空格
        for (int i = 0; i < ba.length(); i++)
        {
            unsigned char n =static_cast<unsigned char>((ba.at(i)));
            QString nhex = QByteArray::number(n, 16).toUpper();
            str.append(nhex);
            // 字节间加空格
            str.append(' ');
        }
    }

    // 16进制字符串转字节数组
    // str：输入字符串，ba：输出字节数组
    static bool HexStringToByte(QString &str,QByteArray &ba)
    {
        // 正则:数字0-9字母a-f、A-F匹配大于等于一次
        QString pattern("[a-fA-F0-9]+");
        QRegExp rx(pattern);
        // 删除所有空格
        str = str.replace(' ', "");
        // 匹配
        if (rx.exactMatch(str))
        {
            bool ok;
            int length = str.length();
            // 双数
            if ((length % 2)==0)
            {
                for (int i = 0; i < length; i+=2)
                {
                    // 每两个字符对应一个hex字符串
                    QString str_hex = str.mid(i, 2);
                    // hex字符串转整数值
                    ba.append(static_cast<char>(str_hex.toInt(&ok, 16)));
                }
            }
            // 单数
            else
            {
                for (int i = 0; i < length-1; i += 2)
                {
                    // 每两个字符对应一个hex字符串
                    QString str_hex = str.mid(i, 2);
                    // hex字符串转整数值
                    ba.append(static_cast<char>(str_hex.toInt(&ok, 16)));
                }
                // 最后一个单独处理
                // hex字符串转整数值
                ba.append(static_cast<char>(str.mid(length - 1,1).toInt(&ok, 16)));
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    // 十六进制转二进制 通过参数进行补位
    static QString Hex2Bin(const QString &Hex, QString add0Str = QString("00000000"))
    {
        QString hexStr = Hex;
        QString binStr = hexStr.setNum(hexStr.toInt(nullptr, 16), 2);
        if (hexStr.length() < add0Str.length())
            binStr = add0Str.left(add0Str.length()-binStr.length())+binStr;
        return binStr;
    }

    // 十进制字符串根据参数转成n位的十六进制字符串
    static QString Dec2Hex(const QString &Dec, uint complement = 8)
    {
        QString strDec = Dec;
        int _numDec = strDec.toInt(nullptr, 10);
        if (_numDec >= 0)
            return QString("%1").arg(_numDec,complement,16,QLatin1Char('0')).toUpper();
        else{
            QByteArray _ba;
            return _ba.setNum(_numDec,16).toUpper();
        }
    }
    // 十六进制字符串 转 int
    static int Hex2Int(QString hex)
    {
        QByteArray _ba;
        HexStringToByte(hex, _ba);
        QString qnum =_ba.toHex().data();
        int num = qnum.toInt(nullptr,16);
        if(!(qnum.at(0)>='0'&&qnum.at(0)<='7')){
            num = num & 0x7FFF;
            num=32768-num;
            num = num * -1;
        }
        return num;
    }
    // 反转字符串
    static QString ReversalStr(QString &str)
    {
        QString ret;
        for(QChar sz:str)
            ret.push_front(sz);
        return ret;
    }

    // 获取本地IP
    static QString GetLocalIP()
    {
        QString ip;
        QList<QHostAddress> addrs = QNetworkInterface::allAddresses();

        foreach (QHostAddress addr, addrs) {
            ip = addr.toString();
            if (ip.startsWith("192.168")) {
                break;
            }
        }

        return ip;
    }

    static char convertHexChar(char ch)
    {
        if ((ch >= '0') && (ch <= '9')) {
            return ch - 0x30;
        } else if ((ch >= 'A') && (ch <= 'F')) {
            return ch - 'A' + 10;
        } else if ((ch >= 'a') && (ch <= 'f')) {
            return ch - 'a' + 10;
        } else {
            return (-1);
        }
    }

    static QByteArray hexStrToByteArray(QString str)
    {
        QByteArray senddata;
        int hexdata, lowhexdata;
        int hexdatalen = 0;
        int len = str.length();
        senddata.resize(len / 2);
        char lstr, hstr;

        for (int i = 0; i < len;) {
            hstr = str.at(i).toLatin1();

            if (hstr == ' ') {
                i++;
                continue;
            }

            i++;

            if (i >= len) {
                break;
            }

            lstr = str.at(i).toLatin1();
            hexdata = convertHexChar(hstr);
            lowhexdata = convertHexChar(lstr);

            if ((hexdata == 16) || (lowhexdata == 16)) {
                break;
            } else {
                hexdata = hexdata * 16 + lowhexdata;
            }

            i++;
            senddata[hexdatalen] = (char)hexdata;
            hexdatalen++;
        }

        senddata.resize(hexdatalen);
        return senddata;
    }

};

#endif // MYHELPER_H
