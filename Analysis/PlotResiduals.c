#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
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
    double kappa = 0.1049 / (p * p) + 11.0038;
    return std::sqrt(std::pow((kappa / (p * beta)) * std::sqrt(segmentLength / X0) * (1 + 0.0038 * std::log(segmentLength / X0)), 2) + std::pow(3.0, 2));
}

double NegativeLogLikelihood(const std::vector<double>& xzAngles, const std::vector<double>& yzAngles, const std::vector<double>& segmentLengths, double p) {
    double nll = 0.0;
    for (size_t i = 0; i < xzAngles.size(); ++i) {
        double sigma = ModifiedHighland(p, segmentLengths[i]);
        double deltaThetaXZ = xzAngles[i];
        nll += std::log(sigma) + (deltaThetaXZ * deltaThetaXZ) / (2 * sigma * sigma);
    }
    for (size_t i = 0; i < yzAngles.size(); ++i) {
        double sigma = ModifiedHighland(p, segmentLengths[i]);
        double deltaThetaYZ = yzAngles[i];
        nll += std::log(sigma) + (deltaThetaYZ * deltaThetaYZ) / (2 * sigma * sigma);
    }
    return nll;
}

void PlotResiduals(const char* filename) {
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

    std::vector<double> *traj_x = nullptr, *traj_y = nullptr, *traj_z = nullptr;
    tree->SetBranchAddress("traj_x", &traj_x);
    tree->SetBranchAddress("traj_y", &traj_y);
    tree->SetBranchAddress("traj_z", &traj_z);

    int nEntries = tree->GetEntries();

    double trueMomentum = 1.0; 

    std::vector<double> residuals;

    for (int entry = 0; entry < nEntries; ++entry) {
        tree->GetEntry(entry);

        double minimisedP = -1;

        std::vector<double> segmentLengthsArray = {0.3, 1, 2, 5, 10};
        std::vector<std::vector<double>> allMomenta(segmentLengthsArray.size());
        std::vector<std::vector<double>> allNLLValues(segmentLengthsArray.size());
        std::vector<double> globalNLLValues(100, 0.0);

        for (size_t segLenIdx = 0; segLenIdx < segmentLengthsArray.size(); ++segLenIdx) {
            double segmentLength = segmentLengthsArray[segLenIdx];

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

                std::vector<double>& momenta = allMomenta[segLenIdx];
                std::vector<double>& nllValues = allNLLValues[segLenIdx];

                double bestP = 0.0;
                double minNLL = std::numeric_limits<double>::max();
                for (double p = 0.01; p <= 2; p += 0.01) {
                    double nll = NegativeLogLikelihood(XZScatteringAngles, YZScatteringAngles, segmentLengths, p);
                    momenta.push_back(p);
                    nllValues.push_back(nll);
                    if (nll < minNLL) {
                        minNLL = nll;
                        bestP = p;
                    }
                }

                minimisedP = bestP;
            }

            for (size_t i = 0; i < allNLLValues[segLenIdx].size() && i < globalNLLValues.size(); ++i) {
                globalNLLValues[i] += allNLLValues[segLenIdx][i];
            }
        

        double residual = (minimisedP - trueMomentum) / trueMomentum;
        residuals.push_back(residual);
    }

    TCanvas* c2 = new TCanvas("c2", "Residuals", 800, 600);
    TH1D* hist = new TH1D("residuals", ";Residual;Entries", 10, -1, 1);
    for (double residual : residuals) {
        hist->Fill(residual);
    }

    hist->Draw();
    c2->SaveAs("Plots/residuals.png");

    delete c2;
    delete hist;

    file->Close();
    delete file;
}