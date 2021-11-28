# btlejack-parser

## Introduction
btlejack-parser it a simple tool which allows to parse and analyze packets intercepted by it, arrived from the Bluetooth Low Energy Link Layer.
The tool currently support analyzing small subset of the packets/fields of the Link Layer. Note that the Link Layer data arrived from btlejack tool
is also partial.

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

## Usage
In order to run btlejack-parser you have to perform the following steps:
1. Run the btlejack-parser executable. This will generate a named pipe which will be uses as IPC between the two processes:
```
./btlejack_parser
```
2. Open a second terminal (in the same path) and run your btlejack command, like in the following example:
```
sudo btlejack -c any | tee btlejack_output
```
This will print the original output of btlejack tool to the second terminal, while the first terminal parses the data comes from the btlejack process.
This command is able to work with other commands which prints a BLE Link-Layer data.

![btlejack-parser-example](https://user-images.githubusercontent.com/46284863/127038054-b66d2bec-ca41-430d-9479-253e7237d8ad.png)
