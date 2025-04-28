#
# Plots the damage and stress-strain curve for the homogeneous validation case
# and compares it to the analytical solution
#

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt 

# Simulation
filename = "validation_data.csv"
data = pd.read_csv(filename)

# Analytical solution
Gc = 2.5
lc = 1.0
E = 210e3/(1-0.3**2)
strain = np.linspace(0, np.max(data["strain"]))
stress = (Gc/(Gc+E*strain**2*lc))**2*E*strain
damage = E*strain**2*lc/(Gc+E*strain**2*lc)

sigma_c = 3/16 * (3*E*Gc/lc)**.5
lc_AT1 = 3*E*Gc/(8*sigma_c**2)
print(lc_AT1)

data_AT1 = pd.read_csv("validation_AT1_data.csv")

plt.figure()
plt.xlabel("Strain")
plt.ylabel("Damage")
plt.plot(strain, damage, label="Analytical")
plt.plot(data["strain"], data["damage"], linestyle="--", label="Lynx")
plt.legend()

plt.figure()
plt.xlabel("Strain")
plt.ylabel("Stress")
plt.plot(strain, stress, label="Analytical")
plt.plot(data["strain"], data["stress"], linestyle="--", label="Lynx")
plt.plot(data_AT1["strain"], data_AT1["stress"], label="AT1")
plt.legend()

plt.show()