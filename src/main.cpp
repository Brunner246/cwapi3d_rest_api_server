#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <boost/locale.hpp>
#include <cwapi3d/CwAPI3D.h>

import domain;
import app.HttpServer;
import adapters.HttpHandlers;
import std;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

std::jthread SERVER_THREAD;

CWAPI3D_PLUGIN bool plugin_x64_init(CwAPI3D::ControllerFactory *aFactory)
{
    try {
        const auto address = rest_api::domain::Address{"127.0.0.1"};
        const auto port = rest_api::domain::Port{8080};
        auto app = std::make_unique<rest_api::app::HttpServer>(address, port);

        app->get(
            "/elements",
            [aFactory](rest_api::domain::HttpRequest &&request_body, rest_api::domain::HttpResponse &response_body)
            {
                const auto controller = std::make_shared<rest_api::adapters::Cadwork3DModelController>(aFactory);
                rest_api::adapters::getActiveElementIds(request_body, response_body, controller);
            });


        app->get(
            "/",
            [](rest_api::domain::HttpRequest &&request_body, rest_api::domain::HttpResponse &response_body)
            {
                std::ignore = request_body;

                const boost::json::object body = {
                    {"message", "Hello, world!"}
                };
                response_body.headers["Content-Type"] = "application/json";
                response_body.body = boost::json::serialize(body);
            });

        app->post(
            "/echo",
            [](rest_api::domain::HttpRequest &&request_body, rest_api::domain::HttpResponse &response_body)
            {
                response_body.status_code = 200;
                response_body.headers["Content-Type"] = "application/json";
                response_body.body = request_body.body;
            });


        auto thread = std::jthread([App = std::move(app)]() { App->run(); });
        SERVER_THREAD = std::move(thread);

        const auto formattedMessage = std::format("Server started on http://{}/", address.toString(), port);
        aFactory->getUtilityController()->printToConsole(
            std::wstring(formattedMessage.begin(), formattedMessage.end()).c_str());

        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
