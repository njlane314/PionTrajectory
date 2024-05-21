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

#include "TTree.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include "Interface/Include/ConfigManager.h"
#include "Interface/Include/DataHandler.h"

namespace ubpiontraj 
{
   class PionFinalStateAnalyser;
}

class ubpiontraj::PionFinalStateAnalyser : public art::EDAnalyzer 
{
   public:

      explicit PionFinalStateAnalyser(fhicl::ParameterSet const& p);

      PionFinalStateAnalyser(PionFinalStateAnalyser const&) = delete;
      PionFinalStateAnalyser(PionFinalStateAnalyser&&) = delete;
      PionFinalStateAnalyser& operator=(PionFinalStateAnalyser const&) = delete;
      PionFinalStateAnalyser& operator=(PionFinalStateAnalyser&&) = delete;

      void analyze(art::Event const& e) override;

      void beginJob() override;
      void endJob() override;

      void FinishEvent();

      void beginSubRun(const art::SubRun& sr);
      void endSubRun(const art::SubRun& sr);

   private:
      ConfigManager& m_config;
      //DataHandler m_dataHandler; 
};