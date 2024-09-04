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

using namespace std;

int ErrorDistribution() {
    gROOT->Reset();

    TFile *file_integral_small = TFile::Open("integral_distribution.root");
    TH2F *integral_small = (TH2F*) file_integral_small->Get("original_hist2D;1");

    TFile *file_integral_large = TFile::Open("integral_distribution_Test.root");
    TH2F *integral_large = (TH2F*) file_integral_large->Get("original_hist2D;1");

    TH2F *error_distri = new TH2F("error_matrix", "error_matrix",
                                  integral_small->GetNbinsX(), integral_small->GetXaxis()->GetXmin(), integral_small->GetXaxis()->GetXmax(),
                                  integral_small->GetNbinsY(), integral_small->GetYaxis()->GetXmin(), integral_small->GetYaxis()->GetXmax());
//Notice there is no GetYmax(), only use GetXmax to acquire the max value of each axis


    for (int XBin = 1; XBin <= integral_small->GetNbinsX(); XBin++) {
        for (int YBin = 1; YBin<= integral_small->GetNbinsY(); YBin++){
            double integral_small_value = integral_small->GetBinContent(XBin,YBin);
            double integral_large_value = integral_large->GetBinContent(XBin,YBin);

            double percent_error = (integral_large_value - integral_small_value)/integral_small_value;
            error_distri->SetBinContent(XBin,YBin,percent_error);

        }
    }

    TCanvas *error_canvas = new TCanvas("error_canvas", "Error Hist",800 ,600);

    error_canvas->cd();
    error_distri->Draw("colz");
    TFile *error_file = TFile::Open("percent_error.root", "RECREATE");
    error_distri->Write("colz");
    error_file->Close();

    /*
    delete integral_small;
    delete integral_large;
    delete file_integral_large;
    delete file_integral_large;
    delete error_canvas;
    delete error_distri;
    delete error_file;
*/
    return 1;
}
