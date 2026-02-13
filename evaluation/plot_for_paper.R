#!
#/bin/Rscript
source("helper.R")

# variables for common labels
lab_estexp <- TeX("Estimated exponent $\\log_m(c)$")
lab_expover <- TeX("expansion overlap / $d_\\alpha(s,t)$")


print("Reading stats csv file..")
stats <- tibble(read.csv("../output/stats.csv"))
stats <- stats %>%
  mutate(
    girg = startsWith(graph, "girg"),
    girg_scale = startsWith(graph, "scale"),
    real = !girg & !girg_scale,
    tree = m == (n - 1),
    sparse = avgdeg < 100,
    #interesting = real & !tree & sparse,
    interesting = real,
    locality = (local_clustering + global_clustering + distance_locality) /3,
    locality2 = (local_clustering + distance_locality) / 2
  )
stats

girg_stats <- stats %>%
  filter(girg | girg_scale) %>%
  extract(
    col = graph,
    into = c("n_gen", "ple", "temp", "deg", "sample"),
    regex = "scale(\\d+)_([\\d\\.]+)_(\\d+(?:\\.\\d+)?)_(\\d+)_(\\d+)\\.edges",
    remove = FALSE,
    convert = TRUE
  )


print("Reading bfs csv file..")
bfs_raw <- tibble(read.csv("../output/bfs_merged.csv"))
bfs <- bfs_raw %>%
  group_by(graph) %>%
  summarise(
    dist = mean(dist),
    cost_uni = mean(cost_uni),
    cost_bi = mean(cost_bi)
  ) %>%
  left_join(stats, by='graph') %>%
  mutate(
    uni_exp_m = log(cost_uni) / log(m),
    bi_exp_m = log(cost_bi) / log(m),
    bi_exp_uni = log(cost_bi) / log(cost_uni),
  )
print(bfs)

print("Reading expan_merged.csv")
expan_raw <- tibble(read.csv("../output/expan_merged.csv"))
print("Creating expan table.")
expan <- expan_raw %>%
  group_by(graph, b, alpha) %>%
  mutate(
    # relative overlap theorem
    overlap_relative = overlap / dist,
    relevant_overlap_rel = if_else(rel_dist <= 0, 1, overlap / rel_dist),
    relevant_overlap_rel = pmin(1, pmax(-1, relevant_overlap_rel)),
    wc_exponent = pmax(0,
                       pmin(1,
                            1 -
                            (relevant_overlap_rel*(1-alpha) / (log(bplus) / log(b) + relevant_overlap_rel))
                            )
                       ),
  ) %>%
  summarise(
    across(everything(), mean)
  ) %>%
  left_join(bfs, by='graph')

print("Reading rho_expan_merged.csv")
rho_expan_raw <- tibble(read.csv('../output/rho_expan_merged.csv'))
rho_expan <- rho_expan_raw %>%
  group_by(graph, b) %>%
  summarise(across(everything(), mean)) %>%
  left_join(bfs, by = "graph")

print("Done reading csv files. Generating plots.")

# plot expansion overlap
some_b <- 2
some_alpha <- 0.1

# Relevant expansion overlap hex
overlap_hexplot <- function(data) {
  data %>%
    ggplot(aes(x=relevant_overlap_rel, y=bi_exp_m)) +
    geom_hex(bins=15, color="lightgray") +
    labs(
      x = lab_expover,
      y = lab_estexp
  #    x = lab_expover,
  #    y = lab_estexp
    ) +
    scale_fill_gradient2("Nr. of networks")
  #  scale_fill_continuous(
  #    trans='log',
  #    breaks=c(1,4,16,64)
  #  ) +
}

# Plot overlap on interesting real networks
expan %>%
  filter(interesting) %>%
  filter(b==some_b, alpha==some_alpha) %>%
  overlap_hexplot
create_pdf('output.pdf/overlap_hex_real.pdf', last_plot(), width=0.48,height=0.33)

expan %>%
  filter(interesting) %>%
  overlap_hexplot +
  facet_grid(b~alpha)
create_pdf('output.pdf/overlap_hex_real_ab.pdf', last_plot(), width=1, height=0.9)

# Plot overlap on girgs
expan %>%
  filter(girg) %>%
  filter(b==some_b, alpha==some_alpha) %>%
  overlap_hexplot
create_pdf('output.pdf/overlap_hex_girg.pdf', last_plot(), width=0.48,height=0.33)


# expansion overlap histogram
overlap_histogram <- function(data) {
  data %>%
    mutate(exp_class = cut(
            bi_exp_m,
            breaks = c(0, 0.75, 2),
            labels = c("<0.75", "≥0.75")
          )) %>%
    ggplot(aes(
      relevant_overlap_rel,
      fill=exp_class
    )) +
    geom_histogram(bins=30) +
    scale_fill_viridis_d() +
    labs(
      x = lab_expover,
      y = "Number of networks",
      fill = "Est. exponent"
    )
}

# overlap histogram real world networks
expan %>%
  filter(interesting) %>%
  filter(b==some_b, alpha==some_alpha) %>%
  overlap_histogram
create_pdf('output.pdf/overlap_hist_real.pdf', last_plot(), width = 0.48, height = 0.33)

