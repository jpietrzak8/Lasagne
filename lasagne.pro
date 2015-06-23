# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
#DEPLOYMENTFOLDERS = # file1 dir1
audioFiles.source = audio/beep.wav
audioFiles.target = audio
DEPLOYMENTFOLDERS = audioFiles

#symbian:TARGET.UID3 = 0xE1E341BF

# Smart Installer package's UID
# This UID is from the protected range 
# and therefore the package will fail to install if self-signed
# By default qmake uses the unprotected range value if unprotected UID is defined for the application
# and 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
#symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

QT += maemo5 dbus

SOURCES += main.cpp mainwindow.cpp \
    lasinputdialog.cpp \
    lassheetdelegate.cpp \
    lassheet.cpp \
    lascameracontrol.cpp \
    lasbarcodeviewform.cpp \
    lasoptionsdialog.cpp \
    lasmaemo5theme.cpp \
    lascsvmodel.cpp \
    lasaboutform.cpp \
    lasnewentrydialog.cpp \
    lasdocumentationform.cpp \
    laspreferencesform.cpp \
    lasdbus.cpp
HEADERS += mainwindow.h \
    lasinputdialog.h \
    lassheetdelegate.h \
    lassheet.h \
    lasbarcodeviewform.h \
    lascameracontrol.h \
    lasoptionsdialog.h \
    lasmaemo5theme.h \
    lascsvmodel.h \
    lasaboutform.h \
    lasnewentrydialog.h \
    lasdocumentationform.h \
    laspreferencesform.h \
    lasdbus.h
FORMS += mainwindow.ui \
    lasinputdialog.ui \
    lasbarcodeviewform.ui \
    lasoptionsdialog.ui \
    lasaboutform.ui \
    lasnewentrydialog.ui \
    lasdocumentationform.ui \
    laspreferencesform.ui

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    README.md \
    LICENSE.md \
    doc/about.html \
    doc/documentation.html

unix {
  CONFIG += link_pkgconfig
  PKGCONFIG += gstreamer-0.10 gstreamer-interfaces-0.10 zbar
  LIBS += -lgstapp-0.10
}

RESOURCES += \
    lasagne.qrc
