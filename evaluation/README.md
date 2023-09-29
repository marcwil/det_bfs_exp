# Visualizing data with R #

There are countless tools to visualise data. 
One of them is the [programming language R](https://en.wikipedia.org/wiki/R_(programming_language)) 
for statistical calculations. 
If you have never used R before, don't be afraid to try it out.
In this folder you will find a small [R script](evaluation/example.R) and [sample data](evaluation/babynames.csv) to plot.

## Install and execute R ##

First, you have to [install R](https://cran.r-project.org/bin/linux/ubuntu/fullREADME.html). 
We also recommend installing a suitable editor such as [RStudio](https://posit.co/download/rstudio-desktop/).
The first time you execute the example script, 
you have to install the necessary libraries for R.
They are already installed by the [.devcontainer](../.devcontainer/devcontainer.json) 
but if you use the framework without it, you have to uncomment the following lines from the R script.

``` consosle
# install.packages("dplyr")
# install.packages("ggplot2")
```
The R script can then be executed in the editor or via the command line:

```console
Rscript example.R
```

Two `.pdf`s should be created.
For subsequent calls to the R script, you can comment the lines that install the libraries.
For more information look at the [R script](evaluation/example.R).