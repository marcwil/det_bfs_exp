# data analysis
library(purrr)

some_b
some_alpha
data <- expan %>% filter(interesting) %>% filter(b == some_b, alpha == some_alpha)

data %>%
  ggplot(aes(x = relevant_overlap_rel, y = bi_exp)) +
  geom_point()

##################################################
df <- expan_raw %>%
  full_join(stats, by = "graph") %>%
  mutate(
    cost_uni = mean((cost_uni_s+cost_uni_t)/2),
    uni_exp_m = log(cost_uni) / log(m),
    bi_exp_m = log(cost_bi) / log(m),
    bi_exp_uni = log(cost_bi) / log(cost_uni),
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
  group_by(graph, b, alpha) %>%
  summarise(
    across(everything(), mean)
  )

df %>%
  filter(interesting == 1, b == some_b, alpha == some_alpha) %>%
  ggplot(aes(x = relevant_overlap_rel, y = cost_uni/(2*m))) +
  geom_point()

expan %>%
  ggplot(aes(x = log(deg_cov+0.1), y = locality, color = bi_exp, shape = log(deg_cov+0.1)<0.2&locality>0.52)) +
  geom_point()
expan %>%
  ggplot(aes(x = log(deg_cov+0.1), y = locality, color = bi_exp)) +
  geom_point()
################################################


xthresh <- 0
ythresh <- 0.85

simplified <- data %>%
  transmute(x = relevant_overlap_rel,
            y = bi_exp,
            xbin = ifelse(x > xthresh, 1, 0))



# Define thresholds for testing
y_thresholds <- seq(0.05, 0.95, by = 0.01)

# Function to calculate metrics
calculate_metrics <- function(y_thresh) {
  classified <- simplified %>%
    mutate(ybin = ifelse(y > y_thresh, 1, 0))
  
  # Confusion matrix components
  TP <- sum(classified$xbin == 1 & classified$ybin == 0)
  FP <- sum(classified$xbin == 1 & classified$ybin == 1)
  FN <- sum(classified$xbin == 0 & classified$ybin == 0)
  TN <- sum(classified$xbin == 0 & classified$ybin == 1)
  
  precision <- ifelse((TP + FP) > 0, TP / (TP + FP), 0)
  recall <- ifelse((TP + FN) > 0, TP / (TP + FN), 0)
  f1 <- ifelse((precision + recall) > 0, 2 * precision * recall / (precision + recall), 0)
  
  data.frame(y_thresh, precision, recall, f1, accuracy = (TP + TN) / nrow(classified))
}

# Apply over all thresholds
metrics <- y_thresholds %>%
  map_dfr(calculate_metrics)

# Find optimal threshold based on F1 score
optimal <- metrics %>% filter(f1 == max(f1))
print(optimal)

# Find Scores for threshold 0.75
print(metrics %>% filter(abs(y_thresh - 0.75) < 1e-6))

# Plot metrics
ggplot(metrics, aes(x = y_thresh)) +
  geom_line(aes(y = precision, color = "Precision")) +
  geom_line(aes(y = recall, color = "Recall")) +
  geom_line(aes(y = f1, color = "F1 Score")) +
  labs(title = "Metrics vs. Threshold", x = "Y-axis Threshold", y = "Score") +
  theme_minimal()


# Second: binary classifier for rho theorem
simplified_rho <- rho_expan %>%
  filter(interesting) %>%
  filter(b == some_b) %>%
  transmute(x = optimal_rho - optimal_rhomax,
            y = bi_exp,
            xbin = ifelse(x > 0, 1, 0),
            ybin = ifelse(y <= 0.75, 1, 0),
            )

confusion <- function(df) {
  TP <- sum(df$xbin == 1 & df$ybin == 1)
  FP <- sum(df$xbin == 1 & df$ybin == 0)
  FN <- sum(df$xbin == 0 & df$ybin == 1)
  TN <- sum(df$xbin == 0 & df$ybin == 0)
  
  precision <- ifelse((TP + FP) > 0, TP / (TP + FP), 0)
  recall <- ifelse((TP + FN) > 0, TP / (TP + FN), 0)
  f1 <- ifelse((precision + recall) > 0, 2 * precision * recall / (precision + recall), 0)
  accuracy = (TP + TN) / nrow(df)
  
  data.frame(precision, recall, f1, accuracy)
}

confusion(simplified_rho)
  


# Next task: wc exponent 

data %>%
  ggplot(aes(x=wc_exponent, y=bi_exp)) +
  geom_hex(bins=15) +
  scale_fill_viridis(
    trans = "log",
    breaks = c(1,4,16,64,256)
  ) +
  labs(
    x = "Exponent by Theorem 6",
    y = TeX("Estimated exponent $\\log_m(\\hat{c})$")
  )

correlation <- cor(data$wc_exponent, data$bi_exp, method = "pearson")
cat("Pearson Correlation:" , correlation, "\n")
spearman <- cor(data$wc_exponent, data$bi_exp, method = "spearman")
cat("Spearman Correlation:", spearman, "\n")

lm_fit <- lm(bi_exp ~ wc_exponent, data = data)
# Summary of the model
summary(lm_fit)


# Predicted values
data <- data %>%
  ungroup() %>%
  mutate(deviation = abs(bi_exp - predict(lm_fit)))  # Absolute deviation

# Calculate percentage
within_0.05 <- mean(data$deviation <= 0.05) * 100
within_0.10 <- mean(data$deviation <= 0.10) * 100
within_0.15 <- mean(data$deviation <= 0.15) * 100
cat(sprintf("%.2f%% of networks are within 0.05 of the linear model.", within_0.05))
cat(sprintf("%.2f%% of networks are within 0.10  of the linear model.", within_0.10))
cat(sprintf("%.2f%% of networks are within 0.15 of the linear model.", within_0.15))

ggplot(data, aes(x = wc_exponent, y = bi_exp)) +
  geom_point(alpha = 0.6) +
  geom_abline(intercept = coef(lm_fit)[1], slope = coef(lm_fit)[2], color = "red", linetype = "dashed") +
  labs(title = "Linear Fit of Predicted vs. Estimated Exponents",
       x = "Theoretical Exponent (wc_exponent)",
       y = "Estimated Exponent (bi_exp)") +
  theme_minimal()

intercept <- coef(lm_fit)[1]
slope <- coef(lm_fit)[2]

# https://slowkow.com/notes/ggplot2-color-by-density/
get_density <- function(x, y, ...) {
  dens <- MASS::kde2d(x, y, ...)
  ix <- findInterval(x, dens$x)
  iy <- findInterval(y, dens$y)
  ii <- cbind(ix, iy)
  return(dens$z[ii])
}

# Create the plot with ablines
data %>% 
  mutate(density = get_density(wc_exponent, bi_exp, n=200)) %>%
  ggplot(aes(
    x = wc_exponent,
    y = bi_exp,
    #color = density
    )) +
  # Scatter plot
#  geom_point(
#    alpha = 0.5,
#    size = 0.5
#  ) +
  # density
  geom_hex(
    bins=15
  ) +
  scale_fill_viridis(
      trans = "log",
     breaks = c(1,4,16,64,256)
  ) +
  # Linear fit line
  geom_abline(intercept = intercept, slope = slope, 
              color = "red", linetype = "dashed") +
  # Deviation bands using ablines
  geom_abline(intercept = intercept + 0.1, slope = slope, 
              color = "black", linetype = "dashed", linewidth = 0.3) +
  geom_abline(intercept = intercept - 0.1, slope = slope, 
              color = "black", linetype = "dashed", linewidth = 0.3) +
  geom_abline(intercept = intercept + 0.15, slope = slope, 
              color = "black", linetype = "dashed", linewidth = 0.3) +
  geom_abline(intercept = intercept - 0.15, slope = slope, 
              color = "black", linetype = "dashed", linewidth = 0.3) +
  geom_abline(intercept = intercept + 0.05, slope = slope, 
              color = "black", linetype = "dashed", linewidth = 0.3) +
  geom_abline(intercept = intercept - 0.05, slope = slope, 
              color = "black", linetype = "dashed", linewidth = 0.3) +
  # Labels and theme
  labs(
    x = "Exponent by Theorem 6",
    y = TeX("Estimated exponent $\\log_m(\\hat{c})$")
  )
create_pdf("output.pdf/overlap_wcexp_fit_real.pdf", last_plot(), width=0.48, height=0.33)




