#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif

#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "RooBreitWigner.h"
#include "TLegend.h"

void Ex122() {
    using namespace RooFit;

    RooRealVar x("x", "x", 5.0, 6);

    auto f = TFile::Open("~/statistical-data-analysis/data/b0_invariant_mass.root");
    auto hh = static_cast<TH1*>(f->Get("massaB0"));
    RooDataHist dh("dh", "dh", x, Import(*hh));

    // Gaussian Model
    RooRealVar mean("mean", "mean of Gaussian", 5.28, 5.0, 5.6);
    RooRealVar sigma("sigma", "sigma of Gaussian", 0.03, 0.001, 0.1);

    RooGaussian gauss("gauss", "Gaussian PDF", x, mean, sigma);
    gauss.fitTo(dh);

    // Breit-Wigner Model
    RooBreitWigner BW("BW", "BW", x, mean, sigma);
    BW.fitTo(dh);

    // Plotting
    RooPlot* xframe = x.frame(Title("B0 inv Mass"));

    dh.plotOn(xframe, Name("data"));
    gauss.plotOn(xframe, Name("gauss_curve"));
    BW.plotOn(xframe, LineColor(kRed), Name("bw_curve"));

    xframe->Draw();

    // Legend
    auto legend = new TLegend(0.6, 0.7, 0.88, 0.88);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0); 

    legend->AddEntry(xframe->findObject("data"), "Data", "lep");
    legend->AddEntry(xframe->findObject("gauss_curve"), "Gaussian Fit", "l");
    legend->AddEntry(xframe->findObject("bw_curve"), "Breit-Wigner Fit", "l");

    legend->Draw();
}
