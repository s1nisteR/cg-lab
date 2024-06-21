#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>


#include <iostream>


int main()
{
    //instance creation
    vkb::InstanceBuilder instanceBuilder;
    auto instanceBuilderReturn = instanceBuilder
    .request_validation_layers()
    .use_default_debug_messenger()
    .build();

    if(!instanceBuilderReturn) { std::cout << "Failed to build an instance" << std::endl; return -1; }

    vkb::Instance instance = instanceBuilderReturn.value();


    //glfw window creation
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);   //no OpenGL pls
    GLFWwindow* window;
    if(!glfwInit()) { std::cout << "Failed to initialize GLFW" << std::endl; return -1; }
    window = glfwCreateWindow(640, 480, "Vulk", NULL, NULL);
    if(!window) { std::cout << "Failed to create the GLFW Window" << std::endl; return -1; }
    

    //surface creation
    VkSurfaceKHR surface = nullptr;
    VkResult err = glfwCreateWindowSurface(instance, window, NULL, &surface);
    if(err != VK_SUCCESS) { std::cout << "Failed to create the surface" << std::endl; return -1; }


    //physical device selection
    vkb::PhysicalDeviceSelector phyDevSelector(instance);
    auto physicalDeviceSelectorReturn = phyDevSelector.set_surface(surface).select();
    if(!physicalDeviceSelectorReturn) { std::cout << "Failed to select device" << std::endl; return -1;}
    auto phyDev = physicalDeviceSelectorReturn.value();


    //physical device creation
    vkb::DeviceBuilder deviceBuilder(phyDev);
    auto devReturn = deviceBuilder.build();
    if(!devReturn) { std::cout << "Failed to create a physical device" << std::endl; return -1; }
    vkb::Device device = devReturn.value();

    


    //cleanup routine
    vkb::destroy_device(device);
    vkb::destroy_instance(instance);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}