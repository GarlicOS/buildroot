#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

void checkEGLError(const char* msg) {
	EGLint error = eglGetError();
	if (error != EGL_SUCCESS) {
		fprintf(stderr, "EGL Error (%s): %d\n", msg, error);
		exit(EXIT_FAILURE);
	}
}

const char* vertexShaderSource =
	"#version 100\n"
	"attribute vec4 vPosition;\n"
	"void main() {\n"
	"	gl_Position = vPosition;\n"
	"}\n";

const char* fragmentShaderSource =
	"#version 100\n"
	"precision mediump float;\n"
	"void main() {\n"
	"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n";

GLuint loadShader(GLenum type, const char* shaderSrc) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderSrc, NULL);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char* infoLog = malloc(infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			fprintf(stderr, "Error compiling shader:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource) {
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char* infoLog = malloc(infoLen);
			glGetProgramInfoLog(program, infoLen, NULL, infoLog);
			fprintf(stderr, "Error linking program:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

int main(int argc, char * argv[]) {
	// Initialize EGL
	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	checkEGLError("eglGetDisplay");

	EGLBoolean initialized = eglInitialize(display, NULL, NULL);
	if (initialized == EGL_FALSE) {
		checkEGLError("eglInitialize");
		fprintf(stderr, "Failed to initialize EGL\n");
		exit(EXIT_FAILURE);
	}

	// Choose config
	EGLint configAttribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_NONE
	};
	EGLConfig config;
	EGLint numConfigs;
	eglChooseConfig(display, configAttribs, &config, 1, &numConfigs);
	checkEGLError("eglChooseConfig");

	// Create window surface
	EGLSurface surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)0, NULL);
	checkEGLError("eglCreateWindowSurface");

	// Create OpenGL ES context
	EGLint contextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
	checkEGLError("eglCreateContext");

	eglMakeCurrent(display, surface, surface, context);
	checkEGLError("eglMakeCurrent");

	// Print OpenGL ES version
	const char* versionStr = eglQueryString(display, EGL_VERSION);
	if (versionStr == NULL) {
		fprintf(stderr, "Failed to retrieve OpenGL ES version\n");
		exit(EXIT_FAILURE);
	}
	printf("OpenGL ES version: %s\n", versionStr);

	// Set up vertex data
	GLfloat vertices[] = {
		0.0f,  0.5f,
	   -0.5f, -0.5f,
		0.5f, -0.5f,
	};

	GLuint program = createProgram(vertexShaderSource, fragmentShaderSource);
	glUseProgram(program);
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);

	// Render loop
	while (1) {
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		eglSwapBuffers(display, surface);
	}

	// Clean up
	eglDestroyContext(display, context);
	eglDestroySurface(display, surface);
	eglTerminate(display);

	return 0;
}