## Libraries
library(data.table)
library(ggplot2)

setwd("D:/00_Git/hardware-accelerated-barnes-hut/Out")

variablesGpu <- data.table::fread(paste("GPU","/variables.csv", sep = ""),fill = TRUE)
variablesCpu <- data.table::fread(paste("CPU","/variables.csv", sep = ""),fill = TRUE)
variablesDirect <- data.table::fread(paste("Direct","/variables.csv", sep = ""),fill = TRUE)

##Check if variables are identical besides method
variablesOk <- identical(variablesGpu[-1], variablesCpu[-1]) && identical(variablesGpu[-1], variablesDirect[-1]) &&
  variablesGpu[[2]][1] == 2 &&
  variablesCpu[[2]][1] == 1 &&
  variablesDirect[[2]][1] == 0

if (variablesOk){
  rowCount <- variablesGpu[[2]][10]
  nCount <- variablesGpu[[2]][4] + 1

  frameTime <- data.table(step = 1:rowCount)
  
  frameTimeGpu <- data.table::fread(paste("GPU","/forceCalcTime.csv", sep = ""), sep = ";")
  frameTimeCpu <- data.table::fread(paste("CPU","/forceCalcTime.csv", sep = ""), sep = ";")
  frameTimeDirect <- data.table::fread(paste("Direct","/forceCalcTime.csv", sep = ""), sep = ";")

  frameTime[, gpu := frameTimeGpu[[1]][1:rowCount]]
  frameTime[, cpu := frameTimeCpu[[1]][1:rowCount]]
  frameTime[, direct := frameTimeDirect[[1]][1:rowCount]]

  ggplot(frameTime, aes(x = step)) +
    geom_line(aes(y = direct, color="Direct"))+
    geom_line(aes(y = cpu, color="CPU")) +
    geom_line(aes(y = gpu, color="GPU")) +
    scale_color_manual(values=c("GPU" = "red", "CPU" = "blue", "Direct" = "green"))+
    labs(color="Method", y = "MS", x ="Fixed Time Step")+
    ggtitle(sprintf("Force calculation time comparison n = %d", nCount))
    
}else{
  warning("Invalid experiment data! The experiment variables of GPU,CPU and Direct doe not match!")
}

