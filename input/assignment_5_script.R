library(ggplot2)
library(dplyr)
library(tidyverse)

# set the working dir to the current dir
# current_path = rstudioapi::getActiveDocumentContext()$path
# setwd(dirname(current_path))

# select a lightweight ggplot theme for cleaner plotting
theme_set(theme_minimal()) 

# returns the plot for task 5
task_5 <- function(n, path) {
  data <- read_csv(path)
  
  # determine whether it is density or diffusion
  if (length(data) == 3) {
    graph <- 'density'
    data[1] <- lapply(data[1], as.integer)
    data <- data %>%
      rename(
        it = Iteration,
        interval = Interval,
        den = Density
      )
  } else {
    graph <- 'diffusion'
    data[1] <- lapply(data[1], as.integer)
    data <- data %>%
      rename(
        it = Iteration,
        dif = Diffusion
      )
  }
  
  if (graph == 'density') {
    intervals <- data %>%    
      mutate(
        # change iterations to x * 10^3 (too many zeroes a little annoying)
        it = as.integer(it/1000),
      )
    
    p <- ggplot(data=intervals %>% filter(it %% n == 0), aes(x=factor(
      interval, 
      ordered = TRUE,
      levels = c('0;0.5', '0.5;1', '1;1.5', '1.5;2', '2;2.5', '2.5;3', '3;3.5', 
                 '3.5;4', '4;4.5', '4.5;5', '5;5.5', '5.5;6', '6;6.5', '6.5;7',
                 '7;7.5', '7.5;8', '8;8.5', '8.5;9', '9;9.5', '9.5;10', '10;10.5', 
                 '10.5;11', '11;11.5', '11.5;12', '12;12.5', '12.5;13', '13;13.5',
                 '13.5;14', '14;14.5', '14.5;15', '15;15.5', '15.5;16')
    ), y = den, color = as.factor(it), group = as.factor(it))) + 
      geom_line() + 
      geom_point(size = 0.5) + 
      labs(x = 'Intervals', y = 'Particle Density', color='Iteration in\n1.000') +
      # title
      ggtitle(paste("Task 5 - Particle Density of the intervals of every ", n, "th (written) Iteration", sep = "")) +
      theme(plot.title = element_text(hjust = 0.5))
    return(p)
  }
  else {
    iterations <- data %>%    
      mutate(
        # change iterations to x * 10^3 (too many zeroes a little annoying)
        it = as.integer(it/1000),
      )
    p <- ggplot(data=iterations %>% filter(it <= (150 + n), it != min(it)), aes(x=it, y = dif)) + 
      geom_line() + 
      geom_point(size = 0.5) + 
      geom_smooth() +
      labs(x = 'Iteration in 1.000', y = 'Diffusion') +
      # title
      ggtitle(paste("Task 5 - Diffusion of the first", n, "(written) Iterations")) +
      theme(plot.title = element_text(hjust = 0.5))
    return(p)
  }
}

# returns the plot for task 4
task_4 <- function(n, path) {
  
  # parsing in the data
  data <- read_csv(
    path,
    
    # determining the type
    col_types = cols(
      Iteration = col_integer(),
      Bin = col_integer(),
      Density = col_double(),
      'Velocity X' = col_double(),
      'Velocity Y' = col_double(),
      'Velocity Z' = col_double()
    )) %>%
    
    # renaming the values
    rename(
      it = Iteration,
      bin = Bin,
      den = Density,
      velX = 'Velocity X',
      velY = 'Velocity Y',
      velZ = 'Velocity Z'
    )
  
  bins <- data %>%
    # omit rows where the velocity is NA
    na.omit(velY) %>%
    mutate(
      # change iterations to x * 10^4 (too many zeroes a little annoying)
      it = as.integer(it/10000),
      # get absolute value of velocity in y direction
      velY = abs(velY)
    )
  
  # plot the first 30 iterations
  p <- ggplot(data=bins %>% dplyr::filter(it < n), aes(x=as.factor(bin), y=velY, color = as.factor(it), group = as.factor(it))) + 
    # lines between the points
    geom_line(linetype = 'dotted') + 
    # regression line with a local regression
    geom_smooth(method = 'loess', formula = y ~ x, se = FALSE) +
    # draws the points
    geom_point(size = 0.75) + 
    # labels
    labs(x = 'Bins',y = 'Velocity Y-Axis', color='Iteration in\n10.000') +
    # title
    ggtitle(paste("Task 4 - Falling Velocity of the first", n, "(written) Iterations")) +
    theme(plot.title = element_text(hjust = 0.5))
  p
  return(p)
}


# default iteration number
n <- 30

# default task
task <- 'task4'

# default csv file
path <- "files/assignment_5/csv/statistics_task4.csv"

args <- commandArgs(trailingOnly=TRUE)

# getting arguments from command line
if (length(args >= 1)) {
  n <- as.integer(args[1])
} 

if (length(args >= 2) && args[2] == 'task5'){
  task <- 'task5'
  path <- 'files/assignment_5/csv/RDF_task5_cooling_statistics.csv'
}

if (length(args == 3)) {
  path <- args[3]
}

# get the file name
file <- tools::file_path_sans_ext(basename(path))

# get the plot
if (task == 'task5') {
  p <- task_5(n, path)
} else {
  p <- task_4(n, path)
}

# saves the plot
ggsave(paste(file, "_", n, "_iterations_", ".pdf"), plot = p, width = 40, height = 20, units = "cm")