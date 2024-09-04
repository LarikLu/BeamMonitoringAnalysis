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


TH1F* AreaDistri1D(TH2F* hist, int binX, int binY, int rangeX, int rangeY, int i, int j){
    int startX = max((binX-(rangeX/2)),1);
    int startY = max((binY-(rangeY/2)),1);

    int endX = min((binX+(rangeX/2)),hist->GetNbinsX());
    int endY = min((binY+(rangeY/2)),hist->GetNbinsY());

    std::string histName = "hist1D_"+ std::to_string(i)+std::to_string(j);
    TH1F *ADCdistri = new TH1F(histName.c_str(), histName.c_str(),4000,-20,20);
    for (int x = startX; x<= endX; x++){
        for (int y = startY; y<= endY; y++){
            double ADCvalue = hist->GetBinContent(x,y);
            ADCdistri->Fill(ADCvalue);
        }
    }

    return ADCdistri;
}

using namespace std;

int AreaDistribution() {
    gROOT->Reset();
    TFile *file = TFile::Open("out.root");

    int selection_X = 100;
    int selection_Y = 100;
    int area_center_X = 300;
    int area_center_Y = 300;

    int neg_range = -30;
    int range = -neg_range;

    for (int i = neg_range; i<=range; i+=10) {
        for (int j = neg_range; j<=range; j+=10){
            std::string objectName = "src_Image21074855-" + std::to_string(i) + std::to_string(j) + "-0.tiff;1";
            TH2F *Image = (TH2F*) file->Get(objectName.c_str());
            if (!Image) {
                cerr << "Failed to retrieve histogram: " << objectName << endl;
                return 1;
            }

            TH1F *area1D = AreaDistri1D(Image,area_center_X, area_center_Y, selection_X, selection_Y, i, j);

            std::string fileName = "Area1D-"+ std::to_string(i) +"_" + std::to_string(j)+".root";
            TFile *area1D_file = TFile::Open(fileName.c_str(),"RECREATE");
            area1D->Write();
            area1D_file->Close();
            delete area1D;
            delete area1D_file;
        }
    }

    return 1;
}
