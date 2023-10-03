# Ray Tracing Engine
This is a [Ray Tracing](https://developer.nvidia.com/discover/ray-tracing) Renderer that aims to render arbitrary scenes with photorealism. The renderer will use Vulkan to leverage the power of the GPU to run our ray tracing algorithms more quickly than is possible using the CPU. Once complete, this renderer can hopefully be used to render scenes using an input file or be integrated into another project like a game or game engine.

Dependencies:
- Vulkan SDK
- GLFW
- ImGui
- Walnut submodule: [Walnut](https://github.com/TheCherno/Walnut) (see repository for more details)

Tasks completed:
- Rendering a single sphere Using Ray Tracing
- Basic Lighting and Shading
- Adding an Interactive 3D Camera System 
- Rendering Multiple Objects

![image](https://github.com/sohaib-nadeem/RayTracing/assets/72913188/eb1d117c-e172-4fe1-9c76-4b6e54065fed)

- Add Materials and Physically Based Rendering

<img width="982" alt="ray_tracing_updated" src="https://github.com/sohaib-nadeem/RayTracing/assets/72913188/27b98e9d-adf3-4a68-9e4a-dc4e86e8f1a8">

## To Do
- Move code to gpu using vulkan (in progress)
- Add support for rendering triangles and 3D meshes
- Path Tracing
- Add Emission and Emissive Materials
- Performance improvements: Multithreading, Fix Slow Code

 ## Setup instructions (to be added)
