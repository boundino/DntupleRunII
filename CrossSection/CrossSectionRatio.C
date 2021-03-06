#include "uti.h"
#include "parameters.h"
#include "TLegendEntry.h"
#include "bFeedDown/bFeedDownCorrection.C"
#include "../Systematics/systematics.C"

void CrossSectionRatio(TString inputFONLL="ROOTfiles/output_inclusiveDd0meson_5TeV_y1.root", TString input="ROOTfiles/hPtSpectrumDzeroPP.root", TString efficiency="test.root", TString inputprescales="prescalePP.root",int usePrescaleCorr=1,TString outputplot="myplot.root",int usePbPb=1,TString label="PbPb",double lumi=1.,Float_t centMin=0., Float_t centMax=100.)
{
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetEndErrorSize(0);
  gStyle->SetMarkerStyle(20);

  TFile* fileReference = new TFile(inputFONLL.Data());  
  TGraphAsymmErrors* gaeBplusReference = (TGraphAsymmErrors*)fileReference->Get("gaeSigmaDzero");
  
  if (!(usePbPb==1||usePbPb==0)) std::cout<<"ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!, you are using a non valid isPbPb option"<<std::endl;
  bool isPbPb=(bool)(usePbPb);

  TFile* file = new TFile(input.Data());
  
  TH1F* hYieldTriggerCorrected = (TH1F*)file->Get("hPt");
  hYieldTriggerCorrected->SetName("hYieldTriggerCorrected");  
  TH1F* hYieldNoTriggerCorrected = (TH1F*)file->Get("hPt");
  hYieldNoTriggerCorrected->SetName("hYieldNoTriggerCorrected");  
  TH1F* hYieldTriggerCorrectedFONLLnorm = (TH1F*)file->Get("hPt");
  hYieldTriggerCorrectedFONLLnorm->SetName("hYieldTriggerCorrectedFONLLnorm");
  TH1F* hDcandidatesTriggerCorrectedFONLLnorm = (TH1F*)file->Get("hDcandidates");
  hDcandidatesTriggerCorrectedFONLLnorm->SetName("hDcandidatesTriggerCorrectedFONLLnorm");
  TH1F* hYieldNoTriggerCorrectedFONLLnorm = (TH1F*)file->Get("hPt");
  hYieldNoTriggerCorrectedFONLLnorm->SetName("hYieldNoTriggerCorrectedFONLLnorm");
  TH1F* hDcandidatesNoTriggerCorrectedFONLLnorm = (TH1F*)file->Get("hDcandidates");
  hDcandidatesNoTriggerCorrectedFONLLnorm->SetName("hDcandidatesNoTriggerCorrectedFONLLnorm");
  
  
  TFile* fileeff = new TFile(efficiency.Data());
  TH1F* hEff = (TH1F*)fileeff->Get("hEff");
  
  TH1F* hfprompt = new TH1F("hfprompt","",nBins,ptBins);

  TH1F*hPrescalesPtBins;
  TH1F*hTriggerEfficiencyPtBins;
  

  TH1F* hPtSigma = (TH1F*)hYieldTriggerCorrected->Clone("hPtSigma");
   hPtSigma->Divide(hEff);
   hPtSigma->Scale(1./(2*lumi*BRchain));
   hPtSigma->SetName("hPtSigma");

/*
  TH1F* hPtSigma = (TH1F*)file->Get("hPtSigma");
  hPtSigma->Scale(1./lumi);
*/
  hYieldTriggerCorrected->Scale(1./lumi);
  hYieldNoTriggerCorrected->Scale(1./lumi);
  hYieldTriggerCorrectedFONLLnorm->Scale(1./lumi);
  hYieldNoTriggerCorrectedFONLLnorm->Scale(1./lumi);
  hDcandidatesTriggerCorrectedFONLLnorm->Scale(1./lumi);
  hDcandidatesNoTriggerCorrectedFONLLnorm->Scale(1./lumi);
  
  for (int i=0;i<nBins;i++) {
    double prompt=bFeedDownCorrection(hPtSigma->GetBinCenter(i+1),isPbPb,1);
    hfprompt->SetBinContent(i+1,prompt);
    hPtSigma->SetBinContent(i+1,hPtSigma->GetBinContent(i+1)*prompt);
    hPtSigma->SetBinError(i+1,hPtSigma->GetBinError(i+1)*prompt);
        
    std::cout<<"pt center="<<hPtSigma->GetBinCenter(i+1)<<", prompt fraction="<<prompt<<std::endl;
  }
  
  if (usePrescaleCorr==1){
    TFile*fprescales=new TFile(inputprescales.Data()); 
    hPrescalesPtBins=(TH1F*)fprescales->Get("hPrescalesPtBins");
    hTriggerEfficiencyPtBins=(TH1F*)fprescales->Get("hTriggerEfficiencyPtBins");
    
    for (int i=0;i<nBins;i++) {
      hPtSigma->SetBinContent(i+1,hPtSigma->GetBinContent(i+1)/hPrescalesPtBins->GetBinContent(i+1)/hTriggerEfficiencyPtBins->GetBinContent(i+1));
      hPtSigma->SetBinError(i+1,hPtSigma->GetBinError(i+1)/hPrescalesPtBins->GetBinContent(i+1)/hTriggerEfficiencyPtBins->GetBinContent(i+1));
      hYieldTriggerCorrected->SetBinContent(i+1,hYieldTriggerCorrected->GetBinContent(i+1)/hPrescalesPtBins->GetBinContent(i+1)/hTriggerEfficiencyPtBins->GetBinContent(i+1));
      hYieldTriggerCorrected->SetBinError(i+1,hYieldTriggerCorrected->GetBinError(i+1)/hPrescalesPtBins->GetBinContent(i+1)/hTriggerEfficiencyPtBins->GetBinContent(i+1));
      hYieldNoTriggerCorrected->SetBinContent(i+1,hYieldNoTriggerCorrected->GetBinContent(i+1)/hPrescalesPtBins->GetBinContent(i+1));
      hYieldNoTriggerCorrected->SetBinError(i+1,hYieldNoTriggerCorrected->GetBinError(i+1)/hPrescalesPtBins->GetBinContent(i+1));
      hYieldTriggerCorrectedFONLLnorm->SetBinContent(i+1,hYieldTriggerCorrectedFONLLnorm->GetBinContent(i+1)/hPrescalesPtBins->GetBinContent(i+1)/hTriggerEfficiencyPtBins->GetBinContent(i+1));
      hYieldTriggerCorrectedFONLLnorm->SetBinError(i+1,hYieldTriggerCorrectedFONLLnorm->GetBinError(i+1)/hPrescalesPtBins->GetBinContent(i+1)/hTriggerEfficiencyPtBins->GetBinContent(i+1));
      hDcandidatesTriggerCorrectedFONLLnorm->SetBinContent(i+1,hDcandidatesTriggerCorrectedFONLLnorm->GetBinContent(i+1)/hPrescalesPtBins->GetBinContent(i+1)/hTriggerEfficiencyPtBins->GetBinContent(i+1));
      hDcandidatesTriggerCorrectedFONLLnorm->SetBinError(i+1,hDcandidatesTriggerCorrectedFONLLnorm->GetBinError(i+1)/hPrescalesPtBins->GetBinContent(i+1)/hTriggerEfficiencyPtBins->GetBinContent(i+1));
      hYieldNoTriggerCorrectedFONLLnorm->SetBinContent(i+1,hYieldNoTriggerCorrectedFONLLnorm->GetBinContent(i+1)/hPrescalesPtBins->GetBinContent(i+1));
      hYieldNoTriggerCorrectedFONLLnorm->SetBinError(i+1,hYieldNoTriggerCorrectedFONLLnorm->GetBinError(i+1)/hPrescalesPtBins->GetBinContent(i+1));
      hDcandidatesNoTriggerCorrectedFONLLnorm->SetBinContent(i+1,hDcandidatesNoTriggerCorrectedFONLLnorm->GetBinContent(i+1)/hPrescalesPtBins->GetBinContent(i+1));
      hDcandidatesNoTriggerCorrectedFONLLnorm->SetBinError(i+1,hDcandidatesNoTriggerCorrectedFONLLnorm->GetBinError(i+1)/hPrescalesPtBins->GetBinContent(i+1));
    }
  }

  Double_t xr[nBins], xrlow[nBins], xrhigh[nBins], ycross[nBins],ycrossstat[nBins],ycrosssysthigh[nBins],ycrosssystlow[nBins], yFONLL[nBins];
  Double_t yratiocrossFONLL[nBins], yratiocrossFONLLstat[nBins], yratiocrossFONLLsysthigh[nBins], yratiocrossFONLLsystlow[nBins];
  Double_t yFONLLrelunclow[nBins], yFONLLrelunchigh[nBins], yunity[nBins];

  for(int i=0;i<nBins;i++)
    {
      gaeBplusReference->GetPoint(i,xr[i],yFONLL[i]);
      xrlow[i] = gaeBplusReference->GetErrorXlow(i);
      xrhigh[i] = gaeBplusReference->GetErrorXhigh(i);
      ycross[i] = hPtSigma->GetBinContent(i+1);
      ycrossstat[i] = hPtSigma->GetBinError(i+1);
      double systematic=0.;
      if (!isPbPb) systematic=0.01*systematicsPP(xr[i],0.);
      else  systematic=0.01*systematicsPbPb(xr[i],centMin,centMax,0.);     
      ycrosssysthigh[i]= hPtSigma->GetBinContent(i+1)*systematic;
      ycrosssystlow[i]= hPtSigma->GetBinContent(i+1)*systematic;
      yratiocrossFONLL[i] = ycross[i]/yFONLL[i];
      yratiocrossFONLLstat[i] = ycrossstat[i]/yFONLL[i];
      yratiocrossFONLLsysthigh[i] = ycrosssysthigh[i]/yFONLL[i];
      yratiocrossFONLLsystlow[i] = ycrosssystlow[i]/yFONLL[i];
      yFONLLrelunclow[i] = gaeBplusReference->GetErrorYlow(i)/yFONLL[i];
      yFONLLrelunchigh[i] = gaeBplusReference->GetErrorYhigh(i)/yFONLL[i];
      yunity[i] = yFONLL[i]/yFONLL[i];
      hYieldTriggerCorrectedFONLLnorm->SetBinContent(i+1,hYieldTriggerCorrectedFONLLnorm->GetBinContent(i+1)/yFONLL[i]);
      hYieldTriggerCorrectedFONLLnorm->SetBinError(i+1,hYieldTriggerCorrectedFONLLnorm->GetBinError(i+1)/yFONLL[i]);
      hDcandidatesTriggerCorrectedFONLLnorm->SetBinContent(i+1,hDcandidatesTriggerCorrectedFONLLnorm->GetBinContent(i+1)/yFONLL[i]);
      hDcandidatesTriggerCorrectedFONLLnorm->SetBinError(i+1,hDcandidatesTriggerCorrectedFONLLnorm->GetBinError(i+1)/yFONLL[i]);
    
      hYieldNoTriggerCorrectedFONLLnorm->SetBinContent(i+1,hYieldNoTriggerCorrectedFONLLnorm->GetBinContent(i+1)/yFONLL[i]);
      hYieldNoTriggerCorrectedFONLLnorm->SetBinError(i+1,hYieldNoTriggerCorrectedFONLLnorm->GetBinError(i+1)/yFONLL[i]);
      hDcandidatesNoTriggerCorrectedFONLLnorm->SetBinContent(i+1,hDcandidatesNoTriggerCorrectedFONLLnorm->GetBinContent(i+1)/yFONLL[i]);
      hDcandidatesNoTriggerCorrectedFONLLnorm->SetBinError(i+1,hDcandidatesNoTriggerCorrectedFONLLnorm->GetBinError(i+1)/yFONLL[i]);

 
 
    }

  TGraphAsymmErrors* gaeCrossSyst = new TGraphAsymmErrors(nBins,xr,ycross,xrlow,xrhigh,ycrosssystlow,ycrosssysthigh);
  gaeCrossSyst->SetName("gaeCrossSyst");
  gaeCrossSyst->SetMarkerStyle(20);
  gaeCrossSyst->SetMarkerSize(0.8);

  TGraphAsymmErrors* gaeRatioCrossFONLLstat = new TGraphAsymmErrors(nBins,xr,yratiocrossFONLL,xrlow,xrhigh,yratiocrossFONLLstat,yratiocrossFONLLstat);
  gaeRatioCrossFONLLstat->SetName("gaeRatioCrossFONLLstat");
  gaeRatioCrossFONLLstat->SetMarkerStyle(20);
  gaeRatioCrossFONLLstat->SetMarkerSize(0.8);
  
  TGraphAsymmErrors* gaeRatioCrossFONLLsyst= new TGraphAsymmErrors(nBins,xr,yratiocrossFONLL,xrlow,xrhigh,yratiocrossFONLLsystlow,yratiocrossFONLLsysthigh);
  gaeRatioCrossFONLLsyst->SetName("gaeRatioCrossFONLLsyst");
  gaeRatioCrossFONLLsyst->SetLineWidth(2);
  gaeRatioCrossFONLLsyst->SetLineColor(1);
  gaeRatioCrossFONLLsyst->SetFillColor(2);
  gaeRatioCrossFONLLsyst->SetFillStyle(0);

  TGraphAsymmErrors* gaeRatioCrossFONLLunity = new TGraphAsymmErrors(nBins,xr,yunity,xrlow,xrhigh,yFONLLrelunclow,yFONLLrelunchigh);
  gaeRatioCrossFONLLunity->SetName("gaeRatioCrossFONLLunity");
  gaeRatioCrossFONLLunity->SetLineWidth(2);
  gaeRatioCrossFONLLunity->SetLineColor(2);
  gaeRatioCrossFONLLunity->SetFillColor(2);
  gaeRatioCrossFONLLunity->SetFillStyle(3002);
  
  TCanvas* cSigma = new TCanvas("cSigma","",600,750);
  cSigma->SetFrameBorderMode(0);
  cSigma->SetFrameBorderMode(0);
  cSigma->Range(-1.989924,-0.2917772,25.49622,2.212202);
  cSigma->SetFillColor(0);
  cSigma->SetBorderMode(0);
  cSigma->SetBorderSize(2);
  cSigma->SetLeftMargin(0.1451613);
  cSigma->SetRightMargin(0.05443548);
  cSigma->SetTopMargin(0.08474576);
  cSigma->SetBottomMargin(0.1165254);
  cSigma->SetFrameBorderMode(0);
  cSigma->SetFrameBorderMode(0);
  cSigma->cd();
  TPad* pSigma = new TPad("pSigma","",0,0.25,1,1);
  pSigma->SetFillColor(0);
  pSigma->SetBorderMode(0);
  pSigma->SetBorderSize(2);
  pSigma->SetLeftMargin(0.1451613);
  pSigma->SetRightMargin(0.05443548);
  pSigma->SetTopMargin(0.08474576);
  pSigma->SetBottomMargin(0);
  pSigma->SetLogy();
  pSigma->Draw();
  pSigma->cd();


  Float_t yaxisMin=1.1,yaxisMax=1.e+9;
  if(label=="PPMB"||label=="PbPbMB")
    {
      yaxisMin=1.e+4;
      yaxisMax=1.e+13;
    }
  TH2F* hemptySigma=new TH2F("hemptySigma","",50,ptBins[0]-5.,ptBins[nBins]+5.,10.,yaxisMin,yaxisMax);  
  hemptySigma->GetXaxis()->CenterTitle();
  hemptySigma->GetYaxis()->CenterTitle();
  hemptySigma->GetYaxis()->SetTitle("d#sigma / dp_{T}( pb GeV^{-1}c)");
  if(isPbPb) hemptySigma->GetYaxis()->SetTitle("1/T_{AA} * dN / dp_{T}( pb GeV^{-1}c)");
  hemptySigma->GetXaxis()->SetTitleOffset(1.);
  hemptySigma->GetYaxis()->SetTitleOffset(1.3);
  hemptySigma->GetXaxis()->SetTitleSize(0.045);
  hemptySigma->GetYaxis()->SetTitleSize(0.045);
  hemptySigma->GetXaxis()->SetTitleFont(42);
  hemptySigma->GetYaxis()->SetTitleFont(42);
  hemptySigma->GetXaxis()->SetLabelFont(42);
  hemptySigma->GetYaxis()->SetLabelFont(42);
  hemptySigma->GetXaxis()->SetLabelSize(0.04);
  hemptySigma->GetYaxis()->SetLabelSize(0.04);  
  hemptySigma->SetMaximum(2);
  hemptySigma->SetMinimum(0.);
  hemptySigma->Draw();
  gaeBplusReference->SetFillColor(2);
  gaeBplusReference->SetFillStyle(3001); 
  gaeBplusReference->SetLineWidth(3);
  gaeBplusReference->SetLineColor(2);
  gaeBplusReference->Draw("5same");
  hPtSigma->SetLineColor(1);
  hPtSigma->SetLineWidth(2);
  hPtSigma->Draw("epsame"); 
  gaeCrossSyst->SetFillColor(1);
  gaeCrossSyst->SetFillStyle(0); 
  gaeCrossSyst->SetLineWidth(2);
  gaeCrossSyst->SetLineColor(1);
  gaeCrossSyst->Draw("5same");  
  
  TLatex* texCms = new TLatex(0.16,0.95, "#scale[1.25]{CMS} Preliminary");
  texCms->SetNDC();
  texCms->SetTextAlign(12);
  texCms->SetTextSize(0.04);
  texCms->SetTextFont(42);
  texCms->Draw();

  TLatex* texCol = new TLatex(0.94,0.95, Form("%s #sqrt{s_{NN}} = 5.02 TeV",label.Data()));
  texCol->SetNDC();
  texCol->SetTextAlign(32);
  texCol->SetTextSize(0.04);
  texCol->SetTextFont(42);
  texCol->Draw();

  TString texper="%";
  TLatex* texCent = new TLatex(0.53,0.815,Form("Centrality %.0f - %.0f%s",centMin,centMax,texper.Data()));
  texCent->SetNDC();
  texCent->SetTextFont(42);
  texCent->SetTextSize(0.04);
  if(isPbPb) texCent->Draw();

  TLatex* texY = new TLatex(0.53,0.77,"|y| < 1.0");
  texY->SetNDC();
  texY->SetTextFont(42);
  texY->SetTextSize(0.04);
  texY->SetLineWidth(2);
  texY->Draw();

  TLegend* leg_CS = new TLegend(0.52,0.64,0.85,0.75);
  leg_CS->SetBorderSize(0);
  leg_CS->SetFillStyle(0);
  leg_CS->AddEntry(hPtSigma,"data","pf");
  leg_CS->AddEntry(gaeBplusReference,"FONLL pp ref.","f");
  leg_CS->Draw("same");
  
  /*
  TLatex * tlatexlumi=new TLatex(0.671371,0.7801268,"L = 9.97 pb^{-1}");
  tlatexlumi->SetNDC();
  tlatexlumi->SetTextColor(1);
  tlatexlumi->SetTextFont(42);
  tlatexlumi->SetTextSize(0.045);
  */
  cSigma->cd();
  TPad* pRatio = new TPad("pRatio","",0,0,1,0.25);
  pRatio->SetLeftMargin(0.1451613);
  pRatio->SetRightMargin(0.05443548);
  pRatio->SetTopMargin(0);
  pRatio->SetBottomMargin(0.25);
  pRatio->Draw();
  pRatio->cd();

  TH2F* hemptyRatio=new TH2F("hemptyRatio","",50,ptBins[0]-5.,ptBins[nBins]+5.,10.,0.,3.1);
  hemptyRatio->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hemptyRatio->GetYaxis()->CenterTitle();
  hemptyRatio->GetYaxis()->SetTitle("Data / FONLL");
  hemptyRatio->GetXaxis()->SetTitleOffset(0.9);
  hemptyRatio->GetYaxis()->SetTitleOffset(0.5);
  hemptyRatio->GetXaxis()->SetTitleSize(0.12);
  hemptyRatio->GetYaxis()->SetTitleSize(0.12);
  hemptyRatio->GetXaxis()->SetTitleFont(42);
  hemptyRatio->GetYaxis()->SetTitleFont(42);
  hemptyRatio->GetXaxis()->SetLabelFont(42);
  hemptyRatio->GetYaxis()->SetLabelFont(42);
  hemptyRatio->GetXaxis()->SetLabelSize(0.1);
  hemptyRatio->GetYaxis()->SetLabelSize(0.1);  
  hemptyRatio->Draw();

  TLine* l = new TLine(ptBins[0]-5.,1,ptBins[nBins]+5.,1);
  l->SetLineWidth(1);
  l->SetLineStyle(2);
  gaeRatioCrossFONLLunity->Draw("5same");
  gaeRatioCrossFONLLstat->Draw("epsame");
  gaeRatioCrossFONLLsyst->Draw("5same");
  l->Draw("same");
  if(!isPbPb) cSigma->SaveAs(Form("plotCrossSection/canvasSigmaDzeroRatio%s.pdf",label.Data()));
  else cSigma->SaveAs(Form("plotCrossSection/canvasSigmaDzeroRatio%s_%.0f_%.0f.pdf",label.Data(),centMin,centMax));
  
  
  TCanvas* cEff = new TCanvas("cEff","",550,500);
  
  TH2F* hemptyEff=new TH2F("hemptyEff","",50,0.,110.,10.,0,1.);  
  hemptyEff->GetXaxis()->CenterTitle();
  hemptyEff->GetYaxis()->CenterTitle();
  hemptyEff->GetYaxis()->SetTitle("#alpha x #epsilon_{reco} x #epsilon_{sel} ");
  hemptyEff->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hemptyEff->GetXaxis()->SetTitleOffset(0.9);
  hemptyEff->GetYaxis()->SetTitleOffset(1.05);
  hemptyEff->GetXaxis()->SetTitleSize(0.045);
  hemptyEff->GetYaxis()->SetTitleSize(0.045);
  hemptyEff->GetXaxis()->SetTitleFont(42);
  hemptyEff->GetYaxis()->SetTitleFont(42);
  hemptyEff->GetXaxis()->SetLabelFont(42);
  hemptyEff->GetYaxis()->SetLabelFont(42);
  hemptyEff->GetXaxis()->SetLabelSize(0.04);
  hemptyEff->GetYaxis()->SetLabelSize(0.04);  
  hemptyEff->SetMaximum(2);
  hemptyEff->SetMinimum(0.);
  hemptyEff->Draw();
  cEff->cd();
  hemptyEff->Draw();
  hEff->SetLineWidth(2);
  hEff->SetLineColor(1);
  hEff->Draw("same");

  
  TString text;
  TString sample;
  if (label=="PbPb") { text="CMS Preliminary     PbPb #sqrt{s}= 5.02 TeV"; sample="Pythia8+Hydjet MC simulation, prompt D^{0}";}
  else {text="CMS Preliminary     pp #sqrt{s}= 5.02 TeV"; sample="Pythia8 MC simulation, prompt D^{0}";}
  
  TLatex * tlatexeff=new TLatex(0.1612903,0.8525793,text.Data());
  tlatexeff->SetNDC();
  tlatexeff->SetTextColor(1);
  tlatexeff->SetTextFont(42);
  tlatexeff->SetTextSize(0.04);
  tlatexeff->Draw();
  TLatex * tlatexeff2=new TLatex(0.1612903,0.7925793,sample.Data());
  tlatexeff2->SetNDC();
  tlatexeff2->SetTextColor(1);
  tlatexeff2->SetTextFont(42);
  tlatexeff2->SetTextSize(0.04);
  tlatexeff2->Draw();
  if(!isPbPb) cEff->SaveAs(Form("plotOthers/efficiency%s.pdf",label.Data()));
  else cEff->SaveAs(Form("plotOthers/efficiency%s_%.0f_%.0f.pdf",label.Data(),centMin,centMax));
  
  
  TCanvas* cFprompt = new TCanvas("cFprompt","",550,500);
  TH2F* hemptyPrompt=new TH2F("hemptyPrompt","",50,0.,110.,10.,0,1.);  
  hemptyPrompt->GetXaxis()->CenterTitle();
  hemptyPrompt->GetYaxis()->CenterTitle();
  hemptyPrompt->GetYaxis()->SetTitle("f_{prompt}");
  hemptyPrompt->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  hemptyPrompt->GetXaxis()->SetTitleOffset(0.9);
  hemptyPrompt->GetYaxis()->SetTitleOffset(1.05);
  hemptyPrompt->GetXaxis()->SetTitleSize(0.045);
  hemptyPrompt->GetYaxis()->SetTitleSize(0.045);
  hemptyPrompt->GetXaxis()->SetTitleFont(42);
  hemptyPrompt->GetYaxis()->SetTitleFont(42);
  hemptyPrompt->GetXaxis()->SetLabelFont(42);
  hemptyPrompt->GetYaxis()->SetLabelFont(42);
  hemptyPrompt->GetXaxis()->SetLabelSize(0.04);
  hemptyPrompt->GetYaxis()->SetLabelSize(0.04);  
  hemptyPrompt->SetMaximum(2);
  hemptyPrompt->SetMinimum(0.);
  hemptyPrompt->Draw();
  cFprompt->cd();
  hemptyPrompt->Draw();
  hfprompt->SetLineWidth(2);
  hfprompt->SetLineColor(1);
  hfprompt->Draw("same");
  if(!isPbPb) cFprompt->SaveAs(Form("plotOthers/cFprompt%s.pdf",label.Data()));
  else cFprompt->SaveAs(Form("plotOthers/cFprompt%s_%.0f_%.0f.pdf",label.Data(),centMin,centMax));


  TFile *outputfile=new TFile(outputplot.Data(),"recreate");
  outputfile->cd();
  gaeCrossSyst->Write();
  gaeRatioCrossFONLLstat->Write();
  gaeBplusReference->Write();
  hPtSigma->Write();
  gaeRatioCrossFONLLstat->Write();
  gaeRatioCrossFONLLsyst->Write();
  gaeRatioCrossFONLLunity->Write();
  hEff->Write();
  hfprompt->Write();
  hYieldTriggerCorrected->Write();
  hYieldNoTriggerCorrected->Write();
  hDcandidatesTriggerCorrectedFONLLnorm->Write();
  hYieldTriggerCorrectedFONLLnorm->Write();
  hDcandidatesNoTriggerCorrectedFONLLnorm->Write();
  hYieldNoTriggerCorrectedFONLLnorm->Write();
  if (usePrescaleCorr==1){
  hPrescalesPtBins->Write();
  hTriggerEfficiencyPtBins->Write();
  }
}


int main(int argc, char *argv[])
{
  if(argc==12)
    {
      CrossSectionRatio(argv[1], argv[2], argv[3],argv[4],atoi(argv[5]),argv[6],atoi(argv[7]),argv[8],atof(argv[9]),atof(argv[10]),atof(argv[11]));
      return 0;
    }
  else if(argc==10)
    {
      CrossSectionRatio(argv[1], argv[2], argv[3],argv[4],atoi(argv[5]),argv[6],atoi(argv[7]),argv[8],atof(argv[9]));
      return 0;
    }
  else
    {
      std::cout << "Wrong number of inputs" << std::endl;
      return 1;
    }
  
}


