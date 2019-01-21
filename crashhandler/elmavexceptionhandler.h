#ifndef elmavexceptionhandler_H
#define elmavexceptionhandler_H

#define __APPLE__

#include <QProcess>
#include <QDebug>
#include <QTemporaryFile>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>

#include <pthread.h>

#if defined(__APPLE__)
#include "client/mac/handler/exception_handler.h"
#include "common/mac/MachIPC.h"
#endif

#define SERVER_BIN ":/crashserver_mac"

namespace elmavexceptionhandler
{
    QString dumpPath = "";
#if defined(__APPLE__)
    mach_port_t handlerPort = MACH_PORT_NULL;
    google_breakpad::ReceivePort receivePort;
#endif
    google_breakpad::ExceptionHandler* eh = nullptr;

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
        QScopedPointer<QTemporaryFile> temp;
        temp.reset(QTemporaryFile::createNativeFile(SERVER_BIN));
        temp.data()->setPermissions(QFileDevice::ExeOwner | QFileDevice::ReadOwner | QFileDevice::WriteOwner);

        if(temp.data()->open()) {

            qDebug() <<  "name of the file " << temp.data()->fileName() ;
            qDebug() << "permissions of the file " << temp.data()->permissions();
            QProcess* process = new QProcess;
            process->setProgram(temp.data()->fileName());
            process->setReadChannelMode(QProcess::MergedChannels);
            process->setArguments(QStringList() << dumpPath);
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
        else {
            qDebug() << "could not open the file   " <<  temp.data()->errorString();
            return false;

        }

    }

    void init()
    {

        dumpPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QDir::separator() + \
                "ElMaven" + QDir::separator() +  QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss") + QDir::separator();
        QDir dir;
        dir.mkpath(dumpPath);

        handlerPort = receivePort.GetPort();
        qDebug() << "initializing breakpad";
        eh = new google_breakpad::ExceptionHandler(filterCallback,
                                  nullptr,
                                  true,
                                  handlerPort);
    }


}

#endif
