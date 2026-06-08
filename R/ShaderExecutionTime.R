## Libraries
library(data.table)
library(ggplot2)
library(Hmisc)

setwd("D:/00_Git/hardware-accelerated-barnes-hut/Out/10241")

variablesGpu <- data.table::fread(paste("GPU","/variables.csv", sep = ""),fill = TRUE)
nCount <- variablesGpu[[2]][4] + 1

frameTime <- data.table::fread(paste("GPU","/shader.csv", sep = ""), sep = ";")
frameTime [, step := .I]

frameLong <- melt(frameTime, id.vars = "step", variable.name = "series", value.name = "value")

ggplot(frameLong, aes(x = step, y = value,
                      color = series)) +
  geom_line(size = 1) +
  #geom_point(aes(shape = series), size = 2) +
  scale_y_log10(breaks = scales::log_breaks(n = 10)) +
  scale_color_manual(values = rainbow(length(unique(frameLong$series)))) +
  scale_shape_manual(values = seq_along(unique(frameLong$series))) +
  labs(color = "Shader", shape = "Shader",
       y = "MS", x = "Fixed Time Step") +
  ggtitle(sprintf("Shader Execution Time n = %d", nCount)) +
  theme_minimal()


