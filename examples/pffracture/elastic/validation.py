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
plt.legend()

plt.show()