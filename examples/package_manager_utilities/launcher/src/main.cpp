// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <unistd.h>

#include <handle/uidtype.h>

#include <filesystem>

#include <component/package_manager/kos_ipc/package_manager_proxy.h>
#include <component/execution_manager/kos_ipc/execution_manager_proxy.h>

constexpr auto pkgMgrMainConnection = "PkgMgrEntity";
constexpr auto pkgControlInterface  = "kl.package_manager.PackageManager.ipkgc";

namespace pkgmgr  = package_manager;
namespace execmgr = execution_manager;
namespace fs      = std::filesystem;
using namespace kos::rtl;

int main([[maybe_unused]] int argc, [[maybe_unused]] const char *argv[])
{
    static_assert(
        (sizeof(PACKAGE_ID) > 1)
        && "PACKAGE_ID (ID of the installed KPA package) is empty! Set it in the CMakeLists.txt root file.");

    /* Gets the pointer to the instance of the IPackageManager interface that is required for
     * working with the PackageManager component. */
    pkgmgr::ipc::PackageManagerConfig pkg_cfg{pkgMgrMainConnection, pkgControlInterface};
    pkgmgr::IPackageManagerPtr        pkg_mgr;
    if (CreatePackageManager(pkg_cfg, pkg_mgr) != result_code::Ok)
    {
        std::cerr << "Get package controller" << std::endl;
        return EXIT_FAILURE;
    }

    /* Gets the pointer to an instance of the IPackageController interface. */
    pkgmgr::IPackageControllerPtr pkg_ctrl;
    if (pkg_mgr->GetPackageController(pkg_ctrl) != result_code::Ok)
    {
        std::cerr << "Cannot get package controller" << std::endl;
        return EXIT_FAILURE;
    }

    /* Gets the pointer to the instance of the IPackageManifest interface required for receiving
     * data on the KPA package manifest. */
    pkgmgr::IPackageManifestPtr pkg_manifest;
    if (pkg_ctrl->GetManifest(PACKAGE_ID, pkg_manifest) != result_code::Ok)
    {
        std::cerr << "Cannot get package manifest" << std::endl;
        return EXIT_FAILURE;
    }

    /* Gets information about the startup configurations of a program installed from the KPA
     * package. */
    std::vector<pkgmgr::IPackageManifest::RunConfigurationInfo> pkg_run_cfgs;
    if (pkg_manifest->GetRunConfigurationsInfo(pkg_run_cfgs) != result_code::Ok)
    {
        std::cerr << "Cannot get package run configuration info" << std::endl;
        return EXIT_FAILURE;
    }

    /* Gets the pointer to an instance of the IExecutionManager interface using the system program
     * DCM to connect to the ExecutionManager process. */
    auto ptr = execmgr::ipc::ExecutionManagerConnector::DcmConnect(
        execmgr::ipc::ExecutionManagerConnector::DefaultTimeout);
    if (!ptr)
    {
        std::cerr << "Cannot create execution manager" << std::endl;
        return EXIT_FAILURE;
    }

    /* Gets the pointer to the instance of the IProcessControl interface that lets you start a
     * process from an executable file installed by the PackageManager component from the KPA
     * package. */
    alm::execution_manager::IProcessControlPtr pc;
    if (ptr->GetProcessController(pc) != result_code::Ok)
    {
        std::cerr << "Cannot get process controller" << std::endl;
        return EXIT_FAILURE;
    }

    std::string run_cfg_id;
    if (pkg_run_cfgs.empty())
    {
        std::cerr << "No run configuration" << std::endl;
    }
    else
    {
        run_cfg_id = pkg_run_cfgs[0].id;
    }

    /* Starts a process. */
    alm::execution_manager::IProcessControl::StartOptions start_options;
    alm::execution_manager::IpcHandle                     proc_token;
    if (pc->StartProcess(PACKAGE_ID, run_cfg_id, start_options, proc_token) != result_code::Ok)
    {
        std::cerr << "Cannot start process " << PACKAGE_ID << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
