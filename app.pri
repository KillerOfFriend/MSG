DESTDIR = $${BIN_PATH}

linux-g++: QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../../Build/lib.$${OS_SUFFIX}/
