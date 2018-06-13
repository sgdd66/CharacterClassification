import numpy as np 
import matplotlib.pyplot as plt

def func1():
    #折线图
    tmp = np.loadtxt("/home/sgdd/Internship/Data/Mat.csv", dtype=np.int32, delimiter="\t")
    print(tmp.shape)
    plt.plot(tmp)
    plt.show()

def func2():
    #云图
    tmp = np.loadtxt("/home/sgdd/Internship/Data/Mat.csv",dtype=np.int32,delimiter=",")
    print(tmp.shape)
    plt.imshow(tmp)
    plt.show()

if __name__=="__main__":
    func1()