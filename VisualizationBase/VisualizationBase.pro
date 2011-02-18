ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror
INCLUDEPATH += ./headers \
    ./src \
    ./test \
    $${ENVISION_ROOT_DIR}/Core/headers \
    $${ENVISION_ROOT_DIR}
TARGET = visualizationbase
DEFINES += VISUALIZATIONBASE_LIBRARY
win32:LIBS += -L$${PLUGINS_DIR} \
    -llogger \
    -lselftest \
    -lmodelbase
QT = core \
    gui \
    xml \
    svg
TEMPLATE = lib
CONFIG += plugin \
    warn_on \
    thread
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
styles.path = $${BUILD_DIR}/styles
styles.files = styles/*
INSTALLS += target \
    pluginmeta \
    styles
HEADERS += headers/items/SingleLayout.h \
    headers/items/SingleLayoutBase.h \
    headers/items/ItemWithNode.h \
    headers/items/SymbolStyle.h \
    headers/items/Symbol.h \
    headers/items/VCharacter.h \
    headers/items/VBoolean.h \
    headers/items/VFloat.h \
    headers/icons/SVGIcon.h \
    headers/icons/SVGIconStyle.h \
    headers/items/VReference.h \
    headers/icons/IconStyle.h \
    headers/icons/Icon.h \
    headers/items/VInteger.h \
    headers/layouts/PositionLayoutStyle.h \
    headers/layouts/PositionLayout.h \
    headers/node_extensions/Position.h \
    headers/items/SelectedItemStyle.h \
    headers/items/SelectedItem.h \
    headers/items/SceneHandlerItem.h \
    headers/items/TextRenderer.h \
    headers/InteractionHandler.h \
    headers/views/MiniMap.h \
    headers/views/View.h \
    headers/views/MainView.h \
    headers/items/VListStyle.h \
    headers/items/VList.h \
    headers/items/Item.h \
    headers/items/ItemMacros.h \
    headers/items/ItemStyle.h \
    headers/items/VExtendableStyle.h \
    headers/StyleNode.h \
    headers/Styles.h \
    headers/layouts/LayoutStyle.h \
    headers/layouts/PanelBorderLayoutStyle.h \
    headers/layouts/PanelBorderLayout.h \
    headers/layouts/PanelLayoutStyle.h \
    headers/layouts/PanelLayout.h \
    headers/items/VExtendable.h \
    headers/items/VText.h \
    headers/items/TextStyle.h \
    headers/items/Text.h \
    headers/shapes/BoxStyle.h \
    headers/shapes/ShapeStyle.h \
    test/BoxTest.h \
    headers/shapes/Box.h \
    headers/shapes/Shape.h \
    headers/layouts/Layout.h \
    headers/layouts/SequentialLayoutStyle.h \
    headers/layouts/SequentialLayout.h \
    headers/ModelRenderer.h \
    headers/VisualizationManager.h \
    headers/Scene.h \
    src/visualizationbase.h \
    headers/VisualizationException.h \
    headers/visualizationbase_api.h
SOURCES += src/items/SingleLayoutBase.cpp \
    src/items/Symbol.cpp \
    src/items/SymbolStyle.cpp \
    src/items/VCharacter.cpp \
    src/items/VBoolean.cpp \
    src/items/VFloat.cpp \
    src/icons/SVGIconStyle.cpp \
    src/icons/SVGIcon.cpp \
    src/items/VReference.cpp \
    src/icons/IconStyle.cpp \
    src/icons/Icon.cpp \
    src/items/VInteger.cpp \
    src/layouts/PositionLayoutStyle.cpp \
    src/layouts/PositionLayout.cpp \
    src/node_extensions/Position.cpp \
    src/items/SelectedItemStyle.cpp \
    src/items/SelectedItem.cpp \
    src/items/SceneHandlerItem.cpp \
    src/items/TextRenderer.cpp \
    src/InteractionHandler.cpp \
    src/views/MiniMap.cpp \
    src/views/MainView.cpp \
    src/views/View.cpp \
    src/items/VListStyle.cpp \
    src/items/VList.cpp \
    src/items/Item.cpp \
    src/items/ItemStyle.cpp \
    src/items/VExtendableStyle.cpp \
    src/StyleNode.cpp \
    src/Styles.cpp \
    src/layouts/LayoutStyle.cpp \
    src/layouts/PanelBorderLayoutStyle.cpp \
    src/layouts/PanelBorderLayout.cpp \
    src/layouts/PanelLayoutStyle.cpp \
    src/layouts/PanelLayout.cpp \
    src/items/VExtendable.cpp \
    src/items/VText.cpp \
    src/items/TextStyle.cpp \
    src/items/Text.cpp \
    src/shapes/BoxStyle.cpp \
    src/shapes/ShapeStyle.cpp \
    test/BoxTest.cpp \
    src/shapes/Box.cpp \
    src/shapes/Shape.cpp \
    src/layouts/Layout.cpp \
    src/layouts/SequentialLayoutStyle.cpp \
    src/layouts/SequentialLayout.cpp \
    src/ModelRenderer.cpp \
    test/SimpleTest.cpp \
    src/VisualizationManager.cpp \
    src/visualizationbase.cpp \
    src/VisualizationException.cpp \
    src/Scene.cpp
