//
// Created by MichaelBrunner on 12/12/2024.
//

module;

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>

module app.HttpServer;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

namespace
{
rest_api::domain::HttpMethod getHttpMethod(const http::request<http::string_body> &req)
{
    return req.method_string() == "GET"
               ? rest_api::domain::HttpMethod(rest_api::domain::HttpMethod::HttpVerb::GET)
               : rest_api::domain::HttpMethod(rest_api::domain::HttpMethod::HttpVerb::POST);
}
}

namespace rest_api::app
{
class HttpServerImpl
{
public:
    using RequestHandler = HttpServer::RequestHandler;

    HttpServerImpl(const domain::Address &address, const domain::Port port)
        : ioContext(1),
          tcpAcceptor(ioContext, tcp::endpoint(net::ip::make_address("127.0.0.1" /*address.toString()*/), port.toInt()))
    {
        std::ignore = address;
    }

    void get(const std::string &path, RequestHandler handler)
    {
        getHandlers[path] = std::move(handler);
    }

    void post(const std::string &path, RequestHandler handler)
    {
        postHandlers[path] = std::move(handler);
    }

    void run()
    {
        std::cout << "Listening on http://" << tcpAcceptor.local_endpoint() << std::endl;
        for (;;) {
            tcp::socket socket{ioContext};
            tcpAcceptor.accept(socket);
            std::thread([this, socket = std::move(socket)]() mutable { doSession(std::move(socket)); }).detach();
        }
    }

private:
    void doSession(tcp::socket socket)
    {
        constexpr auto buffer_size = 8192;
        beast::flat_buffer buffer(buffer_size);
        http::request<http::string_body> req;
        http::response<http::string_body> res;

        beast::error_code ec;
        http::read(socket, buffer, req, ec);
        if (ec == http::error::end_of_stream) {
            std::ignore = socket.shutdown(tcp::socket::shutdown_send, ec);
            return;
        }

        handleRequest(std::move(req), res);
        http::write(socket, res, ec);
        std::ignore = socket.shutdown(tcp::socket::shutdown_send, ec);
    }

    static domain::HttpRequest mapOnHttpRequest(const http::request<http::string_body> &req)
    {
        domain::HttpRequest result;
        result.method = getHttpMethod(req);
        //TODO Only GET and POST are supported in this example
        result.path = req.target();
        result.body = req.body();
        for (const auto &field : req) {
            result.headers[field.name_string()] = field.value();
        }

        return result;
    }
    void handleRequest(http::request<http::string_body> &&req, http::response<http::string_body> &res)
    {
        domain::HttpResponse appRes;
        if (!processRequest(mapOnHttpRequest(req), appRes)) {
            setNotFoundResponse(appRes);
        }

        buildHttpResponse(appRes, res);
    }

    bool processRequest(domain::HttpRequest appReq, domain::HttpResponse &appRes)
    {
        const auto &method = appReq.method.httpMethod;
        const auto &path = appReq.path;

        if (method == domain::HttpMethod::HttpVerb::GET && getHandlers.contains(path)) {
            getHandlers[path](std::move(appReq), appRes);
            return true;
        }

        if (method == domain::HttpMethod::HttpVerb::POST && postHandlers.contains(path)) {
            postHandlers[path](std::move(appReq), appRes);
            return true;
        }

        return false;
    }

    static void setNotFoundResponse(domain::HttpResponse &appRes)
    {
        appRes.status_code = 404;
        appRes.body = "The resource was not found.";
        appRes.headers["Content-Type"] = "text/plain";
    }

    static void buildHttpResponse(const domain::HttpResponse &appRes, http::response<http::string_body> &res)
    {
        res.result(static_cast<http::status>(appRes.status_code));
        res.body() = appRes.body;

        for (const auto &[key, value] : appRes.headers) {
            res.set(http::string_to_field(key), value);
        }

        res.prepare_payload();
    }

    net::io_context ioContext;
    tcp::acceptor tcpAcceptor;
    std::unordered_map<std::string, RequestHandler> getHandlers{};
    std::unordered_map<std::string, RequestHandler> postHandlers{};
};

HttpServer::HttpServer(const domain::Address &address, const domain::Port port)
    : impl(std::make_unique<HttpServerImpl>(address, port))
{
}

void HttpServer::get(const std::string &path, RequestHandler handler) const
{
    impl->get(path, std::move(handler));
}

void HttpServer::post(const std::string &path, RequestHandler handler) const
{
    impl->post(path, std::move(handler));
}

void HttpServer::run() const
{
    impl->run();
}

HttpServer::~HttpServer() = default;
}
