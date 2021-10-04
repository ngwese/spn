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

the built `spn` directory is structured as a "mod" project. to install it run
either:

- `make install-zip` to build a zip file which is suitable for
  distribution/installation via `maiden`
- `make install-dust` to copy the built mod into `/home/we/dust/code/spn`

once installed the mod needs to be enabled via the `SYSTEM > MODS` menu and then
`SYSTEM > RESTART` used to pick up mod.

## using

more documentation forthcoming, until then examine the `mod/vis.lua` script for an example of starting the client and defining a `touch` callback.
