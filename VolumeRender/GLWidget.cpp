#include "GLWidget.h"
#include <QDebug>
#include <fstream>
#include <vector>

GLWidget::GLWidget(RenderMode rMode, const QGLFormat &format, QWidget *parent) :
    QGLWidget(format, parent)
{
    setFocusPolicy(Qt::ClickFocus);
    setMinimumSize(256, 256);
    setWindowTitle("Volume Ray-casting");

    datasetName = "SampleVolume.raw";
    volumeDim.x = 32;
    volumeDim.y = 32;
    volumeDim.z = 32;

    lighttype = 2;

    mode = rMode;
}

void GLWidget::initializeGL()
{
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
        qDebug() << glewGetErrorString(err);
#endif

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDisable(GL_DEPTH_TEST);
    glColorMask(true, true, true, true);
    glDepthMask(true);

    // initialize transformation
    rotation = Mat3f::identity();
    translation = Vec3f(0.0f, 0.0f, -2.0f);

    // load sample data volume
    //volumeDim.x = 256;
    //volumeDim.y = 256;
    //volumeDim.z = 178;
    int volumeSize = volumeDim.x * volumeDim.y * volumeDim.z;
    std::vector<unsigned char> volumeData;
//    std::vector<float> volumeData;
    volumeData.resize(volumeSize);
    std::ifstream ifs(datasetName.toStdString(), std::ios::binary);
    if(!ifs.is_open())
    {
        qDebug() << "Failed to open sample volume!";
        close();
    }

    ifs.read(reinterpret_cast<char *>(&volumeData.front()), volumeSize * sizeof(unsigned char));
    ifs.close();

    // prepare transfer function data
    tfdata.clear();
//    tfdata.push_back(Vec4f(1.0f, 0.0f, 0.0f, 0.0f));
//    tfdata.push_back(Vec4f(1.0f, 1.0f, 0.0f, 0.1f));
//    tfdata.push_back(Vec4f(0.0f, 1.0f, 0.0f, 0.2f));
//    tfdata.push_back(Vec4f(0.0f, 1.0f, 1.0f, 0.3f));
//    tfdata.push_back(Vec4f(0.0f, 0.0f, 1.0f, 0.4f));
//    tfdata.push_back(Vec4f(1.0f, 0.0f, 1.0f, 0.5f));
    for(int i = 0; i< 512; i++)
    {
        if(i < 64)
            tfdata.push_back(Vec4f(1.0f, 0.0f, 0.0f, 0.0f));
        else if(i<128)
            tfdata.push_back(Vec4f(1.0f, 1.0f, 0.0f, 0.001f*i));
        else if(i<196)
            tfdata.push_back(Vec4f(0.0f, 1.0f, 0.0f, 0.001f*i));
        else if(i<312)
            tfdata.push_back(Vec4f(0.2f, 1.0f, 1.0f, 0.0001f*i));
        else if(i<400)
            tfdata.push_back(Vec4f(0.1f, 0.3f, 1.0f, 0.0001f*i));
        else
            tfdata.push_back(Vec4f(1.0f, 0.0f, 1.0f, 0.0002f*i));
    }

    // create OpenGL textures
    volumeTex = GLTexture::create();
    volumeTex->updateTexImage3D(GL_R8, volumeDim, GL_RED, GL_UNSIGNED_BYTE, &volumeData.front());
    // create OpenGL textures for tricubic interpolation
    std::vector<unsigned char> volumeDataCubic;
    volumeDataCubic.resize(volumeSize*64);

    volumeTexTriCubic = GLTexture::create();
    volumeTexTriCubic->updateTexImage3DNoInterpolation(GL_R8, volumeDim, GL_RED, GL_UNSIGNED_BYTE, &volumeData.front());

    transferFuncTex = GLTexture::create();
    transferFuncTex->updateTexImage2D(GL_RGBA32F, Vec2i(int(tfdata.size()), 1), GL_RGBA, GL_FLOAT, &tfdata.front());

    // create full screen rectangle for volume ray-casting
    std::vector<Vec2f> rectVertices;
    rectVertices.push_back(Vec2f(0.0f, 0.0f));
    rectVertices.push_back(Vec2f(0.0f, 1.0f));
    rectVertices.push_back(Vec2f(1.0f, 1.0f));
    rectVertices.push_back(Vec2f(1.0f, 0.0f));
    rectVertexBuffer = GLArrayBuffer::create(GL_ARRAY_BUFFER);
    rectVertexBuffer->update(rectVertices.size() * sizeof(Vec2f), &rectVertices.front(), GL_STATIC_DRAW);
    rectVertexArray = GLVertexArray::create();

    // create OpenGL shaders
    volumeRayCastingProgram = GLShaderProgram::create("VolumeRayCasting.vert", "VolumeRayCasting.frag");
    if(volumeRayCastingProgram == NULL)
        close();

    // assign vertex buffer to the shader attribute input called "Vertex"
    volumeRayCastingProgram->setVertexAttribute("Vertex", rectVertexArray, rectVertexBuffer, 2, GL_FLOAT, false);

    //Pre-integration
    preIntTex = GLTexture::create();
    preIntTex->updateTexImage2D(GL_RGBA32F, Vec2i(int(tfdata.size()), int(tfdata.size())), GL_RGBA, GL_FLOAT, &tfdata.front());

    preIntFBO = GLFramebuffer::create();
    preIntFBO->bind();
    colAtt = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &colAtt);
    preIntFBO->attachTexture(colAtt,preIntTex);
    preIntFBO->release();

    std::vector<Vec2f> rectVerticesPreint;
    rectVerticesPreint.push_back(Vec2f(-1.0f, -1.0f));
    rectVerticesPreint.push_back(Vec2f(-1.0f, 1.0f));
    rectVerticesPreint.push_back(Vec2f(1.0f, 1.0f));
    rectVerticesPreint.push_back(Vec2f(1.0f, -1.0f));
    preIntArrayBuffer = GLArrayBuffer::create(GL_ARRAY_BUFFER);
    preIntArrayBuffer->update(rectVerticesPreint.size() * sizeof(Vec2f), &rectVerticesPreint.front(), GL_STATIC_DRAW);
    preIntVertexArray = GLVertexArray::create();

    //initial shader
    preIntProgram = GLShaderProgram::create("PreIntegration.vert", "PreIntegration.frag");
    if(preIntProgram == NULL)
        close();
    preIntProgram->setVertexAttribute("V", preIntVertexArray, preIntArrayBuffer, 2, GL_FLOAT, false);

    //draw the pre-integration texture
    int w = width();
    int h = height();
    glViewport(0,0,tfdata.size(),tfdata.size());
    preIntFBO->bind();
