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
ROOT.SetAtlasStyle()

# Load some histos from the example file
# (Gaussian limits from 2016 TLA conf)
infile = ROOT.TFile.Open("hist.root","READ")
histos = []
legendLines = []

mini=9999.
maxi=0.

histo1 = infile.Get("hs")
histo1.SetDirectory(0)
histos.append(histo1)
print("Shield Mean: ",histo1.GetMean()," RMS: ",histo1.GetRMS())
legendLines.append("With Shield")

histo2 = infile.Get("hn")
histo2.SetDirectory(0)
histos.append(histo2)
print("Shield Mean: ",histo2.GetMean()," RMS: ",histo2.GetRMS())
legendLines.append("Without shield")

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
goodColours = [ROOT.kCyan+2,ROOT.kBlue+1,ROOT.kMagenta+1,ROOT.kOrange,ROOT.kBlack]

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
  histo.GetXaxis().SetTitle("Reco_E [MeV]")
  histo.GetYaxis().SetTitle("Events/bin")
  # Move the label around if you want
  histo.GetYaxis().SetTitleOffset(1.5)

  # Set the limit for the axes
  #histo.GetXaxis().SetLimits(xRange[0],xRange[1])
  histo.GetYaxis().SetRangeUser(0,15000.)

  if index==0 :
    histo.Draw("H") # Draw data points (you'll get error bars by default)
  else :
    histo.Draw("H SAME") # SAME means don't get rid of the previous stuff on the canvas

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
myLatex.DrawLatex(0.18,0.18,"VLast")
# Now we switch back to normal font for the "Internal"
myLatex.SetTextFont(42)
myLatex.DrawLatex(0.35,0.18,"Internal")

# Update the canvas
c.Update()

# Save the output as a .eps, a .C, and a .root
c.SaveAs("EM_reso.png")
