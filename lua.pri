# Lua dependency for Lua4RS and the standalone JSON API runner.
#
# Supported versions: 5.4 and 5.5 (both provide _ENV, which the sandbox needs).
# Default: 5.4 if available, otherwise 5.5.
#
#   qmake LUA_VERSION=5.5        pick a version explicitly
#   qmake LUA_PKGCONFIG=<name>   use a specific pkg-config package as is
#
# Windows: set LUA_LIBS / LUA_INCLUDEPATH (default -llua from the toolchain:
# MSYS2 package mingw-w64-<arch>-lua, which is Lua 5.4). LUA_VERSION does not
# pick a library there; the compiler stops if the headers are not 5.4 or 5.5.

win32 {
    !isEmpty(LUA_VERSION):isEmpty(LUA_LIBS): \
        warning("Lua4RS: LUA_VERSION is ignored on Windows, set LUA_LIBS and LUA_INCLUDEPATH")
    isEmpty(LUA_LIBS): LUA_LIBS = -llua
    LIBS += $$LUA_LIBS
    INCLUDEPATH += $$LUA_INCLUDEPATH
} else {
    CONFIG += link_pkgconfig

    isEmpty(LUA_PKGCONFIG) {
        isEmpty(LUA_VERSION): LUA_CANDIDATES = 5.4 5.5
        else:                 LUA_CANDIDATES = $$LUA_VERSION

        for(ver, LUA_CANDIDATES) {
            !equals(ver, 5.4):!equals(ver, 5.5): \
                error("Unsupported LUA_VERSION=$$ver (supported: 5.4, 5.5)")

            # Package names differ between distributions:
            # lua5.4 (Debian/Ubuntu/Arch), lua54 (Fedora/Arch),
            # lua-5.4 (FreeBSD), lua (whatever the default Lua is).
            ver_short = $$replace(ver, \\., )
            for(pkg, $$list(lua$$ver lua$$ver_short lua-$$ver lua)) {
                isEmpty(LUA_PKGCONFIG) {
                    pkg_ver = $$system(pkg-config --modversion $$pkg 2>$$QMAKE_SYSTEM_NULL_DEVICE)
                    !isEmpty(pkg_ver):versionAtLeast(pkg_ver, $${ver}.0):versionAtMost(pkg_ver, $${ver}.99) {
                        LUA_PKGCONFIG = $$pkg
                        message("Lua4RS: using Lua $$pkg_ver (pkg-config: $$pkg)")
                    }
                }
            }
        }

        isEmpty(LUA_PKGCONFIG): error("No Lua $$join(LUA_CANDIDATES, " or ") found via pkg-config")
    }

    PKGCONFIG *= $$LUA_PKGCONFIG
}
