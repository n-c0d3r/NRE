# NRE
NCoder's Learning Aimed C++ Render Engine.

---

## Features
+ Render pipelines:
  + FirstRP:
    + Support simple graphics APIs (DirectX 11)
    + Cascaded shadows
    + PBR
    + Image-based sky lighting
  + NewRG
    + Support advanced graphics APIs (DirectX 12)
    + Render graph
    + Multithreaded rendering
    + Virtual Geometry
      + Order-Independent Transparency
    + Unified Mesh System
      + Directed Acyclic Graph Generation
      + Mesh Clustering
+ Task system
+ Basic actor system
+ Hierarchy:
  + Transform node
+ Spectator
+ Camera
+ Renderable system
  + Static mesh renderable
+ Render view system
  + Simple render view
+ Light system
  + Directional light
+ General textures:
  + Texture builders to build texel data
  + Support basic shader resource view, unordered access view,...
+ Basic asset system
  + Static mesh asset:
    + Wavefront object file format (.obj)
  + Texture 2d asset:
    + HDR file format (.hdr)
    + PNG file format (.png)
    + JPG file format (.jpg)
  + Shader asset:
    + HLSL file format (.hlsl)
+ Panorama map to cube map

---

## Samples
+ Abytek Render Path
![alt text](./nre/resources/textures/screenshots/oit_vg.png)
![alt text](./nre/resources/textures/screenshots/abytek_render_path.png)
+ Hello NRE
![alt text](./nre/resources/textures/screenshots/hello_nre.png)
+ Cube
![alt text](./nre/resources/textures/screenshots/cube.png)
+ Cube with texture 2D
![alt text](./nre/resources/textures/screenshots/cube_with_texture_2d.png)
+ HDRI Sky
![alt text](./nre/resources/textures/screenshots/hdri_sky.png)
+ Image Based Lighting (Diffuse + Specular)
![alt text](./nre/resources/textures/screenshots/ibl.png)

---

## Core Dependencies
+ [NCPP](https://github.com/n-c0d3r/NCPP)
+ [NMath](https://github.com/n-c0d3r/NMath)
+ [NSurface](https://github.com/n-c0d3r/NSurface)
+ [NRHI](https://github.com/n-c0d3r/NRHI)
+ [NTS](https://github.com/n-c0d3r/NTS)
+ OBJ-Loader
+ [FreeImage](https://github.com/danoli3/FreeImage)
+ libpng
+ [imgui](https://github.com/ocornut/imgui)
+ zlib
+ [meshoptimizer](https://github.com/zeux/meshoptimizer)
+ [nanoflann](https://github.com/jlblancoc/nanoflann/tree/master)
+ [METIS](https://github.com/n-c0d3r/METIS)

---

## Supported Platforms 
  + Windows

---

## Future-Compatible Platforms 
  + MacOS
  + Android
  + IOS

---

## Supported IDEs and Text Editors (for scripts)
  + Visual Studio
  + Xcode
  + CLion
  + Visual Studio Code

---

## Building
  + **Visual Studio 2022** (Windows):
    + Step 1: Clone [NRE git repository](https://github.com/Abytek/NRE) from GitHub
    + Step 2: Generate Visual Studio 2022 solution by running **scripts/generate_vs2022.bat**
    + Step 3: Navigate to **build/vs2022**
    + Step 4: Open **nre.sln**
    + Step 5: Set **nre.samples.hello_nre** as startup project
    + Step 6: Run and enjoy!
  + **Xcode** (MacOS):
    + Step 1: Clone [NRE git repository](https://github.com/Abytek/NRE) from GitHub
    + Step 2: Generate Xcode solution by running **scripts/generate_xcode.sh**
    + Step 3: Navigate to **build/xcode**
    + Step 4: Open **nre.xcodeproj**
    + Step 5: Choose **nre.samples.hello_nre** for current scheme
    + Step 6: Run and enjoy!
  + **CLion** (Windows, MacOS, Linux):
    + Step 1: Clone [NRE git repository](https://github.com/Abytek/NRE) from GitHub
    + Step 2: Open CLion by running one of these scripts based on your environment (you may need to use chmod +x command first on Linux and MacOS for execute permission):
      + Windows
        + **scripts/python_open_clion.bat**
        + **scripts/python3_open_clion.bat**
      + MacOS, Linux:
        + **scripts/python_open_clion.sh**
        + **scripts/python3_open_clion.sh** 
    + Step 3: At **On Project Wizard**, click "OK" (if you dont see it, dont worry, go to the next step).
    + Step 4: Run and enjoy!
  + **Unix makefiles**:
    + Step 1: Clone [NRE git repository](https://github.com/Abytek/NRE) from GitHub
    + Step 2: Generate makefiles by running **scripts/generate_unix_makefiles.sh**
    + Step 3: execute **scripts/build_unix_makefiles.sh** to build project
    + Step 4: Navigate to **build/unix_makefiles/nre/samples/hello_nre**
    + Step 5: Run "nre.samples.hello_nre" and enjoy!

---

## License
+ NRE is licensed under the [MIT License](https://github.com/n-c0d3r/NRE/blob/main/LICENSE)
