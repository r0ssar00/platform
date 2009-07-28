/**
 * @file libplatform.cpp
 * @short Includes all user-required components
 * @license GPLv2
 * @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 *
 * @details You must define one or more of the following to use libplatform
 * <table>
 * <tr>
 *  <td>PL_CORE</td>      <td>Includes the core classes</td>
 * </tr>
 * <tr>
 *  <td>PL_DATABASE</td>  <td>Includes the tagdb class</td>
 * </tr>
 * <tr>
 *  <td>PL_TAGS</td>      <td>Includes the tag access class</td>
 * </tr>
 * <tr>
 *  <td>PL_SEARCH</td>    <td>Includes the search class</td>
 * </tr>
 * <tr>
 *  <td>PL_SETTINGS</td>  <td>Includes the settings access class</td>
 * </tr>
 * <tr>
 *  <td>PL_DBUS</td>      <td>Includes the DBus class</td>
 * </tr>
 */
#ifdef PL_CORE
#include "core.h"
#endif
#ifdef PL_DATABASE
#include "database.h"
#endif
#ifdef PL_TAGS
#include "tags.h"
#endif
#ifdef PL_SEARCH
#include "search.h"
#endif
#ifdef PL_SETTINGS
#include "settings.h"
#endif
#ifdef PL_DBUS
#include "dbus_platform.h"
#endif