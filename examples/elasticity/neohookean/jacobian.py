import numpy as np
import mygrad as mg

def main():

    # F = mg.random.rand(3,3)

    # Jf = lambda F: det(F)
    # Jinv23f = lambda F: 1.0 / Jf(F)**(2.0/3.0)
    # I1f = lambda F: np.einsum("ij,jk,ki", mg.eye(3), F.T, F)

    # J = Jf(F)
    # dJ = J * inv(F).T
    # J.backward()
    # print("dJ", np.sum(np.abs(dJ.data - F.grad)) / dJ.size)

    # Jinv23 = Jinv23f(F)
    # dJinv23 = -2.0/3.0 * Jinv23 * inv(F).T
    # Jinv23.backward()
    # print("dJinv23", np.sum(np.abs(dJinv23.data - F.grad)) / dJinv23.size)


    # I1 = I1f(F)
    # dI1 = 2.0 * F
    # I1.backward()
    # print("dI1", np.sum(np.abs(dI1.data - F.grad)) / dI1.size)

    J = lambda F: np.linalg.det(F)
    Jinv23 = lambda F: 1.0 / J(F)**(2.0/3.0)
    I1 = lambda F: np.trace(np.dot(F.T, F))
    I1b = lambda F: Jinv23(F) * I1(F)

    Finv = lambda F: np.linalg.inv(F)
    FinvT = lambda F: Finv(F).T
    Fb = lambda F: Jinv23(F) * F

    print("dJ")
    dJ = lambda F: J(F) * np.linalg.inv(F).T
    test_dadA(J, dJ)
    
    print("dJinv23")
    dJinv23 = lambda F: -2.0/3.0 * Jinv23(F) * np.linalg.inv(F).T
    test_dadA(Jinv23, dJinv23)
    
    print("dI1")
    dI1 = lambda F: 2.0 * F 
    test_dadA(I1, dI1)

    print("dI1b")
    dI1b = lambda F: 2.0 * Fb(F) - 2./3. * I1b(F) * np.linalg.inv(F).T
    test_dadA(I1b, dI1b)

    print("dFb")
    dFb = lambda F: Jinv23(F) * I4RT - 2./3. * np.einsum("ij,kl", Fb(F), np.linalg.inv(F).T)
    test_dAdA(Fb, dFb)

    print("dFinv")
    dFinv = lambda F: -np.einsum("il,kj->ijkl", Finv(F), Finv(F))
    dFinv = lambda F: -np.einsum("ik,lj->ijkl", Finv(F), Finv(F))
    test_dAdA(Finv, dFinv)
    
    print("dFinvT")
    dFinvT = lambda F: -np.einsum("ki,jl->ijkl", Finv(F), Finv(F))
    dFinvT = lambda F: -np.einsum("li,jk->ijkl", Finv(F), Finv(F))
    test_dAdA(FinvT, dFinvT)

    print("d2J")
    d2J = lambda F: np.einsum("ij,kl", FinvT(F), dJ(F)) + J(F) * dFinvT(F)
    test_dAdA(dJ, d2J)

    print("d2I1b")
    d2I1b = lambda F: 2.0 * dFb(F) - 2./3. * (np.einsum("ij,kl", FinvT(F), dI1b(F)) + I1b(F) * dFinvT(F))
    test_dAdA(dI1b, d2I1b)

    print("dP vol")
    Pvol = lambda F: 0.5*(J(F) - 1.0/J(F)) * dJ(F)
    dPvol = lambda F: 0.5*(1.0 + 1.0 / J(F)**2 ) * np.einsum("ij,kl", dJ(F), dJ(F)) + 0.5*(J(F) - 1.0/J(F)) * d2J(F)
    test_dAdA(Pvol, dPvol)

def test_dadA(function, dfunction):
    """
    Test the Jacobian of a function that takes a 2nd order tensor as input.
    """
    # Compute the function value and its Jacobian
    A = np.random.rand(3, 3) + np.eye(3)
    f = function(A)
    df = dfunction(A)

    # Compute the Jacobian using finite differences
    eps = 1e-6
    error = 0.0
    for i in range(3):
        for j in range(3):
            A_eps = A.copy()
            A_eps[i, j] += eps
            f_eps = function(A_eps)
            error += np.abs((f_eps - f) / eps - df[i,j])

    error /= 9.0

    if error > eps * 1e2:
        print("test FAILED with", error) 
    else:
        print("test PASSED with", error)

    return

def test_dAdA(function, dfunction):
    """
    Test the Jacobian of a function that takes a 2nd order tensor as input.
    """
    # Compute the function value and its Jacobian
    A = np.random.rand(3, 3) + np.eye(3)
    f = function(A)
    df = dfunction(A)

     # Compute the Jacobian using finite differences
    eps = 1e-6
    error = 0.0
    for i in range(3):
        for j in range(3):
            A_eps = A.copy()
            A_eps[i, j] += eps
            f_eps = function(A_eps)
            error += np.sum(np.abs((f_eps - f) / eps - df[:,:,i,j])) / 9.0
            
    error /= 9.0

    if error > eps * 1e2:
        print("test FAILED with", error) 
    else:
        print("test PASSED with", error)

I4RT = np.zeros((3,3,3,3))
for i in range(3):
    for j in range(3):
        I4RT[i,j,i,j] = 1.0

if __name__ == "__main__":
    main()