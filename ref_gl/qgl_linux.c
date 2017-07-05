/*
** QGL_WIN.C
**
** This file implements the operating system binding of GL to QGL function
** pointers.  When doing a port of Quake2 you must implement the following
** two functions:
**
** QGL_Init() - loads libraries, assigns function pointers, etc.
** QGL_Shutdown() - unloads libraries, NULLs function pointers
*/
#define QGL
#include "../ref_gl/gl_local.h"

#include <float.h>
#include <stdbool.h>
#include <assert.h>


const char *vertexShaderSrc =
"attribute vec3 pos;\n"
"attribute vec4 col;\n"
"attribute vec2 texcoord0;\n"
"attribute vec2 texcoord1;\n"

"varying vec4 vColor;\n"
"varying vec2 vTex0;\n"
"varying vec2 vTex1;\n"

"uniform mat4 mvp;\n"

"void main(void) {\n"
"	vColor = col;\n"
"	vTex0 = texcoord0;\n"
"	vTex1 = texcoord1;\n"

"	gl_Position = mvp * vec4(pos, 1.0);\n"
"}\n"
;


const char *fragmentShaderSrc =
"uniform sampler2D tex0;\n"
"uniform sampler2D tex1;\n"

#ifndef __APPLE__
"precision highp float;\n"
#endif

"#ifdef ALPHA\n"
"uniform float alphaRef;\n"
"#endif  // ALPHA\n"

"varying vec4 vColor;\n"
"varying vec2 vTex0;\n"
"varying vec2 vTex1;\n"

"void main(void) {\n"
"	vec4 temp = vColor;\n"

"#ifdef TEX0\n"
"	vec4 t0col = texture2D(tex0, vTex0);\n"
"#ifdef TEX0_MODULATE\n"
"	temp = temp * t0col;\n"
"#elif TEX0_REPLACE\n"
"	temp = t0col;\n"
"#else\n"
"#error Unknown tex0 mode\n"
"#endif  // TEX0 mode\n"
"#endif  // TEX0\n"

"#ifdef TEX1\n"
"	vec4 t1col = texture2D(tex1, vTex1);\n"
"#ifdef TEX1_MODULATE\n"
"	temp = temp * t1col;\n"
"#elif TEX1_REPLACE\n"
"	temp = t1col;\n"
"#else\n"
"#error Unknown tex1 mode\n"
"#endif  // TEX1 mode\n"
"#endif  // TEX1\n"

"#ifdef ALPHA\n"
	// alpha function shows the fragment if test passes and discard if fails
	// so for GREATER we check <=
"	if (temp.a <= alphaRef) {\n"
"		discard;\n"
"	}\n"
"#endif  // ALPHA\n"

"	gl_FragColor = temp;\n"
"}\n"
;


QGLState *qglState = NULL;


void qglGetFloatv(GLenum pname, GLfloat *params);
GLboolean ( APIENTRY * qglIsEnabled )(GLenum cap);
GLboolean ( APIENTRY * qglIsTexture )(GLuint texture);


/*
** QGL_Shutdown
**
** Unloads the specified DLL then nulls out all the proc pointers.
*/
void QGL_Shutdown( void )
{
	if (qglState->vbos[0] != 0) {
		glDeleteBuffers(NUMVBOS, qglState->vbos);
		memset(qglState->vbos, 0, sizeof(GLuint) * NUMVBOS);
	}

	// this pointer is not unique
	qglState->activeShader = NULL;

	// these are
	while (qglState->shaders != NULL) {
		glDeleteProgram(qglState->shaders->program);
		Shader *old = qglState->shaders;
		qglState->shaders = old->next;
		free(old);
	}

	free(qglState); qglState = NULL;
}


// add a new vertex to vertices array
// resize if necessary
static void pushVertex(const Vertex *v) {
	if (qglState->numVertices == qglState->usedVertices) {
		// resize needed
		size_t oldVerticesSize = qglState->numVertices * sizeof(Vertex);

		qglState->numVertices *= 2;
		Vertex *newVertices = (Vertex *) malloc(qglState->numVertices * sizeof(Vertex));
		memset(newVertices, 0, qglState->numVertices * sizeof(Vertex));
		memcpy(newVertices, qglState->vertices, oldVerticesSize);

		free(qglState->vertices);
		qglState->vertices = newVertices;
	}

	memcpy(&qglState->vertices[qglState->usedVertices++], v, sizeof(Vertex));
}


