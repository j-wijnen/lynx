import examples.pffracture.exodus_reader as exodus_reader
import matplotlib.pyplot as plt
import numpy as np

data = exodus_reader.read("test_out.e")

strain = data.cell_data["strain_xx"][0]
stress = data.cell_data["stress_xx"][0]
phi = data.point_data["phi"]

data_picard = exodus_reader.read("test_picard_out.e")

strain_picard = data_picard.cell_data["strain_xx"][0]
stress_picard = data_picard.cell_data["stress_xx"][0]
phi_picard = data.point_data["phi"]

Gc = 2.5
lc = 1.5
E = 210e3/(1-0.3**2)
strain_a = np.linspace(0, np.max(strain))
stress_a = (Gc/(Gc+E*strain_a**2*lc))**2*E*strain_a
phi_a = E*strain_a**2*lc/(Gc+E*strain_a**2*lc)

plt.figure()
plt.plot(strain[0], stress[0])
plt.plot(strain_picard[0], stress_picard[0], linestyle="--")
plt.plot(strain_a, stress_a, linestyle=":")

# plt.figure()
# plt.plot(strain[0], np.mean(phi, axis=0))
# plt.plot(strain_a, phi_a, linestyle=":")

plt.show()