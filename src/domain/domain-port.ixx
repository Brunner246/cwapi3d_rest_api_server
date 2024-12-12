//
// Created by MichaelBrunner on 12/12/2024.
//

export module domain:port;

export namespace rest_api::domain
{
    class Port
    {
        unsigned short port;

    public:
        explicit Port(const unsigned short port)
            : port{port}
        {
        }

        [[nodiscard]] unsigned short toInt() const
        {
            return port;
        }
    };
}