import pandas as pd 
import matplotlib.pyplot as plt

data = pd.read_csv("pff_plastic_data.csv")
plt.plot(data["cracktip"], data["Jvalue"])

data = pd.read_csv(f"pff_plastic_staggered_data.csv")
plt.plot(data["cracktip"], data["Jvalue"], linestyle="--")
    
plt.show()
