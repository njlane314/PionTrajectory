#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "art/Framework/Principal/Event.h"

#include "nusimdata/SimulationBase/MCParticle.h"

#include "ubana/PionTrajectory/Objects/Scatter.h"
#include "ubana/PionTrajectory/Objects/FinalState.h"

namespace ubpiontraj
{
   class DataHandler 
   {
   public:
      static DataHandler* Instance();
      ~DataHandler();

      DataHandler(const DataHandler&) = delete;
      DataHandler& operator=(const DataHandler&) = delete;

      void AddEntry();
      void WriteFile();
      void Reset();

   private:
      DataHandler(const char* filename = "Data/output.root");

      static DataHandler* m_Instance;

      TFile* m_RootFile;

      TTree* m_TrajTree;
      TTree* m_ScatTree;

      std::vector<std::vector<double>> traj_n, traj_x, traj_y, traj_z, traj_px, traj_py, traj_pz, traj_e;
      std::vector<double> m_scat_elas, m_scat_inelas, m_scat_pfls, m_scat_thta; 
   };
}

#endif  // DATAHANDLER_H