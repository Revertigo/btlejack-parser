# btlejack-parser

## Introduction
btlejack-parser it a simple tool which allows to parse and analyze packets intercepted by it, arrived from the Bluetooth Low Energy Link Layer.
The tool currently support analyzing small subset of the packets/fields of the Link Layer.

## Installation
First, you have to install boost:
```
sudo apt-get install libboost-all-dev
```
Then build with cmake:
```
mkdir build
cd build
cmake ..
make
```
