#!/bin/sh
emconfigure ./configure --host=asmjs-unknown-emscripten --disable-audio --disable-assembly --disable-haptic --disable-joystick --disable-loadso --disable-power --disable-render --disable-threads --enable-cpuinfo=false CFLAGS="-O2"
echo "configure overwrote SDL_config.h, go fix that up"
echo "then do emmake make"
