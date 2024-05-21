// PlotTrajectories.c
// To run:
// $ root -l -b -q 'PlotTrajectories.c("your_output_file.root", entrynumber, offset)'

#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TCanvas.h>
#include <TMath.h>
#include <vector>

void PlotTrajectories(const char* filename, Long64_t entryNumber = 0, double offset = 50) {
    TFile* file = new TFile(filename);
    TTree* tree = (TTree*)file->Get("TrajTree");  

    std::vector<std::vector<double>> *traj_x = nullptr, *traj_y = nullptr, *traj_z = nullptr;
    tree->SetBranchAddress("traj_x", &traj_x);
    tree->SetBranchAddress("traj_y", &traj_y);
    tree->SetBranchAddress("traj_z", &traj_z);

    double minX = 1e5, maxX = -1e5;
    double minY = 1e5, maxY = -1e5;
    double minZ = 1e5, maxZ = -1e5;

    // Get the specific entry
    tree->GetEntry(entryNumber);
    for (const auto& traj : *traj_x) {
        for (double x : traj) {
            if (x < minX) minX = x;
            if (x > maxX) maxX = x;
        }
    }
    for (const auto& traj : *traj_y) {
        for (double y : traj) {
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
        }
    }
    for (const auto& traj : *traj_z) {
        for (double z : traj) {
            if (z < minZ) minZ = z;
            if (z > maxZ) maxZ = z;
        }
    }

    minX -= offset;
    maxX += offset;
    minY -= offset;
    maxY += offset;
    minZ -= offset;
    maxZ += offset;

    int nBins = 500;
    TH2F* histXZ = new TH2F("histXZ", "x vs z; z [mm]; x [mm]", 
                            nBins, minZ, maxZ,  
                            nBins, minX, maxX); 
    TH2F* histXY = new TH2F("histXY", "x vs y; y [mm]; x [mm]", 
                            nBins, minY, maxY,  
                            nBins, minX, maxX); 

    TH3F* histXYZ = new TH3F("histXYZ", "", 
                             nBins, minX, maxX, 
                             nBins, minY, maxY, 
                             nBins, minZ, maxZ);
    
    for (size_t traj_id = 0; traj_id < traj_x->size(); traj_id++) {
        for (size_t pnt_id = 0; pnt_id < (*traj_x)[traj_id].size(); pnt_id++) {
            histXZ->Fill((*traj_z)[traj_id][pnt_id], (*traj_x)[traj_id][pnt_id]);
            histXY->Fill((*traj_y)[traj_id][pnt_id], (*traj_x)[traj_id][pnt_id]);
            histXYZ->Fill((*traj_x)[traj_id][pnt_id], (*traj_y)[traj_id][pnt_id], (*traj_z)[traj_id][pnt_id]);
        }
    }

    TCanvas* c1 = new TCanvas("c1", "", 800, 600);
    histXZ->Draw("colz");
    c1->SaveAs("plots/plot_x_vs_z.png");
    
    TCanvas* c2 = new TCanvas("c2", "", 800, 600);
    histXY->Draw("colz");
    c2->SaveAs("plots/plot_x_vs_y.png");

    TCanvas* c3 = new TCanvas("c3", "", 1000, 1000);
    histXYZ->Draw("BOX2");
    c3->SaveAs("plots/plot_x_vs_y_vs_z.png");
    
    delete c1;
    delete c2;
    delete c3;

    delete histXY;
    delete histXZ;
    delete histXYZ;

    file->Close();
    delete file;
}