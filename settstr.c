#include "settstr.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[out]  ws_sett  Pointer to WallSett with all settings
 * @return      none
 */
void
free_wall_sett (WallSett *ws_sett)
{
    g_slist_free_full (ws_sett->gsl_files, g_free);
    g_free (ws_sett->s_bgcmd);
    g_free (ws_sett->s_cfgfile);
    g_free (ws_sett->s_lastused);
}
/*----------------------------------------------------------------------------*/
