# An example python script for making a tidy plot with several lines on it
# I'm using TProfiles because that's what is in the TIDA output, but 
# the standard histogram is a "TH1F" and the exact same commands work for it.
# If you want to use graphs instead of histograms you can check out
# the other example script in here.

import ROOT
import math

# Imports ATLAS style for plotting
# You have to have set it up first (see README for instructions)
# You can run it without this but it will have an ugly stats box and so on
# that you'd have to turn off manually.
import AtlasStyle
ROOT.gROOT.SetBatch(True)
ROOT.SetAtlasStyle()
def plot_etotal():
	# Load some histos from the example file
	# (Gaussian limits from 2016 TLA conf)
	infile = ROOT.TFile.Open("result.root","READ")
	histos = []
	legendLines = []
	
	mini=9999.
	maxi=0.

	hname = ["hconve20","hconve50","hconve100","hconve200","hconve300","hconve500","hconve800","hconve1000","hconve2000","hconve3000","hconve4000","hconve5000"]
	legs = ["20","50","100","200","300","500","800","1000","2000","3000","4000","5000"]
	
	for i in range(len(hname)-1,-1,-1):
		histo = infile.Get(hname[i])
		histo.SetDirectory(0)
		histo.Scale(1./histo.Integral())
		histos.append(histo)
		legendLines.append(legs[hname.index(hname[i])])
	
	# Close the input file
	infile.Close()
	
	# Make a canvas to put the plot on.
	# We don't want log axes for this plot, 
	# but if you do you can control them here.
	c = ROOT.TCanvas("canvas",'',0,0,1024,768)
	c.SetLogx(False)
	c.SetLogy(True)
	c.SetGridx(0)
	c.SetGridy(0)
	
	# Decide what x and y range to use in the display.
	xRange = [-3.2,3.2]
	yRange = [0.9,1.1]
	
	# Decide what colours to use.
	# These ones look decent, but obviously use
	# whatever you like best.
	goodColours = [ROOT.kRed+1, ROOT.kBlue+1, ROOT.kGreen+2, ROOT.kMagenta+2, ROOT.kOrange+7, ROOT.kCyan+1, ROOT.kPink+1, ROOT.kViolet+1, ROOT.kTeal+1, ROOT.kSpring+5, ROOT.kYellow+2, ROOT.kAzure+4, ROOT.kGray+2, ROOT.kBlack, ROOT.kOrange+2]

	# Make a legend.
	# These are the locations of the left side, bottom side, right
	# side, and top, as fractions of the canvas.
	legend = ROOT.TLegend(0.6,0.72,0.92,0.92)
	legend.SetNColumns(2)
	# Make the text a nice fond, and big enough
	legend.SetTextFont(42)
	legend.SetTextSize(0.04)
	# A few more formatting things .....
	legend.SetBorderSize(0)
	legend.SetLineColor(0)
	legend.SetLineStyle(1)
	legend.SetLineWidth(3)
	legend.SetFillColor(0)
	legend.SetFillStyle(0)
	
	# Draw each histogram.
	# You really shouldn't put two histograms with different
	# x axes on the same plot - I'm only doing it here
	# to show you how to draw multiple plots on the same
	# canvas.
	for histo, line in zip(histos,legendLines) :
	
	  index = histos.index(histo)
	  colour = goodColours[index]
	
	  # Set up marker to look nice
	  histo.SetMarkerColor(colour)
	  histo.SetMarkerSize(1)  
	  histo.SetMarkerStyle(20+index)
	
	  # Set up line to look nice
	  histo.SetLineColor(colour)
	  histo.SetLineWidth(3)
	  histo.SetLineStyle(1)
	
	  # Make sure we don't get a fill
	  histo.SetFillColor(0)
	
	  # Label my axes!!
	  histo.GetXaxis().SetTitle("Converter E[MeV]")
	  histo.GetYaxis().SetTitle("Fraction")
	  # Move the label around if you want
	  histo.GetYaxis().SetTitleOffset(1.5)
	
	  # Set the limit for the axes
	  histo.GetXaxis().SetRangeUser(0.,100.)
	  histo.GetYaxis().SetRangeUser(1e-4,1)
	
	  if index==0 :
	    histo.Draw("P") # Draw data points (you'll get error bars by default)
	  else :
	    histo.Draw("P SAME") # SAME means don't get rid of the previous stuff on the canvas
	
	  # Fill entry into legend
	  # "PL" means both the line and point style
	  # will show up in the legend.
	  legend.AddEntry(histo,line,"PL")
	
	# Actually draw the legend
	legend.Draw()
	
	# This is one way to draw text on the plot
	myLatex = ROOT.TLatex()
	myLatex.SetTextColor(ROOT.kBlack)
	myLatex.SetNDC()
	
	# Put an ATLAS Internal label
	# I think it has to be Helvetica
	myLatex.SetTextSize(0.05)
	myLatex.SetTextFont(72)
	# These are the x and y coordinates of the bottom left corner of the text
	# as fractions of the canvas
	myLatex.DrawLatex(0.18,0.18,"VLAST-P")
	# Now we switch back to normal font for the "Internal"
	myLatex.SetTextFont(42)
	myLatex.DrawLatex(0.35,0.18,"Internal")
	
	# Update the canvas
	c.Update()
	
	# Save the output as a .eps, a .C, and a .root
	c.SaveAs("conve.png")

