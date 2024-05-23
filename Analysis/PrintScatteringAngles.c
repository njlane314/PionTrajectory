#include "TFile.h"
#include "TTree.h"
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"
#include <vector>
#include <cmath>
#include <iostream>

void SegmentTrajectory(const std::vector<double>& traj_x, const std::vector<double>& traj_y, const std::vector<double>& traj_z,
                       std::vector<size_t>& breakpoints, std::vector<double>& segmentLengths, double segLen) {
    size_t n = traj_x.size();
    breakpoints.push_back(0);
    double currentLength = 0.0;

    for (size_t i = 1; i < n; ++i) {
        double dx = traj_x[i] - traj_x[i - 1];
        double dy = traj_y[i] - traj_y[i - 1];
        double dz = traj_z[i] - traj_z[i - 1];
        double length = std::sqrt(dx * dx + dy * dy + dz * dz);
        currentLength += length;

        if (currentLength >= segLen) {
            breakpoints.push_back(i);
            segmentLengths.push_back(currentLength);
            currentLength = 0.0;
        }
    }

    if (currentLength > 0) {
        breakpoints.push_back(n - 1);
        segmentLengths.push_back(currentLength);
    }
}

std::vector<double> ComputeSegmentDirection(const std::vector<double>& traj_x, const std::vector<double>& traj_y, const std::vector<double>& traj_z,
                                            size_t start, size_t end) {
    double dx = traj_x[end] - traj_x[start];
    double dy = traj_y[end] - traj_y[start];
    double dz = traj_z[end] - traj_z[start];
    double length = std::sqrt(dx * dx + dy * dy + dz * dz);

    return {dx / length, dy / length, dz / length};
}

double ComputeScatteringAngle(const std::vector<double>& dir1, const std::vector<double>& dir2) {
    double dotProduct = dir1[0] * dir2[0] + dir1[1] * dir2[1] + dir1[2] * dir2[2];

    double magnitude1 = std::sqrt(dir1[0] * dir1[0] + dir1[1] * dir1[1] + dir1[2] * dir1[2]);
    double magnitude2 = std::sqrt(dir2[0] * dir2[0] + dir2[1] * dir2[1] + dir2[2] * dir2[2]);

    double cosTheta = dotProduct / (magnitude1 * magnitude2);

    return std::acos(cosTheta) * (180.0 / M_PI);
}

double ComputeXZAngle(const std::vector<double>& dir1, const std::vector<double>& dir2) {
    double dotProduct = dir1[0] * dir2[0] + dir1[2] * dir2[2];

    double magnitude1 = std::sqrt(dir1[0] * dir1[0] + dir1[2] * dir1[2]);
    double magnitude2 = std::sqrt(dir2[0] * dir2[0] + dir2[2] * dir2[2]);

    double cosTheta = dotProduct / (magnitude1 * magnitude2);

    double angle = std::acos(cosTheta) * 1000;

    double xprod = dir1[0] * dir2[2] - dir1[2] * dir2[0];
    if(xprod < 0){
        angle = -angle;
    }

    return angle;
}

double ComputeYZAngle(const std::vector<double>& dir1, const std::vector<double>& dir2) {
    double dotProduct = dir1[1] * dir2[1] + dir1[2] * dir2[2];

    double magnitude1 = std::sqrt(dir1[1] * dir1[1] + dir1[2] * dir1[2]);
    double magnitude2 = std::sqrt(dir2[1] * dir2[1] + dir2[2] * dir2[2]);

    double cosTheta = dotProduct / (magnitude1 * magnitude2);

    double angle = std::acos(cosTheta) * 1000;//(180.0 / M_PI);

    double xprod = dir1[1] * dir2[2] - dir1[2] * dir2[1];
    if(xprod < 0) {
        angle = -angle;
    }

    return angle;
}


