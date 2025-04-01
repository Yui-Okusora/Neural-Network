# Neural Network

This is a MLP framework made from scratch in C/C++ by [Yui Okusora](https://github.com/Yui-Okusora).

## About

I made this to gain more knowledge about ML algorithms (especially in Artificial Neural Network architectures) and more advanced stuff about C/C++ and GPU programming.

## Modules
These are modules that make up this framework.

### Foundations
**1. Advanced Memory**
  - Container for memory-mapped files.
  - Allows fast read-write speeds, especially random access.

**2. Memory Manager**
  - Dedicated manager for `Advanced Memory`.
  - Including creating files and handles.
  - Multithreaded copying memory-mapped files.
   
### Maths Framework
**1. Matrix**
  - Basic Matrix manipulations and calculations.
  - Boosted by CUDA-accelerated GPU and `Advanced Memory`.

**2. Tensor** (comming soon)

### Neural Network framework
**1. Net** is the root.

**2. Layers** - templates

  - Fully Connected Layer.
  - Convolution Layer (coming soon).

## Requirements 

  - Windows 11 64-bit
  - NVIDIA GPU (recommended RTX A2000 or above)
### IDE
  - Visual Studio 2022
  - NVIDIA CUDA Toolkit

## License 
- Currently none