void qglColor3f(GLfloat red, GLfloat green, GLfloat blue) {
	qglColor4f(red, green, blue, 1.0f);
}


#define CLAMP(x) if ((x) > 1.0f) { (x) = 1.0f; } else if ((x) < 0.0f) { (x) = 0.0f; }


void qglColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
	CLAMP(red)
	CLAMP(green)
	CLAMP(blue)
	CLAMP(alpha)

	uint32_t r = red * 255, g = green * 255, b = blue * 255, a = alpha * 255;

	// TODO: big-endian, if anyone cares
	uint32_t c =
		  (r <<  0)
		| (g <<  8)
		| (b << 16)
		| (a << 24);
	qglState->currentVertex.color = c;
}


#undef CLAMP


void qglVertex2f(GLfloat x, GLfloat y) {
	qglVertex3f(x, y, 0.0f);
}


void qglVertex3f(GLfloat x, GLfloat y, GLfloat z) {
	qglState->currentVertex.pos[0] = x;
	qglState->currentVertex.pos[1] = y;
	qglState->currentVertex.pos[2] = z;

	pushVertex(&qglState->currentVertex);
}


void qglMTexCoord2f(GLenum tex, GLfloat s, GLfloat t) {
	if (tex == GL_TEXTURE0) {
		qglState->currentVertex.tex0[0] = s;
		qglState->currentVertex.tex0[1] = t;
	} else {
		qglState->currentVertex.tex1[0] = s;
		qglState->currentVertex.tex1[1] = t;
	}
}


static void pushDraw(GLenum primitive, unsigned int firstVert, unsigned int numVertices) {
	if (qglState->numDrawCalls > 0 && primitive == GL_TRIANGLES) {
		unsigned int prevCall = qglState->numDrawCalls - 1;
		if (qglState->drawCalls[prevCall].primitive == GL_TRIANGLES
			&& (qglState->drawCalls[prevCall].firstVert
				+ qglState->drawCalls[prevCall].numVertices) == firstVert) {
			// append to the previous draw call
			qglState->drawCalls[prevCall].numVertices += numVertices;
			return;
		}
	}

	if (qglState->numDrawCalls == qglState->maxDrawCalls) {
		// add more space
		qglState->maxDrawCalls *= 2;
		qglState->drawCalls = (DrawCall *) realloc(qglState->drawCalls, qglState->maxDrawCalls * sizeof(DrawCall));
	}

	qglState->drawCalls[qglState->numDrawCalls].primitive = primitive;
	qglState->drawCalls[qglState->numDrawCalls].firstVert = firstVert;
	qglState->drawCalls[qglState->numDrawCalls].numVertices = numVertices;

	qglState->numDrawCalls++;
}


static void commitShaderState();


static void bindVBO() {
	if (qglState->vbos[qglState->currentVBOidx] == 0) {
		// can't be called in QGL_Init, GL context doesn't exist there
		glGenBuffers(NUMVBOS, qglState->vbos);
	}

	qglState->currentVBOidx = (qglState->currentVBOidx + 1) % NUMVBOS;

	glBindBuffer(GL_ARRAY_BUFFER, qglState->vbos[qglState->currentVBOidx]);
}


static void commitPipelineState() {
	commitShaderState();

	if (qglState->activePipeline.blend != qglState->wantPipeline.blend) {
		qglState->activePipeline.blend = qglState->wantPipeline.blend;
		if (qglState->activePipeline.blend) {
			glEnable(GL_BLEND);
		} else {
			glDisable(GL_BLEND);
		}
	}

	qglState->pipelineDirty = false;
}


void flushDraws(const char *reason) {
	if (qglState->numDrawCalls == 0) {
		// nothing to do
		return;
	}

#ifndef NDEBUG
	if (GLEW_GREMEDY_string_marker) {
		char temp[512];
		Com_sprintf(temp, 5126, "flushDraws due to %s", reason);
		glStringMarkerGREMEDY(0, temp);
	}

#endif  // NDEBUG

	commitPipelineState();

	bindVBO();

	glBufferData(GL_ARRAY_BUFFER, qglState->usedVertices * sizeof(Vertex), &qglState->vertices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, pos));
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, color));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, tex0));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, tex1));

	for (unsigned int i = 0; i < qglState->numDrawCalls; i++) {
		DrawCall *d = qglState->drawCalls + i;
		glDrawArrays(d->primitive, d->firstVert, d->numVertices);
	}

	qglState->numDrawCalls = 0;
	qglState->usedVertices = 0;
}


