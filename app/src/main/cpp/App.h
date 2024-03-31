#ifndef APP_H
#define APP_H

#include <thread>
#include <android/asset_manager_jni.h>

#include "SimpleRender.h"
#include "vk_utils.h"

namespace android {

class App {
public:
  App(JNIEnv* env, jobject activity, jobject assetManager) noexcept;
  App(const App &another) = delete;
  App &operator=(const App &another) = delete;
  ~App() = default;

  bool doSurfaceCreated(JNIEnv* env, jobject surface) noexcept;

private:
  SimpleRender m_renderer {};
  ANativeWindow* m_window;
  VkSurfaceKHR m_surface;
  std::thread m_thread {};

  void run();
};

} // namespace android

#endif// APP_H