#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif

#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "TAxis.h"
#include "TCanvas.h"

using namespace RooFit;

void Ex20212(){


   // init vars
   RooRealVar x("x","x", 0, 10);
   RooRealVar meanlt("meanlt", "mean life time", 4.5, 0, 10);
   
   RooFormulaVar rate("rate", "rate", "-1/meanlt", meanlt);


   // def pdf
   RooExponential exp("exp", "exp", x, rate);
   
   RooPlot *xframe = x.frame(Title("exp PDF"));

   exp.plotOn(xframe);
//    exp.plotOn(xframe);
//    xframe->Draw();


   //generating data
   x.setBins(20); // nbins = 2*max(x)
   std::unique_ptr<RooDataSet> data{exp.generate(x, 10000)};

   // histogram
   RooDataHist dh("dh", "dh", x, *data);


   // fitting

   exp.fitTo(dh);

   RooPlot *xframe2 = x.frame(Title("Exp histogram"));
   dh.plotOn(xframe2);
   exp.plotOn(xframe2);

     //Drawing
   TCanvas *c = new TCanvas("Ex2021.2", "Ex2021.2", 800, 400); c->Divide(2);
   c->cd(1);
   gPad->SetLeftMargin(0.15);
   xframe->GetYaxis()->SetTitleOffset(1.6);
   xframe->Draw();
   c->cd(2);
   gPad->SetLeftMargin(0.15);
   xframe2->GetYaxis()->SetTitleOffset(1.6);
   xframe2->Draw();
}