void qglBegin(GLenum mode) {
	if (qglState->pipelineDirty) {
		flushDraws("qglBegin");
	}

	qglState->currentDrawFirstVertex = qglState->usedVertices;
	qglState->primitive = mode;
}


static Shader *createShader(const ShaderState *state) {
#define BUFSIZE 512
#define EMITDEF(x) defSize += snprintf(defineBuf + defSize, BUFSIZE - defSize, "#define %s 1\n", x)
	char defineBuf[BUFSIZE];
	unsigned int defSize = 0;
	memset(defineBuf, '\0', BUFSIZE);

	if (state->alphaTest) {
		EMITDEF("ALPHA");

		// the only one currently used
		assert(state->alphaFunc == GL_GREATER);
	}

	if (state->texState[0].texEnable) {
		EMITDEF("TEX0");

		switch(state->texState[0].texMode) {
		case GL_MODULATE:
			EMITDEF("TEX0_MODULATE");
			break;

		case GL_REPLACE:
			EMITDEF("TEX0_REPLACE");
			break;

		default:
			assert(false);
			break;
		}
	}

	if (state->texState[1].texEnable) {
		EMITDEF("TEX1");

		switch(state->texState[1].texMode) {
		case GL_MODULATE:
			EMITDEF("TEX1_MODULATE");
			break;

		case GL_REPLACE:
			EMITDEF("TEX1_REPLACE");
			break;

		default:
			assert(false);
			break;
		}
	}

	const char *srcArray[3];
#ifdef __APPLE__
	srcArray[0] = "#version 120\n";
#else
	srcArray[0] = "#version 100\n";
#endif
	srcArray[1] = (const char *) &defineBuf;
	GLuint program = glCreateProgram();
	GLint temp = 0;

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	srcArray[2] = vertexShaderSrc;
	glShaderSource(vertexShader, 3, srcArray, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &temp);
	if (temp != GL_TRUE) {
		printf("vertex shader compile failed:\n");
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &temp);
		char *buf = (char *) malloc(temp + 1);
		memset(buf, '\0', temp + 1);
		glGetShaderInfoLog(vertexShader, temp, NULL, buf);

		printf("%s\n", buf);
		free(buf);
		abort();
	}
	glAttachShader(program, vertexShader);
	glDeleteShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	srcArray[2] = fragmentShaderSrc;
	glShaderSource(fragmentShader, 3, srcArray, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &temp);
	if (temp != GL_TRUE) {
		printf("fragment shader compile failed:\n");
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &temp);
		char *buf = (char *) malloc(temp + 1);
		memset(buf, '\0', temp + 1);
		glGetShaderInfoLog(fragmentShader, temp, NULL, buf);

		printf("%s\n", buf);
		free(buf);
		abort();
	}
	glAttachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);

	glBindAttribLocation(program, 0, "pos");
	glBindAttribLocation(program, 1, "col");
	glBindAttribLocation(program, 2, "texcoord0");
	glBindAttribLocation(program, 3, "texcoord1");

	glLinkProgram(program);
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "tex0"), 0);
	glUniform1i(glGetUniformLocation(program, "tex1"), 1);

	Shader *shader = (Shader *) malloc(sizeof(Shader));
	shader->program = program;
	memcpy(&shader->key, state, sizeof(ShaderState));
	shader->next = NULL;


	shader->mvpUniform = glGetUniformLocation(program, "mvp");
	if (state->alphaTest) {
		shader->alphaRefUniform = glGetUniformLocation(program, "alphaRef");
	} else {
		shader->alphaRefUniform = -1;
	}

	return shader;
}


static Shader *findShader(const ShaderState *state) {
	Shader *shader = qglState->shaders;
	while (shader != NULL) {
		if (memcmp(&shader->key, state, sizeof(ShaderState)) == 0) {
			return shader;
		}

		shader = shader->next;
	}

	// not found, create
	shader = createShader(state);
	shader->next = qglState->shaders;
	qglState->shaders = shader;

	return shader;

}


