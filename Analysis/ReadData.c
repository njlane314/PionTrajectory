#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TROOT.h>
#include <iostream>
#include <vector>

void ReadData(const char* filename) {
    TFile* file = new TFile(filename);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    TDirectory* dir = (TDirectory*)file->Get("ana");
    if (!dir) {
        std::cerr << "Error: Could not find directory 'ana' in file " << filename << std::endl;
        file->Close();
        delete file;
        return;
    }

    TTree* trajTree = (TTree*)dir->Get("TrajTree");
    if (!trajTree) {
        std::cerr << "Error: Could not find TTree 'TrajTree' in directory 'ana' in file " << filename << std::endl;
        file->Close();
        delete file;
        return;
    }

    // Variables to hold the data from the branches
    int traj_n;
    std::vector<double>* traj_x = nullptr;
    std::vector<double>* traj_y = nullptr;
    std::vector<double>* traj_z = nullptr;
    std::vector<double>* traj_px = nullptr;
    std::vector<double>* traj_py = nullptr;
    std::vector<double>* traj_pz = nullptr;
    std::vector<double>* traj_e = nullptr;

    // Set the branch addresses
    trajTree->SetBranchAddress("traj_n", &traj_n);
    trajTree->SetBranchAddress("traj_x", &traj_x);
    trajTree->SetBranchAddress("traj_y", &traj_y);
    trajTree->SetBranchAddress("traj_z", &traj_z);
    trajTree->SetBranchAddress("traj_px", &traj_px);
    trajTree->SetBranchAddress("traj_py", &traj_py);
    trajTree->SetBranchAddress("traj_pz", &traj_pz);
    trajTree->SetBranchAddress("traj_e", &traj_e);

    // Loop over the entries in TrajTree
    Long64_t nEntriesTraj = trajTree->GetEntries();
    for (Long64_t i = 0; i < nEntriesTraj; ++i) {
        trajTree->GetEntry(i);
        std::cout << "Entry " << i << ": traj_n = " << traj_n << std::endl;
        for (int j = 0; j < traj_n; ++j) {
            std::cout << "  Point " << j << ": "
                      << "x = " << traj_x->at(j) << ", "
                      << "y = " << traj_y->at(j) << ", "
                      << "z = " << traj_z->at(j) << ", "
                      << "px = " << traj_px->at(j) << ", "
                      << "py = " << traj_py->at(j) << ", "
                      << "pz = " << traj_pz->at(j) << ", "
                      << "e = " << traj_e->at(j) << std::endl;
        }
    }

    // Clean up
    file->Close();
    delete file;
}
