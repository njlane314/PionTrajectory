#include <TFile.h>
#include <TKey.h>
#include <TClass.h>
#include <TDirectory.h>
#include <iostream>

void ListTreesInDirectory(TDirectory* dir) {
    TIter next(dir->GetListOfKeys());
    TKey* key;
    while ((key = (TKey*)next())) {
        TObject* obj = key->ReadObj();
        if (obj->IsA()->InheritsFrom(TDirectory::Class())) {
            std::cout << "Found directory: " << key->GetName() << std::endl;
            ListTreesInDirectory((TDirectory*)obj);
        } else if (obj->IsA()->InheritsFrom(TTree::Class())) {
            std::cout << "Found tree: " << key->GetName() << std::endl;
        }
    }
}

void ListTrees(const char* filename) {
    TFile* file = TFile::Open(filename);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::cout << "Listing all trees in file: " << filename << std::endl;
    ListTreesInDirectory(file);

    file->Close();
    delete file;
}
