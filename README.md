# CharacterClassification

## 项目说明

这是我的暑期实习项目，工作内容是识别纸币上的序列号。方案是通过二值化，提取文字区域，分割字符，字符识别4个步骤完成

## 二值化

二值化使用的方法是局部二值化。为了加速计算，在嵌入式设备上最好先计算积分图，然后在积分图上使用局部二值化方法

## 文字区域提取

首先对文本可能旋转角度采样，例如文本的角度在【-10,10】度之间，我们在这个范围内均匀采10个点，我们就有10个旋转角。然后对每一个旋转角，对每一个图像左侧起点做一条直线，计算这个直线上侧黑色点数。那么只有旋转角与文本的旋转角度最接近时，上下两条直线之间的距离最短。

得到上下两条边线，应用旋转算法将文本调整到水平位置

## 文本分割

将黑色斑点数目在横轴上投影，