def plot_conve():
	# Load some histos from the example file
	# (Gaussian limits from 2016 TLA conf)
	infile = ROOT.TFile.Open("result.root","READ")
	histos = []
	legendLines = []
	
	mini=9999.
	maxi=0.

	hname = ["hconve20","hconve50","hconve100","hconve200","hconve300","hconve500","hconve800","hconve1000","hconve2000","hconve3000","hconve4000","hconve5000"]
	legs = ["20","50","100","200","300","500","800","1000","2000","3000","4000","5000"]
	
	for i in range(len(hname)-1,-1,-1):
		histo = infile.Get(hname[i])
		histo.SetDirectory(0)
		histo.Scale(1./histo.Integral())
		histos.append(histo)
		legendLines.append(legs[hname.index(hname[i])])
	
	# Close the input file
	infile.Close()
	
	# Make a canvas to put the plot on.
	# We don't want log axes for this plot, 
	# but if you do you can control them here.
	c = ROOT.TCanvas("canvas",'',0,0,1024,768)
	c.SetLogx(False)
	c.SetLogy(True)
	c.SetGridx(0)
	c.SetGridy(0)
	
	# Decide what x and y range to use in the display.
	xRange = [-3.2,3.2]
	yRange = [0.9,1.1]
	
	# Decide what colours to use.
	# These ones look decent, but obviously use
	# whatever you like best.
	goodColours = [ROOT.kRed+1, ROOT.kBlue+1, ROOT.kGreen+2, ROOT.kMagenta+2, ROOT.kOrange+7, ROOT.kCyan+1, ROOT.kPink+1, ROOT.kViolet+1, ROOT.kTeal+1, ROOT.kSpring+5, ROOT.kYellow+2, ROOT.kAzure+4, ROOT.kGray+2, ROOT.kBlack, ROOT.kOrange+2]

	# Make a legend.
	# These are the locations of the left side, bottom side, right
	# side, and top, as fractions of the canvas.
	legend = ROOT.TLegend(0.6,0.72,0.92,0.92)
	legend.SetNColumns(2)
	# Make the text a nice fond, and big enough
	legend.SetTextFont(42)
	legend.SetTextSize(0.04)
	# A few more formatting things .....
	legend.SetBorderSize(0)
	legend.SetLineColor(0)
	legend.SetLineStyle(1)
	legend.SetLineWidth(3)
	legend.SetFillColor(0)
	legend.SetFillStyle(0)
	
	# Draw each histogram.
	# You really shouldn't put two histograms with different
	# x axes on the same plot - I'm only doing it here
	# to show you how to draw multiple plots on the same
	# canvas.
	for histo, line in zip(histos,legendLines) :
	
	  index = histos.index(histo)
	  colour = goodColours[index]
	
	  # Set up marker to look nice
	  histo.SetMarkerColor(colour)
	  histo.SetMarkerSize(1)  
	  histo.SetMarkerStyle(20+index)
	
	  # Set up line to look nice
	  histo.SetLineColor(colour)
	  histo.SetLineWidth(3)
	  histo.SetLineStyle(1)
	
	  # Make sure we don't get a fill
	  histo.SetFillColor(0)
	
	  # Label my axes!!
	  histo.GetXaxis().SetTitle("Converter E[MeV]")
	  histo.GetYaxis().SetTitle("Fraction")
	  # Move the label around if you want
	  histo.GetYaxis().SetTitleOffset(1.5)
	
	  # Set the limit for the axes
	  histo.GetXaxis().SetRangeUser(0.,100.)
	  histo.GetYaxis().SetRangeUser(1e-4,1)
	
	  if index==0 :
	    histo.Draw("P") # Draw data points (you'll get error bars by default)
	  else :
	    histo.Draw("P SAME") # SAME means don't get rid of the previous stuff on the canvas
	
	  # Fill entry into legend
	  # "PL" means both the line and point style
	  # will show up in the legend.
	  legend.AddEntry(histo,line,"PL")
	
	# Actually draw the legend
	legend.Draw()
	
	# This is one way to draw text on the plot
	myLatex = ROOT.TLatex()
	myLatex.SetTextColor(ROOT.kBlack)
	myLatex.SetNDC()
	
	# Put an ATLAS Internal label
	# I think it has to be Helvetica
	myLatex.SetTextSize(0.05)
	myLatex.SetTextFont(72)
	# These are the x and y coordinates of the bottom left corner of the text
	# as fractions of the canvas
	myLatex.DrawLatex(0.18,0.18,"VLAST-P")
	# Now we switch back to normal font for the "Internal"
	myLatex.SetTextFont(42)
	myLatex.DrawLatex(0.35,0.18,"Internal")
	
	# Update the canvas
	c.Update()
	
	# Save the output as a .eps, a .C, and a .root
	c.SaveAs("conve.png")

