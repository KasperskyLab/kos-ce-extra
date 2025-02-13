/*
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#pragma once

#include <kosipc/retcode_exception_converter.h>
#include <rtl/retcode.h>

#include <iostream>
#include <exception>
#include <stdexcept>

#include <common/errors.h>
#include <common/log.h>

namespace err          {

class RetcodeExceptionConverter : public kosipc::IRetcodeExceptionConverter
{
    void CatchException(Retcode& rc) override
    {
        try
        {
            throw;
        }
        catch (const exception::AnimalNotSupported&)
        {
            rc = code::AnimalNotSupported;
        }
        catch (const exception::FoodNotSuitable&)
        {
            rc = code::FoodNotSuitable;
        }
        catch (const std::exception& e)
        {
            std::cerr << Tag << ": Service fatal error: " << e.what() << std::endl;
            rc = rcFail;
        }
        catch (...)
        {
            std::cerr << Tag << ": Unknown exception occurred!\n";
            rc = rcFail;
        }
    }

    void ThrowException(Retcode rc) override
    {
        switch (rc)
        {
            case code::AnimalNotSupported:
                throw exception::AnimalNotSupported{};
            case code::FoodNotSuitable:
                throw exception::FoodNotSuitable{};
        }
    }
};

} // namespace err
