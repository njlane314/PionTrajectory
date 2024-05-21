#include "Include/DataHandler.h"
//_________________________________________________________________________________________
namespace ubpiontraj {
//_________________________________________________________________________________________
DataHandler* DataHandler::m_Instance = nullptr;
//_________________________________________________________________________________________
DataHandler* DataHandler::GetInstance() 
{
    if(m_Instance == nullptr){
        m_Instance = new DataHandler();
    }

    return m_Instance;
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

    m_FinSttTree = new TTree("FinalStateTree", "Final State Tree");
    m_FinSttTree->Branch("fstt_typ", &m_fstt_typ);
    m_FinSttTree->Branch("fstt_nprd", &m_fstt_nprd);
    m_FinSttTree->Branch("fstt_prdpdg", &m_fstt_prdpdg);
}
//_________________________________________________________________________________________
DataHandler::~DataHandler() 
{
    if (m_RootFile){
        m_RootFile->Close();
        delete m_RootFile;
    }
}
//_________________________________________________________________________________________
void DataHandler::AddTrajectory(const art::Ptr<simb::MCParticle> part) 
{    
    const simb::MCTrajectory traj = part->Trajectory();
    
    std::vector<double> traj_x, traj_y, traj_z, traj_px, traj_py, traj_pz, traj_e;
    int traj_n = traj.size(); 

    for(int i = 0; i < traj_n; i++){
        traj_x.push_back(traj.X(i));
        traj_y.push_back(traj.Y(i));
        traj_z.push_back(traj.Z(i));
        traj_px.push_back(traj.Px(i));
        traj_py.push_back(traj.Py(i));
        traj_pz.push_back(traj.Pz(i));
        traj_e.push_back(traj.E(i));
    }

    m_traj_n.push_back(traj_n);
    m_traj_x.push_back(traj_x);
    m_traj_y.push_back(traj_y);
    m_traj_z.push_back(traj_z);
    m_traj_px.push_back(traj_px);
    m_traj_py.push_back(traj_py);
    m_traj_pz.push_back(traj_pz);
    m_traj_e.push_back(traj_e);
}
//_________________________________________________________________________________________
void DataHandler::AddScatter(const Scatter& scat)
{
    m_scat_elas.push_back(scat.isElastic());
    m_scat_inelas.push_back(scat.isInelastic());

    m_scat_pfls.push_back(scat.MomentumFractionLoss());
    m_scat_thta.push_back(scat.CosTheta());
}
//_________________________________________________________________________________________
int FinalStateTypeKey(FinalState::Type type) {
    switch (type) {
        case FinalState::DecayToMuon:
            return 1;
        case FinalState::InelasticAbsorption:
            return 2;
        case FinalState::NeutralPionChargeExchange:
            return 3;
        case FinalState::Other: 
            return 0;
        case FinalState::None:
        default:
            return -1;
    }
}
//_________________________________________________________________________________________
void DataHandler::AddFinalState(const FinalState& fstt)
{
    int type = FinalStateTypeKey(fstt.getType());
    m_fstt_typ.push_back(type);
    m_fstt_nprd.push_back(fstt.getProducts().size());

    std::vector<int> prod_pdgs;
    for (const auto& product : fstt.getProducts()) {
        prod_pdgs.push_back(product->PdgCode());
    }
    m_fstt_prdpdg.push_back(prod_pdgs);
}
//_________________________________________________________________________________________
void DataHandler::AddEntry() 
{
    m_TrajTree->Fill();
    m_ScatTree->Fill();
    m_FinSttTree->Fill();
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

    m_fstt_typ.clear();
    m_fstt_nprd.clear();
    m_fstt_prdpdg.clear();
}
//_________________________________________________________________________________________
}