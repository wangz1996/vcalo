#include "IAlgorithm.hh"

IAlgorithm::IAlgorithm(std::string name,Acts::Logging::Level level)
    : m_name(std::move(name)),
    m_logger(Acts::getDefaultLogger(m_name,level)){}

std::string IAlgorithm::name() const {
    return m_name;
}