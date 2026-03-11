// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <iostream>
#include <chrono>
#include <thread>

#include <kosipc/connect_static_channel.h>

// Generated headers.
#include <sequence_example/Writer.edl.cpp.h>
#include <sequence_example/SequenceLogging.idl.cpp.h>

#include "common.h"

namespace {

const char Tag[] = "Writer";

} // namespace

using namespace std::chrono_literals;

namespace {

constexpr const std::chrono::duration writeDelay{3s};

} // namespace

using namespace kosipc::stdcpp;
using namespace sequence_example;
using namespace seq_helpers;

void TryLogSequence(kosipc::unique_ptr<SequenceLogging>& logger, const Seq& seq)
try
{
    logger->LogSequence(seq);
    if (seq.size() > MaxSecurityGrantedSequenceLength)
    {
        std::cerr << "[" << Tag << "]: Error: something goes wrong!\n";
    }
}
catch (const std::exception& e)
{
    if (seq.size() > MaxSecurityGrantedSequenceLength)
    {
        std::cerr << "[" << Tag << "]: Expected behavior: can`t send message to Reader entity!\n";
    }
    else
    {
        std::cerr << " [" << Tag << "]: " << e.what() << '\n';
    }
}

void Example(kosipc::Application& app)
{
    auto logger = app.MakeProxy<SequenceLogging>(
        kosipc::ConnectStaticChannel(ServerChannelName, "sequence_example.Reader.log"));

    for (auto i = MinSequenceLength;; i != MaxSequenceLength ? ++i : i = MinSequenceLength)
    {
        TryLogSequence(logger, seq_helpers::MakeSeq(i, 1));
        std::this_thread::sleep_for(writeDelay);
    }
}

// Writer entry point.
int main([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    // Create Application object.
    kosipc::Application app = kosipc::MakeApplicationAutodetect();

    std::cerr << "[" << Tag << "]: Hello from " << Tag << " program!\n";

    try
    {
        Example(app);
    }
    catch (...)
    {
        assert(false);
    }

    return EXIT_SUCCESS;
}