static void identityMatrix(float *matrix) {
	memset(matrix, 0, sizeof(float) * 16);
	matrix[0 * 4 + 0] = 1.0f;
	matrix[1 * 4 + 1] = 1.0f;
	matrix[2 * 4 + 2] = 1.0f;
	matrix[3 * 4 + 3] = 1.0f;
}


static void multMatrices(float *target, const float *left, const float *right) {
	// target and left/right must not alias
	// should probably put some __restrict__ on this
	assert(target != left);
	assert(target != right);

	target[0 * 4 + 0] = right[0 * 4 + 0] * left[0 * 4 + 0] + right[0 * 4 + 1] * left[1 * 4 + 0] + right[0 * 4 + 2] * left[2 * 4 + 0] + right[0 * 4 + 3] * left[3 * 4 + 0];
	target[0 * 4 + 1] = right[0 * 4 + 0] * left[0 * 4 + 1] + right[0 * 4 + 1] * left[1 * 4 + 1] + right[0 * 4 + 2] * left[2 * 4 + 1] + right[0 * 4 + 3] * left[3 * 4 + 1];
	target[0 * 4 + 2] = right[0 * 4 + 0] * left[0 * 4 + 2] + right[0 * 4 + 1] * left[1 * 4 + 2] + right[0 * 4 + 2] * left[2 * 4 + 2] + right[0 * 4 + 3] * left[3 * 4 + 2];
	target[0 * 4 + 3] = right[0 * 4 + 0] * left[0 * 4 + 3] + right[0 * 4 + 1] * left[1 * 4 + 3] + right[0 * 4 + 2] * left[2 * 4 + 3] + right[0 * 4 + 3] * left[3 * 4 + 3];

	target[1 * 4 + 0] = right[1 * 4 + 0] * left[0 * 4 + 0] + right[1 * 4 + 1] * left[1 * 4 + 0] + right[1 * 4 + 2] * left[2 * 4 + 0] + right[1 * 4 + 3] * left[3 * 4 + 0];
	target[1 * 4 + 1] = right[1 * 4 + 0] * left[0 * 4 + 1] + right[1 * 4 + 1] * left[1 * 4 + 1] + right[1 * 4 + 2] * left[2 * 4 + 1] + right[1 * 4 + 3] * left[3 * 4 + 1];
	target[1 * 4 + 2] = right[1 * 4 + 0] * left[0 * 4 + 2] + right[1 * 4 + 1] * left[1 * 4 + 2] + right[1 * 4 + 2] * left[2 * 4 + 2] + right[1 * 4 + 3] * left[3 * 4 + 2];
	target[1 * 4 + 3] = right[1 * 4 + 0] * left[0 * 4 + 3] + right[1 * 4 + 1] * left[1 * 4 + 3] + right[1 * 4 + 2] * left[2 * 4 + 3] + right[1 * 4 + 3] * left[3 * 4 + 3];
	
	target[2 * 4 + 0] = right[2 * 4 + 0] * left[0 * 4 + 0] + right[2 * 4 + 1] * left[1 * 4 + 0] + right[2 * 4 + 2] * left[2 * 4 + 0] + right[2 * 4 + 3] * left[3 * 4 + 0];
	target[2 * 4 + 1] = right[2 * 4 + 0] * left[0 * 4 + 1] + right[2 * 4 + 1] * left[1 * 4 + 1] + right[2 * 4 + 2] * left[2 * 4 + 1] + right[2 * 4 + 3] * left[3 * 4 + 1];
	target[2 * 4 + 2] = right[2 * 4 + 0] * left[0 * 4 + 2] + right[2 * 4 + 1] * left[1 * 4 + 2] + right[2 * 4 + 2] * left[2 * 4 + 2] + right[2 * 4 + 3] * left[3 * 4 + 2];
	target[2 * 4 + 3] = right[2 * 4 + 0] * left[0 * 4 + 3] + right[2 * 4 + 1] * left[1 * 4 + 3] + right[2 * 4 + 2] * left[2 * 4 + 3] + right[2 * 4 + 3] * left[3 * 4 + 3];

	target[3 * 4 + 0] = right[3 * 4 + 0] * left[0 * 4 + 0] + right[3 * 4 + 1] * left[1 * 4 + 0] + right[3 * 4 + 2] * left[2 * 4 + 0] + right[3 * 4 + 3] * left[3 * 4 + 0];
	target[3 * 4 + 1] = right[3 * 4 + 0] * left[0 * 4 + 1] + right[3 * 4 + 1] * left[1 * 4 + 1] + right[3 * 4 + 2] * left[2 * 4 + 1] + right[3 * 4 + 3] * left[3 * 4 + 1];
	target[3 * 4 + 2] = right[3 * 4 + 0] * left[0 * 4 + 2] + right[3 * 4 + 1] * left[1 * 4 + 2] + right[3 * 4 + 2] * left[2 * 4 + 2] + right[3 * 4 + 3] * left[3 * 4 + 2];
	target[3 * 4 + 3] = right[3 * 4 + 0] * left[0 * 4 + 3] + right[3 * 4 + 1] * left[1 * 4 + 3] + right[3 * 4 + 2] * left[2 * 4 + 3] + right[3 * 4 + 3] * left[3 * 4 + 3];
}


