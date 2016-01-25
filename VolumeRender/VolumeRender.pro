#-------------------------------------------------
#
# Project created by QtCreator 2016-01-09T14:33:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

CONFIG += c++11

TARGET = VolumeRender
TEMPLATE = app


SOURCES += main.cpp\
    DatasetSelect.cpp \
    GLArrayBuffer.cpp \
    GLFramebuffer.cpp \
    GLShaderProgram.cpp \
    GLTexture.cpp \
    GLVertexArray.cpp \
    GLWidget.cpp \
    LightSetting.cpp \
    MainWindow.cpp \
    MytfButton.cpp \
    PhongShading.cpp \
    RaycastingSetting.cpp \
    TransferFun.cpp \
    TransferFunEditor.cpp \
    Volumediminput.cpp \
    VolumeVisEditor.cpp \
    InterpolationCompare.cpp

HEADERS  += \
    DatasetSelect.h \
    GLArrayBuffer.h \
    GLConfig.h \
    GLFramebuffer.h \
    Global.h \
    GLShaderProgram.h \
    GLTexture.h \
    GLVertexArray.h \
    GLWidget.h \
    LightSetting.h \
    MainWindow.h \
    Mat.h \
    MathUtil.h \
    MytfButton.h \
    PhongShading.h \
    RaycastingSetting.h \
    TransferFun.h \
    TransferFunEditor.h \
    Vec.h \
    Volumediminput.h \
    VolumeVisEditor.h \
    InterpolationCompare.h

FORMS    +=

OTHER_FILES += \
    BoundingBox.frag \
    LAO.frag \
    PreIntegration.frag \
    VolumeRayCasting.frag \
    BoundingBox.vert \
    LAO.vert \
    PreIntegration.vert \
    VolumeRayCasting.vert \
    aneurism.raw \
    backpack16.raw \
    ball.raw \
    bonsai.raw \
    BostonTeapot.raw \
    RayCastingAlt.frag \
    SampleVolume.raw \
    TriCubicCof.frag \
    TriCubicCof.vert

Data.files += $$OTHER_FILES

macx {
    Data.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += Data
    QMAKE_MAC_SDK = macosx10.9
}

win32 {
    Data.path = $$OUT_PWD
    INSTALLS += Data
    LIBS += -L$$PWD/glew/ -lglew32
    INCLUDEPATH += $$PWD/glew
    DEPENDPATH += $$PWD/glew
    PRE_TARGETDEPS += $$PWD/glew/glew32.lib
    DESTDIR = $$OUT_PWD
}

unix:!macx {
    Data.path = $$OUT_PWD
    INSTALLS += Data
    LIBS += -lGLEW
}

FORMS +=
