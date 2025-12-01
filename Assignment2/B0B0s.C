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
using namespace RooFit;
 

// AUTHORS NOTE: the choice of initial variables changes a lot regarding how the fit looks.
// sadly i did not manage to get the B0s peak to show up properly. 
// the comp. bkg and the B0 signal look relatively good when compared to the LHCb plot
void B0B0s(){
    //  IMPORTANT: MIND THE PATH
    RooRealVar x{"x", "x", 5070, 5550};
    RooDataSet data = *RooDataSet::read("/home/hamza/statistical-data-analysis/data/rarest_b0_decay.dat", x, "v");
    x.setBins(25);
    RooDataHist hh{"hh", "hh", x, data};



    // defining model
    // bkg
    RooRealVar a0("a0","a0",0.0,-1,1);
    RooRealVar a1("a1","a1",0.0,-1,1);


    RooChebychev bkg("bkg", "Background", x, RooArgSet(a0, a1));

    // peaks
    RooRealVar meanB0{"meanB0","meanB0", 5270, 5100, 5500};
    RooRealVar meanBs{"meanBs","meanBs", 5400, 5100, 5500};
    RooRealVar sB0{"sB0", "SB0", 20, 1.0, 100.0};
    RooRealVar sBs{"sBs", "SBs", 50, 1.0, 100.0};

    RooGaussian sigB0{"sigB0", "sigB0", x, meanB0, sB0};
    RooGaussian sigBs{"sigBs", "sigBs", x, meanBs, sBs};

    RooRealVar nbkg("nbkg", "expected background", 10000, 0, 100000); // note thesee values are set arbitrarely
    RooRealVar nB0("nB0", "expected signal B0", 1000, 0, 100000);
    RooRealVar nBs("nBs", "expected signal Bs", 125, 0, 100000);
    //Marked Poisson model
    RooAddPdf model("m", "m", RooArgList(bkg, sigB0, sigBs), RooArgList(nbkg, nB0, nBs));


    // I have opted to do an automatic fit with pdf.fitTo to the histogram

    model.fitTo(hh);

    RooPlot* frame1 = x.frame(Title("B0 and B0s"));
    hh.plotOn(frame1);
    model.plotOn(frame1);
    model.plotOn(frame1, Components("bkg"), LineStyle(kDashed));
    model.plotOn(frame1, Components("sigB0"), LineColor(kRed));
    model.plotOn(frame1, Components("sigBs"), LineColor(kGreen));    
    frame1->Draw();

    RooHist *hresid = frame1->residHist();
    RooHist *hpull = frame1->pullHist();
    RooPlot *frame2 = x.frame(Title("Residual Distribution"));
    frame2->addPlotable(hresid, "P");
       RooPlot *frame3 = x.frame(Title("Pull Distribution"));
   frame3->addPlotable(hpull, "P");
 
   TCanvas *c = new TCanvas("residpull", "residpull", 900, 300);
   c->Divide(3);
   c->cd(1);
   gPad->SetLeftMargin(0.15);
   frame1->GetYaxis()->SetTitleOffset(1.6);
   frame1->Draw();
   c->cd(2);
   gPad->SetLeftMargin(0.15);
   frame2->GetYaxis()->SetTitleOffset(1.6);
   frame2->Draw();
   c->cd(3);
   gPad->SetLeftMargin(0.15);
   frame3->GetYaxis()->SetTitleOffset(1.6);
   frame3->Draw();
}