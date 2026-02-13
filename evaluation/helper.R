library(ggplot2)
library(tidyr)
library(dplyr)
library(purrr)
library(stringr)
#library(readr)
library(xtable)

library(viridis)
library(patchwork)
library(scales)
library(showtext)
library(latex2exp)

### Theme stuff taken from https://github.com/thobl/external-validity/blob/main/R/helper/theme.R
## general theme
theme_set(theme_bw())

theme_update(
  plot.margin = margin(t=0.1,r=0.1,b=0.1,l=0.1, unit="cm"),
  legend.key.size = unit(0.3, "cm"),
                                        #    legend.margin = unit(0, "cm"),
  legend.box.margin = margin(-10,-5,-10,-10)
)

## latex font
font_add("LM Roman", "lmroman10-regular.otf")
font_add("LM Math", "latinmodern-math.otf")
showtext_auto()
theme_update(text = element_text(family = "LM Math", size = 8))

## some colors
blue   <- "#5EA6E5"
green  <- "#6EC620"
yellow <- "#EEC200"
red    <- "#E62711"
violet <- "#c6468d"
purple <- "#613872"
gray   <- "#666666"

## pdf output; width andheight relative to LIPIcs text width
create_pdf <- function(file, plot, width = 1, height = 0.43) {
  print(paste("Writing plot to ", file))
  textwidth <- 13.70499 / 2.54 # elsarticle
  #textwidth <- 14.05 / 2.54 # thesis
  #textwidth <- 14 / 2.54 # lipics
  pdf(file, width = width * textwidth, height = height * textwidth)
  print(plot)
  no_output <- dev.off()
}

fit_power_law_from_plot <- function(plot_obj) {
  # Extract the original data from the plot object
  plot_data <- plot_obj$data

  # Extract the variables from the aes mapping (x, y, color)
  mapping <- plot_obj$mapping
  x_var <- as_label(mapping$x)
  y_var <- as_label(mapping$y)
  color_var <- as_label(mapping$colour)

  # Use ggplot_build to get the color assignments
  plot_build <- ggplot_build(plot_obj)
  plot_colors <- unique(plot_build$data[[1]]$colour)  # Extract colors from the first layer (geom_point layer)

  # Ensure that the length of the colors matches the groups
  unique_groups <- unique(plot_data[[color_var]])

  if (length(plot_colors) != length(unique_groups)) {
    stop("Number of colors doesn't match the number of unique groups. Something went wrong.")
  }

  # Prepare a list to store the fitted lines
  fitted_lines <- list()
  results <- data.frame(Group = character(), Coefficient_a = numeric(), Exponent_b = numeric(), Equation = character(), stringsAsFactors = FALSE)

  for (i in seq_along(unique_groups)) {
    group <- unique_groups[i]

    # Filter data for the current group
    group_data <- plot_data %>%
      filter(.data[[color_var]] == group)

    # Fit the power-law model on log-transformed data
    fit <- lm(log10(group_data[[y_var]]) ~ log10(group_data[[x_var]]))

    # Extract coefficients
    coefficients <- coef(fit)
    intercept <- 10^coefficients[1]  # Back-transform intercept to original scale
    slope <- coefficients[2]         # Slope (exponent)

    # Construct the equation string
    equation <- paste0("y = ", round(intercept, 3), " * x^", round(slope, 3))

    # Append results to the data frame
    results <- rbind(results, data.frame(Group = as.character(group), Coefficient_a = intercept, Exponent_b = slope, Equation = equation, stringsAsFactors = FALSE))

    # Create a fitted line using the extracted color for this group
    group_color <- plot_colors[i]
    fitted_lines[[i]] <- geom_abline(intercept = log10(intercept), slope = slope, color = group_color, linetype = "dashed")
  }
  
  # Print the results as a table
  rownames(results) <- NULL
  print(results)
  
  # Return both the results data frame and the fitted lines
  return(list(results = results, fitted_lines = fitted_lines))
}
