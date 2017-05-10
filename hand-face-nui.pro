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
#             -lopencv_video \
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

LIBS += "$$DIR_OTHER_LIBS/libespeak.so"

SOURCES += \
    main.cpp \
    graph/graph.cpp \
    graph/node.cpp \
    graph/xmlreader.cpp \
    interface/block.cpp \
    interface/camerawidget.cpp \
    interface/image.cpp \
    interface/interface.cpp \
    processor/camera.cpp \
    theme/colorizer.cpp \
    theme/theme.cpp \
    speech.cpp

HEADERS += \
    graph/graph.hpp \
    graph/node.hpp \
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
    speech.hpp

FORMS += \
    interface/block.ui \
    interface/interface.ui

