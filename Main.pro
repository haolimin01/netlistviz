#####################
# netlistviz 
#####################

TEMPLATE = app
TARGET = netlistviz

QMAKE_CXXFLAGS += -std=c++11

QT += widgets
requires(qtConfig(fontcombobox)))

INCLUDEPATH += ./Src ./Src/Schematic
# DEFINES += QT_NO_DEBUG_OUTPUT

CONFIG += debug
CONFIG -= app_bundle
MOC_DIR = ./build
OBJECTS_DIR = ./build
RCC_DIR = ./build

HEADERS += ./Src/Schematic/MainWindow.h\
           ./Src/Schematic/SchematicTextItem.h\
           ./Src/Schematic/SchematicScene.h\
           ./Src/Schematic/SchematicNode.h\
           ./Src/Schematic/SchematicDevice.h


SOURCES += ./Src/Main/Main.cpp\
           ./Src/Schematic/MainWindow.cpp\
           ./Src/Schematic/SchematicTextItem.cpp\
           ./Src/Schematic/SchematicScene.cpp\
           ./Src/Schematic/SchematicNode.cpp\
           ./Src/Schematic/SchematicDevice.cpp

RESOURCES += ./Src/Schematic/Schematic.qrc

