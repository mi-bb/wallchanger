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

bin_PROGRAMS=wchangerd wchangercfg

wchangerd_SOURCES=wcngdmn.c\
                  chkwch.c chkwch.h\
                  wpset.c wpset.h\
                  setts.c setts.h\
                  jsof.c jsof.h\
                  jsfun.c jsfun.h\
                  rwdt.c rwdt.h\
                  randomm.c randomm.h\
                  setting.c setting.h\
                  strfun.c strfun.h\
                  fdfn.c fdfn.h\
                  cfgfile.c cfgfile.h\
                  cres.c cres.h\
                  hashfun.c hashfun.h\
                  dmfn.c dmfn.h\
                  procfn.c procfn.h\
                  procitem.c procitem.h\
                  proclist.c proclist.h\
                  cmdfn.c cmdfn.h\
                  cmdline.c cmdline.h\
                  wmsfn.c wmsfn.h\
                  errs.c errs.h\
                  defs.h

wchangercfg_SOURCES=wchanger.c\
                    dialogdata.c dialogdata.h\
                    treev.c treev.h\
                    dlgs.c dlgs.h\
                    dlgcmd.c dlgcmd.h\
                    dlgothsett.c dlgothsett.h\
                    dlgsmsg.c dlgsmsg.h\
                    searchitem.c searchitem.h\
                    urldata.c urldata.h\
                    iminfo.c iminfo.h\
                    wpset.c wpset.h\
                    setts.c setts.h\
                    jsof.c jsof.h\
                    jsfun.c jsfun.h\
                    rwdt.c rwdt.h\
                    imgs.c imgs.h\
                    randomm.c randomm.h\
                    setting.c setting.h\
                    preview.c preview.h\
                    strfun.c strfun.h\
                    fdfn.c fdfn.h\
                    cfgfile.c cfgfile.h\
                    dirlist.c dirlist.h\
                    cres.c cres.h\
                    hashfun.c hashfun.h\
                    dmfn.c dmfn.h\
                    procfn.c procfn.h\
                    procitem.c procitem.h\
                    proclist.c proclist.h\
                    wmsfn.c wmsfn.h\
                    errs.c errs.h\
                    nstrings.c nstrings.h\
                    chquery.c chquery.h\
                    thumbs.c thumbs.h\
                    webwidget.c webwidget.h\
                    webwidget_c.c webwidget_c.h\
                    webpixabay.c webpixabay.h\
                    webflickr.c webflickr.h\
                    webwallhaven.c webwallhaven.h\
                    webabyss.c webabyss.h\
                    webwidget_s.h\
                    defs.h

wchangercfg_CFLAGS = $(GTK_CFLAGS) $(JSON_CFLAGS) $(CURL_CFLAGS) \
					 -DG_DISABLE_DEPRECATED
wchangercfg_LDADD = $(GTK_LIBS) $(JSON_LIBS) $(CURL_LIBS)

if FLICKCURL
wchangercfg_CFLAGS += $(FLICKCURL_CFLAGS)
wchangercfg_LDADD += $(FLICKCURL_LIBS)
if LIBXML
wchangercfg_CFLAGS += $(LIBXML_CFLAGS)
wchangercfg_LDADD += $(LIBXML_LIBS)
endif
endif

wchangerd_CFLAGS = $(JSON_CFLAGS)
wchangerd_LDADD = $(JSON_LIBS)