std::vector<double> ComputeScatteringAngles(const std::vector<std::vector<double>>& segmentDirections) {
    std::vector<double> scatteringAngles;

    for (size_t i = 1; i < segmentDirections.size(); ++i) {
        double angle = ComputeScatteringAngle(segmentDirections[i-1], segmentDirections[i]);
        scatteringAngles.push_back(angle);
    }

    return scatteringAngles;
}

std::vector<double> ComputeXZAngles(const std::vector<std::vector<double>>& segmentDirections) {
    std::vector<double> scatteringAngles;

    for (size_t i = 1; i < segmentDirections.size(); ++i) {
        double angle = ComputeXZAngle(segmentDirections[i-1], segmentDirections[i]);
        scatteringAngles.push_back(angle);
    }

    return scatteringAngles;
}

std::vector<double> ComputeYZAngles(const std::vector<std::vector<double>>& segmentDirections) {
    std::vector<double> scatteringAngles;

    for (size_t i = 1; i < segmentDirections.size(); ++i) {
        double angle = ComputeYZAngle(segmentDirections[i-1], segmentDirections[i]);
        scatteringAngles.push_back(angle);
    }

    return scatteringAngles;
}

void PrintScatteringAngles(const char* filename, Long64_t entryNumber = 0) {
    TFile* file = new TFile(filename);

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

    std::vector<std::vector<double>> *traj_x = nullptr, *traj_y = nullptr, *traj_z = nullptr;
    tree->SetBranchAddress("traj_x", &traj_x);
    tree->SetBranchAddress("traj_y", &traj_y);
    tree->SetBranchAddress("traj_z", &traj_z);

    TH1D* hScatteringAngles = new TH1D("hScatteringAngles", "Scattering Angles;Angle (degrees);Counts", 180, 0, 180);
    TH2F* hScat = new TH2F("", ";XZ Angle (mrad);YZ Angle (mrad)", 800, -400, 400, 800, -400, 400); 

    for(int it = 0; it < tree->GetEntries(); it++){
        tree->GetEntry(it);  

        for (size_t traj_id = 0; traj_id < traj_x->size(); ++traj_id) {
            const std::vector<double>& x = (*traj_x)[traj_id];
            const std::vector<double>& y = (*traj_y)[traj_id];
            const std::vector<double>& z = (*traj_z)[traj_id];

            std::vector<size_t> breakpoints;
            std::vector<double> segmentLengths;

            double segmentLength = 5.0;
            SegmentTrajectory(x, y, z, breakpoints, segmentLengths, segmentLength);

            std::vector<std::vector<double>> segmentDirections;
            for (size_t i = 0; i < breakpoints.size() - 1; ++i) {
                std::vector<double> direction = ComputeSegmentDirection(x, y, z, breakpoints[i], breakpoints[i+1]);
                segmentDirections.push_back(direction);
            }

            std::vector<double> scatteringAngles = ComputeScatteringAngles(segmentDirections);
            
            for (double angle : scatteringAngles) {
                hScatteringAngles->Fill(angle);
            }

            std::vector<double> XZScatteringAngles = ComputeXZAngles(segmentDirections);
            std::vector<double> YZScatteringAngles = ComputeYZAngles(segmentDirections);

            for(int i = 0; i < XZScatteringAngles.size(); i++){
                hScat->Fill(XZScatteringAngles[i], YZScatteringAngles[i]);
            }
        }
    }

    TCanvas* c1 = new TCanvas("c1", "", 800, 600);
    hScatteringAngles->Draw("HIST");
    c1->SaveAs("Plots/plot_scattering_angles.png");
    delete c1;
    delete hScatteringAngles;

    TCanvas* c2 = new TCanvas("c2", "", 800, 600);
    hScat->Draw("COLZ"); // Using COLZ to provide a better color map representation
    hScat->SetMarkerStyle(2); // Set the marker style to 'x'
    hScat->SetMarkerColor(kBlue); // Set the marker color to red
    hScat->Draw("P"); // Draw the scatter plot with the specified marker style
    c2->SaveAs("Plots/plot_scattering_plane_angles.png");
    delete c2;
    delete hScat;

    file->Close();
    delete file;
}