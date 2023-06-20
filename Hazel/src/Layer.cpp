#include "hzpch.h"
#include "Layer.h"

namespace Hazel {
    Layer::Layer(const std::string& debugname)
        : m_DebugName(debugname) {}

    Layer::~Layer() {}
}