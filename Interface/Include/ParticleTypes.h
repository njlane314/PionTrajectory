#ifndef PARTICLETYPES_H
#define PARTICLETYPES_H

namespace ubpiontraj
{
   inline bool isPion(int pdg){ return abs(pdg) == 111 || abs(pdg) == 211; }
}

#endif  // PARTICLETYPES_H