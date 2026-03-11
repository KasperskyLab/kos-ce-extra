// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <handle/uidtype.h>

#include <filesystem>

#include <component/execution_manager/kos_ipc/execution_manager_proxy.h>

#include "notification.h"

using namespace kos::rtl;

namespace execmgr = execution_manager;
namespace fs      = std::filesystem;

extern const char Tag[] = "[Client]";

constexpr char mainConnection[]      = "ExecMgrEntity";
constexpr char appControlInterface[] = "kl.execution_manager.ExecutionManager.iac";
constexpr char appStateInterface[]   = "kl.execution_manager.ExecutionManager.ias";

int main([[maybe_unused]] int argc, [[maybe_unused]] const char *argv[])
{
    using namespace std::literals::chrono_literals;

    std::cout << Tag << ": "
              << "Client started\n";

    // We publish the endpoints on the name server to allow
    // the running program to access the client later.
    if (PublishService() != result_code::Ok)
    {
        return EXIT_FAILURE;
    }

    // Get the pointer to the instance of the IExecutionManager interface
    // that is required for working with the ExecutionManager component.
    execmgr::IExecutionManagerPtr        ptr;
    execmgr::ipc::ExecutionManagerConfig cfg{
        mainConnection, appControlInterface, appStateInterface};
    if (CreateExecutionManager(cfg, ptr) != result_code::Ok)
    {
        std::cerr << Tag << ": "
                  << "Cannot create an execution manager\n";
        return EXIT_FAILURE;
    }

    // The IExecutionManager interface lets you access pointers to the
    // IApplicationController – interface for starting and stopping processes.
    execmgr::IApplicationControllerPtr ac;
    if (ptr->GetApplicationController(ac) != result_code::Ok)
    {
        std::cerr << Tag << ": "
                  << "Cannot get an application controller\n";
        return EXIT_FAILURE;
    }

    const fs::path appPath{"/Application"};

    execmgr::IApplicationController::StartEntityResultInfo result;
    execmgr::IApplicationController::StartEntityInfo       info;

    info.eiid = std::string{"application.Application"};
    // clang-format off
    info.args = std::vector<std::string>{"1", "ARG1", "ARG2", "ARG3"};
    info.envs = std::vector<std::string>
    {
        "ENV1=10",
        "ENV2=envStr",
        "VFS_FILESYSTEM_BACKEND=client:kl.VfsSdCardFs"
    };

    std::cout << Tag << ": " << "Starting applications from ELF\n";
    // clang-format on

    // The StartEntity() method starts a process. It receives the path to the
    // executable file that should be run and the structure containing the run
    // parameters for the StartEntityInfo process, and returns the structure
    // containing the StartEntityResultInfo process run results. All fields of
    // the StartEntityInfo structure are optional for initialization.
    if (ac->StartEntity(appPath, info, result) != result_code::Ok)
    {
        std::cerr << Tag << ": "
                  << "Cannot start the application from " << appPath << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << Tag << ": "
              << "Application started with SID " << result.sid << std::endl;

    auto firstAppId = result.entId;

    // We are waiting for a message from the running program indicating that
    // it has started and completed all the necessary work.
    if (WaitForNotification() != result_code::Ok)
    {
        std::cerr << Tag << ": "
                  << "Cannot get a notification from the application1\n";
        return EXIT_FAILURE;
    }

    const fs::path                                   app2Path{"/Application"};
    execmgr::IApplicationController::StartEntityInfo info2;

    info2.entityName = std::string{"application.Application"};
    info2.eiid       = std::string{"application.Application"};
    info2.args       = std::vector<std::string>{"2"};
    info2.envs       = std::vector<std::string>{"VFS_FILESYSTEM_BACKEND=client:kl.VfsSdCardFs"};

    if (ac->StartEntity(app2Path, info2, result) != result_code::Ok)
    {
        std::cerr << Tag << ": "
                  << "Cannot start the application from " << app2Path << std::endl;
        return EXIT_FAILURE;
    }

    auto secondAppId = result.entId;

    // We are waiting for a message from the running process indicating that
    // it has started and completed all the necessary work.
    if (WaitForNotification() != result_code::Ok)
    {
        std::cerr << Tag << ": "
                  << "Cannot get a notification from the application2\n";
        return EXIT_FAILURE;
    }

    // The StopEntity() method immediately stops the execution of a process. It
    // receives the firstAppId and secondAppId variables that identify the
    // started processes.
    if (ac->StopEntity(firstAppId) != result_code::Ok)
    {
        std::cerr << Tag << ": "
                  << "Cannot stop the application from " << appPath << std::endl;
        return EXIT_FAILURE;
    }

    if (ac->StopEntity(secondAppId) != result_code::Ok)
    {
        std::cerr << Tag << ": "
                  << "Cannot stop the application from " << app2Path << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