//    glClearColor(0.0,0.0,0.0,0.0);
    glDisable(GL_BLEND);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    preIntProgram->setTexture("TF", transferFuncTex);
    preIntProgram->setUniform("deltastep",deltaStep);
    preIntProgram->setUniform("OCSize", opacityCorrection);
    preIntProgram->begin();
    preIntVertexArray->drawArrays(GL_TRIANGLE_FAN, 4);
    preIntProgram->end();
    preIntFBO->release();
    glEnable(GL_BLEND);
    glViewport(0,0,w ,h);
//    glClearColor(1.0,1.0,1.0,1.0);

    //bounding box shader
    boundBoxProgram = GLShaderProgram::create("BoundingBox.vert", "BoundingBox.frag");
    if(boundBoxProgram == NULL)
        close();

    //local Ambient Occulusion
    //initialize
    LAOTex = GLTexture::create();
    LAOTex->updateTexImage3D(GL_RGBA32F, volumeDim, GL_RGBA, GL_FLOAT,NULL);

    LAOFrameBuffer = GLFramebuffer::create();
    LAOFrameBuffer->bind();
    colAtt = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &colAtt);
    LAOFrameBuffer->attachTexture(colAtt,LAOTex);
    LAOFrameBuffer->release();

    LAOProgram = GLShaderProgram::create("LAO.vert", "LAO.frag");
    if(LAOProgram == NULL)
        close();
    drawLAOTexture();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    projectionMatrix = Mat4f::perspective(Math::pi<float>() * 0.25f, float(w) / float(h), 0.01f, 10.0f);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClearColor(1.0,1.0,1.0,1.0);
    Vec3f volumeScale = Vec3f(volumeDim);
    volumeScale = volumeScale / volumeScale.norm();
    modelViewMatrix = Mat4f(rotation, translation);
    Mat4f transform = Mat4f::ortho(0, 1, 0, 1, -1, 1);
    Mat4f invModelView = modelViewMatrix.inverse();

    volumeRayCastingProgram->setUniform("renderMode", int(mode));
    volumeRayCastingProgram->setUniform("volumeDim", Vec3f(volumeDim));
    volumeRayCastingProgram->setUniform("volumeScale", volumeScale);
    volumeRayCastingProgram->setUniform("transform", transform);
    volumeRayCastingProgram->setUniform("invModelView", invModelView);
    volumeRayCastingProgram->setUniform("aspect", float(width()) / float(height()));
    volumeRayCastingProgram->setUniform("cotFOV", projectionMatrix.elem[1][1]);
    volumeRayCastingProgram->setUniform("azimuth", azimuth);
    volumeRayCastingProgram->setUniform("elevation", elevation);
    volumeRayCastingProgram->setUniform("clipPlaneDepth", clipPlaneDepth);
    volumeRayCastingProgram->setUniform("clipFlag", gb_Clipflag);
    volumeRayCastingProgram->setUniform("deltaStep", deltaStep);
    volumeRayCastingProgram->setUniform("ambient", gb_ambient);
    volumeRayCastingProgram->setUniform("ambientCoE", gb_AmbientCoE);
    volumeRayCastingProgram->setUniform("specularC", gb_SpecularC);
    volumeRayCastingProgram->setUniform("diffuseC", gb_DiffuseC);
    volumeRayCastingProgram->setUniform("lightcolor", gb_lightcolor);
    volumeRayCastingProgram->setUniform("posdir", gb_PosOrDirect);
    volumeRayCastingProgram->setUniform("fshiness", gb_Shiness);
    volumeRayCastingProgram->setUniform("fstrength", gb_LightStrength);
    volumeRayCastingProgram->setUniform("fcosattent", gb_CosAttent);
    volumeRayCastingProgram->setUniform("flineattent", gb_LineAttent);
    volumeRayCastingProgram->setUniform("fquadattent", gb_QuadAttent);
    volumeRayCastingProgram->setUniform("lighttype", lighttype);
    volumeRayCastingProgram->setUniform("preintFlag", gb_PreIntFlag);
    volumeRayCastingProgram->setUniform("LAOFlag", gb_AOFlag);
    volumeRayCastingProgram->setUniform("OCSize", opacityCorrection);

    volumeRayCastingProgram->setTexture("volumeTex", volumeTex);