def plot_epangle():
	# Load some histos from the example file
	# (Gaussian limits from 2016 TLA conf)
	infile = ROOT.TFile.Open("result.root","READ")
	histos = []
	legendLines = []
	
	mini=9999.
	maxi=0.

	hname = ["hep20","hep50","hep100","hep200","hep300","hep500","hep800","hep1000","hep2000","hep3000","hep4000","hep5000"]
	legs = ["20","50","100","200","300","500","800","1000","2000","3000","4000","5000"]
	
	for name in hname:
		histo = infile.Get(name)
		histo.SetDirectory(0)
		histo.Scale(1./histo.Integral())
		histos.append(histo)
		legendLines.append(legs[hname.index(name)])
	
	# Close the input file
	infile.Close()
	
	# Make a canvas to put the plot on.
	# We don't want log axes for this plot, 
	# but if you do you can control them here.
	c = ROOT.TCanvas("canvas",'',0,0,1024,768)
	c.SetLogx(True)
	c.SetLogy(True)
	c.SetGridx(0)
	c.SetGridy(0)
	
	# Decide what x and y range to use in the display.
	xRange = [-3.2,3.2]
	yRange = [0.9,1.1]
	
	# Decide what colours to use.
	# These ones look decent, but obviously use
	# whatever you like best.
	goodColours = [ROOT.kRed+1, ROOT.kBlue+1, ROOT.kGreen+2, ROOT.kMagenta+2, ROOT.kOrange+7, ROOT.kCyan+1, ROOT.kPink+1, ROOT.kViolet+1, ROOT.kTeal+1, ROOT.kSpring+5, ROOT.kYellow+2, ROOT.kAzure+4, ROOT.kGray+2, ROOT.kBlack, ROOT.kOrange+2]

	# Make a legend.
	# These are the locations of the left side, bottom side, right
	# side, and top, as fractions of the canvas.
	legend = ROOT.TLegend(0.6,0.72,0.92,0.92)
	legend.SetNColumns(2)
	# Make the text a nice fond, and big enough
	legend.SetTextFont(42)
	legend.SetTextSize(0.04)
	# A few more formatting things .....
	legend.SetBorderSize(0)
	legend.SetLineColor(0)
	legend.SetLineStyle(1)
	legend.SetLineWidth(3)
	legend.SetFillColor(0)
	legend.SetFillStyle(0)
	
	# Draw each histogram.
	# You really shouldn't put two histograms with different
	# x axes on the same plot - I'm only doing it here
	# to show you how to draw multiple plots on the same
	# canvas.
	for histo, line in zip(histos,legendLines) :
	
	  index = histos.index(histo)
	  colour = goodColours[index]
	
	  # Set up marker to look nice
	  histo.SetMarkerColor(colour)
	  histo.SetMarkerSize(1)  
	  histo.SetMarkerStyle(20+index)
	
	  # Set up line to look nice
	  histo.SetLineColor(colour)
	  histo.SetLineWidth(3)
	  histo.SetLineStyle(1)
	
	  # Make sure we don't get a fill
	  histo.SetFillColor(0)
	
	  # Label my axes!!
	  histo.GetXaxis().SetTitle("e^{+}e^{-} Angle[Rad]")
	  histo.GetYaxis().SetTitle("Fraction")
	  # Move the label around if you want
	  histo.GetYaxis().SetTitleOffset(1.5)
	
	  # Set the limit for the axes
	  #histo.GetXaxis().SetLimits(xRange[0],xRange[1])
	  histo.GetYaxis().SetRangeUser(1e-4,10)
	
	  if index==0 :
	    histo.Draw("P") # Draw data points (you'll get error bars by default)
	  else :
	    histo.Draw("P SAME") # SAME means don't get rid of the previous stuff on the canvas
	
	  # Fill entry into legend
	  # "PL" means both the line and point style
	  # will show up in the legend.
	  legend.AddEntry(histo,line,"PL")
	
	# Actually draw the legend
	legend.Draw()
	
	# This is one way to draw text on the plot
	myLatex = ROOT.TLatex()
	myLatex.SetTextColor(ROOT.kBlack)
	myLatex.SetNDC()
	
	# Put an ATLAS Internal label
	# I think it has to be Helvetica
	myLatex.SetTextSize(0.05)
	myLatex.SetTextFont(72)
	# These are the x and y coordinates of the bottom left corner of the text
	# as fractions of the canvas
	myLatex.DrawLatex(0.18,0.18,"VLAST-P")
	# Now we switch back to normal font for the "Internal"
	myLatex.SetTextFont(42)
	myLatex.DrawLatex(0.35,0.18,"Internal")
	
	# Update the canvas
	c.Update()
	
	# Save the output as a .eps, a .C, and a .root
	c.SaveAs("epangle.png")
