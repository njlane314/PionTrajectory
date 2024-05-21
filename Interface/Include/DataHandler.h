#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "art/Framework/Principal/Event.h"

#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTrajectory.h"

#include "ubana/PionTrajectory/Objects/Scatter.h"
#include "ubana/PionTrajectory/Objects/FinalState.h"

namespace ubpiontraj
{
   class DataHandler 
   {
   public:
      static DataHandler* GetInstance();
      ~DataHandler();

      DataHandler(const DataHandler&) = delete;
      DataHandler& operator=(const DataHandler&) = delete;

      void AddTrajectory(const art::Ptr<simb::MCParticle> part);
      void AddScatter(const Scatter& scat);
      
      void AddEntry();
      void WriteFile();
      void Reset();

   private:
      DataHandler(const char* filename = "Data/output.root");

      static DataHandler* m_Instance;

      TFile* m_RootFile;

      TTree* m_TrajTree;
      TTree* m_ScatTree;

      std::vector<int> m_traj_n;
      std::vector<std::vector<double>> m_traj_x, m_traj_y, m_traj_z, m_traj_px, m_traj_py, m_traj_pz, m_traj_e;
      std::vector<double> m_scat_elas, m_scat_inelas, m_scat_pfls, m_scat_thta; 
   };
}

#endif // DATAHANDLER_H