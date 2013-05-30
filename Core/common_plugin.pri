include(common.pri)

win32:LIBS += -L$${PLUGINS_DIR}
TEMPLATE = lib
CONFIG += plugin

target.path = $$PLUGINS_DIR
pluginmeta.path = $$PLUGINS_DIR
pluginmeta.files = $${TARGET}.plugin
styles.path = $${BUILD_DIR}/styles
styles.files = styles/*
INSTALLS += target pluginmeta styles

LIBS += -L$${BUILD_DIR} -lcore