QT += widgets sql

# Si se define, entonces no usara multimedia ni opengl
DEFINES += RASPBERRY

! defined(RASPBERRY)  {
#    QT += multimedia opengl
#    unix:INCLUDEPATH += "/usr/include/GL/"
}

DIR_OPENCV_LIBS = /usr/local/lib
DIR_OTHER_LIBS = /usr/lib/x86_64-linux-gnu


unix:LIBS += -L$$DIR_OPENCV_LIBS

unix:LIBS += -lopencv_core \
             -lopencv_highgui \
             -lopencv_imgproc \
             -lopencv_objdetect \
#             -lopencv_calib3d \
#             -lopencv_ml \
             -lopencv_video \
#             -lopencv_features2d \
#             -lopencv_flann \
#             -lopencv_photo \
#             -lopencv_stitching \
#             -lopencv_superres \
#             -lopencv_video \
#             -lopencv_videostab \
#             -lopencv_imgcodecs \
             -lopencv_videoio \
#             -lopencv_bgsegm

win32:DIR_OPENCV_LIBS = C:/Qt/OpenCV-3.1.0

win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/core/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/imgproc/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/video/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/flann/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/features2d/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/calib3d/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/legacy/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/objdetect/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/highgui/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/photo/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/ml/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/contrib/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/hal/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/imgcodecs/include"
win32:INCLUDEPATH += "$$DIR_OPENCV_LIBS/opencv/sources/modules/videoio/include"

win32:LIBS += -L"$$DIR_OPENCV_LIBS/opencv/compilado/lib"

win32:LIBS += -lopencv_core310.dll
win32:LIBS += -lopencv_highgui310.dll
win32:LIBS += -lopencv_imgproc310.dll
win32:LIBS += -lopencv_objdetect310.dll
#win32:LIBS += -lopencv_calib3d310.dll

#win32:LIBS += -lopencv_ml310.dll
win32:LIBS += -lopencv_video310.dll
#win32:LIBS += -lopencv_features2d310.dll
#win32:LIBS += -lopencv_flann310.dll
#win32:LIBS += -lopencv_photo310.dll
#win32:LIBS += -lopencv_stitching310.dll
#win32:LIBS += -lopencv_superres310.dll
#win32:LIBS += -lopencv_video310.dll
#win32:LIBS += -lopencv_videostab310.dll
#win32:LIBS += -lopencv_imgcodecs310.dll
win32:LIBS += -lopencv_videoio310.dll

win32:LIBS += -lopengl32
win32:LIBS += -lglu32

SOURCES += \
    main.cpp \
    graph/graph.cpp \
    graph/xmlreader.cpp \
    interface/block.cpp \
    interface/camerawidget.cpp \
    interface/image.cpp \
    interface/interface.cpp \
    processor/camera.cpp \
    theme/colorizer.cpp \
    theme/theme.cpp \
    pointmapper.cpp \
    featuresprocessor.cpp \
    gesturerecognizer.cpp \
    slider.cpp \
    handdetection.cpp \
    graph/nodo.cpp \
    database/admindb.cpp

HEADERS += \
    graph/graph.hpp \
    graph/xmlreader.hpp \
    interface/block.hpp \
    interface/camerawidget.h \
    interface/interface.hpp \
    interface/texture.h \
    processor/camera.hpp \
    theme/colorizer.hpp \
    theme/theme.hpp \
    common.hpp \
    pointmapper.hpp \
    featuresprocessor.hpp \
    gesturerecognizer.hpp \
    slider.h \
    handdetection.h \
    skinfilter.h \
    skin.h \
    graph/nodo.hpp \
    database/admindb.h \
    interface/image.h

FORMS += \
    interface/block.ui \
    interface/interface.ui \
    slider.ui

DISTFILES += \
    classifiers/face.xml \
    classifiers/smile.xml \
    sounds/selected.wav \
    data/xml/nui.xml \
    data/icons/luz_ambiente.png \
    data/icons/luz_azul.png \
    data/icons/luz_entrada.png \
    data/icons/luz_roja.png \
    database/sensors.sqlite \
    data/db/sensors.sqlite \
    data/icons/closed_hand.png \
    data/icons/open_hand.png

