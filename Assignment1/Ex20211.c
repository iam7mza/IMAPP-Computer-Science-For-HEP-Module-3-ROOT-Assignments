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

int Ex20211()
{
  // initializing variables
  RooRealVar x("x", "x", -10, 10);
  RooRealVar mean("mean", "mean of Gaussian", 0, -10, 10);
  RooRealVar sigma("sigma", "std of Gaussian", 1, 0.1, 10);

  // initializing Gaussian
  RooGaussian gauss("gauss", "Gaussian PDF", x, mean, sigma);

  // initializing frame
  RooPlot *xframe = x.frame(Title("Gaussian PDF"));

  //First Plot

  gauss.plotOn(xframe);

  // chaning sigma
  sigma.setVal(3);

  gauss.plotOn(xframe, LineColor(kRed));

  //Generating unbinned dataset
  std::unique_ptr<RooDataSet> data{gauss.generate(x, 10000)};
  RooPlot *xframe2 = x.frame(Title("Gaussian with Data"));
  data->plotOn(xframe2);
  gauss.plotOn(xframe2);

  //fitting
  gauss.fitTo(*data);


  //Drawing
   TCanvas *c = new TCanvas("Ex2021.1", "Ex2021.1", 800, 400); c->Divide(2);
   c->cd(1);
   gPad->SetLeftMargin(0.15);
   xframe->GetYaxis()->SetTitleOffset(1.6);
   xframe->Draw();
   c->cd(2);
   gPad->SetLeftMargin(0.15);
   xframe2->GetYaxis()->SetTitleOffset(1.6);
   xframe2->Draw();

  return 0;
}