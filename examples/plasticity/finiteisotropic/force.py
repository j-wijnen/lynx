import pandas as pd
import matplotlib.pyplot as plt 

data = pd.read_csv('validation_force.csv')

plt.figure()
plt.plot(data["disp"], data["stress"])

# plt.figure()
# plt.plot(smallstrain["strain"], smallstrain["stress"])
# plt.plot(stvenantkirchhoff["strain"], stvenantkirchhoff["stress"])
# plt.plot(hencky["strain"], hencky["stress"])

plt.show()