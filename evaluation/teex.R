
diam <- tibble(read.csv("../output/teex_diameter.csv"))

stats <- diam %>%
  select(graph, diameter, teex_iter) %>%
  full_join(stats, by="graph")

stats <- stats %>%
  mutate(
    diam_norm = log(diameter)/log(m),
    teexcost = log(teex_iter)/log(n)
  )

stats %>%
  ggplot(aes(x = diam_norm, y = log(teex_iter)/log(n))) +
  geom_hex()

stats %>%
  ggplot(aes(x = log(deg_cov+0.1), y = locality, colour = teexcost)) +
  geom_point()

stats %>%
  filter(avgdeg<100, interesting) %>%
  ggplot(aes(x = log(deg_cov+0.1), y = locality, color = log(diameter)/log(m))) +
  scale_color_viridis(
    transform="log"
  ) +
  geom_point()

expan %>%
  full_join(stats) %>%
  ggplot(aes(x = diam_norm, y = bi_exp)) +
  geom_point()


confusion(expan %>%
  full_join(stats) %>%
  transmute(x = diam_norm,
            y = bi_exp,
            xbin = ifelse(x < 0.40, 1, 0),
            ybin = ifelse(y <= 0.75, 1, 0),
            ))

expan %>%
  full_join(stats) %>%
  filter(interesting) %>%
  ggplot(aes(x = log(deg_cov+0.1), y = locality, color = bi_exp<0.75)) +
  geom_point()
  
confusion(
expan %>%
  full_join(stats) %>%
  filter(interesting) %>%
  transmute(
    xbin = ifelse(log(deg_cov+0.1)<=0 & locality>=0.5, 0, 1),
    ybin = ifelse(bi_exp <= 0.75, 1, 0)
  )
)



