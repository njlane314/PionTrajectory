#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <vector>
#include <string>
#include <TLorentzVector.h>

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
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTrajectory.h"

namespace ubpiontraj {
    class Trajectory {
    public:
        void AddPoint(const TLorentzVector& position, const TLorentzVector& momentum, const std::string& process);
        void SetProcess(size_t index, const std::string& process);
        void AddSimParticle(const art::Ptr<simb::MCParticle>& particle);
        const std::vector<TLorentzVector>& GetPositions() const;
        const std::vector<TLorentzVector>& GetMomenta() const;
        const std::vector<double>& GetEnergies() const;
        const std::vector<std::string>& GetProcesses() const;
        const std::vector<art::Ptr<simb::MCParticle>>& GetSimParticles() const;

    private:
        std::vector<TLorentzVector> positions;
        std::vector<TLorentzVector> momenta;
        std::vector<double> energies;
        std::vector<std::string> processes;
        std::vector<art::Ptr<simb::MCParticle>> simParticles;
    };
}

#endif // TRAJECTORY_H
