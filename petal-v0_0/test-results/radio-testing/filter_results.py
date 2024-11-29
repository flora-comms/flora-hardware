from scipy.signal import butter, filtfilt
from matplotlib import pyplot as plt
import pandas as pd
import numpy as np

# read in the csv data
csvfile = pd.read_csv(filepath_or_buffer="results.csv")
print(csvfile)

# create a 4th order lowpass butterworth filter. 
# Cutoff frequency is 0.25, so should filter noise that has a "period" less than 4MHz
num, den = butter(4, 0.25)

# create matrix of zeroes
filtered = np.zeros((3,29))

# filter each column
filtered[0] = filtfilt(num, den, csvfile["VSWR"])
filtered[1] = filtfilt(num, den, csvfile["VSWR.1"])
filtered[2] = filtfilt(num, den, csvfile["VSWR.2"])

# convert to dataframe
df = pd.DataFrame({"Frequency (MHz)" : csvfile["Frequency (MHz)"], "Cheap" : filtered[0], "Mid" : filtered[1], "Expensive" : filtered[2]})
df.to_csv("filtered_results.csv")

# plot
fig, ax = plt.subplots()

ax.plot(df["Frequency (MHz)"], df["Cheap"], label='< $10')
ax.plot(df["Frequency (MHz)"], df["Mid"], label='$10 - $30')
ax.plot(df["Frequency (MHz)"], df["Expensive"], label='> $30')
plt.title("VSWR for Petal Radio and 915MHz LoRa\nAntenna Combinations at Various Pricepoints")
plt.ylabel("VSWR")
plt.xlabel("Frequency (MHz)")
plt.xticks(np.arange(900, 929, 5))
plt.yticks(np.arange(1.0,2.5,0.5))
plt.grid()
plt.legend()
plt.savefig("vswr.png")