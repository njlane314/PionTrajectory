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

    std::vector<double> *traj_x = nullptr, *traj_y = nullptr, *traj_z = nullptr, *traj_e = nullptr;
    tree->SetBranchAddress("traj_x", &traj_x);
    tree->SetBranchAddress("traj_y", &traj_y);
    tree->SetBranchAddress("traj_z", &traj_z);
    tree->SetBranchAddress("traj_e", &traj_e);

    double minX = 1e5, maxX = -1e5;
    double minY = 1e5, maxY = -1e5;
    double minZ = 1e5, maxZ = -1e5;

    int nEntries = tree->GetEntries();

    int nBins = 100;

    TH3F* histXYZ = new TH3F("histXYZ", "", 
                                256, 0, 256, 
                                249, -116.5, 116.5, 
                                1037, 0, 1037);

    TH2F* histXZ = new TH2F("histXZ", "; z [cm]; x [cm]", 
                                1037, 0, 1037,  
                                256, 0, 256); 
    TH2F* histXY = new TH2F("histXY", "; y [cm]; x [cm]", 
                                249, -116.5, 116.5,  
                                256, 0, 256); 

    TH2F* histYZ = new TH2F("histYZ", "; z [cm]; y [cm]", 
                                1037, 0, 1037,
                                249, -116.5, 116.5); 

    histYZ->SetStats(0);
    histXZ->SetStats(0);
    histXY->SetStats(0);

    for (int entry = 0; entry < nEntries; ++entry) {
        tree->GetEntry(entry);


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

         for (size_t pnt_id = 0; pnt_id < traj_x->size(); pnt_id++) {
            double x = (*traj_x)[pnt_id];
            double y = (*traj_y)[pnt_id];
            double z = (*traj_z)[pnt_id];
            double e = (*traj_e)[pnt_id];

            histXZ->Fill(z, x, e);
            histXY->Fill(y, x, e);
            histYZ->Fill(z, y, e);
            histXYZ->Fill(x, y, z, e);
        }
    }

    TCanvas* c1 = new TCanvas("c1", "", 1037*10, 256*10);
    histXZ->Draw("col");
    c1->SaveAs("Plots/plot_x_vs_z.png");
    
    TCanvas* c2 = new TCanvas("c2", "", 249*10, 256*10);
    histXY->Draw("col");
    c2->SaveAs("Plots/plot_x_vs_y.png");

    TCanvas* c3 = new TCanvas("c3", "", 1000, 1000);
    histXYZ->Draw("BOX2");
    c3->SaveAs("Plots/plot_x_vs_y_vs_z.png");

    TCanvas* c4 = new TCanvas("c4", "", 1037*10, 249*10);
    histYZ->Draw("col");
    c4->SaveAs("Plots/plot_z_vs_y.png");
    
    delete c1;
    delete c2;
    delete c3;
    delete c4;

    delete histXY;
    delete histXZ;
    delete histXYZ;
    delete histYZ;

    file->Close();
    delete file;
}
