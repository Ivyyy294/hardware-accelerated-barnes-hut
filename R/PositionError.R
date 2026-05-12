## Libraries
library(data.table)
library(rgl)

PlotPosition <- function (folder, color)
{
  ##Data definition
  folderPos = paste(folder,"/positions.csv", sep = "")
  gpuPosData <- data.table::fread(paste(folder,"/positions.csv", sep = ""))
  gpuSettingsData <- data.table::fread(paste(folder,"/variables.csv", sep = ""),fill = TRUE)
  
  ##Get settings values Column/Row
  numElements <- gpuSettingsData[[2]][2]
  numRows <- gpuSettingsData[[2]][8]
  
  for (i in 1:numElements)
  {
    base <- (i - 1) * 3
    xPos <- gpuPosData[[base+1]]
    yPos <- gpuPosData[[base+2]]
    zPos <- gpuPosData[[base+3]]
    
    lines3d(x = xPos,  z = zPos, y = yPos, col = color, lwd = 2)
  }
}

setwd("D:/00_Git/hardware-accelerated-barnes-hut/Out")

open3d()
axes3d()
title3d(xlab="x", ylab = "y", zlab="z")

PlotPosition ("GPU", "blue")
PlotPosition ("CPU", "red")
