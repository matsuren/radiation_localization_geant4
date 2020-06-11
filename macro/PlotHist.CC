// command %root ‘PlotHist.CC(“detect_result.root”)’
void PlotHist(const char *file)
{
gROOT->Reset();
TFile *f = TFile::Open(file);
TCanvas * P_Canvas = new TCanvas("P_Canvas", "Energy Deposit");

TH1D * sum_eDep = (TH1D*)f->Get("h1:0");
TH1D * sum_stepLength = (TH1D*)f->Get("h1:1");

P_Canvas->Divide(1,2);

P_Canvas->cd(1);
sum_eDep->Draw("HIST");

P_Canvas->cd(2);
sum_stepLength->Draw("HIST");

}

