suppressPackageStartupMessages(library(dplyr))
suppressPackageStartupMessages(library(patchwork))
suppressPackageStartupMessages(library(tidyr))
suppressPackageStartupMessages(library(ggplot2))

stats <- tibble(read.csv("../output/stats.csv"))

stats %>%
  mutate(
    maxdeg_log = log(maxdeg)
  ) %>%
  pivot_longer(c(n,avgdeg,maxdeg_log),names_to = 'statistic', values_to = 'val') %>%
  ggplot(aes(x=statistic, y=val)) +
  geom_boxplot() +
  geom_violin(alpha=0.3) +
  facet_wrap(~statistic,scales = 'free')

stats %>%
  mutate(
    maxdeg_log = log(maxdeg)
  ) %>%
  pivot_longer(c(n,avgdeg,maxdeg_log),names_to = 'statistic', values_to = 'val') %>%
  ggplot(aes(y=val)) +
  geom_histogram() +
  facet_wrap(~statistic,scales = 'free')

p1 <-
  stats %>%
  ggplot(aes(y=avgdeg)) +
  geom_density() +
  ggtitle("Average degree")
p2 <-
  stats %>%
  ggplot(aes(y=maxdeg)) +
  geom_density() +
  ggtitle("Maximum degree")
p3 <-
  stats %>%
  ggplot(aes(y=maxdeg)) +
  geom_density() +
  scale_y_log10() + annotation_logticks(sides = 'l') +
  ggtitle("Maximum degree")
degree_plots <- p1|p2|p3

n_plot <- stats %>%
  ggplot(aes(x=n)) +
  ggtitle("Network size (nodes)")
  geom_histogram()
n_plot

# sive degree_plots and n_plot to png in /plots
ggsave("plots/degree_plots.png", degree_plots, width=25, height=14, unit="cm")
ggsave("plots/n_plot.png", n_plot, width=25, height=14, unit="cm")
