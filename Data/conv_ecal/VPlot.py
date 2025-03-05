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
import VPStyle
ROOT.SetVPStyle()

# Load some histos from the example file
# (Gaussian limits from 2016 TLA conf)
# 所读文件名称
infile = ROOT.TFile.Open("ECAL_ONLY_CONV.root","READ")
histos = []
legendLines = []

mini=9999.
maxi=0.

# 直方图名称
# energy={"E20.root","E50.root","E100.root","E200.root","E300.root","E500.root","E800.root","E1000.root","E2000.root","E3000.root","E4000.root","E5000.root"}
histo1 = infile.Get("h20")
histo1.SetDirectory(0)
histo1.Scale(1./histo1.Integral())
histos.append(histo1)
# 曲线名称
legendLines.append("20MeV")
histo2 = infile.Get("h50")
histo2.SetDirectory(0)
histo2.Scale(1./histo2.Integral())
histos.append(histo2)
legendLines.append("50MeV")
histo3 = infile.Get("h100")
histo3.SetDirectory(0)
histo3.Scale(1./histo3.Integral())
histos.append(histo3)
legendLines.append("100MeV")
histo4 = infile.Get("h200")
histo4.SetDirectory(0)
histo4.Scale(1./histo4.Integral())
histos.append(histo4)
legendLines.append("200MeV")
#histo5 = infile.Get("h300")
#histo5.SetDirectory(0)
#histo5.Scale(1./histo5.Integral())
#histos.append(histo5)
#legendLines.append("300MeV")
histo6 = infile.Get("h500")
histo6.SetDirectory(0)
histo6.Scale(1./histo6.Integral())
histos.append(histo6)
legendLines.append("500MeV")
#histo7 = infile.Get("h800")
#histo7.SetDirectory(0)
#histo7.Scale(1./histo7.Integral())
#histos.append(histo7)
#legendLines.append("800MeV")
histo8 = infile.Get("h1000")
histo8.SetDirectory(0)
histo8.Scale(1./histo8.Integral())
histos.append(histo8)
legendLines.append("1000MeV")
#histo9 = infile.Get("h2000")
#histo9.SetDirectory(0)
#histo9.Scale(1./histo9.Integral())
#histos.append(histo9)
#legendLines.append("2000MeV")
histo10 = infile.Get("h3000")
histo10.SetDirectory(0)
histo10.Scale(1./histo10.Integral())
histos.append(histo10)
legendLines.append("3000MeV")
#histo11 = infile.Get("h4000")
#histo11.SetDirectory(0)
#histo11.Scale(1./histo11.Integral())
#histos.append(histo11)
#legendLines.append("4000MeV")
histo12 = infile.Get("h5000")
histo12.SetDirectory(0)
histo12.Scale(1./histo12.Integral())
histos.append(histo12)
legendLines.append("5000MeV")



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
goodColours = [ROOT.kPink,ROOT.kGreen,ROOT.kCyan,ROOT.kOrange,ROOT.kBlack,ROOT.kViolet,ROOT.kSpring,ROOT.kBlue,ROOT.kMagenta,ROOT.kRed,ROOT.kYellow,ROOT.kGreen]

# Make a legend.
# These are the locations of the left side, bottom side, right
# side, and top, as fractions of the canvas.
legend = ROOT.TLegend(0.2,0.72,0.52,0.92)
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
legend.SetNColumns(2)

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
  histo.GetYaxis().SetRangeUser(0.0003, 0.5)

  # Make sure we don't get a fill
  histo.SetFillColor(0)

  # Label my axes!!横纵坐标名称
  histo.GetXaxis().SetTitle("E_{ECAL}^{Reco}/E_{Inc}^{Truth}")
  histo.GetYaxis().SetTitle("Fraction")
  # Move the label around if you want
  histo.GetYaxis().SetTitleOffset(1.5)

  # Set the limit for the axes
  #histo.GetXaxis().SetLimits(xRange[0],xRange[1])
  #histo.GetYaxis().SetRangeUser(mini*0.5,maxi*1.4)

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

# Put an VLAST-P Internal label
# I think it has to be Helvetica
myLatex.SetTextSize(0.05)
myLatex.SetTextFont(72)
# These are the x and y coordinates of the bottom left corner of the text
# as fractions of the canvas
myLatex.DrawLatex(0.62,0.85,"VLAST-P")
# Now we switch back to normal font for the "Internal"
myLatex.SetTextFont(42)
myLatex.DrawLatex(0.79,0.85,"Internal")

# Update the canvas
c.Update()

# Save the output as a .eps, a .C, and a .root
# 输出文件名称
c.SaveAs("ECAL_ONLY_CONV.png")
