# Kaesar
一个渲染引擎，本来是跟着 Cherno 要完成一个游戏引擎的，但发现自己好像更对渲染感兴趣些，所以最后只做了渲染的部分，不过比 Cherno 的 Hazel 渲染部分做的深一些好像（也可能没有，因为视频我没看完😂）。最后主要是参考了 [ErfanMo77](https://github.com/ErfanMo77) 的 [Syndra](https://github.com/ErfanMo77/Syndra)。

目前还没完成，最近比较忙没太多时间来继续写这个，可能论文开题之后会继续，之后也可能会将其完善成一个引擎😊。

### 截图

![PBR](https://cdn.jsdelivr.net/gh/qzlu-cyber/PicgoLib@main/images/202309151956224.png)

![PCSS](https://cdn.jsdelivr.net/gh/qzlu-cyber/PicgoLib@main/images/202309151959453.png)

### 特性

- 引擎
  - 使用了 [ImGui](https://github.com/ocornut/imgui) 编辑器 Editor 支持 Docking
  - 实体组件系统（ECS）
  - 事件系统
  - 控制台日志
  - 代码高度封装
  - 模型和纹理加载，使用了 [Assimp](https://github.com/assimp/assimp) 支持 40 余种文件格式
  - 使用 YAML 序列化和反序列化以保存和加载场景
  - 调试可视化工具（GBuffer、深度/法线贴图等）
  - 组件编辑器（材质、网格、光源、名称和相机）
  - Gizmos
  - 加载 HDRI环 境贴图等等
- 渲染
  - 延迟渲染
  - 材质系统
  - PBR (Physically-Based Rendering)
  - IBL (Image-Based Lighting)
  - PCF (Percentage Closer Filtering)
  - PCSS (Percentage Closer Soft Shadows) 软阴影
  - FXAA 抗锯齿等
  - 不同类型的光源等等

### Bugs

😅😅😅目前我已知的大概有两个比较严重的 bug：

1. 从文件中打开场景时，渲染出的颜色会泛白，应该是 HDR 到 LDR 转换的问题；
2. 从文件打开场景或是新建场景时有概率无法渲染出物体（物体材质和 Shader 失效导致全是黑色），但是 HDRI 环境贴图能正常显示。

### 未来（小声🤫）

1. 修复所有 bug
2. 设计一个 Logo
3. 完善环境光部分的代码封装工作
4. 引擎：
   - 资源管理器和内容浏览器
   - 物理引擎
   - 脚本系统等
5. 渲染：
   - 使用 VSSM (Variance Soft Shadow Mapping) 加速 PCSS
   - PRT (Precomputed Radiance Transfer)
   - RSM (Reflective Shadow Maps)
   - SSR (Screen Space Reflection)

### 编译

目前，仅支持 Windows 操作系统，推荐使用 Visual Studio 2022。

首先，使用以下命令克隆存储库：`git clone --recursive https://github.com/qzlu-cyber/Kaesar`。如果之前是非递归地克隆存储库的，则使用 `git submodule update --init` 命令来克隆必要的子模块。

~~然后，运行 `scripts/setup.py` 脚本来下载并安装所有必要的依赖项，如 VulkanSDK，为什么需要 Vulkan 可以看看[这篇文章](https://blog.csdn.net/alexhu2010q/article/details/126233020)。~~ 这个地方不好配置，而且现在 Vulkan 官网已经没有 Debug 版本的下载链接了（所以也就无法通过 `scripts/setup.py`  来配置了），需要在配置好 Vulkan 的环境后单独在 Vulkan 根目录打开 `maintenancetool.exe` 通过添加组件下载，建议使用 `1.3.216.0` 版本的 VulkanSDK。完成之后双击项目根目录的 `GenerateProjects.bat` 会生成一个 Visual Studio 解决方案。之后，就可以在 Visual Studio 中编译和运行引擎。

### 第三方库

以下是引擎使用的一些第三方库：

- GLFW：用于处理窗口、鼠标和键盘事件。
- GLAD：OpenGL 函数加载器。
- GLM：矩阵和向量操作。
- spdlog：用于控制台调试日志记录。
- Assimp：加载支持 40 多种文件格式的 3D 模型。
- stb_image：用于读取和加载纹理。
- ImGui：用于编辑器用户界面的即时模式图形用户界面。
- ImGuizmo：编辑器中的工具。
- Entt：用于实体组件系统（ECS）。
- yaml-cpp：场景序列化和反序列化。
- Vulkan SDK：着色器编译和反射。
- Premake：项目配置和 Visual Studio 解决方案生成器。

这些第三方库在构建和运行 Kaesar 引擎时发挥了重要作用，提供了各种功能和工具的支持。

### 作者

[qzlu-cyber](https://github.com/qzlu-cyber)

### 感谢

[The Cherno](https://github.com/TheCherno) 的系列课程 C++ 和游戏引擎系列

[ErfanMo77](https://github.com/ErfanMo77) [Syndra](https://github.com/ErfanMo77/Syndra) 引擎

[GAMES 平台](https://games-cn.org/)、[闫令琪](http://sites.cs.ucsb.edu/~lingqi/)老师

- [GAMES101](https://www.bilibili.com/video/BV1X7411F744/) 图形学入门课程

- [GAMES202](https://www.bilibili.com/video/BV1YK4y1T7yY/) 高质量实时渲染

[LearnOpenGL CN (learnopengl-cn.github.io)](https://learnopengl-cn.github.io/) OpenGL 系列教程

### License

Licensed under the Apache License 2.0, see [LICENSE](https://github.com/qzlu-cyber/Kaesar/blob/main/LICENSE) for details.

