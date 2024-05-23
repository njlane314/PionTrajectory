#ifndef PIONTRAJECTORYANALYSER_H
#define PIONTRAJECTORYANALYSER_H

#include <vector>
#include <string>

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/FindMany.h"				

#include "larcoreobj/SummaryData/POTSummary.h"
#include "lardataobj/RecoBase/Wire.h"
#include "larsim/EventWeight/Base/MCEventWeight.h"
#include "nusimdata/SimulationBase/MCTrajectory.h"

#include "TTree.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include "Objects/FinalState.h"
#include "Objects/Scatter.h"
#include "Module/Include/PionSimulationAnalyser.h"

namespace ubpiontraj 
{
   class PionTrajectoryAnalyser;
}

class ubpiontraj::PionTrajectoryAnalyser : public art::EDAnalyzer 
{
   public:
      explicit PionTrajectoryAnalyser(fhicl::ParameterSet const& p);

      PionTrajectoryAnalyser(PionTrajectoryAnalyser const&) = delete;
      PionTrajectoryAnalyser(PionTrajectoryAnalyser&&) = delete;
      PionTrajectoryAnalyser& operator=(PionTrajectoryAnalyser const&) = delete;
      PionTrajectoryAnalyser& operator=(PionTrajectoryAnalyser&&) = delete;

      void analyze(art::Event const& e) override;

      void beginJob() override;
      void endJob() override;

      void beginSubRun(const art::SubRun& sr);
      void endSubRun(const art::SubRun& sr);

   private:
      std::string m_SimLabel;
      bool m_Debug;
      
      TTree* m_TrajTree;
      TTree* m_ScatTree;
      TTree* m_FinSttTree;

      std::vector<int> m_traj_n;
      std::vector<std::vector<double>> m_traj_x, m_traj_y, m_traj_z, m_traj_px, m_traj_py, m_traj_pz, m_traj_e;
      std::vector<double> m_scat_elas, m_scat_inelas, m_scat_pfls, m_scat_thta; 
      int m_fstt_nprd, m_fstt_typ;
      std::vector<int> m_fstt_prdpdg;
};

#endif // PIONTRAJECTORYANALYSER_H