def plot_reso():
	# Load some histos from the example file
	# (Gaussian limits from 2016 TLA conf)
	infile = ROOT.TFile.Open("result.root","READ")
	histos = []
	legendLines = []
	
	mini=9999.
	maxi=0.
	
	histo1 = infile.Get("hreso")
	histos.append(histo1)
	legendLines.append("FWHM #font[22]{/} 2.35")
	
	# Close the input file
	infile.Close()
	
	# Make a canvas to put the plot on.
	# We don't want log axes for this plot, 
	# but if you do you can control them here.
	c = ROOT.TCanvas("canvas",'',0,0,1024,768)
	c.SetLogx(False)
	c.SetLogy(False)
	c.SetGridx(0)
	c.SetGridy(0)
	
	# Decide what x and y range to use in the display.
	xRange = [-3.2,3.2]
	yRange = [0.9,1.1]
	
	# Decide what colours to use.
	# These ones look decent, but obviously use
	# whatever you like best.
	goodColours = [ROOT.kRed+1, ROOT.kBlue+1, ROOT.kGreen+2, ROOT.kMagenta+2, ROOT.kOrange+7, ROOT.kCyan+1, ROOT.kPink+1, ROOT.kViolet+1, ROOT.kTeal+1, ROOT.kSpring+5, ROOT.kYellow+2, ROOT.kAzure+4, ROOT.kGray+2, ROOT.kBlack, ROOT.kOrange+2]
	
	# Make a legend.
	# These are the locations of the left side, bottom side, right
	# side, and top, as fractions of the canvas.
	legend = ROOT.TLegend(0.6,0.72,0.92,0.92)
	# Make the text a nice fond, and big enough
	legend.SetTextFont(42)
	legend.SetTextSize(0.04)
	# A few more formatting things .....
	legend.SetBorderSize(0)
	legend.SetLineColor(0)
	legend.SetLineStyle(1)
	legend.SetLineWidth(3)
	legend.SetFillColor(0)
	legend.SetFillStyle(0)
	
	# Draw each histogram.
	# You really shouldn't put two histograms with different
	# x axes on the same plot - I'm only doing it here
	# to show you how to draw multiple plots on the same
	# canvas.
	for histo, line in zip(histos,legendLines) :
	
	  index = histos.index(histo)
	  colour = goodColours[index]
	
	  # Set up marker to look nice
	  histo.SetMarkerColor(colour)
	  histo.SetMarkerSize(1)  
	  histo.SetMarkerStyle(20+index)
	
	  # Set up line to look nice
	  histo.SetLineColor(colour)
	  histo.SetLineWidth(3)
	  histo.SetLineStyle(1)
	
	  # Make sure we don't get a fill
	  histo.SetFillColor(0)
	
	  # Label my axes!!
	  histo.GetXaxis().SetTitle("E_{Reco}[MeV]")
	  histo.GetYaxis().SetTitle("#frac{#sigma_{E}}{E}")
	  # Move the label around if you want
	  histo.GetYaxis().SetTitleOffset(1.5)
	
	  # Set the limit for the axes
	  #histo.GetXaxis().SetLimits(xRange[0],xRange[1])
	  #histo.GetYaxis().SetRangeUser(mini*0.5,maxi*1.4)
	
	  if index==0 :
	    histo.Draw("APL") # Draw data points (you'll get error bars by default)
	  else :
	    histo.Draw("P SAME") # SAME means don't get rid of the previous stuff on the canvas
	
	  # Fill entry into legend
	  # "PL" means both the line and point style
	  # will show up in the legend.
	  legend.AddEntry(histo,line,"PL")
	
	# Actually draw the legend
	legend.Draw()
	
	# This is one way to draw text on the plot
	myLatex = ROOT.TLatex()
	myLatex.SetTextColor(ROOT.kBlack)
	myLatex.SetNDC()
	
	# Put an ATLAS Internal label
	# I think it has to be Helvetica
	myLatex.SetTextSize(0.05)
	myLatex.SetTextFont(72)
	# These are the x and y coordinates of the bottom left corner of the text
	# as fractions of the canvas
	myLatex.DrawLatex(0.18,0.18,"VLAST-P")
	# Now we switch back to normal font for the "Internal"
	myLatex.SetTextFont(42)
	myLatex.DrawLatex(0.35,0.18,"Internal")
	
	# Update the canvas
	c.Update()
	
	# Save the output as a .eps, a .C, and a .root
	c.SaveAs("reso.png")

if __name__=="__main__":
	plot_reso()
	#plot_epangle()
	#plot_conve()
	#plot_etotal()
