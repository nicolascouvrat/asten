# ASTE(N)

**ASTE(N)** stands for **Another STandard Emulator (Nes)**. This is a homemade version of a NES emulator, that
supports basic functionality, such as:

* CPU: Official opcodes
* Horizontal and vertical scrolling
* 8x8 sprites

Code wise, it features:

* Interface abstractions of UIs allowing to compile minimal versions (having a graphical output is
  not required to run Asten! Although I don't see why you would want to do that).
* Test suite

# Installation

## Install minimal libraries

Asten requires some `-dev` libraries, along with C++11.

## Compile

After cloning this repository, in the repository folder

```
mkdir build
cd build
cmake ..
make
sudo make install
```

## Usage

Simply run:

```
asten <ROM_FILE>
```

## Compatibility

This has been tested and should work on both IOS and linux.

# Purpose

Why bother creating another (imperfect) emulator when there are so many out there?

* Challenging myself with creating an emulator
* Learning more about CPUs and graphical units using legacy hardware (arguably easier to understand
  than modern one)
* Learning C++ (that I had not touched before) and CMake with a real, consequent project

Besides, another goal of mine (in line with learning C++ from the ground up) was to _not_ use any
fancy libraries such as Boost. Outside of graphical libraries required to compile the UI, the Asten
core is written in pure vanilla C++11!

# Screenshots

## v0.0

![Donkey Kong](asten_dk.jpeg "Donkey Kong")

# Passing tests

* `nestest`
* `ram_after_reset`

See [here](https://github.com/christopherpow/nes-test-roms) for test list.
