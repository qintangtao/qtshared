#include "OpenGLPlayer.h"
#include <QDebug>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


#include <QLoggingCategory>
Q_LOGGING_CATEGORY(logOpengl, "log.player.opengl")

////GLSL3.0版本后,废弃了attribute关键字（以及varying关键字），属性变量统一用in/out作为前置关键字
#define GL_VERSIONS  "#version 330 core\n"
#define GET_GLSTR(x) GL_VERSIONS#x


const char *vsrc = GET_GLSTR(

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}

);

const char *fsrc = GET_GLSTR(

out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texY;
uniform sampler2D texU;
uniform sampler2D texV;

void main()
{
    vec3 yuv;
    vec3 rgb;

    yuv.x = texture(texY, TexCoord).r;
    yuv.y = texture(texU, TexCoord).r - 0.5;
    yuv.z = texture(texV, TexCoord).r - 0.5;

    rgb = mat3(1.0,         1.0,             1.0,
                       0.0,         -0.3455,      1.779,
                      1.4075,    -0.7169,       0.0) * yuv;

    FragColor = vec4(rgb, 1.0);
}

);


/*
YUV转RGB:

R = Y + 1.4075 * V;
G = Y - 0.3455 * U - 0.7169*V;
B = Y + 1.779 * U;
*/

OpenGLPlayer::OpenGLPlayer(QWidget *parent)
	: QOpenGLWidget(parent)
	, program(NULL)
	, m_nWidth(0)
	, m_nHeight(0)
{
	for (int i = 0; i < 3; i++)
		m_textureYUV[i] = NULL;
}


OpenGLPlayer::~OpenGLPlayer()
{
	makeCurrent();

	vao.destroy();
	vbo.destroy();

	if (program)
		program->deleteLater();

	for (int i = 0; i < 3; i++)
	{
		if (m_textureYUV[i] != NULL)
			m_textureYUV[i]->destroy();
	}

	doneCurrent();
}

void OpenGLPlayer::showYUV(QByteArray byte, int width, int height)
{
    //qCDebug(logOpengl) << width << height;

	m_mutex.lock();
	m_byteArray = byte;
	m_nWidth = width;
	m_nHeight = height;
	m_mutex.unlock();
	update();
}


#include <QOffscreenSurface>
void PrintVersion()
{
    QOffscreenSurface surf;
    surf.create();

   QOpenGLContext ctx;
   ctx.create();
   ctx.makeCurrent(&surf);

   GLint major, minor;
   ctx.functions()->glGetIntegerv(GL_MAJOR_VERSION, &major); //gl大版本（整型）
   ctx.functions()->glGetIntegerv(GL_MINOR_VERSION, &minor); //gl小版本（整型）

   qCDebug(logOpengl) << "OpenGL Version Info:" << (const char *)ctx.functions()->glGetString(GL_VERSION);
   qCDebug(logOpengl) << "OpenGL Version Major:" << major << "OpenGL Version Minor:" << minor;

   //ctx.doneCurrent();
}


/*virtual*/ void OpenGLPlayer::initializeGL()
{
    qCDebug(logOpengl) << "";

    //PrintVersion();

	//为当前环境初始化OpenGL函数
	initializeOpenGLFunctions();

	//设置背景色为白色
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//初始化纹理对象
	for (int i = 0; i < 3; i++)
		m_textureYUV[i] = new QOpenGLTexture(QOpenGLTexture::Target2D);

	program = new  QOpenGLShaderProgram(this);
	program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
	program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
	program->link();
	program->bind();

	//初始化VBO,将顶点数据存储到buffer中,等待VAO激活后才能释放
	float vertices[] = {
	   // 顶点坐标              //纹理坐标
	   -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,        //左下
	   1.0f , -1.0f, 0.0f,  1.0f, 1.0f,        //右下
	   -1.0f, 1.0f,  0.0f,  0.0f, 0.0f,        //左上
	   1.0f,  1.0f,  0.0f,  1.0f, 0.0f         //右上
	};

	vbo.create();
	vbo.bind();              //绑定到当前的OpenGL上下文,
	vbo.allocate(vertices, sizeof(vertices));
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);  //设置为一次修改，多次使用(坐标不变,变得只是像素点)

	//初始化VAO,设置顶点数据状态(顶点，法线，纹理坐标等)
	vao.create();
	vao.bind();

	// void setAttributeBuffer(int location, GLenum type, int offset, int tupleSize, int stride = 0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(float));   //设置aPos顶点属性
	program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));   //设置aColor顶点颜色
	program->enableAttributeArray(0); //使能
	program->enableAttributeArray(1);

	//解绑所有对象
	vao.release();
	vbo.release();
}
/*virtual*/ void OpenGLPlayer::resizeGL(int w, int h)
{
    //qCDebug(logOpengl) << w << h;
	QOpenGLWidget::resizeGL(w, h);
}
/*virtual*/ void OpenGLPlayer::paintGL()
{
    //qCDebug(logOpengl) << m_nWidth << m_nHeight;

	if (m_nWidth == 0 || m_nHeight == 0)
		return;

	glClear(GL_COLOR_BUFFER_BIT);

	// 渲染Shader
	vao.bind();

	program->setUniformValue("texY", 0);
	program->setUniformValue("texU", 1);
	program->setUniformValue("texV", 2);

	m_mutex.lock();
	for (int i = 0; i < 3; i++)
	{
		int width = m_nWidth;
		int height = m_nHeight;
		if (i == 0)
		{
			yuvArr[i] = m_byteArray.mid(0, m_nWidth*m_nHeight);
		}
		else
		{
			width = m_nWidth / 2;
			height = m_nHeight / 2;

			yuvArr[i] = m_byteArray.mid(m_nWidth * m_nHeight + (m_nWidth*m_nHeight / 4) * (i - 1), m_nWidth * m_nHeight / 4);
		}

		if (m_textureYUV[i]->width() != width || m_textureYUV[i]->height() != height)
		{
			if (m_textureYUV[i]->isCreated())
				m_textureYUV[i]->destroy();

			m_textureYUV[i]->create();
			m_textureYUV[i]->setSize(width, height);
			m_textureYUV[i]->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
			m_textureYUV[i]->setFormat(QOpenGLTexture::R8_UNorm);
			m_textureYUV[i]->allocateStorage();        //存储配置(放大缩小过滤、格式、size)
		}

		m_textureYUV[i]->setData(QOpenGLTexture::Red, QOpenGLTexture::UInt8, yuvArr[i]);
		m_textureYUV[i]->bind(i);
	}
	m_mutex.unlock();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	for (int i = 0; i < 3; i++)
		m_textureYUV[i]->release();

	vao.release();       //解绑
}
