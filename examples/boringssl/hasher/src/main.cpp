// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <openssl/base.h>
#include <openssl/evp.h>
#include <string_view>
#include <type_traits>

namespace hasher {

template <typename OStream, typename T>
OStream& operator << (OStream& os, std::vector<T> const& vect)
{
    for (auto const& val : vect)
    {
        if constexpr (std::is_same_v<T, unsigned char>)
            os << std::setfill('0') << std::setw(2) << std::hex
               << static_cast<unsigned int>(val);
        else
            os << val;
    }
    return os;
}

class Digest
{
public :
    using Data = std::vector<unsigned char>;

    Digest(Digest&& other) = default;
    Digest& operator = (Digest&&) = default;

    explicit Digest(std::string name)
        : m_name{std::move(name)}
        , m_context{EVP_MD_CTX_new()}
        , m_value(EVP_MAX_MD_SIZE)
    {
        auto md = EVP_get_digestbyname(m_name.c_str());
        if (!md)
            throw std::runtime_error{"Unknown message digest" + m_name};

        if (!EVP_DigestInit_ex(m_context.get(), md, NULL))
            throw std::runtime_error{"Digest initialization failed"};
    }

    void Update(const void* data, size_t length) noexcept
    {
        if (m_context && data != nullptr && length != 0)
        {
            EVP_DigestUpdate(m_context.get(), data, length);
        }
    }

    Digest& operator << (const std::string_view data) &
    {
        if (m_context && !data.empty())
        {
            Update(data.data(), data.size());
        }
        return *this;
    }

    Digest&& operator << (std::string_view data) &&
    {
        if (m_context && !data.empty())
        {
            Update(data.data(), data.size());
        }
        return std::move(*this);
    }

    const Data& Get()
    {
        if (m_context)
        {
            unsigned int length{};
            EVP_DigestFinal_ex(m_context.get(), m_value.data(), &length);
            m_value.resize(length);
            m_context.reset();
        }

        return m_value;
    }

    const std::string Name() const noexcept
    {
        return m_name;
    }

private:

    std::string                 m_name;
    bssl::UniquePtr<EVP_MD_CTX> m_context;
    Data                        m_value;
};

template <typename... MsgParts>
inline void PrintMessageDigest(std::string digestAlgorithm, MsgParts&&... msgParts) try
{
    auto digest = (Digest{digestAlgorithm} << ... << msgParts);
    std::cerr << "Message parts: ";
    ((std::cerr << '[' << msgParts << ']'), ...) << std::endl
                << "Message digest obtain with " << digest.Name()
                << " algorithm is: " << digest.Get() << std::endl;
}
catch (const std::exception& e)
{
    std::cerr << "Error occurred :" << e.what() << std::endl;
}

} // namespace hasher


int main(void)
{
    hasher::PrintMessageDigest("sha256", "Test", "Message", "Hello", "World");
    return EXIT_SUCCESS;
}
