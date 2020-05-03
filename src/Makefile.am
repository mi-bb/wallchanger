# Copyright (C) 2019-2020 Michał Bąbik
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
                  jsfun.c jsfun.h\
                  rwdt.c rwdt.h\
                  randomm.c randomm.h\
                  wallsett.c wallsett.h\
                  settlist.c settlist.h\
                  setting.c setting.h\
                  strfun.c strfun.h\
                  cfgfile.c cfgfile.h\
                  cres.c cres.h\
                  hashfun.c hashfun.h\
                  dmfn.c dmfn.h\
                  procfn.c procfn.h\
                  procitem.c procitem.h\
                  proclist.c proclist.h\
                  cmdfn.c cmdfn.h\
                  cmdline.c cmdline.h\
                  errs.h\
                  defs.h

wchangercfg_SOURCES=wchanger.c\
                    dialogdata.c dialogdata.h\
                    treev.c treev.h\
                    dlgs.c dlgs.h\
                    iminfo.c iminfo.h\
                    wpset.c wpset.h\
                    setts.c setts.h\
                    jsfun.c jsfun.h\
                    rwdt.c rwdt.h\
                    imgs.c imgs.h\
                    randomm.c randomm.h\
                    wallsett.c wallsett.h\
                    settlist.c settlist.h\
                    setting.c setting.h\
                    preview.c preview.h\
                    strfun.c strfun.h\
                    cfgfile.c cfgfile.h\
                    fdops.c fdops.h\
                    cres.c cres.h\
                    hashfun.c hashfun.h\
                    dmfn.c dmfn.h\
                    procfn.c procfn.h\
                    procitem.c procitem.h\
                    proclist.c proclist.h\
                    cmddialog.c cmddialog.h\
                    errs.c errs.h\
                    defs.h

wchangercfg_CFLAGS = $(GTK_CFLAGS) $(JSON_CFLAGS) -DG_DISABLE_DEPRECATED
wchangercfg_LDADD = $(GTK_LIBS) $(JSON_LIBS)

wchangerd_CFLAGS = $(JSON_CFLAGS) $(X_CFLAGS)
wchangerd_LDADD = $(JSON_LIBS) -lX11

