PROJECT_ROOT_PATH = $${PWD}/

CONFIG += c++11

win32: OS_SUFFIX = win32
linux-g++: OS_SUFFIX = linux

CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
    LIB_SUFFIX = d
} else {
    BUILD_FLAG = release
}

BUILD_FOLDER = $${PROJECT_ROOT_PATH}/Build/$${OS_SUFFIX}

LIBS_PATH = $${BUILD_FOLDER}/Project/lib/$${BUILD_FLAG}/
BIN_PATH = $${BUILD_FOLDER}/Project/bin/$${BUILD_FLAG}/

BUILD_PATH = $${BUILD_FOLDER}/BuildingFiles/$${TARGET}/

RCC_DIR = $${BUILD_PATH}/rcc/
UI_DIR = $${BUILD_PATH}/ui/
MOC_DIR = $${BUILD_PATH}/moc/
OBJECTS_DIR = $${BUILD_PATH}/obj/
