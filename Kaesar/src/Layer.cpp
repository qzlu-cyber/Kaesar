#include "krpch.h"
#include "Layer.h"

namespace Kaesar {
    Layer::Layer(const std::string& debugname)
        : m_DebugName(debugname) {}

    Layer::~Layer() {}
}