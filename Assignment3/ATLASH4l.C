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
#include "RooPoisson.h"
#include "RooFormulaVar.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/ProfileLikelihoodCalculator.h"
#include "RooDataHist.h"
#include "RooCBShape.h"


using namespace RooFit;
using namespace RooStats;

// Excercise: ATLAS H->llll

void ATLASH4l(){
    // Part 1: Model, Fit, Plot

    RooRealVar x{"x", "invariant mass", 110, 135, "GeV"};
    x.setBins(10);
    // mind the path!!
    RooDataSet data = *RooDataSet::read("/home/hamza/statistical-data-analysis/data/higgs_4l.dat", x, "v");
    // Ensure a stable name in the workspace
    data.SetName("data");

    // model: 2 degree polynomial background + Crystal Ball signal
    RooRealVar a1("a1", "The a1 of background", 0, -1, 1);
    RooRealVar a2("a2", "The a2 of background", 0, -1, 1);
    RooChebychev bkg{"bkg", "background pdf", x, RooArgList(a1, a2)};

    RooRealVar mass{"mass", "mass peak", 125, 110, 150};
    RooRealVar width{"width", "width peak", 4.1};
    width.setConstant(true);
    RooRealVar alpha_param{"alpha_param", "alpha peak", 0.6};
    alpha_param.setConstant(true);
    RooRealVar n("n", "power law constant", 20);
    n.setConstant(true);
    
    RooCBShape signal{"signal", "signal pdf", x, mass, width, alpha_param, n};
    
    RooRealVar nsig{"nsig", "number of signal events", 50, 0, 500};
    RooRealVar nbkg{"nbkg", "number of bkg events", 300, 0, 1000};

    RooAddPdf model{"model", "signal + bkg", RooArgList(signal, bkg), RooArgList(nsig, nbkg)};

    // fitting
    RooDataHist hh{"hh", "hh", x, data};
    model.fitTo(hh);

    // NLL
    auto nll = model.createNLL(hh);
    RooMinimizer m(*nll);
    m.setVerbose(1);
    m.migrad();

    // plotting
    auto frame = x.frame();
    hh.plotOn(frame);
    model.plotOn(frame);
    model.plotOn(frame, Components(bkg), LineStyle(kDashed), LineColor(kRed));
    model.plotOn(frame, Components(signal), LineStyle(kDotted), LineColor(kGreen+2));
    frame->Draw();

    RooWorkspace w("w");
    w.import(model);
    w.import(data);

    ModelConfig mc("mc");
    mc.SetWorkspace(w);
    mc.SetPdf(*w.pdf("model"));
    mc.SetParametersOfInterest(RooArgSet(*w.var("nsig")));
    mc.SetObservables(RooArgSet(*w.var("x")));  
    
    w.defineSet("nuisParams", "nbkg,a1,a2,mass,width,alpha_param,n");
    mc.SetNuisanceParameters(*w.set("nuisParams"));
    // Keep shape parameters free to be profiled; set constants only if intended

    w.import(mc);

    // second mc
    ModelConfig mc_mass("mc_mass");
    mc_mass.SetWorkspace(w);
    mc_mass.SetPdf(*w.pdf("model"));
    mc_mass.SetParametersOfInterest(RooArgSet(*w.var("mass")));
    mc_mass.SetObservables(RooArgSet(*w.var("x")));
    w.defineSet("nuisParams2", "nbkg,a1,a2,nsig,width,alpha_param,n");
    mc_mass.SetNuisanceParameters(*w.set("nuisParams2"));



    w.import(mc_mass);
    w.writeToFile("ATLASH4l_model.root");

    ProfileLikelihoodCalculator plc(*w.data("data"), mc_mass);
    w.var("mass")->setVal(125);
    plc.SetNullParameters(RooArgSet(*w.var("mass")));
    auto hp = plc.GetHypoTest();
    auto alpha_val = hp->NullPValue();
    auto significance = hp->Significance();

    // plotting likelihood interval
    plc.SetConfidenceLevel(0.68);
    LikelihoodInterval* interval = plc.GetInterval();
    auto poi = static_cast<RooRealVar*>(mc_mass.GetParametersOfInterest()->first());
    double upper = interval->UpperLimit(*poi);
    double lower = interval->LowerLimit(*poi);

    LikelihoodIntervalPlot liPlot(interval);
    // liPlot.Draw(); // uncomment to draw the interval plot


    std::cout << "p-value: " << alpha_val << std::endl;
    std::cout << "significance: " << significance << std::endl;

    
}