#include "gpurender.h"

#include <QElapsedTimer>
#include <QDebug>
#include <QFile>
#include <QEvent>
#include <QGesture>
#include <QtMath>

#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl3.h>

#define EGL_API_FB
#include <EGL/egl.h>

#ifndef GL_VIV_direct_texture
#define GL_VIV_YV12                     0x8FC0
#define GL_VIV_NV12                     0x8FC1
#define GL_VIV_YUY2                     0x8FC2
#define GL_VIV_UYVY                     0x8FC3
#define GL_VIV_NV21                     0x8FC4
#endif

/* GL_VIV_clamp_to_border */
#ifndef GL_VIV_clamp_to_border
#define GL_VIV_clamp_to_border 1
#define GL_CLAMP_TO_BORDER_VIV         0x812D
#endif

GpuRender::GpuRender(QWidget *parent) :
    QOpenGLWidget(parent),
    pFNglTexDirectVIVMap(NULL),
    pFNglTexDirectInvalidateVIV(NULL),
    camParas(NULL),
    m_program(0),
    pblockBuf(NULL),
    paintFlag(false)
{
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAttribute(Qt::WA_AcceptTouchEvents);

    pFNglTexDirectVIVMap = (PFNGLTEXDIRECTVIVMAP)
            eglGetProcAddress("glTexDirectVIVMap");
    if(pFNglTexDirectVIVMap == NULL) {
        qInfo() << "error no glTexDirectVIVMap function";
    }

    pFNglTexDirectInvalidateVIV = (PFNGLTEXDIRECTINVALIDATEVIV)
            eglGetProcAddress("glTexDirectInvalidateVIV");
    if(pFNglTexDirectInvalidateVIV == NULL) {
        qInfo() << "error no glTexDirectInvalidateVIV function";
    }

    for(unsigned char i = 0; i < PLANES; i++) {
        alphaMasks[i] = NULL;
    }

//    cameraNames << "front" << "left" << "back" << "right";

//    QString cameraParasDir("./resources/cameras/cameras.xml");
//    foreach(const QString &item, cameraNames) {
//        camParas.append(new CameraParameter(cameraParasDir.toStdString(),
//                                        item.toStdString()));
//    }

    camera3D = new Camera3D;
}

GpuRender::~GpuRender()
{
    makeCurrent();

    for(unsigned char i = 0; i < PLANES; i++) {
        if(alphaMasks[i]) {
            delete [] alphaMasks[i];
        }
    }
    delete m_program;

    foreach (auto &item, objModels) {
        delete item;
    }
    delete camera3D;
    doneCurrent();
}

void GpuRender::allocate_buffer(uint num)
{
    buffers.fill(NULL, num);
//    pTexBuffers.assign(num, NULL);
}

void GpuRender::setBuf(QVector<V4l2Capture::CapBuffers *> &pbuf)
{
    for(int i = 0; i < buffers.size(); ++i) {
        buffers[i] = pbuf[i];
    }

//    for(uint i = 0; i < pTexBuffers.size(); ++i) {
//        pTexBuffers[i] = pbuf[i]->start;
    //    }
}

void GpuRender::enablePaint()
{
    paintFlag = 1;
}

void GpuRender::setCameraPara(QVector<CameraParameter> &p)
{
    camParas = &p;
}

static const GLint eglTexture[4] = {
    GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3
};