static void commitShaderState() {
	bool shaderChanged = false;
	if (qglState->shaderDirty) {
		Shader *newShader = findShader(&qglState->wantShader);
		if (qglState->activeShader != newShader) {
			qglState->activeShader = newShader;
			glUseProgram(qglState->activeShader->program);
			shaderChanged = true;
		}
	}

	if (shaderChanged || qglState->mvMatrixDirty || qglState->projMatrixDirty) {
		float mvp[16];

		float depthAdjust[16];
		identityMatrix(depthAdjust);
		depthAdjust[2 * 4 + 2] = qglState->zFar - qglState->zNear;
		depthAdjust[3 * 4 + 2] = qglState->zNear;

		float temp[16];
		multMatrices(temp, depthAdjust, qglState->projMatrices[qglState->projMatrixTop]);
		multMatrices(mvp, temp, qglState->mvMatrices[qglState->mvMatrixTop]);

		glUniformMatrix4fv(qglState->activeShader->mvpUniform, 1, GL_FALSE, &mvp[0]);
		qglState->mvMatrixDirty = false;
		qglState->projMatrixDirty = false;
	}

	if (qglState->wantShader.alphaTest) {
		assert(qglState->activeShader->alphaRefUniform != -1);
		glUniform1f(qglState->activeShader->alphaRefUniform, qglState->wantShader.alphaRef);
	}
}


void qglEnd(void) {
	pushDraw(qglState->primitive, qglState->currentDrawFirstVertex, qglState->usedVertices - qglState->currentDrawFirstVertex);

	qglState->primitive = GL_NONE;
}


void qglMatrixMode(GLenum mode) {
	qglState->matrixMode = mode;
}


void qglLoadIdentity(void) {
	float idM[16];
	identityMatrix(idM);

	qglLoadMatrixf(idM);
}


void qglFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {
	float mat[16];
	memset(mat, 0, sizeof(float) * 16);

	mat[0 * 4 + 0] = 2 * zNear / (right - left);

	mat[1 * 4 + 1] = 2 * zNear / (top - bottom);

	mat[2 * 4 + 0] = (right + left) / (right - left);
	mat[2 * 4 + 1] = (top + bottom) / (top - bottom);
	mat[2 * 4 + 2] = -(zFar + zNear) / (zFar - zNear);
	mat[2 * 4 + 3] = -1.0f;

	mat[3 * 4 + 2] = -(2 * zFar * zNear) / (zFar - zNear);

	// should really be MultMatrix but since we always load identity before...
	qglLoadMatrixf(mat);
}


void qglOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {
	float mat[16];
	memset(mat, 0, sizeof(float) * 16);

	mat[0 * 4 + 0] = 2  / (right - left);

	mat[1 * 4 + 1] = 2  / (top - bottom);

	mat[2 * 4 + 2] = -2 / (zFar - zNear);

	mat[3 * 4 + 0] = -(right + left) / (right - left);
	mat[3 * 4 + 1] = -(top + bottom) / (top - bottom);
	mat[3 * 4 + 2] = -(zFar + zNear) / (zFar - zNear);
	mat[3 * 4 + 3] = 1.0f;

	// should really be MultMatrix but since we always load identity before...
	qglLoadMatrixf(mat);
}


