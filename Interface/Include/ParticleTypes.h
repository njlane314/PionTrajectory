#ifndef PARTICLETYPES_H
#define PARTICLETYPES_H

namespace ubpiontraj
{
   inline bool isChargedPion(int pdg){ return abs(pdg) == 211; }
   inline bool isNeutralPion(int pdg){ return abs(pdg) == 111; }
   inline bool isMuonLepton(int pdg){ return abs(pdg) == 13; }
   inline bool isMuonNeutrino(int pdg){ return abs(pdg) == 14; }
}

#endif  // PARTICLETYPES_H