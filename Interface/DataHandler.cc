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

    m_TrajTree = new TTree("TrajTree", "Trajectory Tree");
    m_TrajTree->Branch("traj_n", &m_traj_n);
    m_TrajTree->Branch("traj_x", &m_traj_x);
    m_TrajTree->Branch("traj_y", &m_traj_y);
    m_TrajTree->Branch("traj_z", &m_traj_z);
    m_TrajTree->Branch("traj_px", &m_traj_px);
    m_TrajTree->Branch("traj_py", &m_traj_py);
    m_TrajTree->Branch("traj_pz", &m_traj_pz);
    m_TrajTree->Branch("traj_e", &m_traj_e);

    m_ScatTree = new TTree("ScatTree", "Scatter Tree");
    m_ScatTree->Branch("scat_elas", &m_scat_elas);
    m_ScatTree->Branch("scat_inelas", &m_scat_inelas);
    m_ScatTree->Branch("scat_pfls", &m_scat_pfls);
    m_ScatTree->Branch("scat_thta", &m_scat_thta);
}
//_________________________________________________________________________________________
void DataHandler::AddTrajectory(const art::Ptr<simb::MCParticle> part) 
{    
    simb::MCTrajectory* traj = part->Trajectory();
    
    std::vector<double> traj_x, traj_y, traj_z, traj_px, traj_py, traj_pz, traj_e;

    for(int i = 0; i < traj->size(); i++){
        traj_x.push_back(traj->X());
        traj_y.push_back(traj->Y());
        traj_z.push_back(traj->Z());
        traj_px.push_back(traj->Px());
        traj_py.push_back(traj->Py());
        traj_pz.push_back(traj->Pz());
        traj_e.push_back(traj->E());
    }

    m_traj_n.push_back(traj->size());
    m_traj_x.push_back(traj_x);
    m_traj_y.push_back(traj_y);
    m_traj_z.push_back(traj_z);
    m_traj_px.push_back(traj_px);
    m_traj_py.push_back(traj_py);
    m_traj_pz.push_back(traj_pz);
}
//_________________________________________________________________________________________
void DataHandler::AddScatter(const Scatter* scat)
{
    m_scat_elas.push_back(scat->isElastic());
    m_scat_inelas.push_back(scat->isInelastic());

    m_scat_pfls.push_back(scat->MomentumFractionLoss());
    m_scat_thta.push_back(scat->CosTheta());
}
//_________________________________________________________________________________________
void DataHandler::AddEntry() 
{
    m_TrajTree->Fill();
    m_ScatTree->Fill();
}
void DataHandler::WriteFile() 
{
    std::cout << "-- Writing to file..." << std::endl;
    m_RootFile->Write();
}
//_________________________________________________________________________________________
void DataHandler::Reset()
{
    m_traj_n.clear(); 
    m_traj_x.clear();
    m_traj_y.clear();
    m_traj_z.clear();
    m_traj_px.clear();
    m_traj_py.clear();
    m_traj_pz.clear();

    m_scat_elas.clear();
    m_scat_inelas.clear();
    m_scat_pfls.clear();
    m_scat_thta.clear();
}
//_________________________________________________________________________________________
}