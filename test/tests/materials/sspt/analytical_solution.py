import numpy as np
import matplotlib.pyplot as plt
import scipy
import csv

def main():

    comp = Composition()

    # X60 #4
    comp.C = 0.176
    comp.Si = 0.217
    comp.Mn = 1.37
    comp.P = 0.016
    comp.V = 0.053
    comp.Ti = 0.002
    comp.Cr = 0.015
    comp.Cu = 0.016
    comp.Ni = 0.020
    comp.Mo = 0.002
    comp.As = 0.011 
    comp.Al = 0.026
    comp.G = 7.0

    comp.Ae_form = "Grange"
    comp.Ms_form = "Andrews"


    T = 750

    X_start = 0.01
    X_end = 0.99
    X = np.linspace(X_start, X_end)

    # Initialize
    tf = np.zeros_like(X)
    tp = np.zeros_like(X)
    tb = np.zeros_like(X)

    #############
    # Ferrite TTT

    Ts = comp.get_Ae3()
    Te = comp.get_Bs()
    
    if T > Te and T <= Ts:
        fun_comp = comp.fun_comp_f()
        fun_T = fun_T_f(T, Ts)

        # analytical
        for i, iX in enumerate(X): 
            tf[i] = fun_comp / fun_T * fun_S(X_start, iX)

    else:
        tf = np.nan

    plt.plot(tf, X)

    # data = np.loadtxt("ferrite.csv", skiprows=1, delimiter=",")
    # plt.plot(data[:,1], data[:,0], linestyle=":")

    plt.show()

class Composition:

    def __init__(self):
        # Composition
        self.C = 0.0
        self.Mn = 0.0
        self.Si = 0.0
        self.Ni = 0.0
        self.Cr = 0.0
        self.Mo = 0.0
        self.Co = 0.0
        self.V = 0.0
        self.Al = 0.0
        self.Cu = 0.0
        self.Ti = 0.0
        self.P = 0.0
        self.W = 0.0
        self.As = 0.0

        # Grain size number
        self.G = 0

        self.Ae_form = ''
        self.Ms_form = ''

    # Starting temperatures
    def get_Ae3(self):
        if hasattr(self, 'Ae3'):
            return self.Ae3

        grange = (1570 - 323*self.C - 25*self.Mn + 80*self.Si - 32*self.Ni - 3*self.Cr - 30)*5/9
        andrews = 910 - 203*self.C**.5 + 44.7*self.Si - 15.2*self.Ni + 31.5*self.Mo + 104*self.V \
            + 13.1*self.W - 30*self.Mn + 11*self.Cr + 20*self.Cu - 700*self.P- 400*self.Al - 120*self.As- 400*self.Ti
        eldis = 871.0 - 254.4*self.C**0.5 + 51.7*self.Si - 14.2*self.Ni

        if self.Ae_form == '':
            return (grange + andrews + eldis) / 3
        if self.Ae_form == 'Grange':
            return grange
        if self.Ae_form == 'Andrews':
            return andrews
        if self.Ae_form == 'Eldis':
            return eldis
        raise ValueError

    def get_Ae1(self):
        # Grange 1961
        # Andrews 1965
        # Eldis 1982
        if hasattr(self, 'Ae1'):
            return self.Ae1

        grange = (1333 - 25*self.Mn + 40*self.Si - 26*self.Ni + 42*self.Cr - 32)*(5/9)
        andrews = 723 - 16.9*self.Ni + 29.1*self.Si + 6.38*self.W - 10.7*self.Mn + 16.9*self.Cr + 290*self.As
        eldis = 712 - 17.8*self.Mn - 19.1*self.Ni + 20.1*self.Si + 11.9*self.Cr + 9.8*self.Mo

        if self.Ae_form == '':
            return (grange + andrews + eldis) / 3
        if self.Ae_form == 'Grange':
            return grange
        if self.Ae_form == 'Andrews':
            return andrews
        if self.Ae_form == 'Eldis':
            return eldis
        raise ValueError

    def Ca(self, T):
        grange = (-T*9./5. + 1570. - 25.*self.Mn + 80.*self.Si - 32.*self.Ni - 3.*self.Cr - 30.)/323.
        andrews = ((-T + 910. - 203.*self.C**.5 + 44.7*self.Si - 15.2*self.Ni + 31.5*self.Mo + 104.*self.V \
          + 13.1*self.W - 30.*self.Mn + 11.*self.Cr + 20.*self.Cu - 700.*self.P - 400.*self.Al - 120.*self.As- 400.*self.Ti) \
          / 203.)**2
        eldis = ((-T + 871.0 + 51.7*self.Si - 14.2*self.Ni)/254.4)**2

        if self.Ae_form == '':
            return (grange + andrews + eldis) / 3
        if self.Ae_form == 'Grange':
            return grange
        if self.Ae_form == 'Andrews':
            return andrews
        if self.Ae_form == 'Eldis':
            return eldis
        raise ValueError

    def get_Bs(self):
        # Li et al. 1998
        if hasattr(self, 'Bs'):
            return self.Bs
        return 637 - 58*self.C - 35*self.Mn - 15*self.Ni - 34*self.Cr - 41*self.Mo

    def get_Ms(self):
        # Kung and Rayment 1982
        if hasattr(self, 'Ms'):
            return self.Ms
        stevenhaynes = 561 - 474*self.C - 33*self.Mn - 17*self.Cr - 17*self.Ni - 21*self.Mo + 10*self.Co - 7.5*self.Si
        andrews = 539 - 423*self.C - 30.4*self.Mn - 12.1*self.Cr - 17.7*self.Ni - 7.5*self.Mo - 10*self.Co - 7.5*self.Si

        if self.Ms_form == '':
            return 0.5*(stevenhaynes + andrews)
        if self.Ms_form == 'StevenHaynes':
            return stevenhaynes
        if self.Ms_form == 'Andrews':
            return andrews
        raise ValueError
        return

    def get_Me(self):
        return 80

    # Composition functions
    def fun_comp_f(self):
        if hasattr(self, 'comp_f'):
            return self.comp_f / (2**(0.41*self.G))
        return np.exp(1.0 + 6.31*self.C + 1.78*self.Mn + 0.31*self.Si + 1.12*self.Ni + 2.7*self.Cr + 4.06*self.Mo) / (2**(0.41*self.G))

    def fun_comp_p(self):
        if hasattr(self, 'comp_p'):
            return self.comp_p / (2**(0.32*self.G))
        return np.exp(-4.25 + 4.12*self.C + 4.36*self.Mn + 0.44*self.Si + 1.71*self.Ni + 3.33*self.Cr + 5.19*self.Mo**0.5) / (2**(0.32*self.G))

    def fun_comp_b(self):
        if hasattr(self, 'comp_b'):
            return self.comp_b / (2**(0.29*self.G))
        return np.exp(-10.23 + 10.18*self.C + 0.85*self.Mn + 0.55*self.Ni + 0.90*self.Cr + 0.36*self.Mo) / (2**(0.29*self.G))



