#ifndef GPU_RENDER_H
#define GPU_RENDER_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include <vector>

#include "v4l2capture.h"
#include "model.h"
#include "cameraparameter.h"
#include "camera3d.h"

QT_BEGIN_NAMESPACE
class QGestureEvent;
QT_END_NAMESPACE

extern "C"
{
#include <pthread.h>
}

class GpuRender : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
   explicit GpuRender(QVector<CameraParameter> &p, QWidget *parent = 0);
    ~GpuRender();

    void allocate_buffer(uint num);
    void setBuf(QVector<V4l2Capture::CapBuffers *> &pbuf);
    void setTextureSize(QSize &s);
    void enablePaint();
    void setCameraPara(QVector<CameraParameter> &p);
    void grabGestures(const QList<Qt::GestureType> &gestures);

public slots:

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    bool event(QEvent *e) Q_DECL_OVERRIDE;

#define PLANES 4

private:
    bool gestureEvent(QGestureEvent *e);


    typedef void (GL_APIENTRY *PFNGLTEXDIRECTVIVMAP)
                 (GLenum Target, GLsizei Width, GLsizei Height,
                  GLenum Format, GLvoid ** Logical, const GLuint *Physical);
    typedef void (GL_APIENTRY *PFNGLTEXDIRECTINVALIDATEVIV) (GLenum Target);
    PFNGLTEXDIRECTVIVMAP pFNglTexDirectVIVMap;
    PFNGLTEXDIRECTINVALIDATEVIV pFNglTexDirectInvalidateVIV;

    QVector<QString> cameraNames;
    QVector<Model*> objModels;
    QVector<CameraParameter> &camParas;

    QSize textureSize;
    GLuint textureCam;
    GLint textureLoc;
    GLint extrinsicLoc;
    QOpenGLShaderProgram *m_program;
    GLuint indexTransformBlock;
    GLuint programId;
    GLuint idUbo;
    GLint blockSize;
    GLfloat *pblockBuf;

    QVector<V4l2Capture::CapBuffers *> buffers;
//    std::vector<void *> pTexBuffers;

    pthread_mutex_t gpu_mutex;
    pthread_cond_t gpu_cond;

    bool paintFlag;

    unsigned char *alphaMasks[PLANES];
    GLuint maskTextureID[PLANES];
    GLint maskTextureLoc;

    Camera3D *camera3D;
};

#endif // GPU_RENDER_H
