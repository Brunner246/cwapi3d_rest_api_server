//
// Created by MichaelBrunner on 12/12/2024.
//
module;

#include <cwapi3d/CwAPI3D.h>

module adapters.HttpHandlers;

import std;

rest_api::adapters::Cadwork3DModelController::Cadwork3DModelController(CwAPI3D::ControllerFactory *factory)
: factory(factory)
{
    if (nullptr == this->factory) {
        throw std::invalid_argument("ControllerFactory is nullptr");
    }
}

std::vector<CwAPI3D::elementID> rest_api::adapters::Cadwork3DModelController::getAllElementIds() const
{
    const auto elementController = factory->getElementController();
    const auto elementList = elementController->getActiveIdentifiableElementIDs();

    std::vector<CwAPI3D::elementID> elementIds;
    for (auto il{0u}; il < elementList->count(); ++il) {
        elementIds.push_back(elementList->at(il));
    }

    return elementIds;
}
