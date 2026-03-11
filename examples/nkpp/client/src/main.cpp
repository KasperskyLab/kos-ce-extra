// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <iostream>
#include <vector>
#include <cassert>

#include <kosipc/connect_static_channel.h>
#include <kosipc/make_application.h>

#include <common/exception_converter.h>

// Generated headers.
#include <example/Client.edl.cpp.h>
#include <example/Animal.idl.cpp.h>

#include <common/connection.h>
#include <common/food.h>

extern const char Tag[] = "[Client]";

using namespace kosipc::stdcpp;
using namespace example;

void ExampleAnimals(kosipc::Application& app)
{
    auto lion =
        app.MakeProxy<Animal>(kosipc::ConnectStaticChannel(ServerChannelName, "lion.animal"));
    auto cat =
        app.MakeProxy<Animal>(kosipc::ConnectStaticChannel(ServerChannelName, "domestic.animal"));
    auto dog =
        app.MakeProxy<Animal>(kosipc::ConnectStaticChannel(ServerChannelName, "collie.animal"));
    auto catdog_cat =
        app.MakeProxy<Animal>(kosipc::ConnectStaticChannel(ServerChannelName, "catdog.cat"));
    auto catdog_dog =
        app.MakeProxy<Animal>(kosipc::ConnectStaticChannel(ServerChannelName, "catdog.dog"));
    auto something =
        app.MakeProxy<Animal>(kosipc::ConnectStaticChannel(ServerChannelName, "something.animal"));

    std::cerr << Tag << ": --------- Say() ---------\n";

    // clang-format off
    const std::array<Animal *, 6> talkingAnimals = {
        lion.get(),
        cat.get(),
        dog.get(),
        catdog_dog.get(),
        catdog_cat.get(),
        something.get()
    };
    // clang-format on

    for (auto animal : talkingAnimals)
    {
        try
        {
            animal->Say();
        }
        catch (const err::exception::AnimalNotSupported& e)
        {
            std::cerr << Tag << ": IPC request error: " << e.what() << ".\n";
        }
    }

    std::cerr << Tag << ": --------- Eat() ---------\n";

    const std::vector<std::pair<Animal*, Food>> eatingAnimalsAndFood = {
        {cat.get(),        food::Fish.data()                                      },
        {dog.get(),        static_cast<unsigned short>(1)                         },
        {catdog_cat.get(), food::KnownFood.back().data()                          },
        {catdog_dog.get(), static_cast<unsigned short>(food::KnownFood.size() - 2)},
        {something.get(),  "spaceship"                                            }
    };

    for (auto& [animal, food] : eatingAnimalsAndFood)
    {
        try
        {
            animal->Eat(food);
        }
        catch (const err::exception::FoodNotSuitable& e)
        {
            std::cerr << Tag << ": IPC request error: " << e.what() << ".\n";
        }
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    // Create Application object.
    kosipc::ApplicationSettings as;
    as.SetRetcodeExceptionConverter(std::make_shared<err::RetcodeExceptionConverter>());
    kosipc::Application app = kosipc::MakeApplicationAutodetect(as);

    std::cerr << Tag << ": Process started\n";

    std::cerr << Tag << ": ==== Example Animals ====\n";
    try
    {
        ExampleAnimals(app);
    }
    catch (...)
    {
        assert(false);
    }

    std::cerr << Tag << ": =========================\n";

    std::cerr << Tag << ": SUCCESS \n";
    return EXIT_SUCCESS;
}