void qglLoadMatrixf(const GLfloat *m) {
	flushDraws("qglLoadMatrixf");

	float *targetMat = NULL;
	if (qglState->matrixMode == GL_MODELVIEW) {
		targetMat = qglState->mvMatrices[qglState->mvMatrixTop];
		qglState->mvMatrixDirty = true;
	} else if (qglState->matrixMode == GL_PROJECTION) {
		targetMat = qglState->projMatrices[qglState->projMatrixTop];
		qglState->projMatrixDirty = true;
	} else {
		__builtin_unreachable();
	}

	memcpy(targetMat, m, sizeof(float) * 16);
}


void qglMultMatrixf(const GLfloat *m) {
	flushDraws("qglMultMatrixf");

	float *targetMat = NULL;
	if (qglState->matrixMode == GL_MODELVIEW) {
		targetMat = qglState->mvMatrices[qglState->mvMatrixTop];
		qglState->mvMatrixDirty = true;
	} else if (qglState->matrixMode == GL_PROJECTION) {
		targetMat = qglState->projMatrices[qglState->projMatrixTop];
		qglState->projMatrixDirty = true;
	} else {
		assert(false);
	}

	float mat[16];
	multMatrices(mat, targetMat, m);
	memcpy(targetMat, mat, sizeof(float) * 16);
}


void qglRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	float mat[16];
	memset(mat, 0, sizeof(float) * 16);

	// someone calls this with zero vector
	// add FLT_MIN to prevent division by zero
	float norm = 1.0f / (sqrtf(x*x + y*y + z*z) + FLT_MIN);
	x *= norm;
	y *= norm;
	z *= norm;

	float c = cosf(angle * M_PI / 180.0f);
	float s = sinf(angle * M_PI / 180.0f);

	mat[0 * 4 + 0] = x * x * (1 - c) + c;
	mat[0 * 4 + 1] = y * x * (1 - c) + z * s;
	mat[0 * 4 + 2] = x * z * (1 - c) - y * s;

	mat[1 * 4 + 0] = x * y * (1 - c) - z * s;
	mat[1 * 4 + 1] = y * y * (1 - c) + c;
	mat[1 * 4 + 2] = y * z * (1 - c) + x * s;

	mat[2 * 4 + 0] = x * z * (1 - c) + y * s;
	mat[2 * 4 + 1] = y * z * (1 - c) - x * s;
	mat[2 * 4 + 2] = z * z * (1 - c) + c;

	mat[3 * 4 + 3] = 1.0f;

	qglMultMatrixf(mat);
}


void qglScalef(GLfloat x, GLfloat y, GLfloat z) {
	float mat[16];
	memset(mat, 0, sizeof(float) * 16);

	mat[0 * 4 + 0] = x;

	mat[1 * 4 + 1] = y;

	mat[2 * 4 + 2] = z;

	mat[3 * 4 + 3] = 1.0f;

	qglMultMatrixf(mat);
}


void qglTranslatef(GLfloat x, GLfloat y, GLfloat z) {
	float mat[16];
	identityMatrix(mat);

	mat[3 * 4 + 0] = x;
	mat[3 * 4 + 1] = y;
	mat[3 * 4 + 2] = z;

	qglMultMatrixf(mat);
}


void qglPopMatrix(void) {
	flushDraws("qglPopMatrix");

	if (qglState->matrixMode == GL_MODELVIEW) {
		qglState->mvMatrixTop--;

		assert(qglState->mvMatrixTop >= 0 && qglState->mvMatrixTop <= NUMMATRICES);
		qglState->mvMatrixDirty = true;
	} else if (qglState->matrixMode == GL_PROJECTION) {
		qglState->projMatrixTop--;

		assert(qglState->projMatrixTop >= 0 && qglState->projMatrixTop <= NUMMATRICES);
		qglState->projMatrixDirty = true;
	} else {
		assert(false);
	}
}


