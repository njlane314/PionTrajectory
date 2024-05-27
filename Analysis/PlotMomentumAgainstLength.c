//root -l -b -q 'PlotMomentumAgainstLength.c("Output/pitraj_unif_ana.root")'

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TLegend.h"
#include <vector>
#include <cmath>
#include <iostream>

double CalculateTrackLength(const std::vector<double>& traj_x, const std::vector<double>& traj_y, const std::vector<double>& traj_z) {
    double totalLength = 0.0;
    size_t n = traj_x.size();

    for (size_t i = 1; i < n; ++i) {
        double dx = traj_x[i] - traj_x[i - 1];
        double dy = traj_y[i] - traj_y[i - 1];
        double dz = traj_z[i] - traj_z[i - 1];
        totalLength += std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    return totalLength;
}

void PlotMomentumAgainstLength(const char* filename) {
    TFile* file = new TFile(filename);
    TTree* tree = (TTree*)file->Get("ana/TrajTree");

    std::vector<double>* traj_x = nullptr;
    std::vector<double>* traj_y = nullptr;
    std::vector<double>* traj_z = nullptr;
    std::vector<double>* traj_px = nullptr;
    std::vector<double>* traj_py = nullptr;
    std::vector<double>* traj_pz = nullptr;

    tree->SetBranchAddress("traj_x", &traj_x);
    tree->SetBranchAddress("traj_y", &traj_y);
    tree->SetBranchAddress("traj_z", &traj_z);
    tree->SetBranchAddress("traj_px", &traj_px);
    tree->SetBranchAddress("traj_py", &traj_py);
    tree->SetBranchAddress("traj_pz", &traj_pz);

    std::vector<double> pionMomenta;
    std::vector<double> trackLengths;

    int nEntries = tree->GetEntries();

    for (int entry = 0; entry < nEntries; ++entry) {
        tree->GetEntry(entry);

        double trackLength = CalculateTrackLength(*traj_x, *traj_y, *traj_z);
        trackLengths.push_back(trackLength);
        
        double p0 = std::sqrt(traj_px->at(0) * traj_px->at(0) + traj_py->at(0) * traj_py->at(0) + traj_pz->at(0) * traj_pz->at(0));
        pionMomenta.push_back(p0);
    }

    TCanvas* c1 = new TCanvas("c1", "Momentum vs Track Length", 800, 600);
    TH2D* hist = new TH2D("", ";Initial Pion Momentum (MeV/c);True Trajectory Length (cm)",
                          40, 0.1, 0.5,
                          40, 0, 200);

    for (size_t i = 0; i < pionMomenta.size(); ++i) {
        hist->Fill(pionMomenta[i], trackLengths[i]);
    }

    hist->Draw("COLZ");

    c1->SaveAs("Plots/momentum_vs_length.png");

    delete hist;
    delete c1;
    file->Close();
    delete file;
}