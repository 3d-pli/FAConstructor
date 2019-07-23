# Fiber Architecture Constructor

![](https://iffmd.fz-juelich.de/uploads/upload_d7724919e724948077db1b64a567e066.png)


The fiber arcitecture constructor (FAConstructor) allows a simple and effective creation of fiber models based on mathematical functions or the manual input of data points. Models are visualized during creation and can be interacted with by translating them in the 3-dimensional space. 

Latest releases can be found at [the release tab](https://github.com/3d-pli/FAConstructor/releases)

Instruction for installation and usage can be found at the [Wiki](https://iffmd.fz-juelich.de/s/HJTaAO9aN#)

**Note:**
Currently, only Linux distributions and macOS are fully supported. macOS might not be supported in future versions due to the deprecation of OpenGL starting at macOS 10.14 (Mojave).

While the program is Windows compatible no installer is currently available. Until then please compile the program using CMake, Visual Studio and vspkg.


Installation
===
## Compile the program

### System Requirements
**Minimal Requirements:**

* Graphics card supporting OpenGL 3.3 natively

* 1GiB RAM

* 2 core processor built since 2007

* 100 MB disk space

**Recommended Requirements:**

* At least NVIDIA Geforce GTX 750Ti / AMD RX560 or more powerful GPU

* 16 GiB RAM for large models

* 4-core processor

### Required programs and packages
* CMake 3.5
* Qt5 5.5
* C++/C compiler supporting C++14
* Make
* HDF5

### Optional programs and packages
* Boost Math (Version 1.65 or newer required)
* OpenMP
* GPerfTools

### Install instructions
Install all needed dependencies using your package manager or by compiling them from source.

Example using Ubuntu or Debian:
```sh
sudo apt install gcc g++ cmake make qt5-default libhdf5-dev libboost-math-dev
```
Example using Manjaro/Arch:
```sh
sudo pacman -S gcc cmake make qt5-base hdf5 boost
```

#### Clone repository
Clone the repository to your local folder via SSH or HTTPS

SSH:
```sh
git clone git@github.com:3d-pli/FAConstructor.git
cd FAConstructor
```
HTTPS:
```sh
git clone git@github.com:3d-pli/FAConstructor.git
cd FAConstructor
```

#### Compile the program
When using Debian the following command may be required before compiling:
```sh
export CPATH=/usr/include/hdf5/serial/:$CPATH
```
Execute the following commands in the project folder:
```sh
mkdir build
cd build/
cmake ..
make -j
```

If everything ran successful the program is located at `FAConstructor/build/FAConstructor` and can be started from there.

## Acknowledgement 
This project does use the C++ Mathematical Expression Parsing And Evaluation Library developed by [ArashPartow](https://github.com/ArashPartow).
It is distributed under the MIT-License
```
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

This open source software code was developed in part or in whole in the Human Brain Project, funded from the European Union’s Horizon 2020 Framework Programme for Research and Innovation under the Specific Grant Agreement No. 785907 (Human Brain Project SGA2).
