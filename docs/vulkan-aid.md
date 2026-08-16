# Vulkan Quick Aid (volk + SDL3 + dynamic rendering)

A pseudocode map from "nothing" to "triangle on screen" and a running frame loop,
written for someone who knows OpenGL but not Vulkan.

**Targets:** Vulkan 1.3+ with **dynamic rendering** (`VK_KHR_dynamic_rendering`, core in 1.3).
No `VkRenderPass` / `VkFramebuffer` objects.
**Loader:** volk. **Window/surface:** SDL3, passed in as parameters.
**Detail level:** step outline + the exact struct/function names to look up in the spec.

Every `>> look up:` line is a symbol to search in the Vulkan spec / Registry:
- Spec: https://registry.khronos.org/vulkan/specs/1.3-extensions/html/
- Man pages: https://registry.khronos.org/vulkan/specs/1.3/man/html/<Name>.html

---

## Mental model: OpenGL vs Vulkan

| OpenGL concept | Vulkan equivalent you build yourself |
|---|---|
| Hidden global context | `VkInstance` + `VkDevice` you create and own |
| Driver picks the GPU | You enumerate + pick a `VkPhysicalDevice` |
| `glClear` / default framebuffer | Swapchain images + you record clear/draw yourself |
| Implicit sync, driver stalls | Explicit `VkSemaphore` / `VkFence` / barriers |
| `glBindBuffer` + upload | Allocate `VkDeviceMemory`, map, or staging-copy |
| Uniforms | Descriptor sets / push constants |
| GLSL compiled by driver | SPIR-V compiled ahead of time (glslc / shaderc) |
| PSO baked at draw time | `VkPipeline` baked up front (mostly immutable) |

Golden rule: **Vulkan validates almost nothing at runtime.** Enable the
**validation layers** (`VK_LAYER_KHRONOS_validation`) during development — they are your compiler-error equivalent.

---

## 0. Ownership / lifetime overview

Create order (roughly) and destroy in **reverse**:

```
Instance
  -> (debug messenger)
  -> Surface (from SDL)
  -> PhysicalDevice (picked, not created)
  -> Device + Queues
  -> Swapchain -> swapchain images -> image views
  -> Command pool -> command buffers
  -> Sync objects (semaphores, fences)
  -> Descriptor set layout -> pipeline layout -> pipeline
  -> Buffers / images + their memory + descriptor pool/sets
```

---

## 1. Loader + Instance

```c
// Renderer holds everything. Pass this around instead of globals.
typedef struct Renderer {
    VkInstance        instance;
    VkSurfaceKHR      surface;
    VkPhysicalDevice  gpu;
    VkDevice          device;
    uint32_t          graphics_family;   // queue family index
    VkQueue           graphics_queue;
    VkQueue           present_queue;     // often same as graphics
    // swapchain, sync, pipeline fields added below...
} Renderer;

int render_create_instance(Renderer* r, SDL_Window* window) {
    volkInitialize();                     // load vkGetInstanceProcAddr etc.
    >> look up: volkInitialize, volkLoadInstance, volkLoadDevice

    // SDL tells you which instance extensions the surface needs.
    Uint32 sdl_ext_count = 0;
    const char* const* sdl_exts = SDL_Vulkan_GetInstanceExtensions(&sdl_ext_count);
    >> look up: SDL_Vulkan_GetInstanceExtensions

    // Copy sdl_exts into a list, then append debug utils in debug builds.
    const char* extensions[32]; uint32_t n = 0;
    for (i in sdl_exts) extensions[n++] = sdl_exts[i];
#ifdef DEBUG
    extensions[n++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#endif

    const char* layers[] = { "VK_LAYER_KHRONOS_validation" }; // debug only

    VkApplicationInfo app = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = APP_NAME,
        .applicationVersion = APP_VERSION,
        .pEngineName = ENGINE_NAME,
        .apiVersion = VK_API_VERSION_1_3,
    };
    VkInstanceCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app,
        .enabledExtensionCount = n,
        .ppEnabledExtensionNames = extensions,
        .enabledLayerCount = DEBUG ? 1 : 0,
        .ppEnabledLayerNames = layers,
    };
    verify(vkCreateInstance(&ci, NULL, &r->instance));

    volkLoadInstance(r->instance);        // now instance-level fns are loaded

    // Optional but recommended: install a debug callback that logs messages.
    >> look up: vkCreateDebugUtilsMessengerEXT, VkDebugUtilsMessengerCreateInfoEXT
    return 0;
}
```

