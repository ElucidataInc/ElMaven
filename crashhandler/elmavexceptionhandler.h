#ifndef elmavexceptionhandler_H
#define elmavexceptionhandler_H

#include <QProcess>
#include <QDebug>
#include <QTemporaryFile>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QCoreApplication>

#include <pthread.h>

#if defined(Q_OS_LINUX)
#include <client/linux/handler/exception_handler.h>
#endif

#if defined(__w64)
#include <client/windows/handler/exception_handler.h>
#define SERVER_BIN ":/crashserver_win"
//extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

#if defined(__APPLE__)
#include "client/mac/handler/exception_handler.h"
#include "common/mac/MachIPC.h"
#define SERVER_BIN ":/crashserver_mac"
#endif



namespace elmavexceptionhandler
{
    QString dumpPath = "";
    enum MessageId : int {
        SERVERSTARTED = 0,
        SERVERFAILED
    };
#if defined(__APPLE__)
    mach_port_t handlerPort = MACH_PORT_NULL;
    google_breakpad::ReceivePort receivePort;
#endif
    google_breakpad::ExceptionHandler* eh = nullptr;
#if defined(Q_OS_LINUX)
    google_breakpad::MinidumpDescriptor* md = nullptr;
#endif

#if defined(Q_OS_LINUX)
    bool filterCallback(const google_breakpad::MinidumpDescriptor& descriptor,void* context, bool succeeded)
    {
        std::cerr << "path of dump file: " << descriptor.directory().c_str() << std::endl;
        QProcess* cReporter = new QProcess(nullptr);

        cReporter->startDetached(qApp->applicationDirPath() + QDir::separator() + "crashreporter", \
        QStringList() <<  QString::fromStdString(descriptor.path()) << QString(descriptor.directory().c_str()));

        return succeeded;
    }
#endif
    bool filterCallback(void *context)
    {
        #if defined(__APPLE__)
        // Swap the bootstrap port for a different port, so we can
        // use the replacement port to communicate with the child.
        google_breakpad::ReceivePort receiver;
        // Hold on to the existing bootstrap port.
        mach_port_t bootstrap_port;
        if (task_get_bootstrap_port(mach_task_self(), &bootstrap_port) != KERN_SUCCESS)
            return false;

        if (task_set_bootstrap_port(mach_task_self(),receiver.GetPort()) != KERN_SUCCESS)
            return false;
        #endif
        QProcess* process = new QProcess;
        process->setProgram(QCoreApplication::applicationDirPath() + QDir::separator() + "crashserver");
        process->setReadChannelMode(QProcess::MergedChannels);
        process->setArguments(QStringList() << dumpPath << QCoreApplication::applicationDirPath());
        process->start();
        if(process->waitForStarted(5000)) {
            qDebug()  << "process started successfully \n";
        }
        else {
            qDebug() << "failed to start : " << process->errorString();
            return false;
        }

        #if defined(__APPLE__)
        // Reset bootstrap port.
        if (task_set_bootstrap_port(mach_task_self(),bootstrap_port) != KERN_SUCCESS) {
            return false;
        }

        // Wait for child to return a port on which to perform messaging.
        google_breakpad::MachReceiveMessage receive_message;
        if (receiver.WaitForMessage(&receive_message,MACH_MSG_TIMEOUT_NONE) != KERN_SUCCESS) {
            return false;
        }

        qDebug() << "crasher: got message from child with %d descriptors\n" <<  receive_message.GetDescriptorCount();
        // Now send back the prearranged port to use, as well as the original
        // bootstrap port;
        google_breakpad::MachPortSender sender(receive_message.GetTranslatedPort(0));
        google_breakpad::MachSendMessage send_message(0);
        if (!send_message.AddDescriptor(bootstrap_port)) {
            qDebug() << "crasher: failed to add bootstrap port\n";
            return false;
        }

        if (!send_message.AddDescriptor(google_breakpad::MachMsgPortDescriptor(handlerPort,MACH_MSG_TYPE_MOVE_RECEIVE))) {
            qDebug() << "crasher: failed to add handler port\n";
            return false;
        }

        if (sender.SendMessage(send_message, MACH_MSG_TIMEOUT_NONE) != KERN_SUCCESS) {
            qDebug() << "crasher: SendMessage failed\n";
            return false;
        }
        qDebug() << "crasher: sent reply message\n";
        #endif
        // Wait for handler to unblock us.
        if(process->waitForReadyRead(-1)) {

          // read the message
          qDebug() << process->readAllStandardOutput() <<  "    " << process->readAllStandardError() << "\n";
          // close the reading channel
          process->closeReadChannel(QProcess::StandardOutput);
          process->closeReadChannel(QProcess::StandardError);
        }

        // server failed to send message in the given time limit or something is messed up on the server side
        else {
            qDebug() << "failed to read message from child : " << process->errorString();
            return false;
        }
        // successfully estabilished connection between the server and child.
        // now it's the job of the server to write the dump. We can safely return now
        qDebug() << " crasher: exiting FilterCallback\n";
        return true;

    }

