#include <iostream>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QRegExp>
#include <QtCore/QFile>
#include <QtCore/QDebug>

#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>

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

    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(fileOption);
    parser.addOption(patternOption);
    parser.process(app);

    QString filename = parser.isSet(fileOption) ? parser.value(fileOption) : "version.h";
    QRegExp expression(parser.isSet(patternOption) ? parser.value(patternOption) : "(.*VER_BUILD\\s+)(\\d+)");

    // 1. Open file for reading
    QFile versionFile(filename);
    if(versionFile.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
        std::cerr << QString("Failed to open file for reading: %1").arg(filename).toStdString() << std::endl;
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
            qint32 newVersion = expression.cap(2).toInt() + 1;
            line = line.replace(expression, QString("\\1%1").arg(newVersion));
            fileContents.replace(i, line);
            rowsAffected++;
        }
    }

    if(rowsAffected == 0) {
        std::cerr << "No rows affected" << std::endl;
        return 3;
    }

    // 4. Write contents to file again
    if(versionFile.open(QIODevice::WriteOnly | QIODevice::Text) == false) {
        std::cerr << QString("Failed to open file for writing: %1").arg(filename).toStdString() << std::endl;
        return 2;
    }

    QTextStream writeStream(&versionFile);
    for(QString line : fileContents) {
        writeStream << line << "\r\n";
    }
    versionFile.close();

    std::cout << QString("Successfully bumped build version (%1 rows affected)").arg(rowsAffected).toStdString() << std::endl;

    return 0;
}
