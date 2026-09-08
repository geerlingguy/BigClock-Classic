# Big Clock for Classic Mac OS

A Big Clock for Classic Mac OS. That's all there is to it.

Tested on Mac OS 9.2, but should work on other versions (TODO which?).

## Compiling

On another Mac (preferably a nice fast one), set up a Retro68 build environment using Docker:

```
docker run --rm -v "$(pwd)":/root -i ghcr.io/autc04/retro68 /bin/bash <<'EOF'
cd /root && rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/Retro68-build/toolchain/powerpc-apple-macos/cmake/retroppc.toolchain.cmake
make
EOF
```

This should produce artifacts including:

  - `build/BigClock.bin`
  - `build/BigClock.dsk`

Copy `BigClock.bin` over to your Classic Mac and expand it with Stuffit Expander.

## Usage Notes

On laptops (like my iBook G3 Clamshell), 'power cycling' can disrupt the clock display's regularity. If you're seeing seconds jump a bit, go to Control Panels > Energy Saver > Advanced Settings > (uncheck) 'Allow processor cycling'.

## AI Disclosure

I built this using the assistance of Claude Fable 5.1, since I've never built a Classic Mac OS application before.
