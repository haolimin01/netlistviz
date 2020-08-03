#####################
# netlistviz 
#####################

TEMPLATE = app
TARGET = netlistviz

QMAKE_CXXFLAGS += -std=c++11

QT += widgets
requires(qtConfig(fontcombobox)))

CONFIG += debug
# CONFIG += release

macx {
    CONFIG -= app_bundle
}

CONFIG(debug, debug|release) {
    DEFINES += TRACE DEBUG
}

CONFIG(release, debug|release) {
    DEFINES += TRACE QT_NO_DEBUG_OUTPUT
}

MOC_DIR = ./build
OBJECTS_DIR = ./build
RCC_DIR = ./build


INCLUDEPATH += ./Src\
               ./Src/Schematic\
               ./Src/Define\
               ./Src/Parser\
               ./Src/Circuit\
               ./Src/ASG\
               ./Src/Utilities


HEADERS += ./Src/Schematic/MainWindow.h\
           ./Src/Schematic/SchematicTextItem.h\
           ./Src/Schematic/SchematicScene.h\
           ./Src/Schematic/SchematicDevice.h\
           ./Src/Schematic/NetlistDialog.h\
           ./Src/Schematic/SchematicData.h\
           ./Src/Schematic/SchematicWire.h\
           ./Src/Parser/CktParser.hpp\
           ./Src/Parser/MyParser.h\
           ./Src/Circuit/CktNode.h\
           ./Src/ASG/ASG.h\
           ./Src/ASG/MatrixElement.h\
           ./Src/ASG/Matrix.h\
           ./Src/ASG/TablePlotter.h\
           ./Src/Utilities/MyString.h


SOURCES += ./Src/Main/Main.cpp\
           ./Src/Schematic/MainWindow.cpp\
           ./Src/Schematic/SchematicTextItem.cpp\
           ./Src/Schematic/SchematicScene.cpp\
           ./Src/Schematic/SchematicDevice.cpp\
           ./Src/Schematic/NetlistDialog.cpp\
           ./Src/Schematic/SchematicData.cpp\
           ./Src/Schematic/SchematicWire.cpp\
           ./Src/Parser/CktScanner.cpp\
           ./Src/Parser/CktParser.cpp\
           ./Src/Parser/MyParser.cpp\
           ./Src/Circuit/CktNode.cpp\
           ./Src/ASG/ASG.cpp\
           ./Src/ASG/MatrixElement.cpp\
           ./Src/ASG/Matrix.cpp\
           ./Src/ASG/TablePlotter.cpp\
           ./Src/Utilities/MyString.cpp


RESOURCES += ./Src/Schematic/Schematic.qrc
RC_ICONS = ./Src/Main/netlistviz.ico
