suppressPackageStartupMessages(library(dplyr))
suppressPackageStartupMessages(library(tidyr))
suppressPackageStartupMessages(library(ggplot2))
library(stringr)

loglogplot <- function() {
  list(
    scale_x_log10(),
    scale_y_log10(),
    annotation_logticks(sides='lb')
  )
}

#library("scales")
#library(tikzDevice)
#options(tz="CA")

print("Reading csv files.")
stats <- tibble(read.csv("../output/stats.csv"))
bfs_raw <- tibble(read.csv("../output/bfs_merged.csv"))
expan_raw <- tibble(read.csv("../output/expan_merged.csv"))

stats <- stats %>%
  mutate(
    girg = startsWith(graph, "girg"),
    girg_scale = startsWith(graph, "scale"),
    locality = (local_clustering + global_clustering + distance_locality) /3,
    locality2 = (local_clustering + distance_locality) / 2
  )
stats


bfs <- bfs_raw %>%
  group_by(graph) %>%
  summarise(
    dist = mean(dist),
    cost_uni = mean(cost_uni),
    cost_bi = mean(cost_bi)
  )
print(bfs)

print("Creating expan table.")
expan <- expan_raw %>%
  group_by(graph, b, alpha) %>%
  mutate(
    # relative overlap theorem
    overlap_relative = overlap / dist,
    overlap_relative_rel = if_else(rel_dist<=0, 1, overlap / rel_dist),
    overlap_relative_rel = pmin(1,overlap_relative_rel),
    overlap_relative_rel = pmax(-1,overlap_relative_rel),
    wc_exponent = pmax(0,
                       pmin(1,
                            1 -
                            (overlap_relative_rel*(1-alpha) / (log(bplus) / log(b) + overlap_relative_rel))
                            )
                       ),
    # rho theorem
    rho_prime = if_else(is.na(rho_prime), Inf, rho_prime),
    max_rho = (1-alpha) / (1-alpha + alpha * log(bplus)/log(b)),
    rho_conv = pmin(1, 1/(1+rho_prime)),
    max_rho_conv = 1/(1+max_rho),
    rho_relative = rho_conv - max_rho_conv
  ) %>%
  summarise(
    across(everything(), mean)
  )

tbl <- expan %>%
  left_join(stats, by='graph') %>%
#  filter(girg) %>%
  mutate(
    bi_exp = log(cost_bi) / log(m),
    overlap = min(overlap, rel_dist),
  )

tbl %>%
  ggplot(aes(x=deg_cov,y=locality2,color=bi_exp)) +
  geom_point() +
  scale_color_viridis_c("Search exponent") +
  scale_x_log10() +
  scale_y_log10() +
  annotation_logticks() +
  xlab("Heterogeneity") +
  ylab("Locality")
print("output.pdf/loc_het_exp.pdf")
ggsave("output.pdf/loc_het_exp.pdf")

tbl %>%
  ggplot(aes(x=overlap_relative_rel, y=bi_exp)) +
  geom_hex() +
  facet_grid(b~alpha, scales='free') +
  xlab("expansion overlap / $d_\\alpha(s,t)$") +
  ylab("Estimated exponent $\\log_m(\\hat{c})$")
print("output.pdf/relative_overlap.pdf")
ggsave("output.pdf/relative_overlap.pdf")

#tikz(file = "./output.tex/plot_many_alphas.tex",
#     width=12, height=3,standAlone = TRUE)
#print(p)
#dev.off()

tbl %>%
  ggplot(aes(x=wc_exponent, y=bi_exp)) +
  geom_hex() +
  facet_grid(b~alpha, scales="free")
print("output.pdf/wc_exponent.pdf")
ggsave("output.pdf/wc_exponent.pdf")

# rho relative
tbl %>%
  ggplot(aes(x=rho_relative, y=bi_exp)) +
  geom_hex() +
  facet_grid(b~alpha, scales="free")
print("output.pdf/rho_revative.pdf")
ggsave("output.pdf/rho_revative.pdf")

tbl %>%
  mutate(class = cut(bi_exp,
                     breaks = c(0, 0.7, 0.8, 2),
                     labels = c("sublinear", "ambiguous", "linear"),
                     include.lowest = TRUE)
         ) %>%
  ggplot(aes(x=rho_relative, after_stat(count), color=class)) +
  geom_density() +
  facet_grid(b~alpha, scales="free")
print("output.pdf/rho_revative_dens.pdf")
ggsave("output.pdf/rho_revative_dens.pdf")



tbl2 <- tbl %>%
  left_join(bfs, by='graph')
tbl2 %>%
  ggplot(aes(x=cost_bi.x, y=cost_bi.y)) +
  geom_point()


# girgs eval
tbl <- expan %>%
  left_join(stats, by='graph') %>%
  filter(girg) %>%
  mutate(
    bi_exp = log(cost_bi) / log(m),
    overlap = min(overlap, rel_dist),
  )

tbl %>%
  ggplot(aes(x = deg_cov, y = locality, color=overlap_relative_rel)) +
  geom_point()

tbl %>%
  ggplot(aes(x=overlap_relative_rel, y=bi_exp)) +
  geom_hex() +
  facet_grid(b~alpha, scales='free') +
  xlab("expansion overlap / $d_\\alpha(s,t)$") +
  ylab("Estimated exponent $\\log_m(\\hat{c})$")

tbl %>%
  ggplot(aes(x=deg_cov,y=locality2,color=bi_exp)) +
  geom_point() +
  scale_color_viridis_c("Search exponent") +
  scale_x_log10() +
  scale_y_log10() +
  annotation_logticks() +
  xlab("Heterogeneity") +
  ylab("Locality")

## girg scale
tbl <- expan %>%
  left_join(stats, by='graph') %>%
  filter(girg_scale) %>%
  mutate(
    bi_exp = log(cost_bi) / log(m),
    overlap = min(overlap, rel_dist),
  )

extract_parameters <- function(tbl) {
    tbl %>%
      extract(
        col = graph,
        into = c("n_gen", "ple", "temp", "deg", "sample"),
        regex = "scale(\\d+)_([\\d\\.]+)_(\\d+(?:\\.\\d+)?)_(\\d+)_(\\d+)\\.edges",
        remove = FALSE,
        convert = TRUE
      )
}

tbl <- extract_parameters(tbl)

tbl_summary <- tbl %>%
  group_by(b, alpha, n_gen, ple, temp, deg,) %>%
  summarise(across(c(overlap_relative_rel,
                     locality,
                     locality2,
                     deg_cov,
                     cost_bi,
                     bi_exp,
                     n,
                     m,
                     rho_relative)))

tbl_summary %>%
  filter(b==2, alpha==0.1) %>%
  ggplot(aes(x=m, y=cost_bi,color=overlap_relative_rel)) +
  geom_point() +
  loglogplot()


tbl %>%
  ggplot(aes(x = m, y = bi_exp)) +
  geom_point() +
  facet_wrap(~graph)




# TODO
# final plots:
# - plots with rhoeval
# - overview grid
# -> no properties plot, see ext. val.
# - girgs?
