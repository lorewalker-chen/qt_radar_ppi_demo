# Qt雷达方位指示器Demo
基于[Qwt库](https://qwt.sourceforge.io/)中的极坐标系统QwtPolar，针对雷达方位指示器功能进行的二次封装。个人学习记录，如有错误，请您批评指正，我将不胜感激。

## 使用方法
1. 从Qwt官网(https://qwt.sourceforge.io/)下载源码并编译，将编译出的Qwt和QwtPolar库拷入ppi文件夹下对应的库文件夹`lib_windows`或`lib_linux`。
2. 将ppi文件夹拷贝进qt工程目录，并在`.pro`文件中添加`include(./ppi/ppi.pri)`
3. 使用处包含头文件`plan_position_indicator.h`，实例化`PlanPositionIndicator`类

## 功能
### PPI相关
1. 量程可设置