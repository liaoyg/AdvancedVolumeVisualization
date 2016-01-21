#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include "GLArrayBuffer.h"
#include "GLFramebuffer.h"
#include "GLShaderProgram.h"
#include "GLTexture.h"
#include "GLVertexArray.h"
#include "Mat.h"
#include "Vec.h"
#include "Global.h"
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QGLWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(const QGLFormat &format = QGLFormat::defaultFormat(), QWidget *parent = 0);
    enum RenderMode {
        AUTOLINER,
        TRICUBIC,
    };

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void update();

    void drawLAOTexture();
    void drawBoundingBox();
//    std::vector<unsigned char> calculateIntermediateofTricubic(std::vector<unsigned char>* volumeData);

protected:
    QPointF mousePos;
    Mat3f rotation;
    Vec3f translation;
    Mat4f modelViewMatrix;
    Mat4f projectionMatrix;

    QString datasetName;

    int lighttype;

    GLWidget::RenderMode mode;

    Vec3i volumeDim;
    GLTexture::Ptr volumeTex;
    GLTexture::Ptr volumeTexTriCubic;
    GLTexture::Ptr transferFuncTex;
    GLArrayBuffer::Ptr rectVertexBuffer;
    GLArrayBuffer::Ptr rectIndexBuffer;
    GLVertexArray::Ptr rectVertexArray;
    GLShaderProgram::Ptr volumeRayCastingProgram;

    GLTexture::Ptr preIntTex;
    GLArrayBuffer::Ptr preIntArrayBuffer;
    GLVertexArray::Ptr preIntVertexArray;
    GLFramebuffer::Ptr preIntFBO;
    GLShaderProgram::Ptr preIntProgram;
    GLenum colAtt;

    GLShaderProgram::Ptr boundBoxProgram;

    //Ambient occlusion
    GLTexture::Ptr LAOTex;
    GLArrayBuffer::Ptr LAOArrayBuffer;
    GLVertexArray::Ptr LAOVertexArray;
    GLFramebuffer::Ptr LAOFrameBuffer;
    GLShaderProgram::Ptr LAOProgram;

public slots:
    void GetClipflag(Vec3f clip);
    void UpdateTransferFun();
    void OpenNewDataset(QString datasetname, Vec3i volumedim);
    void UpdateStepSize(float size);
    void UpdateOpacityCorretion(float para);
    void UpdateLight(int type);
};

#endif // GLWIDGET_H
