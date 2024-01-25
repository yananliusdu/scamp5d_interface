# Created by Yanan Liu on 16:27 07/01/2024
# Location: Your Location
# Log: Your Log Information
# Version: Your Version Information

import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
file_path = r'D:/上大工作/Research/autoFocus/对焦示意.csv'

# Load the CSV into a DataFrame
df = pd.read_csv(file_path)

# Renaming the column '638' to 'Sharpness' for clarity
df.rename(columns={'637': 'Sharpness'}, inplace=True)
index_range = df.index.max() + 1  # Adding 1 because index starts from 0
# Plotting the index (assumed focus distance) against the Sharpness values
plt.figure(figsize=(10, 5), dpi=300)
# plt.title('Sharpness vs. Focus Distance', fontsize=16)  # Enlarged title font
plt.xlabel('Focus distance', fontsize=16)  # Enlarged x-axis label font
plt.ylabel('Sharpness', fontsize=16)  # Enlar
plt.plot(df.index, df['Sharpness'], marker='.', linestyle='-', color='blue')
plt.xlabel('Focus distance')
plt.ylabel('Sharpness')
plt.xticks([0, index_range//2, index_range-1], ['Near', 'Mid', 'Far'])  # Custom x-axis labels
plt.grid(True)

# Identifying the maximum sharpness value and its corresponding focus distance
max_sharpness = df['Sharpness'].max()
max_focus_distance = df['Sharpness'].idxmax()

# Plotting the point of maximum sharpness with a red point and text label
plt.scatter(max_focus_distance, max_sharpness, marker='*',color='red',zorder=2)  # Red point
plt.text(max_focus_distance, max_sharpness, ' Best focus point', color='red', verticalalignment='bottom')  # Text label

# Define the path and filename for saving the figure
save_path = r'D:/上大工作/Research/autoFocus/sharpness_vs_focus_distance.png'

# Save the figure
plt.savefig(save_path)

# Display the figure
plt.show()

