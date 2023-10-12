[![Build Status](https://github.com/tgalaj/Mango/actions/workflows/cpp_cmake.yml/badge.svg)](https://github.com/tgalaj/Mango/actions)
[![License](https://img.shields.io/github/license/tgalaj/Mango)](https://github.com/tgalaj/Mango/blob/master/LICENSE)

# Mango
For the time being, Mango (previously Vertex Engine) is an early-stage open source 3D graphics framework mostly for proof of concept graphics applications purposes. 

However, the aim of this project is to create a game engine with featuring a rendering engine (based on Vulkan API), physics engine, audio and animation systems, that is capable of creating 3D games. The engine is being built with doing a remake of a 3D game that I and my team created during the university time (more on that in the future).

## Getting Started

#### 1. Downloading the repository
Make sure to clone the repository with the one of the following commands (to also clone the sources of the dependencies):

```
git clone --recursive https://github.com/tgalaj/Mango.git
```

If you've already cloned the repo run the following command in the root directory to download the submodules:

```
git submodule update --init --recursive
```

#### 2. Building the Project
After cloning the repository, run one of the *.bat* scripts to generate Visual Studio 2019/2022 solution:

* **setup_vs2019.bat** - to generate VS 2019 solution.
* **setup_vs2022.bat** - to generate VS 2022 solution.

Or run the following command in the root directory to generate project files with the default build system for your system:

```
cmake -B build
```

Either of these approaches will create project files in the *build* directory.

## Documentation
Online documentation can be found [here](https://tgalaj.github.io/Mango/).
