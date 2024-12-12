//
// Created by MichaelBrunner on 12/12/2024.
//

export module domain:httpRequest;
import std;

export namespace rest_api::domain
{
class HttpMethod
{
public:
    enum class HttpVerb
    {
        GET,
        POST,
    };

    explicit HttpMethod(const HttpVerb method)
        : httpMethod{method}
    {
    }

    [[nodiscard]] auto toString() const
    {
        switch (httpMethod) {
        case HttpVerb::GET:
            return "GET";
        case HttpVerb::POST:
            return "POST";
        }
        throw std::runtime_error("Unknown HTTP_METHOD");
    }

    HttpVerb httpMethod = HttpVerb::GET;
};
struct HttpRequest
{
    HttpMethod method{HttpMethod::HttpVerb::GET};
    std::string path{}; // e.g., "/"
    std::string body{};
    std::unordered_map<std::string, std::string> headers{};
};
}
