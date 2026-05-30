# Vendored Third-Party Sources

EUI-NEO prefers these local source snapshots by default, so a normal CMake configure does not download build-time dependencies while the `3rd/` sources are present.

Bundled build dependencies:

- `glfw`: GLFW 3.4
- `glad`: `libigl/libigl-glad` snapshot `651a425101365aa6e8504988ef9bb363d066c5ee`
- `tray`: `zserge/tray` snapshot `8dd1358b92562faf7c032cf5362fa97cbc7e13e9`
- `freetype`: FreeType 2.13.3, trimmed to build-required sources
- `harfbuzz`: HarfBuzz 8.5.0, trimmed to build-required sources
- `libpng-1.6.43`: libpng 1.6.43, trimmed to build-required sources
- `zlib-1.3.1`: zlib 1.3.1, trimmed to build-required sources
- single-file headers already used by the project, such as `stb_image.h`, `stb_truetype.h`, and `nanosvg.h`

CMake dependency modes:

- `auto` (default): use `3rd/` when present, and fetch only missing dependencies.
- `bundled`: only use sources under `3rd/`; fail if a bundled dependency is missing.
- `fetch`: fetch build-time dependencies from the pinned upstream URLs.

Strict offline configure:

```sh
cmake -S . -B build -DEUI_DEPS_MODE=bundled
```

Online configure:

```sh
cmake -S . -B build -DEUI_DEPS_MODE=fetch
```

The vendored source trees intentionally exclude upstream tests, generated release assets, large fuzzing fixtures, and documentation that is not needed by the EUI-NEO build.