def split_inc_cool(dt, T, T_prev, Ts, Te):
    if T_prev > Ts:
        dt_nuc = (Ts-T)/(T_prev-T)*dt
        T_nuc = T
    elif T < Te:
        dt_nuc = (T_prev-Te)/(T_prev-T)*dt
        T_nuc = Te
    else:
        dt_nuc = dt
        T_nuc = T
    return dt_nuc, T_nuc

def split_inc_heat(dt, T, T_prev, Ts):
    if T_prev < Ts:
        dt_nuc = (T-Ts)/(T-T_prev)*dt
        T_nuc = T
    else:
        dt_nuc = dt
        T_nuc = T
    return dt_nuc, T_nuc

# G-T functions
def fun_T_f(T, T_start):
    return (T_start-T)**3*np.exp(-1.38e4/(T+273))

def fun_T_p(T, T_start):
    return (T_start-T)**3*np.exp(-1.38e4/(T+273))

def fun_T_b(T, T_start):
    return (T_start-T)**2*np.exp(-1.38e4/(T+273))

# Sigmoid function
def fun_X(x):
    return x**(0.4*(1-x)) * (1-x)**(0.4*x)

def fun_dX_dx(x):
    return x**(0.4*(1-x)) * (1-x)**(0.4*x) \
        * ((0.4*(1-x))/x - 0.4*np.log(x) + 0.4*np.log(x)*(1-x) - 0.4*x/(1-x))

def fun_S(X_start, X_end):
    fun_eval = lambda x: 1.0/(x**(0.4*(1-x)) * (1-x)**(0.4*x))
    return scipy.integrate.quad(fun_eval, X_start, X_end)[0]

if __name__ == "__main__":
    main()
