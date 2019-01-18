#include <iostream>
#include <string>

#include <QTemporaryFile>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QProcess>

#if defined(__w64)
#include "client/windows/crash_generation/crash_generation_server.h"
#include "client/windows/crash_generation/client_info.h"
#include "client/windows/common/ipc_protocol.h"
#endif

#if defined(__APPLE__)
#include "client/mac/crash_generation/client_info.h"
#include "client/mac/crash_generation/crash_generation_server.h"
#include "common/mac/MachIPC.h"
#endif

using google_breakpad::ClientInfo;
using google_breakpad::CrashGenerationServer;
#if defined(__APPLE__)
using google_breakpad::ReceivePort;
using google_breakpad::MachPortSender;
using google_breakpad::MachReceiveMessage;
using google_breakpad::MachSendMessage;
#endif
using std::string;


enum MessageId : int {
    SERVERSTARTED = 0,
    SERVERFAILED
};

QMutex mutex;
QWaitCondition handlerWait;


void OnChildProcessDumpRequested(void* aContext,
                                 #if defined(__APPLE__)
                                 const ClientInfo& aClientInfo,
                                 const std::string& aFilePath
                                 #endif

                                 #if defined(__w64)
                                 const ClientInfo* aClientInfo,
                                 const std::wstring* aFilePath
                                 #endif
                                 )
{
#if defined(__APPLE__)
    std::cerr << "handler: wrote dump for client " <<  aClientInfo.pid() << "   at path  " <<  aFilePath << "\n";
#endif

#if defined(__w64)
    std::cerr << "handler: wrote dump for client " <<  aClientInfo->pid() << "   at path  " <<  aFilePath->c_str() << "\n";
#endif


    mutex.lock();
    handlerWait.wakeOne();
    mutex.unlock();

}

int main(int argc, char** argv)
{
    mutex.lock();

    std::cerr << "handler starting \n";

#if defined(__w64)
    std::wstring dumpPath = QString(argv[1]).toStdWString();
#endif

#if defined(__APPLE__)
    std::string dumpPath = argv[1];
#endif


#if defined(__w64)
     CrashGenerationServer* server = new CrashGenerationServer(L"\\\\.\\pipe\\ELMAVEN_HANDLER",
                                                   NULL,
                                                   NULL,
                                                    NULL,
                                                    OnChildProcessDumpRequested,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   true,
                                                   &dumpPath);

     if(!server->Start())
        std::cout << MessageId::SERVERFAILED;
     else
        std::cout  << MessageId::SERVERSTARTED;

    handlerWait.wait(&mutex);
#endif

#if defined(__APPLE__)
    // Use the bootstrap port, which the parent process has set, to
    // send a message to the parent process.
    mach_port_t bootstrap_port;
    if (task_get_bootstrap_port(mach_task_self(),&bootstrap_port) != KERN_SUCCESS) {
        std::cerr <<  "handler: failed to get bootstrap port\n";
        return 1;
    }

    ReceivePort receiver;
    MachPortSender sender(bootstrap_port);
    MachSendMessage send_message(0);
    // Include a port to send a reply on.
    send_message.AddDescriptor(receiver.GetPort());
    sender.SendMessage(send_message, MACH_MSG_TIMEOUT_NONE);
    std::cerr <<  "handler: sent message to parent\n" ;

  // Now wait for a reply.
    MachReceiveMessage receive_message;
    if (receiver.WaitForMessage(&receive_message, MACH_MSG_TIMEOUT_NONE) != KERN_SUCCESS) {
        std::cerr << "handler: failed to get a reply from parent\n";
        return 1;
    }
    std::cerr <<  "handler: got message from parent with %d descriptors\n" <<  receive_message.GetDescriptorCount() << "\n";

  // Restore the bootstrap port to the original that the parent just sent.
    if (task_set_bootstrap_port(mach_task_self(),receive_message.GetTranslatedPort(0)) != KERN_SUCCESS) {
        std::cerr  <<  "handler: failed to reset bootstrap port\n" ;
        return 1;
    }

  // Now use the prearranged port to start the crash server
    std::cerr << " initializing the crash generation server " << std::endl;
    CrashGenerationServer crash_server(
                receive_message.GetTranslatedPort(1),
                OnChildProcessDumpRequested,
                NULL,
                NULL,
                NULL,
                true,
                dumpPath);

    std::cerr << "initialized the server " << std::endl;

    if (!crash_server.Start()) {
       std::cerr << "handler: Failed to start CrashGenerationServer\n";
        return 1;
    }
    std::cerr << "handler: started server\n";


    std::cerr << "handler waiting for client request \n";
    handlerWait.wait(&mutex);

    std::cerr <<  "handler: shutting down\n";
    crash_server.Stop();
    std::cerr << __TIME__  <<  "  :  handler: exiting \n";

#endif

  return 0;
}
