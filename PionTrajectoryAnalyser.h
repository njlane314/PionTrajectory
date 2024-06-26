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

#include "Objects/Include/Trajectory.h"
#include "Module/Include/PionSimulationAnalyser.h"
#include "Module/Include/PionReconstructionAnalyser.h"

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
      std::string m_SimParticleLabel;
      
      std::string m_RecoParticleLabel;
      std::string m_RecoTrackLabel;
      std::string m_RecoHitLabel;
      
      std::string m_RecoTrackHitAssocLabel;
      std::string m_RecoHitSimParticleLabel;

      std::string m_RecoCaloLabel;
      bool m_Debug;
      
      TTree* m_TrajTree;
   
      int m_traj_n;
      std::vector<double> m_traj_x, m_traj_y, m_traj_z, m_traj_px, m_traj_py, m_traj_pz, m_traj_e;
      std::vector<std::string> m_traj_proc;
};

#endif // PIONTRAJECTORYANALYSER_H