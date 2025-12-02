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


using namespace RooFit;
using namespace RooStats;

//  Exercise: OPERA-nu oscillations Part 1

void OPERAnu(){
    // Creating the model
    RooRealVar nobs{"nobs", "number of observed events", 5}; 
    RooRealVar mu{"mu", "signal strength", 1, 0, 5};
    RooRealVar nsig{"nsig", "n. of signal events", 2.64};
    RooRealVar nbkg{"nbkg", "n. of bkg events", 0.25, 0, 10};

    RooFormulaVar nexp{"nexp", "n. of expected true total events (unknown)", "@0*@1+@2", RooArgList(mu, nsig, nbkg)};
    RooPoisson pois{"pois", "poisson", nobs, nexp};

    RooRealVar nbkg_err{"nbkg_err", "sigma", 0.05};
    RooRealVar nbkg_estimate{"nbkg_estimate", "Global Obs.", 0.25};

    RooGaussian gauss{"gauss", "gauss", nbkg_estimate, nbkg, nbkg_err};

    // creating a model as the product of the two pdfs
    RooProdPdf model{"model", "1 channel numb. counting model w/ constraints", RooArgList(pois, gauss)};
    
    // Create dataset with observed value
    RooDataSet data{"data", "data", RooArgSet(nobs, nbkg_estimate)};
    data.add(RooArgSet(nobs, nbkg_estimate));

    RooWorkspace w("w");
    w.import(model);
    w.import(data);

    // printing
    w.Print();

    w.var("nbkg_estimate")->Print();
    w.pdf("model")->Print();
    w.function("nexp")->Print();
    w.data("data")->Print();

    // model config
    ModelConfig mc("ModelConfig");
    mc.SetWorkspace(w);
    mc.SetPdf(*w.pdf("model"));
    mc.SetParametersOfInterest(RooArgSet(*w.var("mu")));
    mc.SetObservables(RooArgSet(*w.var("nobs")));
    mc.SetGlobalObservables(RooArgSet(*w.var("nbkg_estimate")));
    mc.SetNuisanceParameters(RooArgSet(*w.var("nbkg")));

    mc.Print();

    w.import(mc);
    w.writeToFile("OPERAnu_model_1channel.root");

    ProfileLikelihoodCalculator plc(*w.data("data"), mc);
    w.var("mu")->setVal(0);
    plc.SetNullParameters(RooArgSet(*w.var("mu")));
    auto hp = plc.GetHypoTest();
    auto alpha = hp->NullPValue();
    auto significance = hp->Significance();
    std::cout << "p-value: " << alpha << std::endl;
    std::cout << "significance: " << significance << std::endl;
}