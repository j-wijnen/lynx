import numpy as np 
import pandas as pd
import matplotlib.pyplot as plt 

data = pd.read_csv('homogeneous_data.csv')
plt.plot(data['strain'], data['stress'], label="Lynx")

# Analytical 
strain = np.linspace(0, 0.1, 100)
yield_stress = 200.0
hardening_modulus = 200e3
hardening_exponent = 0.1

stress = yield_stress * (1.0 + hardening_modulus * strain / yield_stress)**hardening_exponent
plt.plot(strain + stress/hardening_modulus, stress, linestyle="--", label="Analytical")

plt.legend()
plt.show()