//    volumeRayCastingProgram->setTexture("volumeTexCubic", volumeTexTriCubic);
    volumeRayCastingProgram->setTexture("transferFuncTex", transferFuncTex);
    volumeRayCastingProgram->setTexture("preInt", preIntTex);
    volumeRayCastingProgram->setTexture("LAOTex", LAOTex);
    volumeRayCastingProgram->begin();
    rectVertexArray->drawArrays(GL_TRIANGLE_FAN, 4);
    volumeRayCastingProgram->end();

//    preIntProgram->setTexture("TF", transferFuncTex);
//    preIntProgram->setUniform("deltastep",deltaStep);
//    preIntProgram->begin();
//    preIntVertexArray->drawArrays(GL_TRIANGLE_FAN, 4);
//    preIntProgram->end();
    
//    boundBoxProgram->setUniform("transform", transform);
//    boundBoxProgram->begin();
//    rectVertexArray->drawArrays(GL_LINES, 4);
//    boundBoxProgram->end();
    drawBoundingBox();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos = event->localPos();
    float dx = pos.x() - mousePos.x();
    float dy = pos.y() - mousePos.y();
    mousePos = pos;

    Qt::MouseButtons mouseButtons = event->buttons();

    if(mouseButtons & Qt::LeftButton)
    {
        rotation = Mat3f::fromAxisAngle(Vec3f::unitY(), dx * 0.005f) * rotation;
        rotation = Mat3f::fromAxisAngle(Vec3f::unitX(), dy * 0.005f) * rotation;
        rotation.orthonormalize();
    }

    if(mouseButtons & Qt::RightButton)
    {
        translation.z += dy * 0.005f;
        translation.z = clamp(translation.z, -9.0f, 1.0f);
    }

    if(mouseButtons & Qt::MidButton)
    {
        float scale = -std::min(translation.z, 0.0f) * 0.001f + 0.000025f;
        translation.x += dx * scale;
        translation.y -= dy * scale;
    }

    updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    mousePos = event->localPos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mousePos = event->localPos();
}

void GLWidget::wheelEvent(QWheelEvent * event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;
    float dx = 0.0f, dy = 0.0f;
    if(!numPixels.isNull())
    {
        dx = numPixels.x();
        dy = numPixels.y();
    }
    else if(!numDegrees.isNull())
    {
        dx = numDegrees.x() / 15;
        dy = numDegrees.y() / 15;
        dy = numDegrees.y() * -1;
    }

    Qt::KeyboardModifiers km = QApplication::keyboardModifiers();

    if(km & Qt::ControlModifier)
    {
        rotation = Mat3f::fromAxisAngle(Vec3f::unitY(), dx * 0.005f) * rotation;
        rotation = Mat3f::fromAxisAngle(Vec3f::unitX(), dy * 0.005f) * rotation;
    }
    else if(km & Qt::AltModifier)
    {
        float scale = -std::min(translation.z, 0.0f) * 0.001f + 0.000025f;
        translation.x += dx * scale;
        translation.y -= dy * scale;
    }
    else// if(km & Qt::ShiftModifier)
    {
        translation.z += dy * 0.005f;
        translation.z = clamp(translation.z, -9.0f, 1.0f);
    }

    updateGL();
}

