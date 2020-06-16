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

CONFIG(release, ddebug|release) {
    DEFINES += TRACE QT_NO_DEBUG_OUTPUT
}

MOC_DIR = ./build
OBJECTS_DIR = ./build
RCC_DIR = ./build


INCLUDEPATH += ./Src\
               ./Src/Schematic\
               ./Src/Define\
               ./Src/Parser


HEADERS += ./Src/Schematic/MainWindow.h\
           ./Src/Schematic/SchematicTextItem.h\
           ./Src/Schematic/SchematicScene.h\
           ./Src/Schematic/SchematicNode.h\
           ./Src/Schematic/SchematicDevice.h\
           ./Src/Parser/CktParser.hpp\
           ./Src/Parser/MyParser.h


SOURCES += ./Src/Main/Main.cpp\
           ./Src/Schematic/MainWindow.cpp\
           ./Src/Schematic/SchematicTextItem.cpp\
           ./Src/Schematic/SchematicScene.cpp\
           ./Src/Schematic/SchematicNode.cpp\
           ./Src/Schematic/SchematicDevice.cpp\
           ./Src/Parser/CktScanner.cpp\
           ./Src/Parser/CktParser.cpp\
           ./Src/Parser/MyParser.cpp


RESOURCES += ./Src/Schematic/Schematic.qrc
