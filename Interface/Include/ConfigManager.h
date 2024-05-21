#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "fhiclcpp/ParameterSet.h"

namespace ubpiontraj {

class ConfigManager {
public:
    static ConfigManager* GetInstance();
    static ConfigManager* GetInstance(fhicl::ParameterSet const& p);

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    bool isDebugEnabled() const { return m_debug; }
    std::string getSimulationLabel() const { return m_simulation_label; }

private:
    ConfigManager();
    ConfigManager(fhicl::ParameterSet const& p);

    static ConfigManager* m_Instance;

    bool m_debug = false;
    std::string m_simulation_label = "largeant";
};

}

#endif // CONFIGMANAGER_H