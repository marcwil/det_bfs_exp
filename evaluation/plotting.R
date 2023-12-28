suppressPackageStartupMessages(library(dplyr))
suppressPackageStartupMessages(library(tidyr))
suppressPackageStartupMessages(library(ggplot2))

stats <- tibble(read.csv("../output/stats.csv"))
bfs <- tibble(read.csv("../output/bfs_merged.csv"))

# plot stats
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

# plot bfs data

bfs <- bfs %>%
    group_by(graph) %>%
    summarise(
        dist = mean(dist),
        cost_uni = mean(cost_uni),
        cost_bi = mean(cost_bi)
    )

# join using column graph
data <- bfs %>%
    inner_join(stats, by = "graph")



# create one column bfs_cost with cost of different algorithms (distinguished by 'algo')
longer <- data %>%
  pivot_longer(
      c(cost_uni, cost_bi),
      names_to = "algo",
      values_to = "bfs_cost"
  ) %>%
  mutate(
    algo = factor(algo, levels = c("cost_uni", "cost_bi"),
                  labels = c("uni", "bi")),
    bfs_exponent = log(bfs_cost)/log(m),
  )

longer %>%
  ggplot(aes(x=n, y=bfs_exponent, color = algo)) +
  geom_point()

exponents <- data %>%
  mutate(
    speedup = cost_bi / cost_uni,
    bi_exp = log(cost_bi) / log(m),
    speedup_exp = log(speedup) / log(m)
  )

plot_cost <- exponents %>%
  ggplot(aes(x = cost_uni, y = cost_bi, color = speedup)) +
  geom_point() +
  xlab("Uni-directional cost") +
  ylab("Bi-directional cost")
plot_cost

plot_cost_log <- plot_cost +
  scale_x_log10() + scale_y_log10() + annotation_logticks()
plot_cost_log


plot_exponent <- exponents %>%
  ggplot(aes(x = m, y = bi_exp, color = bi_exp)) +
  geom_point() +
  ylab("Exponent bi-dir. cost")
  scale_x_log10() + scale_y_log10() + annotation_logticks()
plot_exponent

# stats
p_stats <- exponents %>%
  ggplot(aes(x = clustering, y = deg_cov, color = bi_exp)) +
  geom_point() +
  scale_x_log10() + scale_y_log10() + annotation_logticks()
p_stats

p_stats_hidden <- p_stats +
  xlab("Mystery property A") +
  ylab("Mystery property B") +
  scale_color_continuous(name="??")
p_stats_hidden

# write plots to png
ggsave("plots/plot_cost.png", plot_cost, width=20, height=14, unit="cm")
ggsave("plots/plot_cost_log.png", plot_cost_log, width=20, height=14, unit="cm")
ggsave("plots/plot_exponent.png", plot_exponent, width=20, height=14, unit="cm")
ggsave("plots/p_stats.png", p_stats, width=20, height=14, unit="cm")
ggsave("plots/p_stats_hidden.png", p_stats_hidden, width=20, height=14, unit="cm")
