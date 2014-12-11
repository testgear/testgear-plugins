AC_PREREQ([2.68])
AC_INIT([testgear-plugins], [0.2], [], [testgear-plugins], [http://github.com/testgear/testgear-plugins])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz no-dist-gzip -Wall -Werror])
AM_SILENT_RULES([yes])
AC_PROG_CC
AM_PROG_AR

AC_DISABLE_STATIC
AC_ENABLE_SHARED

LT_PREREQ([2.2])
LT_INIT

# Dummy plugin checks
AC_ARG_ENABLE([dummy], AS_HELP_STRING([--disable-dummy], [Disable dummy plugin]), [], [enable_dummy="check"])
if test "x$enable_dummy" != "xno"; then
    TG_PLUGINS_SELECTED="$TG_PLUGINS_SELECTED dummy"
    enable_dummy="yes"
fi

# Shell plugin checks
AC_ARG_ENABLE([shell], AS_HELP_STRING([--disable-shell], [Disable shell plugin]), [], [enable_shell="check"])
if test "x$enable_shell" != "xno"; then
    TG_PLUGINS_SELECTED="$TG_PLUGINS_SELECTED shell"
    enable_shell="yes"
fi

# Framebuffer plugin checks
AC_ARG_ENABLE([fb], AS_HELP_STRING([--disable-fb], [Disable framebuffer plugin]), [], [enable_fb="check"])
if test "x$enable_fb" != "xno"; then
    TG_PLUGINS_SELECTED="$TG_PLUGINS_SELECTED fb"
    enable_fb="yes"
fi

# Audio plugin checks
AC_ARG_ENABLE([audio], AS_HELP_STRING([--disable-audio], [Disable audio plugin]), [], [enable_audio="check"])
if test "x$enable_audio" != "xno"; then
    AC_CHECK_LIB([m], [cos], [have_libm="yes"], [AC_MSG_WARN(libm not found)])
    AC_CHECK_LIB([asound], [snd_pcm_open], [have_libasound="yes"], [AC_MSG_WARN(libasound not found)])
    AC_CHECK_LIB([audiofile], [afOpenFile], [have_libaudiofile="yes"], [AC_MSG_WARN(libaudiofile not found)])

    if test "x$have_libm" = "xyes" && test "x$have_libasound" = "xyes" && test "x$have_libaudiofile" = "xyes"; then
        TG_PLUGINS_SELECTED="$TG_PLUGINS_SELECTED audio"
        enable_audio="yes"
    else
        enable_audio="no"
    fi
fi

# CPU plugin checks
AC_ARG_ENABLE([cpu], AS_HELP_STRING([--disable-cpu], [Disable cpu plugin]), [], [enable_cpu="check"])
if test "x$enable_cpu" != "xno"; then
    AC_CHECK_LIB([pthread], [pthread_create], [have_libpthread="yes"], [AC_MSG_WARN(libpthread not found)])

    if test "x$have_libpthread" = "xyes"; then
        TG_PLUGINS_SELECTED="$TG_PLUGINS_SELECTED cpu"
        enable_cpu="yes"
    else
        enable_cpu="no"
    fi
fi

# Keyboard input plugin checks
AC_ARG_ENABLE([keyboard], AS_HELP_STRING([--disable-keyboard], [Disable keyboard plugin]), [], [enable_keyboard="check"])
if test "x$enable_keyboard" != "xno"; then
    AC_CHECK_LIB([input], [libinput_path_create_context], [have_libinput="yes"], [AC_MSG_WARN(libinput not found)])

    if test "x$have_libinput" = "xyes"; then
        TG_PLUGINS_SELECTED="$TG_PLUGINS_SELECTED keyboard"
        enable_keyboard="yes"
    else
        enable_keyboard="no"
    fi
fi

# USB plugin checks
AC_ARG_ENABLE([cpu], AS_HELP_STRING([--disable-usb], [Disable usb plugin]), [], [enable_usb="check"])
if test "x$enable_usb" != "xno"; then
    AC_CHECK_LIB([pthread], [pthread_create], [have_libpthread="yes"], [AC_MSG_WARN(libpthread not found)])
    AC_CHECK_LIB([usb], [usb_init], [have_libusb="yes"], [AC_MSG_WARN(libusb not found)])

    if test "x$have_libpthread" = "xyes" && test "x$have_libusb" = "xyes"; then
        TG_PLUGINS_SELECTED="$TG_PLUGINS_SELECTED usb"
        enable_usb="yes"
    else
        enable_usb="no"
    fi
fi

AC_CONFIG_MACRO_DIR([m4])
AC_LANG([C])
AC_PROG_INSTALL
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/dummy/Makefile])
AC_CONFIG_FILES([src/shell/Makefile])
AC_CONFIG_FILES([src/fb/Makefile])
AC_CONFIG_FILES([src/audio/Makefile])
AC_CONFIG_FILES([src/cpu/Makefile])
AC_CONFIG_FILES([src/keyboard/Makefile])
AC_CONFIG_FILES([src/usb/Makefile])
AC_SUBST(TG_PLUGINS_SELECTED)
AC_SUBST(TG_PLUGINS_ALL, "dummy shell fb audio cpu keyboard")
AC_OUTPUT

cat <<EOF

$PACKAGE-$VERSION configuration summary:

    Build dummy plugin ............... : ${enable_dummy}
    Build shell plugin ............... : ${enable_shell}
    Build fb plugin .................. : ${enable_fb}
    Build audio plugin ............... : ${enable_audio}
    Build cpu plugin ................. : ${enable_cpu}
    Build keyboard plugin ............ : ${enable_keyboard}
    Build usb plugin ................. : ${enable_usb}

EOF
