#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <vector>
#include <string>
#include <TLorentzVector.h>

namespace ubpiontraj
{
   class Trajectory {
   public:
      void AddPoint(const TLorentzVector& position, const TLorentzVector& momentum, const std::string& process);
      void SetProcess(size_t index, const std::string& process); 
      const std::vector<TLorentzVector>& GetPositions() const;
      const std::vector<TLorentzVector>& GetMomenta() const;
      const std::vector<double>& GetEnergies() const;
      const std::vector<std::string>& GetProcesses() const;

   private:
      std::vector<TLorentzVector> positions;
      std::vector<TLorentzVector> momenta;
      std::vector<double> energies;
      std::vector<std::string> processes;
   };

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
}

#endif // TRAJECTORY_H