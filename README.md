# Qt雷达方位指示器Demo
基于[Qwt库](https://qwt.sourceforge.io/)中的极坐标系统QwtPolar，针对雷达方位指示器功能进行的二次封装。个人学习记录，如有错误，请您批评指正，我将不胜感激。

## 使用方法
1. 从Qwt官网(https://qwt.sourceforge.io/)下载源码并编译，将编译出的Qwt和QwtPolar库拷入ppi文件夹下对应的库文件夹`lib_windows`或`lib_linux`。
2. 将ppi文件夹拷贝进qt工程目录，并在`.pro`文件中添加`include(./ppi/ppi.pri)`
3. 使用处实例化`PlanPositionIndicator`类

## 功能
### PPI相关
1. 设置量程：坐标系比例尺会相应改变
2. 设置雷达角度范围：显示于坐标系中
3. 设置雷达地理位置：显示于右上角
4. 设置雷达北向角：显示于右上角，绘制的所有点迹航迹的角度会随之改变
5. 设置时间：显示于右下角，自动消航时使用
6. 清屏：清除当前绘制的所有点迹航迹

### 点迹相关
1. 设置点迹颜色
2. 显示/隐藏点迹
3. 添加点迹
4. 删除指定CPI的点迹
5. 删除所有点迹
6. 设置是否自动消点，以及自动消点的CPI数

### 航迹相关
1. 设置航迹颜色：包括普通航迹颜色，重点航迹颜色
2. 显示/隐藏航迹
3. 添加航迹
4. 删除指定批号的航迹
5. 删除所有航迹
6. 设置是否自动消航，以及自动消航的时间间隔

### 航迹表格相关
航迹表格模型类`RadarTrackTableModel`继承自`QAbstractTableModel`，实现了一个Qt表格模型，调用`QTableView`或`QTableWidget`的`setModel`函数使用。实现了对普通航迹和重点航迹的添加、删除等处理。