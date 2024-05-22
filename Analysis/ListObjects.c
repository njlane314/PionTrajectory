#include <TFile.h>
#include <TKey.h>
#include <TClass.h>
#include <iostream>

void ListObjects(const char* filename) {
    TFile* file = TFile::Open(filename);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::cout << "Listing all objects in file: " << filename << std::endl;

    TIter next(file->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)next())) {
        std::cout << "Found object: " << key->GetName() << ", Type: " << key->GetClassName() << std::endl;
    }

    file->Close();
    delete file;
}
