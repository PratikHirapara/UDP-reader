# Installation is completely performed by linuxdeploy.
# It creates AppDir and populates it with files required for creating and running an appimage.
#
# The following observations have been made:
# 1) linuxdeploy uses RPATH instead of RUNPATH. Current build tools provide RUNPATH instead of RPATH.
#    This could be fixed with "add_link_options(-Wl,--disable-new-dtags)".
# 2) The regular "make install" removes all absolute paths from RUNPATH and RPATH.
# 3) linuxdeploy extracts RPATH from the executable in AppDir/usr/bin and not from the file specified by --executable.
#
# All of the above can be fixed by providing LD_LIBRARY_PATH retrieved from the original executable.
#
# Installing any files (exe, icon, desktop) via iindividual cmake commands does not provide any advantage.
# NOTE: linuxdeploy only works properly in the following cases:
#       a) AppDir is fully prepared (incl. symlinks for icon and desktop)
#       b) exe, icon and desktop files are in the current directory
#       c) all files are supplied via command line options

set(EXEFILE ${CMAKE_BINARY_DIR}/CAN_bus_interface)
set(ICONFILE ${CMAKE_SOURCE_DIR}/Deployment/CanbusInterface.png)
set(DESKTOPFILE ${CMAKE_SOURCE_DIR}/Deployment/CanbusInterface.desktop)
set(CONFIGFILE ${CMAKE_SOURCE_DIR}/config.xml)

# Copy the config.xml file to the AppDir
install(
    FILES ${CONFIGFILE}
    DESTINATION /home/ph01/.vs/CAN_bus_interface/out/build/Linux-Debug/AppDir/usr/bin
)
 
install(
    CODE "execute_process(
          COMMAND bash -c \"LD_PATH=$(readelf -d ${EXEFILE} | awk -F'[' '/RUNPATH/ {print $2}'|awk -F']' '{print $1}'); \
                            LD_LIBRARY_PATH=$LD_PATH /home/ph01/.vs/CAN_bus_interface/src/Deployment/linuxdeploy-x86_64.AppImage  --appdir ./AppDir/ --output appimage \
                                                                                  --executable ${EXEFILE} \
                                                                                  -i ${ICONFILE} \
                                                                                  -d ${DESKTOPFILE}\"
          )"
    )



