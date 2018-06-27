import numpy as np 
import matplotlib.pyplot as plt

def func1():
    #折线图
    while(1):
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
    aimChar=['0','1','2','3','4','5','6','7','8','9','A','L','J','K','M','G','H','E','F','C','I','B','D','W']
    charSum=len(aimChar)
    kernals=np.zeros((17,charSum))
    maxDistanceInClass=np.zeros(charSum)
    meanDistanceInClass=np.zeros(charSum)
    varDistanceInClass=np.zeros(charSum)
    distanceBetweenClass=np.zeros((charSum,charSum))

    #先把每个类的几何中心找到，找到每个中个体距离自己类中心的最大距离，找到这些类中心之间的最小距离

    for i in range(charSum):
        data=np.loadtxt("{0}/{1}.csv".format(Path,aimChar[i]),dtype=np.float,delimiter=",")
        avg=np.mean(data,axis=1,keepdims=True)
        kernals[:,i]=avg[:,0]
        sampleSum=data.shape[1]
        distance=[]
        for j in range(sampleSum):
            distance.append( np.sqrt( np.sum( (data[:,j]-kernals[:,i])**2 ) ) )
        maxDistanceInClass[i]=max(distance)
        meanDistanceInClass[i]=np.mean(distance)
        varDistanceInClass[i]=np.std(distance)
    for i in range(charSum-1):
        for j in range(i+1,charSum):
            distanceBetweenClass[i,j]=np.sqrt( np.sum( (kernals[:,i]-kernals[:,j])**2 ) )
            distanceBetweenClass[j,i]=distanceBetweenClass[i,j]
        distanceBetweenClass[i,i]=100
    distanceBetweenClass[charSum-1,charSum-1]=100
    distanceBetweenClass/=2    

    # print(np.round(maxDistanceInClass,2))
    # print(np.round(distanceBetweenClass,2))
    # print(max(maxDistanceInClass))
    # print(np.min(distanceBetweenClass))
    a=np.row_stack((maxDistanceInClass,meanDistanceInClass,varDistanceInClass,distanceBetweenClass))
 
    np.savetxt("{0}/outcome.csv".format(Path),a)
    np.savetxt("{0}/label.txt".format(Path),kernals)   

def func4():
    #处理特征向量
    Path="/home/sgdd/Internship/Data/DensityFeature"
    aimChar=['0','1','2','3','4','5','6','7','8','9']
    charSum=len(aimChar)
    kernals=np.zeros((17,charSum))

    #先把每个类的几何中心找到，找到每个中个体距离自己类中心的最大距离，找到这些类中心之间的最小距离

    for i in range(charSum):
        data=np.loadtxt("{0}/{1}.csv".format(Path,aimChar[i]),dtype=np.float,delimiter=",")
        data=data.reshape((17,-1))
        avg=np.mean(data,axis=1,keepdims=True)
        kernals[:,i]=avg[:,0]
 
    np.savetxt("{0}/Dollar_number.txt".format(Path),kernals)               
    
    aimChar=['A','L','J','K','M','G','H','E','F','C','I','B','D','W']
    charSum=len(aimChar)
    kernals=np.zeros((17,charSum))

    #先把每个类的几何中心找到，找到每个中个体距离自己类中心的最大距离，找到这些类中心之间的最小距离

    for i in range(charSum):
        data=np.loadtxt("{0}/{1}.csv".format(Path,aimChar[i]),dtype=np.float,delimiter=",")
        data=data.reshape((17,-1))
        avg=np.mean(data,axis=1,keepdims=True)
        kernals[:,i]=avg[:,0]
 
    np.savetxt("{0}/Dollar_character.txt".format(Path),kernals)             

if __name__=="__main__":
    func4()




