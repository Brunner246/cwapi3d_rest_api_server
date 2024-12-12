//
// Created by MichaelBrunner on 12/12/2024.
//

export module domain:address;
import std;

export namespace rest_api::domain
{
    class Address
    {
        std::string address{};

    public:
        explicit Address(std::string address)
            : address{std::move(address)}
        {
            //TODO invariant check for address
        }

        [[nodiscard]] std::string toString() const
        {
            return address;
        }
    };
}