---

## 2. Surface (from SDL)

```c
int render_create_surface(Renderer* r, SDL_Window* window) {
    SDL_Vulkan_CreateSurface(window, r->instance, NULL, &r->surface);
    >> look up: SDL_Vulkan_CreateSurface, VkSurfaceKHR
    // Destroy later with vkDestroySurfaceKHR (NOT an SDL call in Vulkan-side).
    return 0;
}
```

---

## 3. Physical device selection + queue families

```c
int render_pick_gpu(Renderer* r) {
    uint32_t count;
    vkEnumeratePhysicalDevices(r->instance, &count, NULL);
    VkPhysicalDevice list[count];
    vkEnumeratePhysicalDevices(r->instance, &count, list);
    >> look up: vkEnumeratePhysicalDevices

    for each gpu in list:
        // Query properties/features to score it (prefer discrete GPU).
        >> look up: vkGetPhysicalDeviceProperties2, vkGetPhysicalDeviceFeatures2

        // Find a queue family that supports GRAPHICS *and* can present.
        uint32_t qcount;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &qcount, NULL);
        for f in [0..qcount):
            has_graphics = (family[f].queueFlags & VK_QUEUE_GRAPHICS_BIT);
            VkBool32 can_present;
            vkGetPhysicalDeviceSurfaceSupportKHR(gpu, f, r->surface, &can_present);
            if (has_graphics && can_present) { pick this gpu + family; }
        >> look up: vkGetPhysicalDeviceQueueFamilyProperties,
        >>          vkGetPhysicalDeviceSurfaceSupportKHR

        // Confirm required device extensions exist (at minimum swapchain).
        >> look up: vkEnumerateDeviceExtensionProperties, VK_KHR_SWAPCHAIN_EXTENSION_NAME

    r->gpu = best; r->graphics_family = chosen_family;
    return 0;
}
```

> Simplification used here: one queue family for both graphics and present.
> If they differ, you create two `VkQueue`s and handle image ownership transfer.

---

## 4. Logical device + queues (enable 1.3 features)

```c
int render_create_device(Renderer* r) {
    float priority = 1.0f;
    VkDeviceQueueCreateInfo qci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = r->graphics_family,
        .queueCount = 1,
        .pQueuePriorities = &priority,
    };

    // Turn ON dynamic rendering + synchronization2 (both core in 1.3).
    VkPhysicalDeviceVulkan13Features feats13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .dynamicRendering = VK_TRUE,
        .synchronization2 = VK_TRUE,
    };
    >> look up: VkPhysicalDeviceVulkan13Features

    const char* dev_exts[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo dci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &feats13,                 // feature chain via pNext
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &qci,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = dev_exts,
    };
    verify(vkCreateDevice(r->gpu, &dci, NULL, &r->device));
    >> look up: vkCreateDevice, VkDeviceCreateInfo

    volkLoadDevice(r->device);             // load device-level fns (fast path)

    vkGetDeviceQueue(r->device, r->graphics_family, 0, &r->graphics_queue);
    r->present_queue = r->graphics_queue;  // same family in this simplification
    >> look up: vkGetDeviceQueue
    return 0;
}
```

---

## 5. Swapchain + image views

This is your "default framebuffer" — a ring of images you draw into and present.

