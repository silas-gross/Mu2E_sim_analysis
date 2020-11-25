#include <iostream>
#include "C:\root_v6.22.02\include\TH1.h"
#include "C:\root_v6.22.02\include\TH2.h"
#include "C:\root_v6.22.02\include\TFile.h"
#include "C:\root_v6.22.02\include\TKey.h"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdlib.h>

int main() {
	TFile* f = TFile::Open("../muons_at_target.root", "RECREATE");
	if (f->IsOpen()) std::cout << "File is open" << std::endl;
	else {
		std::cout << "File has failed to open";
		return 0;
	}
	std::vector<TKey*> histoskeys;
	TIter names(f->GetListOfKeys());
	TKey* key;
	while ((key = (TKey*)names())) {
		std::string objname = key->GetName();
		std::string objtype = key->GetClassName();

		if (objname.find("all") !=std::string::npos && objtype.find("TH")!= std::string::npos)
		{
			histoskeys.push_back(key);
		}
	}
	std::vector <TH1*> phis;
	for(int i=0; i<histoskeys.size(); i++)
	{
		TH2D* h_full = (TH2D*)histoskeys[i]->Clone();
		std::string name = histoskeys[i]->GetName();
		TH1* hx = h_full->ProjectionX();
		TH1* hy = h_full->ProjectionY();
		name = "phi_" + name;
		std::string title = "Angular Ditribution of " + name;
		TH1F* hphi = new TH1F(name.c_str(), title.c_str(), 100, -3.14159, 3.14159);
		std::vector< std::pair <float, int> > allphi;
		for (int j = 0, k=0; j < hx->GetNbinsX(), k<hy->GetNbinsX(); j++, k++)
		{
			float xval=hx->GetBinCenter(j);
			float yval = hy->GetBinCenter(k);
			std::pair  <float, int> phibins (atan(yval / xval), hx->GetBinContent(j) + hy->GetBinContent(k));
			allphi.push_back(phibins);
		}
		std::vector <std::pair <float, int>> phi;
		std::vector<bool> to_skip(allphi.size());
		for (int j = 0; j < allphi.size(); j++) to_skip[j] = false;
		for (int j = 0; j < allphi.size(); j++)
		{
			if (to_skip[j] == true) continue;
			bool has_double = false;
			for (int k = j; k < allphi.size(); k++)
			{
				if (allphi[j].first == allphi[k].first)
				{
					std::pair <float, int> nph(allphi[j].first, allphi[j].second + allphi[k].second);
					phi.push_back(nph);
					to_skip[k] = true;
					has_double = true;
				}
			}
			if (has_double = false) phi.push_back(allphi[j]);
		}
		std::vector< int> binned_phi(100);
		for (int j = 0; j < phi.size(); j++) {
			float phi_bin_vals = (2 * 3.14159) / 100;
			int binnum;
			for (int k = 0; k < 100; k++)
			{
				if (phi[j].first > k * phi_bin_vals - 3.14159 && phi[j].first < (k + 1) * phi_bin_vals - 3.14159) binnum = k;
			}
			binned_phi[binnum] += phi[j].second;
		}
		for (int j = 0; j < binned_phi.size(); j++)
		{
			hphi->SetBinContent(j, binned_phi[j]);
		}
		hphi->Write();
	}
	f->Close();
	return 0;
}