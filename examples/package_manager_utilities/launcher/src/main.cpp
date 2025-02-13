/*
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <unistd.h>

#include <handle/uidtype.h>

#include <filesystem>

#include <component/execution_manager/kos_ipc/execution_manager_proxy.h>

namespace execmgr = execution_manager;
namespace fs = std::filesystem;
using namespace kos::rtl;

int main([[maybe_unused]] int argc, [[maybe_unused]] const char *argv[])
{
    static_assert(
        (sizeof(PACKAGE_ID) > 1)
        && "PACKAGE_ID (ID of the installed KPA package) is empty! Set it in the CMakeLists.txt root file.");
    static_assert(
        (sizeof(APP_INSTALL_DIR) > 1)
        && "APP_INSTALL_DIR (path where KPA packages are installed) is empty! Set it in the CMakeLists.txt root file.");

    using namespace std::literals::chrono_literals;

    execmgr::IExecutionManagerPtr ptr;
    char mainConnection[] = "ExecMgrEntity";
    char appControlInterface[] = "kl.execution_manager.ExecutionManager.iac";
    char appStateInterface[] = "kl.execution_manager.ExecutionManager.ias";
    execmgr::ipc::ExecutionManagerConfig cfg
    {
        mainConnection,
        appControlInterface,
        appStateInterface
    };

    if (CreateExecutionManager(cfg, ptr) != result_code::Ok)
    {
        std::cerr << "Cannot create an execution manager" << std::endl;
        return EXIT_FAILURE;
    }

    execmgr::IApplicationControllerPtr ac;
    if (ptr->GetApplicationController(ac) != result_code::Ok)
    {
        std::cerr << "Cannot get an application controller" << std::endl;
        return EXIT_FAILURE;
    }

    const fs::path appPath{"/" APP_INSTALL_DIR "/" PACKAGE_ID "/bin/Application"};
    execmgr::IApplicationController::StartEntityInfo info;
    info.eiid = std::string{"application.Application"};
    info.entityName = std::string{"application.Application"};
    info.envs = std::vector<std::string>{"VFS_FILESYSTEM_BACKEND=client:kl.VfsSdCardFs"};
    execmgr::IApplicationController::StartEntityResultInfo result;

    if (ac->StartEntity(appPath, info, result) != result_code::Ok)
    {
        std::cerr << "Cannot start the application from " << appPath << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