void GLWidget::GetClipflag(Vec3f clip)
{
    //volumeDim = dim;
    azimuth = clip.x;
    elevation = clip.y;
    clipPlaneDepth = clip.z;
    updateGL();
}

void GLWidget::UpdateTransferFun()
{
    //transferFuncTex = GLTexture::create();
    transferFuncTex->updateTexImage2D(GL_RGBA32F, Vec2i(int(tfdata.size()), 1), GL_RGBA, GL_FLOAT, &tfdata.front());
    //draw the pre-integration texture
    preIntTex->updateTexImage2D(GL_RGBA32F, Vec2i(int(tfdata.size()), int(tfdata.size())), GL_RGBA, GL_FLOAT, &tfdata.front());

    int w = width();
    int h = height();
    glViewport(0,0,tfdata.size(),tfdata.size());
    preIntFBO->bind();
//    glClearColor(0.0,0.0,0.0,0.0);
    glDisable(GL_BLEND);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    preIntProgram->setTexture("TF", transferFuncTex);
    preIntProgram->begin();
    preIntVertexArray->drawArrays(GL_TRIANGLE_FAN, 4);
    preIntProgram->end();
    glEnable(GL_BLEND);
    preIntFBO->release();
    glViewport(0,0,w ,h);
//    glClearColor(1.0,1.0,1.0,1.0);
    updateGL();
}

void GLWidget::update()
{
    QGLWidget::update();
    transferFuncTex->updateTexImage2D(GL_RGBA32F, Vec2i(int(tfdata.size()), 1), GL_RGBA, GL_FLOAT, &tfdata.front());
}

void GLWidget::OpenNewDataset(QString datasetname, Vec3i volumedim)
{
    datasetName = datasetname;
    volumeDim = volumedim;
    initializeGL();
    updateGL();
}

void GLWidget::UpdateStepSize(float size)
{
    if(size < 0.001)
        size = 0.001;
    deltaStep = size;
    //initializeGL();
    updateGL();
}

void GLWidget::UpdateOpacityCorretion(float para)
{
    opacityCorrection = para;
    //initializeGL();
    updateGL();
}

void GLWidget::UpdateLight(int type)
{
    lighttype = type;
    updateGL();
}

void GLWidget::drawLAOTexture()
{
    Vec3f volumeScale = Vec3f(volumeDim);
    volumeScale = volumeScale / volumeScale.norm();
    std::vector<Vec2f> LAOrectVertices;
    LAOrectVertices.push_back(Vec2f(-1.0f, -1.0f));
    LAOrectVertices.push_back(Vec2f(-1.0f, 1.0f));
    LAOrectVertices.push_back(Vec2f(1.0f, 1.0f));
    LAOrectVertices.push_back(Vec2f(1.0f, -1.0f));
    LAOArrayBuffer = GLArrayBuffer::create(GL_ARRAY_BUFFER);
    LAOArrayBuffer->update(LAOrectVertices.size() * sizeof(Vec2f), &LAOrectVertices.front(), GL_STATIC_DRAW);
    LAOVertexArray = GLVertexArray::create();
    LAOProgram->setVertexAttribute("V", LAOVertexArray, LAOArrayBuffer, 2, GL_FLOAT, false);
    for(int i = 0; i< volumeDim.z; i++)
    {
        int w = width();
        int h = height();
        LAOFrameBuffer->bind();
        glClearColor(0.0,0.0,0.0,0.0);
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
//        LAOProgram->setTexture("TF", transferFuncTex);
        LAOProgram->setTexture("volumeTex", volumeTex);
        LAOProgram->setTexture("preIntTex", preIntTex);
        LAOProgram->setUniform("layer", i);
        LAOProgram->setUniform("volumeScale", volumeScale);

        LAOProgram->begin();
        LAOVertexArray->drawArrays(GL_TRIANGLE_FAN, 4);
        LAOProgram->end();
        LAOFrameBuffer->release();
    //    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glClearColor(1.0,1.0,1.0,1.0);

        glViewport(0,0,w ,h);
    }
}

void GLWidget::drawBoundingBox()
{
    Vec3f volumeScale = Vec3f(volumeDim);
    volumeScale = volumeScale / volumeScale.norm();
    std::vector<Vec3f> BoundingBoxPlane1;
    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, volumeScale.y, volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, -volumeScale.y, volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, -volumeScale.y, -volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, volumeScale.y, -volumeScale.z));
