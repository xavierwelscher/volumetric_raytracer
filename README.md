# Volumetric Raytracer

Raymarcher to render participating media in real-time.

## Build

Prerequisites
- **C++ Compiler**
- **CMake**
- **Git**

> [!WARNING]
> This Project might require the following packages
> * **Python 3**
> * **Jinja2** for GLAD-Code-Generation. Can be installed via `pip install jinja2`

### Linux
> Buiding-mode: RelWithDebInfo
```
cmake -B build
cmake --build build

./build/bin/caustx
```

### Windows

```
cmake -B build
cmake --build build --config Release

.\build\bin\caustx.exe
```

## Related Works:

- SimonDev: [How Big Budget AAA Games Render Clouds](https://www.youtube.com/watch?v=Qj_tK_mdRcA)
- Hillaire, Sebastien: [Physically Based Sky, Atmosphere and Cloud Rendering in Frostbite](https://media.contentapi.ea.com/content/dam/eacom/frostbite/files/s2016-pbs-frostbite-sky-clouds-new.pdf)
- Hillaire, Sebastien: [Physically Based and Unified Volumetric Rendering in Frostbite](https://www.slideshare.net/slideshow/physically-based-and-unified-volumetric-rendering-in-frostbite/51840934)
- Schneider, Andrew: [The Real-Time Volumetric Cloudscape of Horizon Zero Dawn](https://www.guerrilla-games.com/read/the-real-time-volumetric-cloudscapes-of-horizon-zero-dawn)
- Schneider, Andrew: [Nubis: Authoring Real-Time Volumetric Cloudscapes with the Decima Engine. SIGGRAPH 2017 Course Notes](https://drive.google.com/file/d/0B-D275g6LH7LOE1RcVFERGpkS28/view?resourcekey=0-P04mYcVQ1lDPdn7FDunEIw)
- Quilez, Inigo: [Rendering Worlds with Two Triangles](https://iquilezles.org/articles/nvscene2008/rwwtt.pdf)
- Quilez, Inigo: [Terrainmarching](https://iquilezles.org/articles/terrainmarching/)
- Pharr, Matt: [Physically Based Rendering: From Theory to Implementation](https://www.pbr-book.org/)
- Akenine-Moeller, Tomas (2018): Real-Time Rendering, 4th Edition
- Perlin, Ken: [An image synthesizer](https://dl.acm.org/doi/10.1145/325165.325247)
- Worley, Steven: [A Cellular Texture Basis Function](https://dl.acm.org/doi/10.1145/237170.237267)
