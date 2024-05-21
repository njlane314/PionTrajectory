#include "Include/ConfigManager.h"
//_________________________________________________________________________________________
namespace ubpiontraj {
//_________________________________________________________________________________________
ConfigManager* ConfigManager::m_Instance = nullptr;
//_________________________________________________________________________________________
ConfigManager* ConfigManager::GetInstance(fhicl::ParameterSet const& p) 
{
    if (m_Instance == nullptr) {
        m_Instance = new ConfigManager(p);
    }

    return m_Instance;
}
//_________________________________________________________________________________________
ConfigManager* ConfigManager::GetInstance() 
{
    if (m_Instance == nullptr) {
        m_Instance = new ConfigManager();
    }

    return m_Instance;
}
//_________________________________________________________________________________________
ConfigManager::ConfigManager(fhicl::ParameterSet const& p) 
{
    m_debug = p.get<bool>("debug", false);
    m_simulation_label = p.get<std::string>("sim_label", "largeant");
}
//_________________________________________________________________________________________
ConfigManager::ConfigManager()
{
    m_debug = false;
    m_simulation_label = "largeant";
}
//_________________________________________________________________________________________
}