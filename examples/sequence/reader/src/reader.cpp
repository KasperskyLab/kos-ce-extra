// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <iostream>

#include <kosipc/serve_static_channel.h>

// Generated headers.
#include <sequence_example/Reader.edl.cpp.h>

#include "common.h"

namespace {

const char Tag[] = "Reader";

} // namespace

using namespace seq_helpers;

class SequenceLogger: public kosipc::stdcpp::sequence_example::SequenceLogging
{
public:
    void LogSequence(const Seq& result) override
    {
        std::cerr << "[" << Tag << "]: Received sequence: " << result << std::endl;
    }
};

// Reader entry point.
int main([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    // Create Application object.
    kosipc::Application app = kosipc::MakeApplicationAutodetect();

    std::cerr << "[" << Tag << "]: Hello from " << Tag << " program!\n";

    // Create objects with interface implementation.
    SequenceLogger logger;

    // Create object which contains description of structure of server components
    // and description of interfaces of endpoints provided with server.
    kosipc::components::Root root;

    // Link root structure fields and objects which serve as endpoints.
    root.log = &logger;

    // Create IPC dispatch cycle.
    kosipc::EventLoop loop = app.MakeEventLoop(ServeStaticChannel(ServerChannelName, root));

    // Start cycle.
    loop.Run();

    return EXIT_SUCCESS;
}
