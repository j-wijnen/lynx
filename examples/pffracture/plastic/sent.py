#
# Plots the force-displacement curve for SENT simulations
#

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt 

data = pd.read_csv("sent_data.csv")
data_stag = pd.read_csv("sent_staggered_data.csv")

plt.figure()
plt.xlabel("Disp")
plt.ylabel("Force")
plt.plot(data["disp"], data["force"], linestyle="-", label="Monolithic")
plt.plot(data_stag["disp"], data_stag["force"], linestyle="--", label="Staggered")
plt.legend()

plt.figure()
plt.xlabel("Increment")
plt.ylabel("Iterations")
plt.plot(data["iterations"], linestyle="-", label="Monolithic")
plt.plot(data_stag["iterations"], linestyle="-", label="Staggered")
plt.legend()

plt.figure()
plt.xlabel("Disp")
plt.ylabel("Crack tip")
plt.plot(data["disp"], data["cracktip"], linestyle="-", label="Monolithic")
plt.plot(data_stag["disp"], data_stag["cracktip"], linestyle="--", label="Staggered")
plt.legend()

plt.show()