void qglPushMatrix(void) {
	if (qglState->matrixMode == GL_MODELVIEW) {
		qglState->mvMatrixTop++;

		assert(qglState->mvMatrixTop >= 0 && qglState->mvMatrixTop <= NUMMATRICES);

		memcpy(qglState->mvMatrices[qglState->mvMatrixTop], qglState->mvMatrices[qglState->mvMatrixTop - 1], sizeof(float) * 16);
	} else if (qglState->matrixMode == GL_PROJECTION) {
		qglState->projMatrixTop++;

		assert(qglState->projMatrixTop >= 0 && qglState->projMatrixTop <= NUMMATRICES);

		memcpy(qglState->projMatrices[qglState->projMatrixTop], qglState->projMatrices[qglState->projMatrixTop - 1], sizeof(float) * 16);
	} else {
		assert(false);
	}
}


void qglGetFloatv(GLenum pname, GLfloat *params) {
	assert(pname == GL_MODELVIEW_MATRIX);

	memcpy(params, qglState->mvMatrices[qglState->mvMatrixTop], sizeof(float) * 16);
}


void qglActiveTexture(GLenum tex) {
	qglState->wantActiveTexture = tex - GL_TEXTURE0;
}


void qglAlphaFunc(GLenum func, GLclampf ref) {
	if (qglState->wantShader.alphaFunc != func) {
		flushDraws("qglAlphaFunc");

		qglState->wantShader.alphaFunc = func;
		qglState->shaderDirty = true;
	}
	qglState->wantShader.alphaRef = ref;
}


void qglDisable(GLenum cap) {
	flushDraws("qglDisable");

	switch (cap) {
	case GL_ALPHA_TEST:
		if (qglState->wantShader.alphaTest) {
			qglState->wantShader.alphaTest = false;
			qglState->shaderDirty = true;
		}
		break;

	case GL_TEXTURE_2D:
		if (qglState->wantShader.texState[qglState->wantActiveTexture].texEnable) {
			qglState->wantShader.texState[qglState->wantActiveTexture].texEnable = false;
			qglState->shaderDirty = true;
		}
		break;

	case GL_BLEND:
		qglState->wantPipeline.blend = false;
		qglState->pipelineDirty = true;
		break;

	default:
		glDisable(cap);
		break;
	}
}


void qglEnable(GLenum cap) {
	flushDraws("qglEnable");

	switch (cap) {
	case GL_ALPHA_TEST:
		if (!qglState->wantShader.alphaTest) {
			qglState->wantShader.alphaTest = true;
			qglState->shaderDirty = true;
		}
		break;

	case GL_TEXTURE_2D:
		if (!qglState->wantShader.texState[qglState->wantActiveTexture].texEnable) {
			qglState->wantShader.texState[qglState->wantActiveTexture].texEnable = true;
			qglState->shaderDirty = true;
		}
		break;

	case GL_BLEND:
		qglState->wantPipeline.blend = true;
		qglState->pipelineDirty = true;
		break;

	default:
		glEnable(cap);
		break;
	}
}


void qglTexEnvi(GLenum target, GLenum pname, GLint param) {
	assert(target == GL_TEXTURE_ENV);

	// we only use one combine mode
	if (pname == GL_TEXTURE_ENV_MODE) {
		assert(param == GL_COMBINE_ARB
			  || param == GL_MODULATE
			  || param == GL_REPLACE);
		if (qglState->wantShader.texState[qglState->wantActiveTexture].texMode != param) {
			flushDraws("qglTexEnvi");

			qglState->shaderDirty = true;
			qglState->wantShader.texState[qglState->wantActiveTexture].texMode = param;
		}
	} else if (pname == GL_COMBINE_RGB_ARB) {
		assert(param == GL_MODULATE);
	} else if (pname == GL_COMBINE_ALPHA_ARB) {
		assert(param == GL_MODULATE);
	} else if (pname == GL_RGB_SCALE_ARB) {
		assert(param == 2);
	} else {
		assert(false);
	}
}


void qglBindTexture(GLenum target, GLuint texture) {
	flushDraws("qglBindTexture");

	if (qglState->activeTexture != qglState->wantActiveTexture) {
		glActiveTexture(GL_TEXTURE0);
		qglState->activeTexture = qglState->wantActiveTexture;
	}

	glBindTexture(target, texture);
}


void qglDepthRange(GLclampd zNear, GLclampd zFar) {
	qglState->zNear = zNear;
	qglState->zFar = zFar;
	qglState->projMatrixDirty = true;
}
