#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <functional>
#include <utility>
#include <cwapi3d/CwAPI3D.h>

import domain;
import app.HttpServer;
import adapters.HttpHandlers;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

std::jthread SERVER_THREAD;

CWAPI3D_PLUGIN bool plugin_x64_init(CwAPI3D::ControllerFactory *aFactory)
{
    try {
        auto app =
            std::make_unique<rest_api::app::HttpServer>(rest_api::domain::Address{"127.0.0.1"},
                                                        rest_api::domain::Port{8080});

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

        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
