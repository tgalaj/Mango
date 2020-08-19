[![Build Status](https://travis-ci.com/Shot511/VertexEngine.svg?branch=master)](https://travis-ci.com/Shot511/VertexEngine)

# Vertex Engine
This is an open source 3D graphics engine mostly for proof of concept graphics applications purposes. In the future it may evolve into some kind of a game engine.

## How to build
Make sure to clone the repository with the one of the following commands (to also clone the sources of the dependencies):

```
git clone --recurse-submodules -j8 https://github.com/Shot511/VertexEngine

OR

git clone --recurse-submodules https://github.com/Shot511/VertexEngine

OR

git clone --recursive https://github.com/Shot511/VertexEngine
```

If you've already cloned the repo run the following command in the root directory:

```git submodule update --init --recursive```

Then, simply run the following commands in the root directory:

```
mkdir build
cd build
cmake ..
```

These will create project (inside the *build* directory) with your system-default build system which will allow you to build and run the engine.

## Short-term TODOs
- [x] Refactor engine's architecture
- [x] Remove watch_ptr class
- [x] Add base abstract class for lights
- [x] Improve scene graph
- [x] Easy to use toolkit to create demo/game scenes
- [x] Is CoreAssetManager necessary? Ans: Yes, it, is
- [x] Better resource management 
- [x] Easy to use material system
- [x] Refactored & enhanced shader class
- [x] Add feature to add #include directive in shaders (generic default shaders)
- [x] Update & move dlls to bin32/bin64 subfolders
- [x] Add Forward rendering technique
- [x] Add Deferred rendering technique
- [ ] Add easy to use interface to switch between different rendering techniques
- [ ] Easy to use interface for postprocess effects
- [x] Basic Shadow mapping
- [ ] Soft shadows
- [ ] Variance Shadow Mapping / Exponential Shadow Mapping, etc.
- [ ] Models' animation (linear blend skinning, dual quaternion skinning)
- [x] Directional ligths support
- [x] Spot ligths support
- [x] Point ligths support
- [ ] Light Shafts
- [x] Support for geometry shaders
- [x] Support for compute shaders
- [x] Support for tesselation shaders

## Long-term TODOs
- [x] Convert project to use CMake
- [ ] Graphical editor to ease creation of demo/game scenes
- [ ] Support for Lua scripting language
- [ ] Constructive solid geometry
- [ ] GPU supported ray tracing, photon mapping
- [ ] Support for Vulkan renderer
- [ ] Efficient and realistic atmospheric scattering
