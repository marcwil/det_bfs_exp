# install and load the tidyverse package it contains many useful functions for 
# working with data in R and visualizing it. 

# Uncomment the next line if you haven't already installed the libraries!!!
# install.packages("dplyr")
# install.packages("ggplot2")

# just load the library (do not comment the next line)
suppressPackageStartupMessages(library(dplyr))
suppressPackageStartupMessages(library(ggplot2))

# read in the csv file and assign it to the variable babynames
# babynames contains for each year from 1880 to 2017, the number of children of 
# each sex given each name: http://www.ssa.gov/oact/babynames/limits.html
# just look at the csv file to get a good understanding of the data
babynames <- read.csv("babynames.csv")

# the variable babynames is of the type "data frame", it is one of the most 
# important types in R. It is basically a database/table and many R functions operate 
# efficiently on it. Most of the time you just want to read in a data frame and 
# call functions from R on it

# The tidyverse library has a lot of useful function to manipulate data frames
# more examples can be found here https://dplyr.tidyverse.org/

# filteredBabies contains only those babies that were born after 1900
filteredBabies <- filter(babynames, year > 1900)
# selects only the columns "name", "sex" and "year"
fewRows <- select(filteredBabies, name, sex, year)

# another possibility to write the above lines is with pipe operator "%>%"
# Syntax: A(a,b) %>% B(c,d), It takes the result of A and inserts it as the 
# first argument of B
result <- babynames %>%
  filter(year > 1900) %>%
  select(name, sex, year)

# Visualization can be done with the ggplot module of the tidyverse library
# an overview of different plots is given in
# https://www.maths.usyd.edu.au/u/UG/SM/STAT3022/r/current/Misc/data-visualization-2.1.pdf

# This plots the number of babies named Parker for each year,
# with the year on the x-axis and the number of babies on the y-axis.
maleParkers <- filter(babynames, name=="Parker" & sex=="M")
ggplot(maleParkers) +
  geom_line(aes(x=year, y=n)) + 
  ggtitle("Parker Population") +
  xlab("Year") + 
  ylab("Number of Parkers")
ggsave("maleParkers.pdf", width=10, height=7, unit="cm")

# a vector of strings containing intersting babynames
interesitngNames <- c("Parker", "River", "James", "Robert", "Olivia", "Riley") 
# Boxplot for how common some interesting babynames are
# the y-axis shows how often a name was used in different years
# You can see whether a name has been used with the same frequency in all years 
# or whether it has a wide variation in use.
interstingBabies <- filter(babynames, name %in% interesitngNames)
ggplot(interstingBabies) + 
  geom_boxplot(aes(x=name, y=n, color=sex)) + 
  ggtitle("Variance of interesting baby names") +
  xlab("Name") + 
  ylab("Number of Babies") +
  scale_y_log10() # logarithmic y axis
ggsave("interestingBabys.pdf", width=10, height=7, unit="cm")

