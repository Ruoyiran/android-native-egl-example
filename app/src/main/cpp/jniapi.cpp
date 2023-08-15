#include <stdint.h>
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer

#include "logger.h"
#include "renderer.h"

#define LOG_TAG "EglSample"

static ANativeWindow *window = nullptr;
static Renderer *renderer = nullptr;

static void egl_nativeOnStart(JNIEnv *, jclass) {
  LOG_INFO("nativeOnStart");
  renderer = new Renderer();
}

static void egl_nativeOnResume(JNIEnv *, jclass) {
  LOG_INFO("nativeOnResume");
  if (renderer) {
    renderer->start();
  }
}

static void egl_nativeOnPause(JNIEnv *, jclass) {
  LOG_INFO("nativeOnPause");
  if (renderer) {
    renderer->stop();
  }
}

static void egl_nativeOnStop(JNIEnv *, jclass) {
  LOG_INFO("nativeOnStop");
  delete renderer;
  renderer = nullptr;
}

static void egl_nativeSetSurface(JNIEnv *jenv, jclass, jobject surface) {
  if (surface) {
    window = ANativeWindow_fromSurface(jenv, surface);
    LOG_INFO("Got window %p", window);
    if (renderer) {
      renderer->setWindow(window);
    }
  } else {
    LOG_INFO("Releasing window");
    ANativeWindow_release(window);
  }
}

extern "C" jint JNI_OnLoad(JavaVM *vm, void *) {
  JNIEnv *env;
  vm->GetEnv((void **) &env, JNI_VERSION_1_6);

  jclass clazz = env->FindClass("com/royran/egl/example/MainActivity");

  static const JNINativeMethod NAPI_METHODS[] = {{"nativeOnStart",    "()V",                       (void *) (egl_nativeOnStart)},
                                                 {"nativeOnResume",   "()V",                       (void *) (egl_nativeOnResume)},
                                                 {"nativeOnPause",    "()V",                       (void *) (egl_nativeOnPause)},
                                                 {"nativeOnStop",     "()V",                       (void *) (egl_nativeOnStop)},
                                                 {"nativeSetSurface", "(Landroid/view/Surface;)V", (void *) (egl_nativeSetSurface)},
  };

  env->RegisterNatives(clazz, NAPI_METHODS, sizeof(NAPI_METHODS) / sizeof(NAPI_METHODS[0]));

  env->DeleteLocalRef(clazz);
  return JNI_VERSION_1_6;
}
