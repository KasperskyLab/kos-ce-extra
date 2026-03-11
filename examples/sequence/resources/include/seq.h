// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include <iterator>
#include <numeric>

#include <sequence_example/SequenceLogging.idl.cpp.h>

namespace seq_helpers {

using Seq = kosipc::stdcpp::sequence_example::Seq;

template <typename OStrm>
OStrm& operator<<(OStrm& os, Seq const& seq)
{
    os << "[";
    if (!seq.empty())
    {
        auto last = seq.end() - 1;
        std::copy(seq.begin(), last, std::ostream_iterator<Seq::value_type>{os, ", "});
        os << *last;
    }
    return os << "]";
}

Seq MakeSeq(Seq::size_type size, Seq::value_type value)
{
    Seq seq(size);
    std::iota(seq.begin(), seq.end(), value);
    return seq;
}

} // namespace seq_helpers
