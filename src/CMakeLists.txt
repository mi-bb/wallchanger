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
    defs.h
    daemon/daemon_main.c
    daemon/daemon.c                     daemon/daemon.h
    daemon/wallpaper_change.c           daemon/wallpaper_change.h
    daemon/cmd_functions.c              daemon/cmd_functions.h
    daemon/cmdline.c                    daemon/cmdline.h
    process/process.c                   process/process.h
    process/proc_item.c                 process/proc_item.h
    process/proc_list.c                 process/proc_list.h
    settings/cfg_file.c                 settings/cfg_file.h
    settings/json_file.c                settings/json_file.h
    settings/json_functions.c           settings/json_functions.h
    settings/setting_item.c             settings/setting_item.h
    settings/settings.c                 settings/settings.h
    util/cres.c                         util/cres.h
    util/errors.c                       util/errors.h
    util/file_dir_functions.c           util/file_dir_functions.h
    util/file_io.c                      util/file_io.h
    util/hash_djb2.c                    util/hash_djb2.h
    util/randomm.c                      util/randomm.h
    util/string_functions.c             util/string_functions.h
    wallpaper/wallpaper_set.c           wallpaper/wallpaper_set.h
    wallpaper/window_manager_info.c     wallpaper/window_manager_info.h)

set(WCHANGERCFG_SOURCES
    defs.h
    daemon/daemon.c                     daemon/daemon.h
    gui/config_main.c
    gui/dialog_data.c                   gui/dialog_data.h
    gui/dialogs_other.c                 gui/dialogs_other.h
    gui/dir_content.c                   gui/dir_content.h
    gui/icons.c                         gui/icons.h
    gui/image_dialogs.c                 gui/image_dialogs.h
    gui/image_info.c                    gui/image_info.h
    gui/message_dialogs.c               gui/message_dialogs.h
    gui/other_settings_dialog.c         gui/other_settings_dialog.h
    gui/preview.c                       gui/preview.h
    gui/tree_view.c                     gui/tree_view.h
    gui/wallpaper_set_command_dialog.c  gui/wallpaper_set_command_dialog.h
    process/process.c                   process/process.h
    process/proc_item.c                 process/proc_item.h
    process/proc_list.c                 process/proc_list.h
    settings/cfg_file.c                 settings/cfg_file.h
    settings/json_file.c                settings/json_file.h
    settings/json_functions.c           settings/json_functions.h
    settings/setting_item.c             settings/setting_item.h
    settings/settings.c                 settings/settings.h
    util/cres.c                         util/cres.h
    util/errors.c                       util/errors.h
    util/file_dir_functions.c           util/file_dir_functions.h
    util/file_io.c                      util/file_io.h
    util/hash_djb2.c                    util/hash_djb2.h
    util/nstrings.c                     util/nstrings.h
    util/randomm.c                      util/randomm.h
    util/string_functions.c             util/string_functions.h
    wallpaper/wallpaper_set.c           wallpaper/wallpaper_set.h
    wallpaper/window_manager_info.c     wallpaper/window_manager_info.h
    web/cache_query.c                   web/cache_query.h
    web/search_item.c                   web/search_item.h
    web/thumbnail.c                     web/thumbnail.h
    web/url_data.c                      web/url_data.h
    web/web_abyss.c                     web/web_abyss.h
    web/web_flickr.c                    web/web_flickr.h
    web/web_pixabay.c                   web/web_pixabay.h
    web/web_wallhaven.c                 web/web_wallhaven.h
    web/webwidget.c                     web/webwidget.h
    web/webwidget_common.c              web/webwidget_common.h
    web/webwidget_struct.h)

add_executable(wchangerd ${WCHANGERD_SOURCES})
target_link_libraries(wchangerd PRIVATE PkgConfig::JSONC)

add_executable(wchangercfg ${WCHANGERCFG_SOURCES})
target_compile_definitions(wchangercfg PRIVATE G_DISABLE_DEPRECATED)
target_link_libraries(wchangercfg PRIVATE PkgConfig::GTK PkgConfig::JSONC PkgConfig::CURL)

foreach(target IN ITEMS wchangerd wchangercfg)
    # ${PROJECT_BINARY_DIR} holds the generated config.h.
    target_include_directories(${target}
        PRIVATE ${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    # HAVE_CONFIG_H: src/cmdline.c/.h (gengetopt output) guards its
    # config.h include on it.
    # _GNU_SOURCE: src/image_info.c needs it for strndup(). Defining it on the
    # command line rather than in config.h also makes it immune to include
    # ordering.
    target_compile_definitions(${target} PRIVATE HAVE_CONFIG_H _GNU_SOURCE)
    if(PROCSTAT_LIBRARY)
        target_link_libraries(${target} PRIVATE ${PROCSTAT_LIBRARY})
    endif()
endforeach()
if(FLICKCURL_FOUND)
    target_link_libraries(wchangercfg PRIVATE PkgConfig::FLICKCURL)
    if(LIBXML_FOUND)
        target_link_libraries(wchangercfg PRIVATE PkgConfig::LIBXML)
    endif()
endif()

install(TARGETS wchangerd wchangercfg RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
