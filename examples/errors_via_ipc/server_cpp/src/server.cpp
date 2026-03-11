// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <cstdint>
#include <iostream>

#include <rtl_cpp/retcode.h>
#include <kosipc/serve_static_channel.h>

// Generated headers.
#include <errors_via_ipc_example/Server.edl.cpp.h>

#include "common.h"
#include "exception_converter.h"

constexpr auto Tag = "CXX Server";

class NumberSummarizer: public kosipc::stdcpp::errors_via_ipc_example::NumberSummation
{
public:
    void SumRequest(const Request& request, uint64_t& result) override
    {
        LOG(INFO,
            "Received request: repeats: {} num1: {} num2: {}",
            request.repeats,
            request.num1,
            request.num2);

        if (m_requestCounter++ % 2 == 0)
        {
            throw err::exception::ServerBusy{};
        }
        result = request.num1 + request.num2;
    }

private:
    uint64_t m_requestCounter = 1;
};

// Server entry point.
int main()
{
    // Create Application object.
    kosipc::ApplicationSettings as;
    as.SetRetcodeExceptionConverter(std::make_shared<err::RetcodeExceptionConverter>());
    kosipc::Application app = kosipc::MakeApplicationAutodetect(as);

    logrr::Init(Tag);
    LOG(INFO, "Started");

    // Create objects with interface implementation.
    NumberSummarizer summarizer;

    // Create object which contains description of structure of server components
    // and description of interfaces of endpoints provided with server.
    kosipc::components::Root root;

    // Link root structure fields and objects which serve as endpoints.
    root.log = &summarizer;

    // Create IPC dispatch cycle.
    kosipc::EventLoop loop = app.MakeEventLoop(ServeStaticChannel(ServerChannelName[1], root));

    // Start cycle.
    loop.Run();

    return EXIT_SUCCESS;
}
