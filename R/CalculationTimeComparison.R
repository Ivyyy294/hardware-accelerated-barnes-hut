## Libraries
library(data.table)
library(ggplot2)
library(Hmisc)
library(car)
library(rstatix)

setwd("D:/00_Git/hardware-accelerated-barnes-hut/Out/10000")

variablesGpu <- data.table::fread(paste("GPU","/variables.csv", sep = ""),fill = TRUE)
variablesCpu <- data.table::fread(paste("CPU","/variables.csv", sep = ""),fill = TRUE)
variablesDirect <- data.table::fread(paste("Direct","/variables.csv", sep = ""),fill = TRUE)

##Check if variables are identical besides method
variablesOk <- identical(variablesGpu[-1], variablesCpu[-1]) && identical(variablesGpu[-1], variablesDirect[-1]) &&
  variablesGpu[[2]][1] == 2 &&
  variablesCpu[[2]][1] == 1 &&
  variablesDirect[[2]][1] == 0

if (variablesOk){
  rowCount <- variablesGpu[[2]][9]
  nCount <- variablesGpu[[2]][3] + 1

  frameTime <- data.table(step = 1:rowCount)
  
  frameTimeGpu <- data.table::fread(paste("GPU","/forceCalcTime.csv", sep = ""), sep = ";")
  frameTimeCpu <- data.table::fread(paste("CPU","/forceCalcTime.csv", sep = ""), sep = ";")
  frameTimeDirect <- data.table::fread(paste("Direct","/forceCalcTime.csv", sep = ""), sep = ";")

  frameTime[, gpu := frameTimeGpu[[1]][1:rowCount]]
  frameTime[, cpu := frameTimeCpu[[1]][1:rowCount]]
  frameTime[, direct := frameTimeDirect[[1]][1:rowCount]]

  frameTimeLong <- melt (frameTime, id.vars = "step"
                         , measure.vars = c("gpu", "cpu", "direct")
                         , variable.name = "method"
                         , value.name = "time")

  ##Test for equal variance
  ## must be <0.05 to reject null hypothesis
  bartlett.test(time ~ method, data = frameTimeLong)

  #perform Welch's ANOVA for p test
  ## must be <0.05 to reject null hypothesis
  oneway.test(time ~ method, data = frameTimeLong, var.equal = FALSE)
  
  #post-hoc
  gamesHowellTable <- games_howell_test(time ~ method, data = frameTimeLong)
  sig_df <- gamesHowellTable %>%
    mutate(
      xmin = group1,
      xmax = group2,
      annotations = p.adj.signif,
      y_position = c(1.05, 1.15, 1.25) * max(frameTimeLong$time)
    )
  
  shapiroTable <- frameTimeLong[
    , .(
      W       = shapiro.test(time)$statistic,
      p_value = shapiro.test(time)$p.value
    ),
    by = method
  ]
  
  ##qq plot
  ggplot(frameTimeLong) +
    aes(sample = time, color = method) +
    stat_qq() +
    stat_qq_line() +
    facet_wrap(~method, scales = "free") +
    labs(color = "Method", y = "MS", x ="Method") +
    ggtitle(sprintf("QQ-Plots per method n = %d", nCount)) +
    theme_minimal()
  
  #box plot
  ggplot(frameTimeLong) +
    aes(x = method, y = time, fill = method) +
    geom_boxplot(alpha = 0.5, outlier.shape = NA) +
    geom_jitter(alpha = 0.2, width = 0.1, size = 0.7) +
    facet_wrap(~method, scales = "free_y") +
    labs(fill = "Method", y = "MS", x ="Method") +
    ggtitle(sprintf("Box-Plots per method n = %d", nCount)) +
    theme_minimal()
  
  ##Scatter plot
  ggplot (frameTimeLong) +
    aes(x = method, y = time, color = method) +
    geom_jitter(alpha = 0.3, width = 0.5) +
    scale_y_log10(breaks = scales::log_breaks(n = 10)) +
    labs(color = "Method", y = "MS", x ="Method") +
    ggtitle(sprintf("Force calculation time comparison n = %d", nCount)) +
    theme_minimal()+
    ##show mean per group
    stat_summary(fun = mean, geom = "point", 
                 size = 3, color = "black", shape = 18)+
    stat_summary(fun.data = mean_cl_normal, 
                 geom = "errorbar", 
                 width = 0.2, color = "black")
  
  ##Line Plot new
  ggplot(frameTimeLong) +
    aes(x = step, y = time, color = method) +
    geom_line(linewidth = 1, alpha = 0.8) +
    scale_y_log10(breaks = scales::log_breaks(n = 10)) +
    labs(color = "Method", y = "MS", x = "Fixed Time Step") +
    ggtitle(sprintf("Force calculation time comparison n = %d", nCount)) +
    theme_minimal()

}else{
  warning("Invalid experiment data! The experiment variables of GPU,CPU and Direct doe not match!")
}

