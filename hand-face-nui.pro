QT += widgets multimedia opengl

DIR_OPENCV_LIBS = /usr/local/lib
DIR_OTHER_LIBS = /usr/lib/x86_64-linux-gnu

unix:INCLUDEPATH += "/usr/include/GL/"

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
    graph/nodo.cpp

HEADERS += \
    graph/graph.hpp \
    graph/xmlreader.hpp \
    interface/block.hpp \
    interface/camerawidget.h \
    interface/image.hpp \
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
    graph/nodo.hpp

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
    data/icons/luz_roja.png

