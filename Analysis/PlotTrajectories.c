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

    TTree* tree = (TTree*)dir->Get("TrajTree");
    if (!tree) {
        std::cerr << "Error: Could not find TTree 'TrajTree' in directory 'ana' in file " << filename << std::endl;
        file->Close();
        delete file;
        return;
    }

    int traj_n;
    tree->SetBranchAddress("traj_n", &traj_n);

    std::vector<double> *traj_x = nullptr, *traj_y = nullptr, *traj_z = nullptr;
    tree->SetBranchAddress("traj_x", &traj_x);
    tree->SetBranchAddress("traj_y", &traj_y);
    tree->SetBranchAddress("traj_z", &traj_z);

    double minX = 1e5, maxX = -1e5;
    double minY = 1e5, maxY = -1e5;
    double minZ = 1e5, maxZ = -1e5;

    tree->GetEntry(entryNumber);
    std::cout << traj_n << std::endl;
    std::cout << "traj_x size: " << traj_x->size() << std::endl;
    std::cout << "traj_y size: " << traj_y->size() << std::endl;
    std::cout << "traj_z size: " << traj_z->size() << std::endl;

    if (traj_x->empty() || traj_y->empty() || traj_z->empty()) {
        std::cerr << "Error: traj_x, traj_y, or traj_z vector is empty for the specified entry." << std::endl;
        file->Close();
        delete file;
        return;
    }

    for (double x : *traj_x) {
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
    }

    for (double y : *traj_y) {
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }
    
    for (double z : *traj_z) {
        if (z < minZ) minZ = z;
        if (z > maxZ) maxZ = z;
    }

    if (minX == 1e5 && maxX == -1e5) {
        std::cerr << "Error: minX and maxX not updated. Exiting." << std::endl;
        file->Close();
        delete file;
        return;
    }
    if (minY == 1e5 && maxY == -1e5) {
        std::cerr << "Error: minY and maxY not updated. Exiting." << std::endl;
        file->Close();
        delete file;
        return;
    }
    if (minZ == 1e5 && maxZ == -1e5) {
        std::cerr << "Error: minZ and maxZ not updated. Exiting." << std::endl;
        file->Close();
        delete file;
        return;
    }

    minX -= offset;
    maxX += offset;
    minY -= offset;
    maxY += offset;
    minZ -= offset;
    maxZ += offset;

    int nBins = 100;
    TH2F* histXZ = new TH2F("histXZ", "x vs z; z [cm]; x [cm]", 
                            nBins, minZ, maxZ,  
                            nBins, minX, maxX); 
    TH2F* histXY = new TH2F("histXY", "x vs y; y [cm]; x [cm]", 
                            nBins, minY, maxY,  
                            nBins, minX, maxX); 

    TH3F* histXYZ = new TH3F("histXYZ", "", 
                             nBins, minX, maxX, 
                             nBins, minY, maxY, 
                             nBins, minZ, maxZ);
    
    for (size_t pnt_id = 0; pnt_id < traj_x->size(); pnt_id++) {
        histXZ->Fill((*traj_z)[pnt_id], (*traj_x)[pnt_id]);
        histXY->Fill((*traj_y)[pnt_id], (*traj_x)[pnt_id]);
        histXYZ->Fill((*traj_x)[pnt_id], (*traj_y)[pnt_id], (*traj_z)[pnt_id]);
    }

    TCanvas* c1 = new TCanvas("c1", "", 800, 600);
    histXZ->Draw("colz");
    c1->SaveAs("Plots/plot_x_vs_z.png");
    
    TCanvas* c2 = new TCanvas("c2", "", 800, 600);
    histXY->Draw("colz");
    c2->SaveAs("Plots/plot_x_vs_y.png");

    TCanvas* c3 = new TCanvas("c3", "", 2000, 2000);
    histXYZ->Draw("BOX2");
    c3->SaveAs("Plots/plot_x_vs_y_vs_z.png");
    
    delete c1;
    delete c2;
    delete c3;

    delete histXY;
    delete histXZ;
    delete histXYZ;

    file->Close();
    delete file;
}