#include "Include/DataHandler.hh"
//_________________________________________________________________________________________
namespace ub_pion_trajectory {
//_________________________________________________________________________________________
DataHandler* DataHandler::m_Instance = nullptr;
//_________________________________________________________________________________________
DataHandler* DataHandler::Instance() 
{
    if (m_Instance == nullptr) {
        m_Instance = new DataHandler();
    }

    return m_Instance;
}
//_________________________________________________________________________________________
DataHandler::~DataHandler() 
{
    if (m_RootFile) {
        m_RootFile->Close();
        delete m_RootFile;
    }
}
//_________________________________________________________________________________________
DataHandler::DataHandler(const char* filename)
{
    m_RootFile = new TFile(filename, "RECREATE");

    //m_EventTree = new TTree("EventTree", "Event Tree");
    //m_EventTree->Branch("edep", &edep_);
}
//_________________________________________________________________________________________
void DataHandler::AddPionTrajectory() 
{}
//_________________________________________________________________________________________
void DataHandler::AddScatter()
{}
//_________________________________________________________________________________________
void DataHandler::AddFinalState()
{}
//_________________________________________________________________________________________
void DataHandler::AddEntry() 
{
    //m_EventTree->Fill();
}
void DataHandler::WriteFile() 
{
    std::cout << "-- Writing to file..." << std::endl;
    m_RootFile->Write();
}
//_________________________________________________________________________________________
void DataHandler::Reset()
{
    //edep_.clear();
}
//_________________________________________________________________________________________
}