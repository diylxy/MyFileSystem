# 数据结构课程设计 - SimpleFS 简易树形文件系统
**请注意：本文件系统相对常见的文件系统而言性能较差，可能存在未知BUG，编写主要目的在于完成数据结构课程设计，不建议用于实际项目**
## 编译
本系统采用cmake构建系统编译。

### Linux
确保你已安装CMake和gcc  
```sh
mkdir build
cd build
cmake ..
make
```
例程和库文件将保存在`MyFileSystem/build/out`  

### Windows
确保你已安装CMake和Visual Studio (MSVC)（mingw未测试）  
在当前目录下打开cmd窗口，运行以下命令  
```sh
mkdir build
cd build
cmake ..
```
之后在build目录下使用Visual Studio打开sln工程文件，点击菜单栏的生成-重新生成解决方案  
DLL和对应调试信息文件保存在`MyFileSystem\build\Debug`  

API使用方法请参考：`test/fs.c`  
