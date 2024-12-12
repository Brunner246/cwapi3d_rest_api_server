//
// Created by MichaelBrunner on 12/12/2024.
//

export module domain:httpResponse;
import std;

export namespace rest_api::domain
{
struct HttpResponse
{
    int status_code = 200;
    std::string body{};
    std::unordered_map<std::string, std::string> headers{};
};

}