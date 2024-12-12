//
// Created by MichaelBrunner on 12/12/2024.
//

module;

#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>
#include <cwapi3d/CwAPI3D.h>

export module adapters.HttpHandlers;
import domain;

namespace rest_api::adapters
{
export class Cadwork3DModelController
{
    CwAPI3D::ControllerFactory *factory{nullptr};

public:
    explicit Cadwork3DModelController(CwAPI3D::ControllerFactory *factory);

    [[nodiscard]] std::vector<CwAPI3D::elementID> getAllElementIds() const;
};

export auto getActiveElementIds = [](const domain::HttpRequest &request_body,
                                     domain::HttpResponse &response_body,
                                     const std::shared_ptr<Cadwork3DModelController> &controller)
{
    std::ignore = request_body;

    const auto elementIds = controller->getAllElementIds();
    boost::json::array jsonElementIds;
    for (const auto &id : elementIds) {
        jsonElementIds.push_back(id);
    }

    const boost::json::object body = {
        {"element_ids", jsonElementIds}
    };
    response_body.headers["Content-Type"] = "application/json";
    response_body.body = boost::json::serialize(body);
    response_body.status_code = 200;

    // if (elementIds.empty()) {
    //     response_body.status_code = 404;
    //     response_body.body = "No elements found";
    //     response_body.headers["Content-Type"] = "text/plain";
    // }
};
}
