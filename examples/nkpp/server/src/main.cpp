// © 2025 AO Kaspersky Lab
// Licensed under the MIT License

#include <iostream>
#include <variant>
#include <vector>

#include <kosipc/make_application.h>
#include <kosipc/serve_static_channel.h>

// Generated header.
#include <example/Server.edl.cpp.h>

#include <common/connection.h>
#include <common/exception_converter.h>
#include <common/food.h>

extern const char Tag[] = "[Server]";

using namespace kosipc::stdcpp;
using namespace example;

const std::string GetFoodStr(const Food &food)
{
    return std::visit(
        overloaded{
            [](const std::string &arg) { return arg; },
            [](unsigned int arg) {
                return (arg >= food::KnownFood.size()) ? "unknown something"
                                                       : std::string{food::KnownFood[arg]};
            }},
        food);
}

// Classes which contain implementation of interface which this program provides with endpoints.
template <typename Certain>
class IAnimal: public Animal
{
public:
    IAnimal(const char *label, const char *name): m_label{label}, m_name{name} {}

    void Say() override
    {
        std::cerr << Tag << ": " << m_label << " " << m_name << ": "
                  << static_cast<Certain *>(this)->GetWord() << std::endl;
    }

    void Eat(const Food &food) override
    {
        auto  foodName = GetFoodStr(food);
        auto &eatable  = static_cast<Certain *>(this)->GetEatable();

        auto it = std::find(std::begin(eatable), std::end(eatable), foodName);
        if (it == std::end(eatable))
        {
            std::cerr << Tag << ": " << m_label << " " << m_name << " don't want to eat "
                      << foodName << ": " << err::exception::FoodNotSuitable().what() << std::endl;
            throw err::exception::FoodNotSuitable{};
        }

        std::cerr << Tag << ": " << m_label << " " << m_name << " eats " << foodName << std::endl;
    }

protected:
    std::string_view m_label;
    std::string_view m_name;
};

class Cat: public IAnimal<Cat>
{
public:
    Cat(const char *label, const char *name): IAnimal(label, name) {}

    std::string_view GetWord()
    {
        return "meow";
    }

    const std::vector<std::string_view> &GetEatable() const
    {
        return Eatable;
    }

    static const std::vector<std::string_view> Eatable;
};

const std::vector<std::string_view> Cat::Eatable = {
    std::string_view{food::Fish}, std::string_view{food::Milk}};

class Dog: public IAnimal<Dog>
{
public:
    Dog(const char *label, const char *name): IAnimal(label, name) {}

    std::string_view GetWord()
    {
        return "wooow";
    }

    const std::vector<std::string_view> &GetEatable() const
    {
        return Eatable;
    }

    static const std::vector<std::string_view> Eatable;
};

const std::vector<std::string_view> Dog::Eatable = {
    std::string_view{food::Meat}, std::string_view{food::Milk}};

class CatDog: public IAnimal<CatDog>
{
public:
    CatDog(const char *label, const char *name, bool isDog): IAnimal(label, name), m_isDog{isDog} {}

    std::string_view GetWord()
    {
        return m_isDog ? "wooow" : "meow";
    }

    const std::vector<std::string_view> &GetEatable() const
    {
        return m_isDog ? Dog::Eatable : Cat::Eatable;
    }

private:
    bool m_isDog;
};

class Something: public Animal
{
public:
    Something(const char *label, const char *name): m_label{label}, m_name{name} {}

    void Say() override
    {
        std::cerr << Tag << ": " << m_label << " " << m_name << ": "
                  << err::exception::AnimalNotSupported().what() << std::endl;
        throw err::exception::AnimalNotSupported{};
    }

    void Eat(const Food &food) override
    {
        std::cerr << Tag << ": " << m_label << " " << m_name << " eats " << GetFoodStr(food)
                  << std::endl;
    }

private:
    const char *m_label;
    const char *m_name;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] const char *argv[])
{
    // Create Application object.
    kosipc::ApplicationSettings as;
    as.SetRetcodeExceptionConverter(std::make_shared<err::RetcodeExceptionConverter>());
    kosipc::Application app = kosipc::MakeApplicationAutodetect(as);

    std::cerr << Tag << ": Process started\n";

    // Create objects with interface implementation.
    Cat       lion{"Cat", "Lion"};
    Cat       domestic{"Cat", "Katty"};
    Dog       dog{"Dog", "Collie"};
    CatDog    catdogAsCat{"CatDog", "Cat", false};
    CatDog    catdogAsDog{"CatDog", "Dog", true};
    Something something{"Something", "Thing"};

    // Create object which contains description of structure of server components
    // and description of interfaces of endpoints provided with server.
    kosipc::components::Root root;

    // Link root structure fields and objects which serve as endpoints.
    root.catdog.cat       = &catdogAsCat;
    root.catdog.dog       = &catdogAsDog;
    root.collie.animal    = &dog;
    root.domestic.animal  = &domestic;
    root.lion.animal      = &lion;
    root.something.animal = &something;

    // Create IPC dispatch cycle.
    kosipc::EventLoop loop = app.MakeEventLoop(ServeStaticChannel(ServerChannelName, root));

    // Start cycle.
    std::cerr << Tag << ": IPC processing started\n";
    loop.Run();

    return EXIT_SUCCESS;
}
