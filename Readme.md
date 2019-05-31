在计图作业的基础上添加assimp库用于加载模型

assimp库安装

1. 到官网下载[assimp 4.1.0 zip版](https://github.com/assimp/assimp/releases/tag/v4.1.0)

2. 解压

3. 用CMake进行编译

4. 添加目录路径 assimp 4.1.0/assimp 4.1.0/include

5. 添加cmake build后的库路径 mybuild/code/Debug

6. 将mybuild/code/Debug/assimp-vc140-mt.dll复制到项目内