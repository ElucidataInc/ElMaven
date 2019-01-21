#define __APPLE__ 1

#include <iostream>
#include <string>

#include <QTemporaryFile>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QProcess>

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


QMutex mutex;
QWaitCondition handlerWait;


#if defined(__APPLE__)
void OnChildProcessDumpRequested(void* aContext,
                                 const ClientInfo& aClientInfo,
                                 const std::string& aFilePath)
#endif
{
#if defined(__APPLE__)
    std::cerr << "handler: wrote dump for client " <<  aClientInfo.pid() << "   at path  " <<  aFilePath << "\n";
#endif

    mutex.lock();
    handlerWait.wakeOne();
    mutex.unlock();

}

int main(int argc, char** argv)
{
    std::cerr  << "handler: starting\n" ;
    mutex.lock();

    std::cerr << "dump path : " << argv[1] << "\n";
    std::string dumpPath = argv[1];
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
#endif

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


  return 0;
}
