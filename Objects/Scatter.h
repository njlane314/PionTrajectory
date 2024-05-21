#ifndef SCATTER_HH
#define SCATTER_HH

#include <vector>
#include "TVector3.h"

#include "nusimdata/SimulationBase/MCParticle.h"

#include "ubana/PionTrajectory/Interface/Include/ParticleTypes.h"

namespace ubpiontraj
{
   class Scatter {
   public:
      Scatter(bool isElastic, bool isInelastic, 
               const art::Ptr<simb::MCParticle>& incidentParticle, 
               const art::Ptr<simb::MCParticle>& scatterParticle,
               const std::vector<art::Ptr<simb::MCParticle>>& products);

      ~Scatter();

      bool isElastic() const;
      bool isInelastic() const;

      art::Ptr<simb::MCParticle> incidentParticle() const;
      art::Ptr<simb::MCParticle> scatterParticle() const;

      std::vector<art::Ptr<simb::MCParticle>> products() const;

      double MomentumFractionLoss() const;
      double CosTheta() const;

   private:
      bool m_isElastic;
      bool m_isInelastic;

      art::Ptr<simb::MCParticle> m_incidentParticle;
      art::Ptr<simb::MCParticle> m_scatterParticle;

      std::vector<art::Ptr<simb::MCParticle>> m_products;
   };

   Scatter::Scatter(bool isElastic, bool isInelastic, 
                  const art::Ptr<simb::MCParticle>& incidentParticle, 
                  const art::Ptr<simb::MCParticle>& scatterParticle,
                  const std::vector<art::Ptr<simb::MCParticle>>& products)
      : m_isElastic(isElastic), m_isInelastic(isInelastic),
         m_incidentParticle(incidentParticle), m_scatterParticle(scatterParticle),
         m_products(products) {}

   Scatter::~Scatter() {}

   bool Scatter::isElastic() const {
      return m_isElastic;
   }

   bool Scatter::isInelastic() const {
      return m_isInelastic;
   }

   art::Ptr<simb::MCParticle> Scatter::incidentParticle() const {
      return m_incidentParticle;
   }

   art::Ptr<simb::MCParticle> Scatter::scatterParticle() const {
      return m_scatterParticle;
   }

   std::vector<art::Ptr<simb::MCParticle>> Scatter::products() const {
      return m_products;
   }

   double Scatter::MomentumFractionLoss() const {
      double initialMagnitude = m_incidentParticle->EndMomentum().Mag();
      double finalMagnitude = m_scatterParticle->EndMomentum().Mag();

      if (initialMagnitude == 0) {
         return 0;
      }

      return (initialMagnitude - finalMagnitude) / initialMagnitude;
   }

   double Scatter::CosTheta() const {
      double dotProduct = m_incidentParticle->EndMomentum().Dot(m_scatterParticle->EndMomentum());

      double initialMagnitude = m_incidentParticle->EndMomentum().Mag();
      double finalMagnitude = m_scatterParticle->EndMomentum().Mag();

      if (initialMagnitude == 0 || finalMagnitude == 0) {
         return 0;
      }
      
      return dotProduct / (initialMagnitude * finalMagnitude);
   }
}

#endif  // SCATTER_HH