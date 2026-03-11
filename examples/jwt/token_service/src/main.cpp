// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wstring-conversion"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#include <strings.h> // strcasecmp for jwt.hpp
#include <jwt/jwt.hpp>
#pragma clang diagnostic pop

#include <kosipc/serve_static_channel.h>
#include <kosipc/make_application.h>

#include <component/logrr/cpp/logger.h>

#include <jwt_example/IToken.idl.cpp.h>
#include <jwt_example/TokenService.edl.cpp.h>

#include <connections.h>

class JwtTokenService: public kosipc::stdcpp::jwt_example::IToken
{
public:
    JwtTokenService(std::string secret): m_secret(secret) {}

    void Generate(Token& token) override
    {
        LOG(INFO, "Generate");

        token = "";

        try
        {
            jwt::jwt_object obj{
                jwt::params::algorithm("HS256"),
                jwt::params::payload({{m_payloadKey, m_payloadValue}}
                ),
                jwt::params::secret(m_secret),
            };

            token = obj.signature();
        }
        catch (std::exception& err)
        {
            LOG(ERROR, "Token create error: {}", err.what());
        }
    }

    void Verify(const Token& token, Boolean& result) override
    {
        LOG(INFO, "Verify");
        result = false;

        try
        {
            auto decObj = jwt::decode(
                token, jwt::params::algorithms({"HS256"}), jwt::params::secret(m_secret));
            LOG(INFO, " HEADER: {}", jwt::to_json_str(decObj.header(), false));
            LOG(INFO, "PAYLOAD: {}", jwt::to_json_str(decObj.payload(), false));
            if (decObj.payload().has_claim(m_payloadKey)
                && decObj.payload().get_claim_value<std::string>(m_payloadKey) == m_payloadValue)
            {
                result = true;
            }
        }
        catch (std::exception& err)
        {
            LOG(ERROR, "Token verify error: {}", err.what());
        }
    }

private:
    std::string m_secret;
    const char* m_payloadKey   = "secret";
    const char* m_payloadValue = "TOKEN";
};

int main(void)
{
    logrr::Init(AppTag);
    LOG(INFO, "Service started");

    kosipc::Application app = kosipc::MakeApplicationAutodetect();

    JwtTokenService          service("some secret word");
    kosipc::components::Root root;
    root.token = &service;

    kosipc::EventLoop loop =
        app.MakeEventLoop(ServeStaticChannel(connections::ConnectionName, root));
    loop.Run();

    LOG(INFO, "Service stopped");

    return 0;
}
