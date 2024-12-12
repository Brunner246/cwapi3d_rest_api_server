//
// Created by MichaelBrunner on 12/12/2024.
//

export module app.HttpServer;

import std;
import domain;

namespace rest_api::app
{

class HttpServerImpl;

export class HttpServer
{
public:
    using RequestHandler = std::function<void(domain::HttpRequest &&req, domain::HttpResponse &res)>;

    explicit HttpServer(const domain::Address &address, domain::Port port);

    void get(const std::string &path, RequestHandler handler) const;

    void post(const std::string &path, RequestHandler handler) const;

    void run() const;

    ~HttpServer();

private:
    std::unique_ptr<HttpServerImpl> impl;
};
}
