#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TLine.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

const double PI = 3.14159265358979323846;

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

double ComputeXZAngle(const std::vector<double>& dir1, const std::vector<double>& dir2) {
    double dotProduct = dir1[0] * dir2[0] + dir1[2] * dir2[2];

    double magnitude1 = std::sqrt(dir1[0] * dir1[0] + dir1[2] * dir1[2]);
    double magnitude2 = std::sqrt(dir2[0] * dir2[0] + dir2[2] * dir2[2]);

    double cosTheta = dotProduct / (magnitude1 * magnitude2);

    double angle = std::acos(cosTheta) * 1000;

    double xprod = dir1[0] * dir2[2] - dir1[2] * dir2[0];
    if (xprod < 0) {
        angle = -angle;
    }

    return angle;
}

double ComputeYZAngle(const std::vector<double>& dir1, const std::vector<double>& dir2) {
    double dotProduct = dir1[1] * dir2[1] + dir1[2] * dir2[2];

    double magnitude1 = std::sqrt(dir1[1] * dir1[1] + dir1[2] * dir1[2]);
    double magnitude2 = std::sqrt(dir2[1] * dir2[1] + dir2[2] * dir2[2]);

    double cosTheta = dotProduct / (magnitude1 * magnitude2);

    double angle = std::acos(cosTheta) * 1000;

    double xprod = dir1[1] * dir2[2] - dir1[2] * dir2[1];
    if (xprod < 0) {
        angle = -angle;
    }

    return angle;
}

std::vector<double> ComputeXZAngles(const std::vector<std::vector<double>>& segmentDirections) {
    std::vector<double> scatteringAngles;

    for (size_t i = 1; i < segmentDirections.size(); ++i) {
        double angle = ComputeXZAngle(segmentDirections[i - 1], segmentDirections[i]);
        scatteringAngles.push_back(angle);
    }

    return scatteringAngles;
}

std::vector<double> ComputeYZAngles(const std::vector<std::vector<double>>& segmentDirections) {
    std::vector<double> scatteringAngles;

    for (size_t i = 1; i < segmentDirections.size(); ++i) {
        double angle = ComputeYZAngle(segmentDirections[i - 1], segmentDirections[i]);
        scatteringAngles.push_back(angle);
    }

    return scatteringAngles;
}

double ModifiedHighland(double p, double segmentLength) {
    double beta = p / std::sqrt(p * p + 0.1396 * 0.1396); // pion mass ~ 139.6 MeV/c^2
    double X0 = 14.0; // radiation length in cm
    return 13.6 / (p * beta) * std::sqrt(segmentLength / X0) * (1 + 0.0038 * std::log(segmentLength / X0));
}

void PlotAngleDistribution(const char* filename, int entrynumber, double segmentLength, double momentum) {
    TFile* file = new TFile(filename);
    TTree* tree = (TTree*)file->Get("ana/TrajTree");

    std::vector<double> *traj_x = nullptr, *traj_y = nullptr, *traj_z = nullptr;
    tree->SetBranchAddress("traj_x", &traj_x);
    tree->SetBranchAddress("traj_y", &traj_y);
    tree->SetBranchAddress("traj_z", &traj_z);

    tree->GetEntry(entrynumber);

    std::vector<size_t> breakpoints;
    std::vector<double> segmentLengths;
    SegmentTrajectory(*traj_x, *traj_y, *traj_z, breakpoints, segmentLengths, segmentLength);

    std::vector<std::vector<double>> segmentDirections;
    for (size_t i = 0; i < breakpoints.size() - 1; ++i) {
        std::vector<double> direction = ComputeSegmentDirection(*traj_x, *traj_y, *traj_z, breakpoints[i], breakpoints[i + 1]);
        segmentDirections.push_back(direction);
    }

    std::vector<double> XZScatteringAngles = ComputeXZAngles(segmentDirections);
    std::vector<double> YZScatteringAngles = ComputeYZAngles(segmentDirections);

    double sigma = ModifiedHighland(momentum, segmentLength);

    TCanvas* c1 = new TCanvas("c1", "Angle Distributions", 1200, 600);
    c1->Divide(2, 1);

    c1->cd(1);
    TH1D* histXZ = new TH1D("histXZ", "XZ Scattering Angles;Angle (mrad);Entries", 100, -500, 500);
    for (double angle : XZScatteringAngles) {
        histXZ->Fill(angle);
    }
    histXZ->Draw();

    c1->cd(2);
    TH1D* histYZ = new TH1D("histYZ", "YZ Scattering Angles;Angle (mrad);Entries", 100, -500, 500);
    for (double angle : YZScatteringAngles) {
        histYZ->Fill(angle);
    }
    histYZ->Draw();

    TCanvas* c2 = new TCanvas("c2", "Normalized Angle Distributions", 1200, 600);
    c2->Divide(2, 1);

    c2->cd(1);
    TH1D* histXZNorm = new TH1D("histXZNorm", "Normalized XZ Scattering Angles;Normalized Angle;Entries", 100, -5, 5);
    for (double angle : XZScatteringAngles) {
        histXZNorm->Fill(angle / sigma);
    }
    histXZNorm->Draw();

    c2->cd(2);
    TH1D* histYZNorm = new TH1D("histYZNorm", "Normalized YZ Scattering Angles;Normalized Angle;Entries", 100, -5, 5);
    for (double angle : YZScatteringAngles) {
        histYZNorm->Fill(angle / sigma);
    }
    histYZNorm->Draw();

    TCanvas* c3 = new TCanvas("c3", "Normal Distribution Comparison", 1200, 600);
    c3->Divide(2, 1);

    c3->cd(1);
    TH1D* histXZNormComp = (TH1D*)histXZNorm->Clone("histXZNormComp");
    histXZNormComp->SetTitle("Normalized XZ Scattering Angles vs Normal Distribution");
    histXZNormComp->Scale(1.0 / histXZNormComp->Integral());
    histXZNormComp->Draw("HIST");

    TF1* normalDistXZ = new TF1("normalDistXZ", "gaus", -5, 5);
    normalDistXZ->SetParameters(1, 0, 1); // mean 0, sigma 1
    normalDistXZ->SetLineColor(kRed);
    normalDistXZ->Draw("SAME");

    c3->cd(2);
    TH1D* histYZNormComp = (TH1D*)histYZNorm->Clone("histYZNormComp");
    histYZNormComp->SetTitle("Normalized YZ Scattering Angles vs Normal Distribution");
    histYZNormComp->Scale(1.0 / histYZNormComp->Integral());
    histYZNormComp->Draw("HIST");

    TF1* normalDistYZ = new TF1("normalDistYZ", "gaus", -5, 5);
    normalDistYZ->SetParameters(1, 0, 1); // mean 0, sigma 1
    normalDistYZ->SetLineColor(kRed);
    normalDistYZ->Draw("SAME");

    c1->SaveAs("Plots/angle_distributions.png");
    c2->SaveAs("Plots/normalized_angle_distributions.png");
    c3->SaveAs("Plots/normal_distribution_comparison.png");

    delete c1;
    delete c2;
    delete c3;
    delete histXZ;
    delete histYZ;
    delete histXZNorm;
    delete histYZNorm;
    delete histXZNormComp;
    delete histYZNormComp;
    delete normalDistXZ;
    delete normalDistYZ;

    file->Close();
    delete file;
}