void GpuRender::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vsh");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.fsh");
    m_program->link();
    m_program->bind();
    programId = m_program->programId();

    textureLoc = m_program->uniformLocation("s_texture");
    glGenTextures(1, &textureCam);
    glUniform1i(textureLoc, 0);

    glBindTexture(GL_TEXTURE_2D, textureCam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    extrinsicLoc = glGetUniformLocation(programId, "extrinsic");

    QString modelDir("./data/");
    QVector<QString> modelName;
    modelName << "bowlfront.obj" << "bowlleft.obj"
              << "bowlback.obj" << "bowlright.obj";
    for(int i = 0; i < modelName.size(); i++) {
        objModels.append(new Model( (modelDir + modelName[i]).toStdString() ));
    }
///*-----------------------------blend mask----------------*/
//    unsigned int width(720);
//    unsigned int height(480);
//    for(unsigned char i = 0; i < PLANES; i++) {
//        alphaMasks[i] = new unsigned char[width * height];
//    }
//    float w1, w2, w3, w4, x1, x2, x3, x4;
//    unsigned char alpha;

//    //front mask
//    for(unsigned int i = 0; i < height; i++) {
//        for(unsigned int j = 0; j < width; j++) {
//            x1 = 479.0*185.0/152.0 - (float)i * 185.0 / 152.0;
//            x2 = 66.0 + 479.0 * 119.0 / 152.0 - (float)i*119.0 / 152.0;
//            w1 = -(float)j + x1;
//            w2 = -(float)j + x2;

//            x3 = 185.0 / 152.0 * (float)i;
//            x4 = 66.0 + 119.0 / 152.0 * (float)i;
//            w3 = -(float)j + x3;
//            w4 = -(float)j + x4;

//            if( (w1 <= 0.0)&&(w2 >= 0.0) ) {
////                if(j==184)
////                    qInfo() << j << i;
//                alpha = (unsigned char)(255.0 / (x2 - x1) * w2);
//            } else if ( (w3 <=0)&&(w4 >=0) ) {
////                if(j>185)
////                    qInfo() << j << i;
//                alpha = (unsigned char)(255.0 / (x4 - x3) * w4);
//            } else {
//                alpha = 255;
//            }
//            alphaMasks[0][i * width + j] = alpha;
//        }
//    }
//    //left mask
//    for(unsigned int i = 0; i < height; i++) {
//        for(unsigned int j = 0; j < width; j++) {
//            x1 = 185.0 / 152.0 * (float)i;
//            x2 = 66.0 + 119.0 / 152.0 * (float)i;
//            w1 = -(float)j + x1;
//            w2 = -(float)j + x2;

//            x3 = 653.0 - (float)i * 118.0 / 152.0;
//            x4 = 719.0 - (float)i*184.0 / 152.0;
//            w3 = -(float)j + x3;
//            w4 = -(float)j + x4;

//            if( (w1 <= 0.0)&&(w2 >= 0.0) ) {
//                alpha = (unsigned char)(255.0 / (x2 - x1) * (-w1));
//            } else if ( (w3 <=0)&&(w4 >=0) ) {
//                alpha = (unsigned char)(255.0 / (x4 - x3) * w4);
//            } else {
//                alpha = 255;
//            }
//            alphaMasks[1][i * width + j] = alpha;
//        }
//    }
//    //back mask
//    for(unsigned int i = 0; i < height; i++) {
//        for(unsigned int j = 0; j < width; j++) {
//            x1 = 653.0 - (float)i * 118.0 / 152.0;
//            x2 = 719.0 - (float)i*184.0 / 152.0;
//            w1 = -(float)j + x1;
//            w2 = -(float)j + x2;

//            x3 = 118.0 / 152.0 * (float)i - 327.0/152.0*118.0 + 535.0;
//            x4 = 184.0 / 152.0 * (float)i - 327.0/152.0*184.0 + 535.0;
//            w3 = -(float)j + x3;
//            w4 = -(float)j + x4;

//            if( (w1 <= 0.0)&&(w2 >= 0.0) ) {
//                alpha = (unsigned char)(255.0 / (x2 - x1) * (-w1));
//            } else if ( (w3 <=0)&&(w4 >=0) ) {
//                alpha = (unsigned char)(255.0 / (x4 - x3) * (-w3));
//            } else {
//                alpha = 255;
//            }
//            alphaMasks[2][i * width + j] = alpha;
//        }
//    }
//    //right mask
//    for(unsigned int i = 0; i < height; i++) {
//        for(unsigned int j = 0; j < width; j++) {
//            x1 = 118.0 / 152.0 * (float)i - 327.0/152.0*118.0 + 535.0;
//            x2 = 184.0 / 152.0 * (float)i - 327.0/152.0*184.0 + 535.0;
//            w1 = -(float)j + x1;
//            w2 = -(float)j + x2;

//            x3 = 327.0/152.0*185.0 + 185.0 - (float)i * 185.0 / 152.0;
//            x4 = 327.0/152.0*119.0 + 185.0 - (float)i * 119.0 / 152.0;
//            w3 = -(float)j + x3;
//            w4 = -(float)j + x4;

//            if( (w1 <= 0.0)&&(w2 >= 0.0) ) {
//                alpha = (unsigned char)(255.0 / (x2 - x1) * w2);
//            } else if ( (w3 <=0)&&(w4 >=0) ) {
//                alpha = (unsigned char)(255.0 / (x4 - x3) * (-w3));
//            } else {
//                alpha = 255;
//            }
//            alphaMasks[3][i * width + j] = alpha;
//        }
//    }

//    maskTextureLoc = m_program->uniformLocation("alpha_mask");
//    glUniform1i(maskTextureLoc, 1);
//    glGenTextures(PLANES, maskTextureID);

//    for(unsigned char i = 0; i < PLANES; i++) {
//        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//        glBindTexture(GL_TEXTURE_2D, maskTextureID[i]);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA,
//                     GL_UNSIGNED_BYTE, alphaMasks[i]);
//    }


//    indexTransformBlock = glGetUniformBlockIndex(programId, "transformBlock");

//    glGetActiveUniformBlockiv(programId, indexTransformBlock,
//                              GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
//    glUniformBlockBinding(programId, indexTransformBlock, 1);
//    glGenBuffers(1, &idUbo);
//    glBindBufferBase(GL_UNIFORM_BUFFER, 1, idUbo);
//    glBufferData(GL_UNIFORM_BUFFER, blockSize, NULL, GL_DYNAMIC_DRAW);
//    pblockBuf = (GLfloat *)
//                 glMapBufferRange(GL_UNIFORM_BUFFER, 0, blockSize,
//                                 GL_MAP_WRITE_BIT |
//                                 GL_MAP_INVALIDATE_BUFFER_BIT);
//    glUnmapBuffer(GL_UNIFORM_BUFFER);
//    if (pblockBuf == NULL) {
//        qWarning() << "Error mapping uniform  buffer object.";
//        return;
//    }

//    model.translate(0., 0., 0.);
//    m_program->setUniformValue("model", model);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GpuRender::paintGL()
{
    if(paintFlag) {

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    //    glViewport(0,0,720, 480);

        m_program->setUniformValue("projection", camera3D->perspectMatrix());
        m_program->setUniformValue("view", camera3D->viewMatrix());
    //    for(uint i = 0; i < pTexBuffers.size(); i++) {

        for(int i = 0; i < buffers.size(); i++) {

            GLuint phy = ~0U;
    //        pTexBuffers[i] = NULL;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureCam);

            (*pFNglTexDirectVIVMap)(GL_TEXTURE_2D, 720, 480, GL_VIV_UYVY,
                                    /*&pTexBuffers[i]*/&(buffers[i]->start),
                                    &phy);
            (*pFNglTexDirectInvalidateVIV)(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, maskTextureID[i]);

            m_program->bind();
            m_program->setUniformValue("intrinsic", camParas->at(i).qIntrinsic);
            m_program->setUniformValue("distCoeffs", camParas->at(i).qDistCoeffs);
            glUniformMatrix4x3fv(extrinsicLoc, 1, GL_FALSE,
                                 camParas->at(i).qExtrinsic.data());
            objModels.at(i)->Draw(programId);
        }
    }
}

void GpuRender::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    camera3D->setAspectRatio(aspect);
}

bool GpuRender::event(QEvent *e)
{
    if(e->type() == QEvent::TouchBegin) {
        return true;
    }
    if(e->type() == QEvent::TouchEnd) {
        return true;
    }
    if(e->type() == QEvent::TouchUpdate) {
//        qInfo() << "touch update";
        QTouchEvent *touchEvent = static_cast<QTouchEvent*>(e);
        auto points = touchEvent->touchPoints();
        auto sizePoints = points.size();

        if(sizePoints == 1) {
            QVector2D diff = QVector2D(points[0].pos() - points[0].lastPos());
            camera3D->viewRotate(QVector2D(diff));
            return true;
        }

        if(points.size() == 2) {
            QPointF p1(points[0].pos() - points[1].pos());
            QPointF p2(points[0].lastPos() - points[1].lastPos());
            float len1 = qSqrt(qPow(p1.x(), 2) + qPow(p1.y(), 2));
            float len2 = qSqrt(qPow(p2.x(), 2) + qPow(p2.y(), 2));
            camera3D->setFovBias(len2 - len1);
            return true;
        }
        return true;
    }

    return QOpenGLWidget::event(e);
}
