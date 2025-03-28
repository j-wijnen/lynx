#
# Plots the damage and stress-strain curve for the homogeneous case
# and compares it to the analytical solution
#

import types
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt 

def main():
    # Simulation
    filename = "homogeneous_data.csv"
    data = pd.read_csv(filename)

    # Analytical solution
    props = types.SimpleNamespace()
    props.nu = 0.3
    props.E = 210.0e3
    props.hardening = "powerlaw"
    props.sy0 = 200
    props.n = 0.1
    props.G = 20.0
    props.lc = 1.0
    props.beta = 0.1
    props.psi_p0 = 0.0

    model = Model(props)
    
    strain = np.linspace(0, np.max(data["strain"]), 1000)
    stress = np.zeros(len(strain))
    damage = np.zeros(len(strain))
    elastic_energy = np.zeros(len(strain))
    plastic_energy = np.zeros(len(strain))

    for inc, ieps in enumerate(strain):
        stress[inc], damage[inc] = model.increment(ieps)
        elastic_energy[inc] = model.psi_e
        plastic_energy[inc] = model.psi_p

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

    plt.figure()
    plt.xlabel("Strain")
    plt.ylabel("Energy")
    plt.plot(strain, elastic_energy, label="Analytical Elastic")
    plt.plot(strain, plastic_energy, label="Analytical Plastic")
    plt.plot(data["strain"], data["elastic_energy"], linestyle="--", label="Lynx Elastic")
    plt.plot(data["strain"], data["plastic_energy"], linestyle="--", label="Lynx Plastic")

    plt.legend()

    plt.show()

    return 

class Model:

    def __init__(self, props):
        self.E = props.E
        self.Gc = props.G
        self.lc = props.lc
        #self.shat = np.dot(props.f, props.shat)

        self.hardening = props.hardening
        self.sy0 = props.sy0
        try: self.n = props.n
        except: self.n = 0.0
        try: self.h0 = props.h0
        except: self.h0 = 0.0
        self.beta = props.beta

        self.sy = self.sy0
        self.eps_p = 0.0
        self.psi_p = -props.psi_p0

        self.phi = 0.0

    def increment(self, eps):
        # Plastic part
        g = (1.0 - self.phi)**2 * (1.0 - 1e-7) + 1e-7
        gp = self.beta*g + 1.0 - self.beta
        if g * self.E * (eps - self.eps_p) > gp * self.sy:
            deps_p, self.sy = get_hardening_powerlaw(self.sy0, self.E, self.n, eps, self.eps_p, g, gp)
            self.eps_p += deps_p
            self.psi_p += deps_p * self.sy
        

        self.psi_e = 0.5 * self.E * (eps - self.eps_p)**2     
        psi = self.psi_e + self.beta * np.maximum(self.psi_p, 0.0)
        self.phi = 2.0*psi / (self.Gc/self.lc + 2.0*psi)
        sigma = (1.0 - self.phi)**2 * self.E * (eps - self.eps_p)
        
        return sigma, self.phi


def get_hardening_powerlaw(sy0, E, n, eps, eps_p, g, gp):
    deps_p = 0.0
    while True:

        r = gp*sy0*(1 + E*(eps_p + deps_p)/sy0)**n - g*E*(eps - eps_p - deps_p)

        if np.abs(r) < 1e-6:
            break

        dr_deps_p = gp*E*n*(1 + E*(eps_p + deps_p)/sy0)**(n-1) + g*E

        deps_p -= r / dr_deps_p

    return deps_p, gp*sy0*(1 + E*(eps_p + deps_p)/sy0)**n

if __name__ == "__main__":
    main()