expan %>%
  filter(interesting) %>%
  overlap_histogram +
  facet_grid(b~alpha)
create_pdf('output.pdf/overlap_hist_real_ab.pdf', last_plot(), width = 1, height = 1)

# …and girg
expan %>%
  filter(girg) %>%
  filter(b==some_b, alpha==some_alpha) %>%
  overlap_histogram
create_pdf('output.pdf/overlap_hist_girg.pdf', last_plot(), width = 0.48, height = 0.33)

expan %>%
  filter(girg) %>%
  overlap_histogram +
  facet_grid(b~alpha)
create_pdf('output.pdf/overlap_hist_girg_ab.pdf', last_plot(), width = 1, height = 1)

# simple exponent
plot_easy_exp <- function(data) {
  data %>%
    mutate(
      thm4c = relevant_overlap_rel / (log(m)/log(2))
    ) %>%
    ggplot(aes(x = 1-(thm4c/2), y = bi_exp_m)) +
  #  geom_bin2d(position="identity", color="black") + 
    geom_hex(bins=15, position="identity", color="lightgray") + 
    scale_fill_gradient2(
  #    low = "white",
  #    high = "blue",
      trans = "log",
      breaks = c(1,4,16,64)
    ) +
    labs(
      x = "Exponent by Theorem 4",
      y = lab_estexp,
      fill = "Nr. of networks"
    )  
}

expan %>%
  filter(b == some_b, alpha == some_alpha) %>%
  plot_easy_exp
create_pdf("output.pdf/overlap_easyexp_hex_real.pdf", last_plot(), width=0.48, height=0.33)
  
expan %>%
  plot_easy_exp +
  facet_grid(b~alpha, scales = "free")
create_pdf("output.pdf/overlap_easyexp_hex_real_ab.pdf", last_plot(), width=0.48, height=0.33)


# wc exponent
plot_wcexp <- function(data) {
  data %>%
    ggplot(aes(x=wc_exponent, y=bi_exp_m)) +
    geom_hex(bins=15, color = "lightgray") +
    scale_fill_gradient2(
#      trans = "log",
#      breaks = c(1,4,16,64,256)
    ) +
    labs(
      x = "Exponent by Theorem 6",
      y = lab_estexp,
      fill = "Nr. of networks"
    )
}

expan %>%
  filter(b==some_b, alpha==some_alpha) %>%
  filter(interesting) %>%
  plot_wcexp
create_pdf("output.pdf/overlap_wcexp_hex_real.pdf", last_plot(), width=0.48, height=0.33)

expan %>%
  filter(interesting) %>%
  plot_wcexp +
  facet_grid(b~alpha, scales = "free")
create_pdf("output.pdf/overlap_wcexp_hex_real_ab.pdf", last_plot(), width=1, height=1)


# rho expan
rho_hex <- function(data) {
  data %>%
    ggplot(aes(x=optimal_rho-optimal_rhomax,y=bi_exp_m)) +
    geom_hex(bins=15, color="lightgray") +
      labs(
        x = TeX("$1/(1+\\rho) - 1/(1+\\rho_\\max)$"),
        y = lab_estexp,
        fill = "Nr. of networks"
    #    x = "expansion overlap / $d_\\alpha(s,t)$",
    #    y = lab_estexp
      ) +
      scale_fill_gradient2()
}


rho_expan %>%
  filter(b == some_b,) %>%
  filter(interesting) %>%
  rho_hex
create_pdf("output.pdf/rho_hex_real.pdf", last_plot(), width=0.48, height=0.33)

rho_expan %>%
  filter(interesting) %>%
  rho_hex +
  facet_wrap(~b, ncol=5)
create_pdf("output.pdf/rho_hex_real_b.pdf", last_plot(), width=1, height=0.28)

rho_expan %>%
  filter(b == some_b,) %>%
  filter(girg) %>%
  rho_hex
create_pdf("output.pdf/rho_hex_girg.pdf", last_plot(), width=0.48, height=0.33)

rho_histogram <- function(data) {
  data %>%
    mutate(exp_class = cut(
            bi_exp_m,
            breaks = c(0, 0.75, 2),
            labels = c("<0.75", "≥0.75")
          )) %>%
    ggplot(aes(
      optimal_rho - optimal_rhomax,
      fill=exp_class
    )) +
    geom_histogram(bins=30) +
    scale_fill_viridis_d() +
    labs(
      x = TeX("$1/(1+\\rho) - 1/(1+\\rho_\\max)$"),
      y = "Number of networks",
      fill = "Est. exponent"
    )
}

rho_expan %>%
  filter(b == some_b,) %>%
  filter(interesting) %>%
  rho_histogram
create_pdf("output.pdf/rho_hist_real.pdf", last_plot(), width=0.48, height=0.33)

rho_expan %>%
  filter(interesting) %>%
  rho_histogram +
  facet_wrap(~b, ncol=5)
create_pdf("output.pdf/rho_hist_real_b.pdf", last_plot(), width=1, height=1)

rho_expan %>%
  filter(b == some_b,) %>%
  filter(girg) %>%
  rho_histogram
create_pdf("output.pdf/rho_hist_girg.pdf", last_plot(), width=0.48, height=0.33)

source("data_analysis.R")
