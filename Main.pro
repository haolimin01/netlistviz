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


HEADERS += ./Src/Main/MainWindow.h\
           ./Src/Schematic/SchematicTextItem.h\
           ./Src/Schematic/SchematicScene.h\
           ./Src/Schematic/SchematicDevice.h\
           ./Src/Schematic/NetlistDialog.h\
           ./Src/Schematic/SchematicWire.h\
           ./Src/Schematic/SchematicTerminal.h\
           ./Src/Schematic/ASGDialog.h\
           ./Src/Schematic/SchematicView.h\
           ./Src/Parser/CktParser.hpp\
           ./Src/Parser/MyParser.h\
           ./Src/Circuit/Node.h\
           ./Src/Circuit/Terminal.h\
           ./Src/Circuit/CircuitGraph.h\
           ./Src/Circuit/Device.h\
           ./Src/Circuit/ConnectDescriptor.h\
           ./Src/Define/Define.h\
           ./Src/Define/TypeDefine.h\
           ./Src/ASG/ASG.h\
           ./Src/ASG/MatrixElement.h\
           ./Src/ASG/Matrix.h\
           ./Src/ASG/Level.h\
           ./Src/ASG/Channel.h\
           ./Src/ASG/TablePlotter.h\
           ./Src/ASG/Wire.h\
           ./Src/Utilities/MyString.h


SOURCES += ./Src/Main/Main.cpp\
           ./Src/Main/MainWindow.cpp\
           ./Src/Schematic/SchematicTextItem.cpp\
           ./Src/Schematic/SchematicScene.cpp\
           ./Src/Schematic/SchematicDevice.cpp\
           ./Src/Schematic/NetlistDialog.cpp\
           ./Src/Schematic/SchematicWire.cpp\
           ./Src/Schematic/SchematicTerminal.cpp\
           ./Src/Schematic/ASGDialog.cpp\
           ./Src/Schematic/RenderSchematic.cpp\
           ./Src/Schematic/IOSchematic.cpp\
           ./Src/Schematic/SchematicView.cpp\
           ./Src/Parser/CktScanner.cpp\
           ./Src/Parser/CktParser.cpp\
           ./Src/Parser/MyParser.cpp\
           ./Src/Circuit/Node.cpp\
           ./Src/Circuit/Terminal.cpp\
           ./Src/Circuit/CircuitGraph.cpp\
           ./Src/Circuit/Device.cpp\
           ./Src/ASG/ASG.cpp\
           ./Src/ASG/LogicalPlacement.cpp\
           ./Src/ASG/LogicalRouting.cpp\
           ./Src/ASG/GeometricalPlacement.cpp\
           ./Src/ASG/GeometricalRouting.cpp\
           ./Src/ASG/MatrixElement.cpp\
           ./Src/ASG/Matrix.cpp\
           ./Src/ASG/Level.cpp\
           ./Src/ASG/Channel.cpp\
           ./Src/ASG/TablePlotter.cpp\
           ./Src/ASG/Wire.cpp\
           ./Src/Utilities/MyString.cpp


RESOURCES += ./Src/Schematic/Schematic.qrc