```c
typedef struct Swapchain {
    VkSwapchainKHR handle;
    VkFormat       format;
    VkExtent2D     extent;
    uint32_t       image_count;
    VkImage        images[MAX_IMAGES];      // owned by swapchain, don't destroy
    VkImageView    views[MAX_IMAGES];       // you create + destroy these
} Swapchain;

int render_create_swapchain(Renderer* r, Swapchain* sc, uint32_t w, uint32_t h) {
    // 1) Query what the surface supports.
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r->gpu, r->surface, &caps);
    // formats:
    vkGetPhysicalDeviceSurfaceFormatsKHR(...);      // pick e.g. B8G8R8A8_SRGB
    // present modes:
    vkGetPhysicalDeviceSurfacePresentModesKHR(...); // FIFO always available (vsync)
    >> look up: VkSurfaceCapabilitiesKHR, VkSurfaceFormatKHR, VkPresentModeKHR

    // 2) Choose extent (clamp desired w/h into caps.minImageExtent..maxImageExtent;
    //    if caps.currentExtent is 0xFFFFFFFF you must set it yourself).
    // 3) Choose image count = caps.minImageCount + 1 (clamped to maxImageCount).

    VkSwapchainCreateInfoKHR ci = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = r->surface,
        .minImageCount = image_count,
        .imageFormat = chosen_format,
        .imageColorSpace = chosen_colorspace,
        .imageExtent = chosen_extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, // single queue family
        .preTransform = caps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = chosen_present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,     // pass old one when recreating on resize
    };
    verify(vkCreateSwapchainKHR(r->device, &ci, NULL, &sc->handle));
    >> look up: vkCreateSwapchainKHR, VkSwapchainCreateInfoKHR

    // 4) Retrieve the images (created by the swapchain).
    vkGetSwapchainImagesKHR(r->device, sc->handle, &sc->image_count, sc->images);

    // 5) One image view per image (dynamic rendering renders into a view).
    for i in [0..image_count):
        VkImageViewCreateInfo vci = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = sc->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = chosen_format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1, .layerCount = 1,
            },
        };
        vkCreateImageView(r->device, &vci, NULL, &sc->views[i]);
    >> look up: vkGetSwapchainImagesKHR, vkCreateImageView, VkImageViewCreateInfo
    return 0;
}
```

> **Resize / out-of-date:** when `vkAcquireNextImageKHR` or `vkQueuePresentKHR`
> returns `VK_ERROR_OUT_OF_DATE_KHR` / `VK_SUBOPTIMAL_KHR`, wait idle, destroy the
> swapchain + views, and recreate. Keep this in a `recreate_swapchain()` you call from the loop.

---

## 6. Commands: pool + buffers

```c
int render_create_commands(Renderer* r, Frames* fr) {
    VkCommandPoolCreateInfo pci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // reset per frame
        .queueFamilyIndex = r->graphics_family,
    };
    vkCreateCommandPool(r->device, &pci, NULL, &fr->pool);

    // Allocate ONE command buffer per frame-in-flight (see section 7).
    VkCommandBufferAllocateInfo ai = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = fr->pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = FRAMES_IN_FLIGHT,
    };
    vkAllocateCommandBuffers(r->device, &ai, fr->cmd);
    >> look up: vkCreateCommandPool, vkAllocateCommandBuffers
    return 0;
}
```

---

## 7. Synchronization (the part OpenGL hid from you)

Use **frames in flight** (usually 2) so the CPU can prepare frame N+1 while the GPU renders frame N.

Per frame-in-flight you need:
- `VkFence in_flight` — CPU waits on it so it doesn't overwrite a command buffer still in use.
- `VkSemaphore image_available` — GPU waits for the swapchain image to be ready.
- `VkSemaphore render_finished` — present waits for rendering to finish.

```c
#define FRAMES_IN_FLIGHT 2
typedef struct Frames {
    VkCommandPool   pool;
    VkCommandBuffer cmd[FRAMES_IN_FLIGHT];
    VkSemaphore     image_available[FRAMES_IN_FLIGHT];
    VkSemaphore     render_finished[FRAMES_IN_FLIGHT];
    VkFence         in_flight[FRAMES_IN_FLIGHT];
    uint32_t        current;                 // 0..FRAMES_IN_FLIGHT-1
} Frames;

// Create with:
>> look up: vkCreateSemaphore, vkCreateFence
// Create fences with VK_FENCE_CREATE_SIGNALED_BIT so the first wait doesn't deadlock.
```

> Note on `render_finished`: a robust engine uses **one semaphore per swapchain image**
> for the present-wait rather than per frame-in-flight, to avoid reuse hazards.
> Start per-frame for simplicity; the validation layers will tell you if it bites.

---

## 8. The frame loop (acquire -> record -> submit -> present)

