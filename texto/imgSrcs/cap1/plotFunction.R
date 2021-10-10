#Create X range
t_values <- seq(0,40,0.05)
# Parameters for the function
alpha <- 1.0 / 10.0
beta <- 1.0
c_1 <- 3.0
c_2 <- 3.0
L <- 3.0
# f(x) values
y_values <- exp((-1.0 * alpha) * t_values) *
            ((c_1 * cos(beta * t_values)) + (c_2 * beta * sin(t_values))) + L
plot(t_values, y_values, # Values in X, Values in Y
     xlab = "t",  # Label for X-axis
     ylab = "x(t)", # Label for Y-axis
     type = "l", # Continuous line (to simulate a smooth curve)
     col = "red", # Color of the curve to plot
     xlim = c(0, 40), # Range of values for the x axis
     ylim = c(0, 7), # Range of values for the y axis
     xaxs = "i", # Range adds a small delta, whit this we tell him not to do
     yaxs = "i", # Same for Y axis
     bty = "n") # Remove the boundary box
#Add the a blue line at L
#abline(a = L, # Intercept
#       b = 0, # Slope (zero since we want an horizontal)
#       col = 'blue')
abline(h = L, # Horizontal line at L
       lty = 2, # Line type = 2 correspond to a dashed line
       col = 'blue')
