# spn
soundplane for norns

## building

```sh
% git clone git@github.com:ngwese/spn.git
% cd spn
% git submodule update --init --recursive
% mkdir build && cd build
% cmake -DCMAKE_CXX_FLAGS="-march=armv7-a -mfpu=neon-vfpv4" -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ..
% make
```

once built, the `spn` directory within the build directory is a "mod" project which can be copied into `~/dust/code` then enable the mod via the `SYSTEM > MODS` menu and restart.

## using

more documentation forthcoming, until then examine the `mod/vis.lua` script for an example of starting the client and defining a `touch` callback.
