#include "Riostream.h"
#include <stdio.h>
#include <TH2.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TTree.h>
#include <TKey.h>
#include <TFile.h>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TSystem.h>
#include <TROOT.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <cmath>

#include "Fit/Fitter.h"
#include "Fit/BinData.h"
#include "TVector.h"
#include <TGraph.h>


std::pair<int, int> getMaxPosition(TH2F* hist){
    int binX, binY;
    double maxValue = 0;

    for (int x=1; x<= hist->GetNbinsX(); x++){
        for (int y=1; y<= hist->GetNbinsY(); y++){
            double binContent = hist->GetBinContent(x,y);
            if (binContent > maxValue){
                maxValue = binContent;
                binX = x;
                binY = y;
            }
        }
    }

    return std::make_pair(binX, binY);
}

double IntegralRegion(TH2F* hist, int binX, int binY, int rangeX, int rangeY){
    int startX = max((binX-(rangeX/2)),1);
    int startY = max((binY-(rangeY/2)),1);

    int endX = min((binX+(rangeX/2)),hist->GetNbinsX());
    int endY = min((binY+(rangeY/2)),hist->GetNbinsY());

    double integral = 0;
    for (int x = startX; x<= endX; x++){
        for (int y = startY; y<= endY; y++){
            integral += hist->GetBinContent(x,y);
            //std::cout<< "the current ADC value is" << hist->GetBinContent(x,y)<< endl;
        }
    }
    return integral;
}

using namespace std;

int CorrectionMatrix() {
    gROOT->Reset();
    TFile *file = TFile::Open("out2.root");

    //int rebin_size = 10;
    int range = 30;
    int neg_range = -range;

    int nBinsX = 7;
    int nBinsY = 7;
    double xMin = -3.5;
    double xMax = 3.5;
    double yMin = -3.5;
    double yMax = 3.5;

    TH2F *ori_hist2D = new TH2F("original_hist2D", "Original 2D Histogram", nBinsX, xMin, xMax, nBinsY, yMin, yMax);

/*
    std::string objectName = "src_Image21074855-" + std::to_string(30) + std::to_string(30) + "-0.tiff 1D;1";
    TH1F *Image = (TH1F*) file->Get(objectName.c_str());
    if (!Image) {
        cerr << "Failed to retrieve histogram: " << objectName << endl;
        return 1;
    }
*/
    int XBin = 1;
    int YBin = 1;
    for (int i = neg_range; i<=range; i+=10) {
        for (int j = neg_range; j<=range; j+=10){
            std::string objectName = "src_Image21074855-" + std::to_string(i) + std::to_string(j) + "-0.tiff_bkgsub;1";

            TH2F *Image = (TH2F*) file->Get(objectName.c_str());
            if (!Image) {
                cerr << "Failed to retrieve histogram: " << objectName << endl;
                return 1;
            }

            int selection_X = Image->GetNbinsX()-10;
            int selection_Y = Image->GetNbinsY()-10;

            //std::pair<int, int> MaxPosition = getMaxPosition(Image);
            //int MaxX = MaxPosition.first;
            //int MaxY = MaxPosition.second;

            int center_X = std::round((selection_X+1)/2);
            int center_Y = std::round((selection_Y+1)/2);

            double integral_value = IntegralRegion(Image, center_X, center_Y, selection_X, selection_Y);
            std::cout<<"integral value for the position X:"<< i << " and position Y: " << j << " is " <<integral_value <<std::endl;
            std::cout<<"the integral starts at X position: "<<center_X << " and the Y position: "<< center_Y << std::endl;

            ori_hist2D->SetBinContent(XBin,YBin,integral_value);
            YBin++;
        }
        XBin++;
        YBin = 1;
    }

    double reference = ori_hist2D->GetBinContent(4,4);

    TH2F* correction_matrix = new TH2F("correction_matrix", "correction_matirx", nBinsX, xMin, xMax, nBinsY, yMin, yMax);

    for (int XBin = 1; XBin <=7; XBin++) {
        for (int YBin = 1; YBin <= 7; YBin++){
            double current_value = ori_hist2D->GetBinContent(XBin,YBin);
            double ratio = current_value/reference;

            std::cout<<"ratio for the position X:"<< XBin << " and position Y: " << YBin << " is " <<ratio <<std::endl;
            correction_matrix->SetBinContent(XBin,YBin,ratio);
        }
    }

    //TFile *correction_matrix_file = TFile::Open("correction_matrix.root");
    //TH2F *Image = (TH2F*) file->Get(objectName.c_str());


    TCanvas *Integral_canvas_Test = new TCanvas("Integral_canvas", "Integral Hist",800 ,600);
    TCanvas *Correction_canvas_Test = new TCanvas("Correction_canvas", "Correction Hist",800 ,600);

    Integral_canvas_Test->cd();
    ori_hist2D->Draw("colz");
    TFile *ori_2D_file_Test = TFile::Open("integral_distribution.root", "RECREATE");
    ori_hist2D->Write();
    ori_2D_file_Test->Close();

//    Correction_canvas->cd();
//    correction_matrix->Draw("colz");
    TFile *correction_matrix_file_Test = TFile::Open("correction_matrix.root","RECREATE");
    correction_matrix->Write();
    correction_matrix_file_Test->Close();

/*
    std::string input;
    std::cout << "Type 'y' to remove hist:";
    std::cin >> input;
*/
//    if (input == 'y'){
//        std::cout << "Deleting hist objects..." << std::endl;
        delete ori_hist2D;
        delete correction_matrix;
        delete ori_2D_file_Test;
        delete correction_matrix_file_Test;
        delete Integral_canvas_Test;
        delete Correction_canvas_Test;
//    }



/*
    TCanvas *filter_canvas = new TCanvas("filter_canvas", "Filtered Hist",800 ,600);
    TCanvas *rebin_canvas = new TCanvas("rebin_canvas", "Rebinned Hist",800 ,600);

    filter_canvas->cd();
    TH1F *filtered_hist = NoiseReduce(Image,filter_threshold);
    TF1 *gaussFit = new TF1("gaussFit", "gaus", filter_threshold, filtered_hist->GetXaxis()->GetXmax());
    filtered_hist->Fit(gaussFit, "Q");
    filtered_hist->Draw();
    gaussFit->Draw("same");

    rebin_canvas->cd();
    TH1F *rebinned_hist = RebinHist(filtered_hist, rebin_size);
    rebinned_hist->Draw();
*/


            //double ADC_processed = SelectMiddle40Percent(gaussFit);
            //delete gaussFit;
            //delete filtered_hist;


    //ori_hist2D->Draw("colz");

    return 1;
}
