import numpy as np 
import matplotlib.pyplot as plt

def func1():

    tmp = np.loadtxt("/home/sgdd/Internship/Data/Mat_gray.csv", dtype=np.int32, delimiter="\t")
    print(tmp.shape)
    plt.plot(tmp)
    plt.show()

if __name__=="__main__":
    func1()