```c
void render_draw_frame(Renderer* r, Swapchain* sc, Frames* fr) {
    uint32_t f = fr->current;

    // 1) Wait until this frame slot's previous work is done, then reset the fence.
    vkWaitForFences(r->device, 1, &fr->in_flight[f], VK_TRUE, UINT64_MAX);

    // 2) Acquire next swapchain image; image_available is signaled when it's ready.
    uint32_t img;
    VkResult acq = vkAcquireNextImageKHR(r->device, sc->handle, UINT64_MAX,
                                         fr->image_available[f], VK_NULL_HANDLE, &img);
    if (acq == VK_ERROR_OUT_OF_DATE_KHR) { recreate_swapchain(); return; }
    >> look up: vkWaitForFences, vkResetFences, vkAcquireNextImageKHR

    vkResetFences(r->device, 1, &fr->in_flight[f]);
    vkResetCommandBuffer(fr->cmd[f], 0);

    // 3) Record commands.
    VkCommandBuffer cb = fr->cmd[f];
    VkCommandBufferBeginInfo bi = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                    .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
    vkBeginCommandBuffer(cb, &bi);

    // 3a) Barrier: UNDEFINED -> COLOR_ATTACHMENT_OPTIMAL before rendering.
    transition_image(cb, sc->images[img],
        srcLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        dstLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    >> look up: vkCmdPipelineBarrier2, VkImageMemoryBarrier2, VkDependencyInfo

    // 3b) Begin dynamic rendering (replaces render pass + framebuffer).
    VkRenderingAttachmentInfo color = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = sc->views[img],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,   // == glClear
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = { .color = {{0,0,0,1}} },
    };
    VkRenderingInfo ri = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = { .extent = sc->extent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color,
    };
    vkCmdBeginRendering(cb, &ri);
    >> look up: vkCmdBeginRendering, VkRenderingInfo, VkRenderingAttachmentInfo

    // 3c) Bind pipeline + dynamic state, then draw.
    vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdSetViewport(cb, 0, 1, &viewport);   // dynamic viewport/scissor is easiest
    vkCmdSetScissor(cb, 0, 1, &scissor);
    // vkCmdBindVertexBuffers / vkCmdBindDescriptorSets / vkCmdPushConstants as needed
    vkCmdDraw(cb, 3, 1, 0, 0);               // 3 verts = triangle
    >> look up: vkCmdBindPipeline, vkCmdSetViewport, vkCmdDraw, vkCmdDrawIndexed

    vkCmdEndRendering(cb);

    // 3d) Barrier: COLOR_ATTACHMENT_OPTIMAL -> PRESENT_SRC_KHR before presenting.
    transition_image(cb, sc->images[img],
        srcLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        dstLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    vkEndCommandBuffer(cb);

    // 4) Submit. Wait on image_available, signal render_finished, signal fence.
    VkCommandBufferSubmitInfo cbs = { .sType=..., .commandBuffer = cb };
    VkSemaphoreSubmitInfo waitS = { .semaphore = fr->image_available[f],
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphoreSubmitInfo sigS  = { .semaphore = fr->render_finished[f],
        .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT };
    VkSubmitInfo2 submit = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = 1,   .pWaitSemaphoreInfos = &waitS,
        .commandBufferInfoCount = 1,   .pCommandBufferInfos = &cbs,
        .signalSemaphoreInfoCount = 1, .pSignalSemaphoreInfos = &sigS,
    };
    vkQueueSubmit2(r->graphics_queue, 1, &submit, fr->in_flight[f]);
    >> look up: vkQueueSubmit2, VkSubmitInfo2, VkSemaphoreSubmitInfo

    // 5) Present. Wait on render_finished.
    VkPresentInfoKHR pi = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1, .pWaitSemaphores = &fr->render_finished[f],
        .swapchainCount = 1, .pSwapchains = &sc->handle, .pImageIndices = &img,
    };
    VkResult pres = vkQueuePresentKHR(r->present_queue, &pi);
    if (pres == VK_ERROR_OUT_OF_DATE_KHR || pres == VK_SUBOPTIMAL_KHR) recreate_swapchain();
    >> look up: vkQueuePresentKHR, VkPresentInfoKHR

    fr->current = (f + 1) % FRAMES_IN_FLIGHT;
}
```

---

## 9. Graphics pipeline (build once, at init)

The PSO bakes shader stages + fixed-function state into one immutable object.
With dynamic rendering you pass the color format instead of a render pass.

