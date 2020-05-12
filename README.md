# RayTracer
## Feature Demos
### Caustics (using Photon Mapping)
![](./Assets/images/room_1024.png)

### UV Mapping
![](./Assets/images/scene2.png)

### Adaptive Anti-Aliasing
![](./Assets/images/adaptive_anti_alias.png)

## Building
Run `premake4 gmake` followed by `make`.

## Running
The program needs the path to a Lua file that describes the scene to render.<br>
The scene files are under the `./Assets` directory.
Navigate to `./Assets` and run `../RayTracer <lua-file>`.
