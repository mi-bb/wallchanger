# Copyright (C) 2019-2026 Michał Bąbik
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

set(WCHANGERD_SOURCES
    wcngdmn.c
    chkwch.c    chkwch.h
    wpset.c     wpset.h
    setts.c     setts.h
    jsof.c      jsof.h
    jsfun.c     jsfun.h
    rwdt.c      rwdt.h
    randomm.c   randomm.h
    setting.c   setting.h
    strfun.c    strfun.h
    fdfn.c      fdfn.h
    cfgfile.c   cfgfile.h
    cres.c      cres.h
    hashfun.c   hashfun.h
    dmfn.c      dmfn.h
    procfn.c    procfn.h
    procitem.c  procitem.h
    proclist.c  proclist.h
    cmdfn.c     cmdfn.h
    cmdline.c   cmdline.h
    wmsfn.c     wmsfn.h
    errs.c      errs.h
    defs.h)

set(WCHANGERCFG_SOURCES
    wchanger.c
    dialogdata.c   dialogdata.h
    treev.c        treev.h
    dlgs.c         dlgs.h
    dlgcmd.c       dlgcmd.h
    dlgothsett.c   dlgothsett.h
    dlgsmsg.c      dlgsmsg.h
    searchitem.c   searchitem.h
    urldata.c      urldata.h
    iminfo.c       iminfo.h
    wpset.c        wpset.h
    setts.c        setts.h
    jsof.c         jsof.h
    jsfun.c        jsfun.h
    rwdt.c         rwdt.h
    imgs.c         imgs.h
    randomm.c      randomm.h
    setting.c      setting.h
    preview.c      preview.h
    strfun.c       strfun.h
    fdfn.c         fdfn.h
    cfgfile.c      cfgfile.h
    dirlist.c      dirlist.h
    cres.c         cres.h
    hashfun.c      hashfun.h
    dmfn.c         dmfn.h
    procfn.c       procfn.h
    procitem.c     procitem.h
    proclist.c     proclist.h
    wmsfn.c        wmsfn.h
    errs.c         errs.h
    nstrings.c     nstrings.h
    chquery.c      chquery.h
    thumbs.c       thumbs.h
    webwidget.c    webwidget.h
    webwidget_c.c  webwidget_c.h
    webpixabay.c   webpixabay.h
    webflickr.c    webflickr.h
    webwallhaven.c webwallhaven.h
    webabyss.c     webabyss.h
    webwidget_s.h
    defs.h)

add_executable(wchangerd ${WCHANGERD_SOURCES})
target_link_libraries(wchangerd PRIVATE PkgConfig::JSONC)
if(HAVE_LIBPROCSTAT AND PROCSTAT_LIBRARY)
    target_link_libraries(wchangerd PRIVATE ${PROCSTAT_LIBRARY})
endif()

add_executable(wchangercfg ${WCHANGERCFG_SOURCES})
target_compile_definitions(wchangercfg PRIVATE G_DISABLE_DEPRECATED)
target_link_libraries(wchangercfg PRIVATE PkgConfig::GTK PkgConfig::JSONC PkgConfig::CURL)
if(HAVE_LIBPROCSTAT AND PROCSTAT_LIBRARY)
    target_link_libraries(wchangercfg PRIVATE ${PROCSTAT_LIBRARY})
endif()
if(FLICKCURL_FOUND)
    target_link_libraries(wchangercfg PRIVATE PkgConfig::FLICKCURL)
    if(LIBXML_FOUND)
        target_link_libraries(wchangercfg PRIVATE PkgConfig::LIBXML)
    endif()
endif()

install(TARGETS wchangerd wchangercfg RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
