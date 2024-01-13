// Hello World with EGL and OpenGL ES 3.2

#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "android_native_app_glue.h"
#include <cstring>
#include <fstream>

std::ofstream flog{"log.txt"};

struct Window
{
	EGLDisplay display;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;
	int width;
	int height;
	bool is_open;
};

void activity_proc(android_app* app, int cmd);

void init_window(Window* window, ANativeWindow* nw);
void destroy_window(Window* window);

void android_main(android_app* state)
{
	Window* window = new Window;
	memset(window, 0, sizeof(Window));
	
	state->userData = (void*) window;
	state->onAppCmd = activity_proc;
	
	while (true)
	{
		android_poll_source* source;
		while (ALooper_pollAll(0, nullptr, nullptr, (void**) &source) >= 0)
		{
			if (source)
			{
				source->process(state, source);
			}
			
			if (state->destroyRequested != 0)
			{
				flog.close();
				delete window;
				return;
			}
		}
		
		if (window->is_open)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			eglSwapBuffers(window->display, window->surface);
		}
	}
}

void activity_proc(android_app* app, int cmd)
{
	Window* window = (Window*) app->userData;
	switch (cmd)
	{
		case APP_CMD_INIT_WINDOW:
			init_window(window, app->window);
			break;
			
		case APP_CMD_TERM_WINDOW:
			destroy_window(window);
			break;
	}
}

void init_window(Window* window, ANativeWindow* nw)
{
	const int window_attrib[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 8,
		EGL_NONE
	};
	
	const int render_buffer[] =
	{
		EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
		EGL_NONE
	};
	
	const int context_version[] =
	{
		EGL_CONTEXT_MAJOR_VERSION_KHR, 3,
		EGL_CONTEXT_MINOR_VERSION_KHR, 2,
		EGL_NONE
	};
	
	window->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(window->display, nullptr, nullptr);
	
	int configs_count;
	eglChooseConfig(window->display, window_attrib, &window->config, 1, &configs_count);
	window->surface = eglCreateWindowSurface(window->display, window->config, nw, render_buffer);
	
	window->context = eglCreateContext(window->display, window->config, EGL_NO_CONTEXT, context_version);
	eglMakeCurrent(window->display, window->surface, window->surface, window->context);
	
	window->is_open = true;
	
	window->width = ANativeWindow_getWidth(nw);
	window->height = ANativeWindow_getHeight(nw);
	
	glViewport(0, 0, window->width, window->height);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	
	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	
	flog << "Version: " << major << "." << minor << "\n";
}

void destroy_window(Window* window)
{
	if (window->display == nullptr)
	{
		return;
	}
	
	eglMakeCurrent(window->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	if (window->context != nullptr)
	{
		eglDestroyContext(window->display, window->context);
	}
	if (window->surface != nullptr)
	{
		eglDestroySurface(window->display, window->surface);
	}
	
	eglTerminate(window->display);
	
	memset(window, 0, sizeof(Window));
}
