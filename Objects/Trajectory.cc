#include "Include/Trajectory.h"

namespace ubpiontraj 
{
    void Trajectory::AddPoint(const TLorentzVector& position, const TLorentzVector& momentum, const std::string& process) {
        positions.push_back(position);
        momenta.push_back(momentum);
        energies.push_back(momentum.E());
        processes.push_back(process);
    }

    void Trajectory::SetProcess(size_t index, const std::string& process) {
        if (index < processes.size()) {
            processes[index] = process;
        }
    }

    void Trajectory::AddSimParticle(const art::Ptr<simb::MCParticle>& particle) {
        simParticles.push_back(particle);
    }

    const std::vector<TLorentzVector>& Trajectory::GetPositions() const {
        return positions;
    }

    const std::vector<TLorentzVector>& Trajectory::GetMomenta() const {
        return momenta;
    }

    const std::vector<double>& Trajectory::GetEnergies() const {
        return energies;
    }

    const std::vector<std::string>& Trajectory::GetProcesses() const {
        return processes;
    }

    const std::vector<art::Ptr<simb::MCParticle>>& Trajectory::GetSimParticles() const {
        return simParticles;
    }

}