```c
int render_create_pipeline(Renderer* r, VkFormat color_format,
                           VkPipelineLayout* out_layout, VkPipeline* out_pipe) {
    // 1) Load SPIR-V (compile GLSL->SPIR-V offline with glslc/glslangValidator).
    VkShaderModule vert = load_shader(r->device, "tri.vert.spv");
    VkShaderModule frag = load_shader(r->device, "tri.frag.spv");
    >> look up: vkCreateShaderModule, VkShaderModuleCreateInfo

    VkPipelineShaderStageCreateInfo stages[2] = {
        { .stage = VK_SHADER_STAGE_VERTEX_BIT,   .module = vert, .pName = "main" },
        { .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = frag, .pName = "main" },
    };

    // 2) Fixed-function state structs (fill each .sType):
    VkPipelineVertexInputStateCreateInfo   vin;   // vertex bindings/attribs (empty for hardcoded tri)
    VkPipelineInputAssemblyStateCreateInfo ia;    // TRIANGLE_LIST
    VkPipelineViewportStateCreateInfo      vp;    // counts = 1, dynamic
    VkPipelineRasterizationStateCreateInfo rs;    // polygonMode FILL, cullMode, lineWidth 1
    VkPipelineMultisampleStateCreateInfo   ms;    // 1 sample
    VkPipelineColorBlendAttachmentState    cba;   // writeMask RGBA, blend off
    VkPipelineColorBlendStateCreateInfo    cb;    // wraps cba
    VkPipelineDepthStencilStateCreateInfo  ds;    // if you add a depth buffer
    >> look up each: VkPipeline*StateCreateInfo

    // 3) Dynamic state so you don't rebuild PSO on resize.
    VkDynamicState dyn[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamic = { ... , .pDynamicStates = dyn };

    // 4) Pipeline layout = descriptor set layouts + push constant ranges.
    VkPipelineLayoutCreateInfo plci = { .sType = ..., /* setLayouts, pushConstants */ };
    vkCreatePipelineLayout(r->device, &plci, NULL, out_layout);
    >> look up: vkCreatePipelineLayout, VkPushConstantRange

    // 5) KEY for dynamic rendering: chain formats via pNext instead of renderPass.
    VkPipelineRenderingCreateInfo rendering = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &color_format,
        // .depthAttachmentFormat = ... if used
    };
    >> look up: VkPipelineRenderingCreateInfo

    VkGraphicsPipelineCreateInfo gp = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &rendering,          // <-- no .renderPass with dynamic rendering
        .stageCount = 2, .pStages = stages,
        .pVertexInputState = &vin, .pInputAssemblyState = &ia,
        .pViewportState = &vp, .pRasterizationState = &rs,
        .pMultisampleState = &ms, .pColorBlendState = &cb,
        .pDynamicState = &dynamic,
        .layout = *out_layout,
        .renderPass = VK_NULL_HANDLE,
    };
    vkCreateGraphicsPipelines(r->device, VK_NULL_HANDLE, 1, &gp, NULL, out_pipe);
    >> look up: vkCreateGraphicsPipelines, VkGraphicsPipelineCreateInfo

    vkDestroyShaderModule(r->device, vert, NULL); // safe once pipeline is built
    vkDestroyShaderModule(r->device, frag, NULL);
    return 0;
}
```

For the very first triangle you can hardcode 3 vertices in the vertex shader with
`gl_VertexIndex` and skip vertex buffers/descriptors entirely.

---

## 10. Resource management (buffers, images, memory, descriptors)

### 10a. Buffers + memory (the manual `glBufferData`)

```c
// Two-step: create buffer (a "shape"), then allocate + bind memory (the "storage").
VkBufferCreateInfo bci = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = bytes,
    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT /* | TRANSFER_DST for staging */,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
};
vkCreateBuffer(device, &bci, NULL, &buffer);

VkMemoryRequirements req;
vkGetBufferMemoryRequirements(device, buffer, &req);

// Pick a memory type index from req.memoryTypeBits that has the properties you want:
//  - HOST_VISIBLE | HOST_COHERENT  -> CPU can map & write (uniforms, staging)
//  - DEVICE_LOCAL                  -> fast GPU memory (vertex/index, via staging copy)
uint32_t type = find_memory_type(gpu, req.memoryTypeBits, desired_props);
>> look up: vkGetPhysicalDeviceMemoryProperties, VkMemoryPropertyFlagBits

VkMemoryAllocateInfo mai = { .sType=..., .allocationSize = req.size, .memoryTypeIndex = type };
vkAllocateMemory(device, &mai, NULL, &memory);
vkBindBufferMemory(device, buffer, memory, 0);
>> look up: vkAllocateMemory, vkBindBufferMemory

// Upload (host-visible path):
void* p; vkMapMemory(device, memory, 0, bytes, 0, &p); memcpy(p, src, bytes); vkUnmapMemory(...);
>> look up: vkMapMemory

// Device-local path: upload into a HOST_VISIBLE staging buffer, then
//   vkCmdCopyBuffer(staging -> device_local) on a one-time command buffer.
>> look up: vkCmdCopyBuffer
```

