#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooProdPdf.h"
#include "RooAddPdf.h"
#include "RooMinimizer.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"
#include "RooChebychev.h"
#include "RooWorkspace.h"
#include "TStyle.h"


// AUTHORS NOTE: I could not get the contour plot to not be a dot.
// I tried everything but to no avail

using namespace RooFit;
 
void Ex11(){
    RooRealVar x{"x", "x", -20, 20};
    RooRealVar mean{"mean", "mean", 0.};
    // fixing the value of mean
    mean.setVal(0.);
    mean.setConstant(true);

    RooRealVar s1{"s1", "std of g1", 3.448};
    // fixing s1
    s1.setVal(3.448);
    s1.setConstant(true);

    RooRealVar s2{"s2", "s2", 4., 3., 6.};

    RooGaussian g1{"g1", "g1", x, mean, s1};
    RooGaussian g2{"g2", "g2", x, mean, s2};

    RooRealVar frac{"frac", "fraction of g1 and g2", 0.5, 0.0, 1.0};

    RooAddPdf model{"model", "model", RooArgList(g1, g2), frac};

    // data generation

    RooDataSet* data = model.generate(x, 1000);

    // workspace
    RooWorkspace w("w");
    w.import(*data);
    w.import(model);

    w.writeToFile("Ex11.root");

    // NLL

    auto nll = model.createNLL(*data);
    RooMinimizer m(*nll);
    // verbose
    m.setVerbose(1);

    // minimizing NLL
    m.migrad();

    // printing vals
    frac.Print();
    mean.Print();
    s1.Print();
    s2.Print();

    


    // Hesse
    m.setVerbose(0);
    m.hesse();
        
    // printing vals
    frac.Print();
    mean.Print();
    s1.Print();
    s2.Print();

    // minos

    m.minos(s2);
    s2.Print();

    // saving
    std::unique_ptr<RooFitResult> fitResult{m.save()};
    // RooPlot *frame = m.contour(frac, s2, 1,2,3);
    // frame->Draw();
    fitResult->Print("v");

    // gStyle->SetPalette(1);
    // fitResult->correlationHist()->Draw("colz");

    // Contour plot


    TCanvas* c1 = new TCanvas("c1","Correlation Matrix",800,600);
    gStyle->SetPalette(1);
    fitResult->correlationHist()->Draw("colz");

    TCanvas* c2 = new TCanvas("c2","Contour",800,600);
    RooPlot* frame = m.contour(frac, s2, 1,2,3);
    frame->Draw();


}