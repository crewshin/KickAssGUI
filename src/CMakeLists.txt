FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)

SET(KickAssGUI_SOURCES
    argumentbuilder.cpp 
    main.cpp 
    mainwindow.cpp
    preferences.cpp
    )
SET(KickAssGUI_HEADERS 
    gcconstants.h
    argumentbuilder.h
    mainwindow.h
    preferences.h
    )
SET(KickAssGUI_UIS
    mainwindow.ui
    preferences.ui
    )

SET (RESOURCES gui_images.qrc)
QT5_ADD_RESOURCES (RCCS ${RESOURCES})

QT5_WRAP_CPP(KickAssGUI_HEADERS_MOC ${KickAssGUI_HEADERS})

QT5_WRAP_UI(KickAssGUI_UI_HEADERS ${KickAssGUI_UIS})

ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

ADD_EXECUTABLE(KickAssGUI 
    ${KickAssGUI_SOURCES}
    ${KickAssGUI_HEADERS_MOC}
    ${KickAssGUI_UI_HEADERS}
    )

qt5_use_modules(KickAssGUI Widgets)

TARGET_LINK_LIBRARIES(KickAssGUI
    ${QT_LIBRARIES}
    ${KickAssGUI_LIBRARY}
    )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
