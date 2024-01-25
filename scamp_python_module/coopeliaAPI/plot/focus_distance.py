# Created by Yanan Liu on 15:34 11/01/2024
# Location: Your Location
# Log: Your Log Information
# Version: Your Version Information

import numpy as np
import matplotlib.pyplot as plt

# Fixed image distance (d_i)
d_i = 50  # Replace with your desired fixed image distance

# Create an array of focal lengths (f) for the x-axis
f_values = np.linspace(10, 200, 1)  # Adjust the range and number of points as needed

# Calculate object distances (d_o) using the lens formula
d_o_values = 1 / (1 / f_values - 1 / d_i)

# Create the plot
plt.figure(figsize=(8, 6))
plt.plot(f_values, d_o_values, label='Object Distance (d_o)')
plt.xlabel('Focal Length (f)')
plt.ylabel('Object Distance (d_o)')
plt.title(f'Object Distance vs. Focal Length (Fixed d_i = {d_i})')
plt.grid(True)
plt.legend()
plt.show()
