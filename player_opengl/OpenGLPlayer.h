#ifndef OpenGLPlayer_H
#define OpenGLPlayer_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMutex>

class QFile;
class QOpenGLTexture;
class QOpenGLShaderProgram;


class OpenGLPlayer :
	public QOpenGLWidget, QOpenGLFunctions
{
	Q_OBJECT
public:
	OpenGLPlayer(QWidget *parent = nullptr);
	~OpenGLPlayer();

public Q_SLOTS:
	void showYUV(QByteArray byte, int width, int height);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	QOpenGLTexture *m_textureYUV[3];
	QByteArray yuvArr[3];
	QOpenGLShaderProgram *program;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vbo;

	QMutex m_mutex;

	QByteArray m_byteArray;
	int m_nWidth;
	int m_nHeight;
};


#endif // OpenGLPlayer_H
