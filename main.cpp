#include <iostream>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QRegExp>
#include <QtCore/QFile>
#include <QtCore/QDebug>

#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>

bool logEnabled;

#define BBLOG(msg) if(logEnabled == false) {} else std::cout << msg << std::endl
#define BBERR(msg) if(logEnabled == false) {} else std::cerr << msg << std::endl

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;


    QCommandLineOption fileOption(QStringList() << "f" << "file",
                                  QCoreApplication::translate("main", "Search for <file>"),
                                  QCoreApplication::translate("main", "file"));

    QCommandLineOption patternOption(QStringList() << "p" << "pattern",
                                     QCoreApplication::translate("main", "Pattern to search for within file"),
                                     QCoreApplication::translate("main", "pattern"));

    QCommandLineOption silentOption(QStringList() << "s" << "silent",
                                    QCoreApplication::translate("main", "Silent mode"));

    QCommandLineOption numberOption(QStringList() << "n",
                                    QCoreApplication::translate("main", "Bump version by <value>"),
                                    QCoreApplication::translate("main", "value"));

    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(fileOption);
    parser.addOption(numberOption);
    parser.addOption(patternOption);
    parser.addOption(silentOption);
    parser.process(app);

    QString filename = parser.isSet(fileOption) ? parser.value(fileOption) : "version.h";
    QRegExp expression(parser.isSet(patternOption) ? parser.value(patternOption) : "(.*VER_BUILD\\s+)(\\d+)");
    logEnabled = parser.isSet(silentOption) == false;

    // Try to parse '-n' parameter value
    qint32 bumpNumber = 1;
    if(parser.isSet(numberOption)) {
        bool conversionOk = false;
        qint32 numberTemp = parser.value(numberOption).toInt(&conversionOk);
        if(conversionOk == false) {
            BBERR("Failed to parse value of '-n' parameter");
            return 4;
        }
        else {
            bumpNumber = numberTemp;
        }
    }

    // 1. Open file for reading
    QFile versionFile(filename);
    if(versionFile.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
        BBERR(QString("Failed to open file for reading: %1").arg(filename).toStdString());
        return 1;
    }

    // 2. Read file contents
    QStringList fileContents;
    {
        QTextStream readStream(&versionFile);
        while(readStream.atEnd() == false) {
            fileContents.append(readStream.readLine());
        }
    }
    versionFile.close();

    // 3. Find and change build number
    qint32 rowsAffected = 0;
    for(qint32 i = 0; i < fileContents.length(); ++i) {
        QString line = fileContents.at(i);
        if(line.contains(expression)) {
            qint32 newVersion = expression.cap(2).toInt() + bumpNumber;
            line = line.replace(expression, QString("\\1%1").arg(newVersion));
            fileContents.replace(i, line);
            rowsAffected++;
        }
    }

    if(rowsAffected == 0) {
        BBERR("No rows affected");
        return 3;
    }

    // 4. Write contents to file again
    if(versionFile.open(QIODevice::WriteOnly | QIODevice::Text) == false) {
        BBERR(QString("Failed to open file for writing: %1").arg(filename).toStdString());
        return 2;
    }

    QTextStream writeStream(&versionFile);
    for(QString line : fileContents) {
        writeStream << line << "\r\n";
    }
    versionFile.close();

    BBLOG(QString("Successfully bumped build version (%1 rows affected)").arg(rowsAffected).toStdString());

    return 0;
}
