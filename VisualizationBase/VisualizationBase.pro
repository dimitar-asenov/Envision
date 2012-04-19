ENVISION_ROOT_DIR = $$PWD/..
CONFIG(release, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/ReleaseBuild
CONFIG(debug, debug|release):BUILD_DIR = $${ENVISION_ROOT_DIR}/DebugBuild
PLUGINS_DIR = $${BUILD_DIR}/plugins
CONFIG(debug, debug|release):DEFINES += DEBUG
QMAKE_CXXFLAGS += -Werror \
    -std=c++0x
INCLUDEPATH += ./src \
    ./test \
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
    thread \
    precompile_header
target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
styles.path = $${BUILD_DIR}/styles
styles.files = styles/*
INSTALLS += target \
    pluginmeta \
    styles
PRECOMPILED_HEADER = src/precompiled.h
HEADERS += src/items/RootItem.h \
    src/renderer/ModelRenderer.h \
    src/renderer/VisualizationGroup.h \
    src/renderer/VisualizationSuitabilityScore.h \
    src/CustomSceneEvent.h \
    src/InteractionHandler.h \
    src/Scene.h \
    src/VisualizationException.h \
    src/VisualizationManager.h \
    src/cursor/Cursor.h \
    src/cursor/CursorShapeItem.h \
    src/cursor/LayoutCursor.h \
    src/cursor/TextCursor.h \
    src/icons/Icon.h \
    src/icons/IconStyle.h \
    src/icons/SVGIcon.h \
    src/icons/SVGIconStyle.h \
    src/items/Item.h \
    src/items/ItemMacros.h \
    src/items/ItemRegion.h \
    src/items/ItemStyle.h \
    src/items/ItemWithNode.h \
    src/items/LayoutProvider.h \
    src/items/LayoutProviderBase.h \
    src/items/SceneHandlerItem.h \
    src/items/SelectedItem.h \
    src/items/Static.h \
    src/items/StaticStyle.h \
    src/items/Symbol.h \
    src/items/SymbolStyle.h \
    src/items/Text.h \
    src/items/TextRenderer.h \
    src/items/TextStyle.h \
    src/items/VBoolean.h \
    src/items/VCharacter.h \
    src/items/VExtendable.h \
    src/items/VExtendableStyle.h \
    src/items/VFloat.h \
    src/items/VInteger.h \
    src/items/VList.h \
    src/items/VListStyle.h \
    src/items/VReference.h \
    src/items/VText.h \
    src/layouts/GridLayout.h \
    src/layouts/GridLayoutStyle.h \
    src/layouts/Layout.h \
    src/layouts/LayoutStyle.h \
    src/layouts/PanelBorderLayout.h \
    src/layouts/PanelBorderLayoutStyle.h \
    src/layouts/PanelLayout.h \
    src/layouts/PanelLayoutStyle.h \
    src/layouts/PositionLayout.h \
    src/layouts/PositionLayoutStyle.h \
    src/layouts/SequentialLayout.h \
    src/layouts/SequentialLayoutStyle.h \
    src/node_extensions/Position.h \
    src/precompiled.h \
    src/shapes/Box.h \
    src/shapes/BoxStyle.h \
    src/shapes/Braces.h \
    src/shapes/BracesStyle.h \
    src/shapes/Diamond.h \
    src/shapes/DiamondStyle.h \
    src/shapes/Shape.h \
    src/shapes/ShapeStyle.h \
    src/shapes/shapeMacros.h \
    src/styles/Style.h \
    src/styles/StyleLoader.h \
    src/styles/StyleNode.h \
    src/styles/StyleSet.h \
    src/views/MainView.h \
    src/views/MiniMap.h \
    src/views/View.h \
    src/visualizationbase_api.h \
    test/BoxTest.h \
    src/visualizationbase.h
SOURCES += src/items/RootItem.cpp \
    src/renderer/ModelRenderer.cpp \
    src/renderer/VisualizationGroup.cpp \
    src/renderer/VisualizationSuitabilityScore.cpp \
    src/items/ItemRegion.cpp \
    src/CustomSceneEvent.cpp \
    src/cursor/LayoutCursor.cpp \
    src/cursor/TextCursor.cpp \
    src/cursor/CursorShapeItem.cpp \
    src/cursor/Cursor.cpp \
    src/items/Static.cpp \
    src/items/StaticStyle.cpp \
    src/styles/Style.cpp \
    src/styles/StyleNode.cpp \
    src/styles/StyleLoader.cpp \
    src/shapes/Braces.cpp \
    src/shapes/BracesStyle.cpp \
    src/layouts/GridLayout.cpp \
    src/layouts/GridLayoutStyle.cpp \
    src/shapes/Diamond.cpp \
    src/shapes/DiamondStyle.cpp \
    src/items/LayoutProviderBase.cpp \
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
    test/SimpleTest.cpp \
    src/VisualizationManager.cpp \
    src/visualizationbase.cpp \
    src/VisualizationException.cpp \
    src/Scene.cpp