    void init()
    {

        dumpPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QDir::separator() + \
                "ElMaven" + QDir::separator() +  QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss") + QDir::separator();
        QDir dir;
        dir.mkpath(dumpPath);
        qDebug() << "dump path : " << dumpPath;
#if defined(__w64)
        QString serverBin = QCoreApplication::applicationDirPath() + QDir::separator() + "crashserver.exe";
        QProcess* _process = new QProcess;
        _process->setProgram(serverBin);
        _process->setArguments(QStringList() << dumpPath << QCoreApplication::applicationDirPath());
        _process->start();
        if(_process->waitForStarted(-1))
            qDebug() << " process started successfully ";
        else
            qDebug() << "process failed to start : " << _process->errorString() << "  " << _process->error();

        while(_process->waitForReadyRead(-1)) {
            bool receivedInput = false;
            QByteArray data = _process->readLine();
            qDebug() << "reading from server side: " <<  data;
            switch (data.toInt()) {
            case MessageId::SERVERSTARTED:

                qDebug() << "server stared successfully ";
                _process->closeReadChannel(QProcess::StandardOutput);
                _process->closeReadChannel(QProcess::StandardError);
                receivedInput = true;
                break;
            case MessageId::SERVERFAILED:
                qDebug() << "server failed to start ";
                _process->closeReadChannel(QProcess::StandardOutput);
                _process->closeReadChannel(QProcess::StandardError);
                receivedInput = true;
                break;

            default:
                qDebug() << "reading from server side: " <<  data;
                break;
            }

            if(receivedInput)
                break;
        }
        eh = new google_breakpad::ExceptionHandler(dumpPath.toStdWString(), NULL, NULL, NULL,
                                           google_breakpad::ExceptionHandler::HANDLER_ALL,
                                           MiniDumpNormal,
                                           L"\\\\.\\pipe\\ELMAVEN_HANDLER",
                                           nullptr);

        qDebug() << "is out of process : " << eh->IsOutOfProcess();

#endif
//        qt_ntfs_permission_lookup++;
//        QTemporaryFile* file = QTemporaryFile::createNativeFile(SERVER_BIN);
//        file->setPermissions(QFileDevice::ReadOwner);
//        LPSTR path = (LPSTR)file->fileName().utf16();
//        SetNamedSecurityInfoA(path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);
//        if(file->open()) {
//            qDebug() << "opened file: " << file->fileName();
//            qDebug() << "file permissions: " << file->permissions();
//             QProcess* _process = new QProcess;
//            _process->setProgram("D:/maven_repo/elmaven_new/ElMaven/binaries/windows/crashserver.exe");

//            _process->setProgram(file->fileName());
//            _process->setArguments(QStringList() << QString(dumpPath));
//            if(file->isOpen() && file->isReadable())
//                _process->start();
//            else
//                qDebug() << file->error();

//                    file->close();
//        delete file;

#if defined(__APPLE__)
        handlerPort = receivePort.GetPort();
        qDebug() << "initializing breakpad";
        eh = new google_breakpad::ExceptionHandler(filterCallback,
                                  nullptr,
                                  true,
                                  handlerPort);
#endif
#if defined(Q_OS_LINUX)
        md = new google_breakpad::MinidumpDescriptor(dumpPath.toStdString());
        eh = new google_breakpad::ExceptionHandler(*md, 0, filterCallback,0, true, -1);

#endif
    }


}

#endif
