suppressPackageStartupMessages(library(dplyr))
suppressPackageStartupMessages(library(patchwork))
suppressPackageStartupMessages(library(tidyr))
suppressPackageStartupMessages(library(ggplot2))

degree <- tibble(read.csv("../output/degree_merged.csv"))

degree <- degree %>%
  mutate(
    real = grepl("\\.edges$", graph)
  )

summary <- degree %>%
  group_by(graph) %>%
  summarise(
    total_deg = deg * num,
    total_num = sum(num),
    mean = sum(total_deg) / total_num,
    diff_sq = (deg - mean)^2,
    variance = sum(num * diff_sq) / total_num,
    stdev = variance^0.5,
    cov = stdev / mean
  )

summary %>%
  ggplot(aes(x = mean, y = stdev, color = cov)) +
  geom_jitter()


binomial_names <- c("0.gr", "9.gr", "5.gr")
powerlaw_names <- c("14.gr", "26.gr", "31.gr")

degree %>%
  filter(graph == binomial_names[1]) %>%
  ggplot(aes(x=deg,y=num)) +
  geom_point()

degree %>%
  filter(graph %in% binomial_names[1]) %>%
  ggplot(aes(x=deg,y=num/sum(num))) +
  geom_point() +
  xlab("degree d") +
  ylab("fraction of vertices with degree d") +
  facet_wrap(~graph, scales = "free")

# heterogenous

# bad
degree %>%
  filter(graph %in% powerlaw_names[1]) %>%
  ggplot(aes(x=deg, y=num/sum(num))) +
  geom_point() +
  xlab("degree d") +
  ylab("fraction of vertices with degree d") +
  facet_wrap(~graph, scales = "free")

# logarithmic
degree %>%
  filter(graph %in% powerlaw_names) %>%
  ggplot(aes(x=deg, y=num/sum(num))) +
  geom_point() +
  scale_x_log10() +
  scale_y_log10() +
  xlab("degree d") +
  ylab("fraction of vertices with degree d") +
  annotation_logticks()
  facet_wrap(~graph, scales = "free")

## CCDF

graph_data <- degree %>%
  filter(real) %>%
  group_by(graph) %>%
  arrange(deg) %>%
  mutate(
    cum_count = rev(cumsum(rev(num))),
    ccdf = cum_count / sum(num)
  )

# Define logarithmic breaks
log_breaks <- function() {
  function(limits) {
    10^seq(floor(log10(min(limits))), ceiling(log10(max(limits))), by = 1)
  }
}

minor_breaks <- function(x) {
  unlist(lapply(1:10, function(i) i * 10^x))
}

# Define minor breaks for the x and y axis
x_minor <- log10(min(graph_data$deg)):log10(max(graph_data$deg))
y_minor <- log10(min(graph_data$ccdf)):log10(max(graph_data$ccdf))

ggplot(graph_data, aes(x = deg, y = ccdf, color=graph)) +
  geom_point() +
  scale_x_log10(breaks = log_breaks(), minor_breaks = minor_breaks(x_minor)) +
  scale_y_log10(breaks = log_breaks(), minor_breaks = minor_breaks(y_minor)) +
  labs(x = "degree d", y = "fraction of vertices with degree > d") +
  theme_minimal()

ggplot(graph_data, aes(x = deg, y = ccdf)) +
  geom_line() +
  scale_x_log10(breaks = log_breaks(), minor_breaks = minor_breaks(x_minor)) +
  scale_y_log10(breaks = log_breaks(), minor_breaks = minor_breaks(y_minor)) +
  labs(x = "degree d", y = "fraction of vertices with degree > d") +
  theme_minimal()

ggplot(graph_data, aes(x = deg, y = ccdf)) +
  geom_line() +
  geom_abline(intercept = 1.7, slope = -1.77, col = "red") +
  scale_x_log10(breaks = log_breaks(), minor_breaks = minor_breaks(x_minor)) +
  scale_y_log10(breaks = log_breaks(), minor_breaks = minor_breaks(y_minor)) +
  labs(x = "degree d", y = "fraction of vertices with degree > d") +
  theme_minimal()
