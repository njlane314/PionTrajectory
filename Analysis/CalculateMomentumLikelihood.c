#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraph.h"
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

double ModifiedHighland(double p, double segmentLength) {
    double beta = p / std::sqrt(p * p + 0.1396 * 0.1396); // muon mass ~ 139.6 MeV/c^2
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

void CalculateMomentumLikelihood(const char* filename, int entrynumber) {
    TFile* file = new TFile(filename);
    TTree* tree = (TTree*)file->Get("ana/TrajTree");

    std::vector<std::vector<double>> *traj_x = nullptr, *traj_y = nullptr, *traj_z = nullptr;
    std::vector<std::vector<double>> *traj_p = nullptr, *traj_e = nullptr;
    tree->SetBranchAddress("traj_x", &traj_x);
    tree->SetBranchAddress("traj_y", &traj_y);
    tree->SetBranchAddress("traj_z", &traj_z);
    tree->SetBranchAddress("traj_e", &traj_e);

    tree->GetEntry(entrynumber);  

    double minimisedP = -1;

    std::vector<double> segmentLengthsArray = {0.3, 1, 2, 5, 10}; 
    std::vector<std::vector<double>> allMomenta(segmentLengthsArray.size());
    std::vector<std::vector<double>> allNLLValues(segmentLengthsArray.size());
    std::vector<double> globalNLLValues(100, 0.0); 

    for (size_t segLenIdx = 0; segLenIdx < segmentLengthsArray.size(); ++segLenIdx) {
        double segmentLength = segmentLengthsArray[segLenIdx];

        for (size_t traj_id = 0; traj_id < traj_x->size(); ++traj_id) {
            const std::vector<double>& x = (*traj_x)[traj_id];
            const std::vector<double>& y = (*traj_y)[traj_id];
            const std::vector<double>& z = (*traj_z)[traj_id];
            const std::vector<double>& e = (*traj_e)[traj_id];

            std::vector<size_t> breakpoints;
            std::vector<double> segmentLengths;
            SegmentTrajectory(x, y, z, breakpoints, segmentLengths, segmentLength);

            std::vector<std::vector<double>> segmentDirections;
            for (size_t i = 0; i < breakpoints.size() - 1; ++i) {
                std::vector<double> direction = ComputeSegmentDirection(x, y, z, breakpoints[i], breakpoints[i + 1]);
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
    }


    TCanvas* c1 = new TCanvas("c1", "Negative Log Likelihood vs Momentum", 800, 600);
    TMultiGraph* mg = new TMultiGraph();
    TLegend* legend = new TLegend(0.1,0.92,0.9,0.99);
    legend->SetNColumns(segmentLengthsArray.size()/2);
    legend->SetBorderSize(0);

    int colors[] = {kRed, kBlue, kGreen, kMagenta, kCyan};

    for (size_t segLenIdx = 0; segLenIdx < segmentLengthsArray.size(); ++segLenIdx) {
        TGraph* graph = new TGraph(allMomenta[segLenIdx].size(), &allMomenta[segLenIdx][0], &allNLLValues[segLenIdx][0]);
        graph->SetLineColor(colors[segLenIdx % 5]);
        mg->Add(graph);
        legend->AddEntry(graph, Form("Segment Length = %.1f cm", segmentLengthsArray[segLenIdx]), "l");
    }

    TGraph* globalGraph = new TGraph(globalNLLValues.size());
    for (size_t i = 0; i < globalNLLValues.size(); ++i) {
        globalGraph->SetPoint(i, 0.01 + 0.01 * i, globalNLLValues[i]);
    }
    globalGraph->SetLineColor(kBlack);
    mg->Add(globalGraph);
    legend->AddEntry(globalGraph, "Global Likelihood", "l");

    mg->Draw("AL");
    mg->GetXaxis()->SetTitle("Pion Momentum (GeV/c)");
    mg->GetYaxis()->SetTitle("Negative Log Likelihood");
    mg->GetXaxis()->SetRangeUser(0.1, 1.5);

    auto minElementIter = std::min_element(globalNLLValues.begin(), globalNLLValues.end());
    double minMomentum = 0.01 + 0.01 * std::distance(globalNLLValues.begin(), minElementIter);

    TLine* minLine = new TLine(minimisedP, mg->GetYaxis()->GetXmin(), minimisedP, mg->GetYaxis()->GetXmax());
    minLine->SetLineColor(kRed);
    minLine->SetLineStyle(7); 
    minLine->Draw();

    TLine* line = new TLine(1, mg->GetYaxis()->GetXmin(), 1, mg->GetYaxis()->GetXmax());
    line->SetLineColor(kBlue);
    line->SetLineStyle(7); 
    line->Draw();
    
    legend->Draw();
    c1->SaveAs("Plots/plot_nll_vs_momentum.png");
    
    delete c1;
    delete mg;
    delete legend;
    
    file->Close();
    delete file;
}