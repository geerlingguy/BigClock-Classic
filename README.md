# Big Clock for Classic Mac OS

A Big Clock for Classic Mac OS. That's it!

<p align="center"><img alt="BigClock on Mac OS 8.5" src="screenshot.png" height="auto" width="600"></p>

This was built for and tested on an iBook G3 running Mac OS 9.2.

It should work on any PowerPC Macintosh running System 7.1.2 or later (including Mac OS 7.6 or later). It will also run in the Classic environment on Mac OS X 10.0 through 10.4.11.

## Download

TODO: Add download link to 'releases' page.

## Compiling for PowerPC

On a modern Mac, compile within a [Retro68](https://github.com/autc04/Retro68) Docker build environment:

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

## Compiling for 68k

This is untested, but you _should_ be able to build BigClock for 68k Macs using the following command:

```
docker run --rm --platform linux/amd64 -v "$(pwd)":/root -i ghcr.io/autc04/retro68 /bin/bash <<'EOF'
cd /root && rm -rf build68k && mkdir build68k && cd build68k
cmake .. -DCMAKE_TOOLCHAIN_FILE=/Retro68-build/toolchain/m68k-apple-macos/cmake/retro68.toolchain.cmake
make
EOF
```

Same as the PowerPC build, grab the `.bin` or `.dsk`, and run it!

## Usage Notes

On laptops (like my iBook G3 Clamshell), 'power cycling' can disrupt the clock display's regularity. If you're seeing seconds jump a bit, go to Control Panels > Energy Saver > Advanced Settings > (uncheck) 'Allow processor cycling'.

## AI Disclosure

I built this using the assistance of Claude Fable 5.1, since I've never built a Classic Mac OS application before.