//    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, volumeScale.y, -volumeScale.z));
//    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, -volumeScale.y, -volumeScale.z));
//    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, -volumeScale.y, -volumeScale.z));
//    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, volumeScale.y, -volumeScale.z));
    GLArrayBuffer::Ptr boundingBoxArrayBuffer = GLArrayBuffer::create(GL_ARRAY_BUFFER);
    boundingBoxArrayBuffer->update(BoundingBoxPlane1.size() * sizeof(Vec3f), &BoundingBoxPlane1.front(), GL_STATIC_DRAW);
    GLVertexArray::Ptr bBoxVertexArray = GLVertexArray::create();
    volumeRayCastingProgram->setVertexAttribute("Vertex", bBoxVertexArray, boundingBoxArrayBuffer, 3, GL_FLOAT, false);


    boundBoxProgram->setUniform("projectM", projectionMatrix);
    boundBoxProgram->setUniform("modelview", modelViewMatrix);
    boundBoxProgram->begin();
    bBoxVertexArray->drawArrays(GL_LINE_LOOP, 4);
    boundBoxProgram->end();

    BoundingBoxPlane1.clear();
    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, volumeScale.y, -volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, -volumeScale.y, -volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, -volumeScale.y, -volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, volumeScale.y, -volumeScale.z));
    boundingBoxArrayBuffer->update(BoundingBoxPlane1.size() * sizeof(Vec3f), &BoundingBoxPlane1.front(), GL_STATIC_DRAW);
    volumeRayCastingProgram->setVertexAttribute("Vertex", bBoxVertexArray, boundingBoxArrayBuffer, 3, GL_FLOAT, false);


    boundBoxProgram->setUniform("projectM", projectionMatrix);
    boundBoxProgram->setUniform("modelview", modelViewMatrix);
    boundBoxProgram->begin();
    bBoxVertexArray->drawArrays(GL_LINE_LOOP, 4);
    boundBoxProgram->end();

    BoundingBoxPlane1.clear();
    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, volumeScale.y, -volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, -volumeScale.y, -volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, -volumeScale.y, volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, volumeScale.y, volumeScale.z));
    boundingBoxArrayBuffer->update(BoundingBoxPlane1.size() * sizeof(Vec3f), &BoundingBoxPlane1.front(), GL_STATIC_DRAW);
    volumeRayCastingProgram->setVertexAttribute("Vertex", bBoxVertexArray, boundingBoxArrayBuffer, 3, GL_FLOAT, false);


    boundBoxProgram->setUniform("projectM", projectionMatrix);
    boundBoxProgram->setUniform("modelview", modelViewMatrix);
    boundBoxProgram->begin();
    bBoxVertexArray->drawArrays(GL_LINE_LOOP, 4);
    boundBoxProgram->end();

    BoundingBoxPlane1.clear();
    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, volumeScale.y, volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(-volumeScale.x, -volumeScale.y, volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, -volumeScale.y, volumeScale.z));
    BoundingBoxPlane1.push_back(Vec3f(volumeScale.x, volumeScale.y, volumeScale.z));
    boundingBoxArrayBuffer->update(BoundingBoxPlane1.size() * sizeof(Vec3f), &BoundingBoxPlane1.front(), GL_STATIC_DRAW);
    volumeRayCastingProgram->setVertexAttribute("Vertex", bBoxVertexArray, boundingBoxArrayBuffer, 3, GL_FLOAT, false);


    boundBoxProgram->setUniform("projectM", projectionMatrix);
    boundBoxProgram->setUniform("modelview", modelViewMatrix);
    boundBoxProgram->begin();
    bBoxVertexArray->drawArrays(GL_LINE_LOOP, 4);
    boundBoxProgram->end();
}

void GLWidget::SetRanderMode(RenderMode MODE)
{
    mode = MODE;
}

//void GLWidget::createCubicCofData(std::vector<unsigned char> &source, std::vector<unsigned char> &cubicdata, Vec3 dataDim)
//{
//    cubicdata.resize(source.size()*8);
//    for(int i = 0; i< dataDim.x; i++)
//        for(int j = 0; j<dataDim.y; j++)
//            for(int k = 0; k< dataDim.z; k++)
//            {
//                Vec3f gradient;
//                if(i > 0 && i <dataDim.x-1)
//                    gradient.x = (source[i+j*dataDim.y+k*dataDim.y*dataDim.z]-source[i+j*dataDim.y+k*dataDim.y*dataDim.z])/2;
//            }

//}
