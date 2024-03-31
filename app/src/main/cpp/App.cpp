#include "App.h"

#include <android/native_window_jni.h>

namespace android {

AAssetManager* g_AssetManager = nullptr;
static App* g_App = nullptr;

App::App(JNIEnv* env, jobject activity, jobject assetManager) noexcept
{
  g_AssetManager = AAssetManager_fromJava(env, assetManager);
  vk_utils::setAssetManager(g_AssetManager);

  std::vector<const char*> requiredExtensions {
    VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
    VK_KHR_SURFACE_EXTENSION_NAME
  };

  m_renderer.InitVulkan(requiredExtensions, 0);
}

bool App::doSurfaceCreated(JNIEnv* env, jobject surface) noexcept {
  m_window = ANativeWindow_fromSurface(env, surface);
  VkAndroidSurfaceCreateInfoKHR androidSurfaceCreateInfoKHR;
  androidSurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
  androidSurfaceCreateInfoKHR.pNext = nullptr;
  androidSurfaceCreateInfoKHR.flags = 0U;
  androidSurfaceCreateInfoKHR.window = m_window;
  vkCreateAndroidSurfaceKHR(m_renderer.GetVkInstance(), &androidSurfaceCreateInfoKHR, nullptr, &m_surface);
  m_renderer.InitPresentation(m_surface);

  m_thread = std::thread (
    [ this ] () noexcept {
        this->run();
    }
  );

  return true;
}

void App::run() {
  while (true) {
    m_renderer.DrawFrame();
  }
}

extern "C" {

JNIEXPORT void Java_com_msu_course_MainActivity_doCreate(JNIEnv* env, jobject obj,
  jobject assetManager) {
  g_App = new App(env, obj, assetManager);
  VK_UTILS_LOG_INFO("App create");
}

JNIEXPORT void Java_com_msu_course_MainActivity_doSurfaceCreated(JNIEnv* env, jobject /*obj*/, jobject surface) {
  if (g_App == nullptr)
    return;

  g_App->doSurfaceCreated(env, surface);
}

} // extern "C"

} // namespace android