> **Do not `vkAllocateMemory` per resource in production** — there's a hard limit on
> allocation count. Use one allocation subdivided, or adopt **VMA**
> (VulkanMemoryAllocator) once you understand the manual path.

### 10b. Images / textures

```c
// Like buffers but 2D: create image, allocate DEVICE_LOCAL memory, bind, make a view.
>> look up: vkCreateImage, VkImageCreateInfo, vkBindImageMemory, vkCreateImageView, vkCreateSampler
// Upload path: staging buffer -> transition to TRANSFER_DST -> vkCmdCopyBufferToImage
//   -> transition to SHADER_READ_ONLY_OPTIMAL.
>> look up: vkCmdCopyBufferToImage, VkImageLayout
```

### 10c. Descriptors (how shaders see buffers/textures)

Descriptor set layout = the "interface". Descriptor set = a bound instance.

```c
// 1) Layout: describe bindings (binding number, type, stage, count).
>> look up: vkCreateDescriptorSetLayout, VkDescriptorSetLayoutBinding
//    e.g. binding 0 = UNIFORM_BUFFER in VERTEX, binding 1 = COMBINED_IMAGE_SAMPLER in FRAGMENT

// 2) Pool: preallocate how many descriptors of each type you'll hand out.
>> look up: vkCreateDescriptorPool, VkDescriptorPoolSize

// 3) Allocate sets from the pool using the layout.
>> look up: vkAllocateDescriptorSets

// 4) Point the set at actual buffers/images.
>> look up: vkUpdateDescriptorSets, VkWriteDescriptorSet, VkDescriptorBufferInfo, VkDescriptorImageInfo

// 5) At draw time: vkCmdBindDescriptorSets before vkCmdDraw.
>> look up: vkCmdBindDescriptorSets
```

> For small, frequently-changing data (e.g. an MVP matrix), prefer **push constants**
> (`vkCmdPushConstants`) — no descriptor plumbing, limited to ~128 bytes.

---

## 11. Shutdown order

```c
vkDeviceWaitIdle(device);   // never destroy things the GPU is still using
>> look up: vkDeviceWaitIdle

// Destroy in reverse of creation:
// pipeline -> pipeline layout -> descriptor pool -> descriptor set layout
// -> buffers/images + free their memory -> sync objects -> command pool
// -> image views -> swapchain -> device -> (debug messenger) -> surface -> instance
>> look up: vkDestroy* (each object has a matching destroy call)
```

---

## 12. Suggested build order (don't do it all at once)

1. Instance (+ validation layers) — verify no validation errors at startup.
2. Surface + physical device + logical device + queues.
3. Swapchain + image views.
4. Command pool/buffers + sync objects.
5. Clear-only frame loop (sections 6–8 without a pipeline) — you should get a solid clear color.
6. Pipeline + hardcoded triangle (section 9) — first triangle.
7. Swapchain recreation on resize.
8. Vertex/index buffers, then uniforms/textures via descriptors (section 10).

Keep validation layers ON the entire time. Treat every validation message as a build error.

---

## 13. Tooling / references

- **Vulkan spec (1.3):** https://registry.khronos.org/vulkan/specs/1.3-extensions/html/
- **Man pages:** https://registry.khronos.org/vulkan/specs/1.3/man/html/
- **vulkan-tutorial (dynamic rendering branch mentally map render-pass steps to sections 5/8/9).**
- **volk:** https://github.com/zeux/volk
- **SDL3 Vulkan:** `SDL_Vulkan_GetInstanceExtensions`, `SDL_Vulkan_CreateSurface`
- **VMA (later):** https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
- **Shader compile:** `glslc tri.vert -o tri.vert.spv` (from the Vulkan SDK)
```
```

> `verify(...)` above stands for your existing macro that checks a `VkResult == VK_SUCCESS`
> and logs/aborts on failure — you already reference it in `render.c`.
