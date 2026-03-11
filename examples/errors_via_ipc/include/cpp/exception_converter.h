// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#pragma once

#include <kosipc/retcode_exception_converter.h>
#include <rtl/retcode.h>

#include <iostream>
#include <exception>
#include <stdexcept>

#include <component/logrr/cpp/logger.h>

#include "errors.h"
#include "common.h"

namespace err {

class RetcodeExceptionConverter: public kosipc::IRetcodeExceptionConverter
{
    void CatchException(Retcode& rc) override
    {
        try
        {
            throw;
        }
        catch (const exception::ServerBusy&)
        {
            rc = code::ServerBusy;
            LOG(INFO, "Exception: {}", err::exception::ServerBusy().what());
        }
        catch (const std::exception& e)
        {
            LOG(ERROR, " Service fatal error: {}", e.what());
            rc = rcFail;
        }
        catch (...)
        {
            LOG(ERROR, " Unknown exception occurred!");
            rc = rcFail;
        }
    }

    void ThrowException(Retcode rc) override
    {
        switch (rc)
        {
            case code::ServerBusy:
                throw exception::ServerBusy{};
        }
    }
};

} // namespace err
