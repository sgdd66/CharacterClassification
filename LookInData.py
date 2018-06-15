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

def func3():
    #处理特征向量
    Path="/home/sgdd/Internship/Data/DensityFeature"
    aimChar=['0','1','2','3','4','5','6','7','8','9','A']
    charSum=len(aimChar)
    kernals=np.zeros((22,charSum))
    maxDistanceInClass=np.zeros(charSum)
    distanceBetweenClass=np.zeros((charSum,charSum))

    for i in range(charSum):
        data=np.loadtxt("{0}/{1}.csv".format(Path,aimChar[i]),dtype=np.float,delimiter=",")
        avg=np.mean(data,axis=1,keepdims=True)
        kernals[:,i]=avg[:,0]
        sampleSum=data.shape[1]
        distance=np.zeros((sampleSum,sampleSum))
        for


    

if __name__=="__main__":
    func3()