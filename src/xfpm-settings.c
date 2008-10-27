/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
 *
 * * Copyright (C) 2008 Ali <aliov@xfce.org>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <gtk/gtk.h>

#include <xfconf/xfconf.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfcegui4/libxfcegui4.h>

#include "xfpm-settings.h"
#include "xfpm-enums.h"
#include "xfpm-common.h"
#include "xfpm-spin-button.h"

#ifdef HAVE_DPMS
#include "xfpm-dpms-spins.h"
#endif


/// Global Variable ///
static GtkWidget *nt;

static GtkWidget *cpu_gov;
static GtkWidget *performance_on_ac;
static GtkWidget *ondemand_on_ac;
static GtkWidget *powersave_on_ac;
static GtkWidget *conservative_on_ac;
static GtkWidget *userspace_on_ac;

static GtkWidget *performance_on_batt;
static GtkWidget *ondemand_on_batt;
static GtkWidget *powersave_on_batt;
static GtkWidget *conservative_on_batt;
static GtkWidget *userspace_on_batt;

#ifdef HAVE_DPMS
static GtkWidget *dpms_op;
static GtkWidget *on_batt_dpms;
static GtkWidget *on_ac_dpms;
#endif

/// Callback Setting Functions ///
static void
set_show_tray_icon_cb(GtkWidget *widget,XfconfChannel *channel)
{
    gboolean value = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    
    if(!xfconf_channel_set_uint(channel,SHOW_TRAY_ICON_CFG,value))
    {
        g_critical("Cannot set value %s\n",SHOW_TRAY_ICON_CFG);
    }
}

static void
set_battery_critical_charge_cb(GtkWidget *widget,XfconfChannel *channel)
{
    guint value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    
    if(!xfconf_channel_set_uint(channel,CRITICAL_BATT_CFG,value))
    {
        g_critical("Cannot set value %s\n",CRITICAL_BATT_CFG);
    }
}

static void
set_critical_action_cb(GtkWidget *widget,XfconfChannel *channel)
{
    guint value = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    
    if(!xfconf_channel_set_uint(channel,CRITICAL_BATT_ACTION_CFG,value))
    {
        g_critical("Cannot set value %s\n",CRITICAL_BATT_ACTION_CFG);
    }
}

static void
set_power_button_action_cb(GtkWidget *widget,XfconfChannel *channel)
{
    guint value = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    
    if(!xfconf_channel_set_uint(channel,POWER_SWITCH_CFG,value))
    {
        g_critical("Cannot set value %s\n",POWER_SWITCH_CFG);
    }
}

static void
set_sleep_button_action_cb(GtkWidget *widget,XfconfChannel *channel)
{
    guint value = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    
    if(!xfconf_channel_set_uint(channel,SLEEP_SWITCH_CFG,value))
    {
        g_critical("Cannot set value %s\n",SLEEP_SWITCH_CFG);
    }
}

static void
set_lid_button_action_cb(GtkWidget *widget,XfconfChannel *channel)
{
    guint value = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    
    if(!xfconf_channel_set_uint(channel,LID_SWITCH_CFG,value))
    {
        g_critical("Cannot set value %s\n",LID_SWITCH_CFG);
    }
}

static void
set_power_save_cb(GtkWidget *widget,XfconfChannel *channel)
{
    gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    
    if(!xfconf_channel_set_bool(channel,POWER_SAVE_CFG,value))
    {
        g_critical("Cannot set value %s\n",POWER_SAVE_CFG);
    }
}

#ifdef HAVE_LIBNOTIFY
static void
set_battery_state_notification_cb(GtkWidget *widget,XfconfChannel *channel)
{
    gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    
    if(!xfconf_channel_set_bool(channel,BATT_STATE_NOTIFICATION_CFG,value))
    {
        g_critical("Cannot set value %s\n",BATT_STATE_NOTIFICATION_CFG);
    }
} 
#endif

static void
set_lcd_brightness_cb(GtkWidget *widget,XfconfChannel *channel)
{
    gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    
    if(!xfconf_channel_set_bool(channel,LCD_BRIGHTNESS_CFG,value))
    {
        g_critical("Cannot set value %s\n",LCD_BRIGHTNESS_CFG);
    }
}

#ifdef HAVE_DPMS
static void
set_dpms_cb(GtkWidget *widget,XfconfChannel *channel)
{
    gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    
    if(!xfconf_channel_set_bool(channel,DPMS_ENABLE_CFG,value))
    {
        g_critical("Cannot set value %s\n",DPMS_ENABLE_CFG);
    }
    
    xfpm_dpms_spins_set_active(XFPM_DPMS_SPINS(on_ac_dpms),value); 
    
    if ( GTK_IS_WIDGET(on_batt_dpms) )
    {
        xfpm_dpms_spins_set_active(XFPM_DPMS_SPINS(on_batt_dpms),value); 
    }
}

static void
set_dpms_on_battery_cb(GtkWidget *spin,guint value1,guint value2,
                      guint value3,XfconfChannel *channel)
{
    GPtrArray *arr = g_ptr_array_sized_new(3);
    GValue *val;
    
    val = g_new0(GValue, 1);
    g_value_init(val, G_TYPE_UINT);
    g_value_set_uint(val, value1);
    g_ptr_array_add(arr, val);
    
    val = g_new0(GValue, 1);
    g_value_init(val, G_TYPE_UINT);
    g_value_set_uint(val, value2);
    g_ptr_array_add(arr, val);
    
    val = g_new0(GValue, 1);
    g_value_init(val, G_TYPE_UINT);
    g_value_set_uint(val, value3);
    g_ptr_array_add(arr, val);
    
    if (!xfconf_channel_set_arrayv(channel,
                                  ON_BATT_DPMS_TIMEOUTS_CFG,
                                  arr) )
    {                              
        g_critical("Cannot set value for %s \n",ON_BATT_DPMS_TIMEOUTS_CFG);
        xfconf_array_free(arr);
        return;
    }
    xfconf_array_free(arr);
}

static void
set_dpms_on_ac_cb(GtkWidget *spin,guint value1,guint value2,
                  guint value3,XfconfChannel *channel)
{
    GPtrArray *arr = g_ptr_array_sized_new(3);
    GValue *val;
    
    val = g_new0(GValue, 1);
    g_value_init(val, G_TYPE_UINT);
    g_value_set_uint(val, value1);
    g_ptr_array_add(arr, val);
    
    val = g_new0(GValue, 1);
    g_value_init(val, G_TYPE_UINT);
    g_value_set_uint(val, value2);
    g_ptr_array_add(arr, val);
    
    val = g_new0(GValue, 1);
    g_value_init(val, G_TYPE_UINT);
    g_value_set_uint(val, value3);
    g_ptr_array_add(arr, val);
    
    if (!xfconf_channel_set_arrayv(channel,
                                  ON_AC_DPMS_TIMEOUTS_CFG,
                                  arr) )
    {                              
        g_critical("Cannot set value %s \n",ON_AC_DPMS_TIMEOUTS_CFG);
        xfconf_array_free(arr);
        return;
    }
    xfconf_array_free(arr);
}
#endif

static void
set_cpu_freq_scaling_cb(GtkWidget *widget,XfconfChannel *channel)
{
    gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    
    if(!xfconf_channel_set_bool(channel,CPU_FREQ_SCALING_CFG,value))
    {
        g_critical("Cannot set value %s\n",CPU_FREQ_SCALING_CFG);
    }
    if ( GTK_IS_WIDGET(ondemand_on_ac) )
    {
        gtk_widget_set_sensitive(ondemand_on_ac,value);
        gtk_widget_set_sensitive(performance_on_ac,value);
        gtk_widget_set_sensitive(powersave_on_ac,value);
        gtk_widget_set_sensitive(conservative_on_ac,value);
        gtk_widget_set_sensitive(userspace_on_ac,value);
    }
    
    if ( GTK_IS_WIDGET(ondemand_on_batt) )  /* enough to check only one widget */ 
    {
        gtk_widget_set_sensitive(ondemand_on_batt,value);
        gtk_widget_set_sensitive(performance_on_batt,value);
        gtk_widget_set_sensitive(powersave_on_batt,value);
        gtk_widget_set_sensitive(conservative_on_batt,value);
        gtk_widget_set_sensitive(userspace_on_batt,value);
    }
}

static void
set_powersave_on_ac_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_AC_CPU_GOV_CFG,POWERSAVE))
    {
        g_critical("Cannot set value %s\n",ON_AC_CPU_GOV_CFG);
    }

}

static void
set_ondemand_on_ac_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_AC_CPU_GOV_CFG,ONDEMAND))
    {
        g_critical("Cannot set value %s\n",ON_AC_CPU_GOV_CFG);
    }

}

static void
set_performance_on_ac_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_AC_CPU_GOV_CFG,PERFORMANCE))
    {
        g_critical("Cannot set value %s\n",ON_AC_CPU_GOV_CFG);
    }

}

static void
set_conservative_on_ac_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_AC_CPU_GOV_CFG,CONSERVATIVE))
    {
        g_critical("Cannot set value %s\n",ON_AC_CPU_GOV_CFG);
    }

}

static void
set_userspace_on_ac_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_AC_CPU_GOV_CFG,USERSPACE))
    {
        g_critical("Cannot set value %s\n",ON_AC_CPU_GOV_CFG);
    }
}

static void
set_powersave_on_batt_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_BATT_CPU_GOV_CFG,POWERSAVE))
    {
        g_critical("Cannot set value %s\n",ON_BATT_CPU_GOV_CFG);
    }

}

static void
set_ondemand_on_batt_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_BATT_CPU_GOV_CFG,ONDEMAND))
    {
        g_critical("Cannot set value %s\n",ON_BATT_CPU_GOV_CFG);
    }

}

static void
set_performance_on_batt_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_BATT_CPU_GOV_CFG,PERFORMANCE))
    {
        g_critical("Cannot set value %s\n",ON_BATT_CPU_GOV_CFG);
    }

}

static void
set_conservative_on_batt_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_BATT_CPU_GOV_CFG,CONSERVATIVE))
    {
        g_critical("Cannot set value %s\n",ON_BATT_CPU_GOV_CFG);
    }

}
static void
set_userspace_on_batt_cb(GtkWidget *widget,XfconfChannel *channel)
{
    if(!xfconf_channel_set_uint(channel,ON_BATT_CPU_GOV_CFG,USERSPACE))
    {
        g_critical("Cannot set value %s\n",ON_BATT_CPU_GOV_CFG);
    }
}
/// End of Callback Setting Functions ///

static XfpmActionRequest
_combo_helper_function(XfconfChannel *channel,const gchar *config,
                       guint8 power_management,XfpmActionRequest action)
{
    if( power_management == 0 && action != XFPM_DO_NOTHING ) 
    {
        xfconf_channel_set_uint(channel,config,XFPM_DO_NOTHING);
        return XFPM_DO_NOTHING;
    }
    
    if( !(power_management & SYSTEM_CAN_SUSPEND)  && action == XFPM_DO_SUSPEND ) 
    {
        xfconf_channel_set_uint(channel,config,XFPM_DO_NOTHING);
        return XFPM_DO_NOTHING;
    }
    
    if( !(power_management & SYSTEM_CAN_HIBERNATE) && action == XFPM_DO_HIBERNATE )  
    {
        xfconf_channel_set_uint(channel,config,XFPM_DO_NOTHING);
        return XFPM_DO_NOTHING;
    }
    
    if ( action < XFPM_DO_NOTHING || action > XFPM_DO_SHUTDOWN )
    {
        xfconf_channel_set_uint(channel,config,XFPM_DO_NOTHING);
        return XFPM_DO_NOTHING;
    }
    return action;
}

/// Settings frames ///
static GtkWidget *
xfpm_settings_battery(XfconfChannel *channel, guint8 power_management,gboolean ups_found)
{
    GtkWidget *table;
    GtkWidget *frame,*align;
    GtkWidget *label;
    GtkWidget *critical_spin;
    GtkWidget *action;
    
    table = gtk_table_new(4,2,TRUE);
    gtk_widget_show(table);
    frame = xfce_create_framebox(ups_found ? _("UPS configuration") :_("Battery configuration"), &align);
    gtk_widget_show(frame);
    gtk_container_set_border_width(GTK_CONTAINER(frame),BORDER);
    
    label = gtk_label_new(ups_found ? _("Consider UPS charge critical"):_("Consider battery charge critical"));
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,0,1);
    
    critical_spin = xfpm_spin_button_new_with_range(1,20,1);
	gchar *suffix = g_strdup_printf(" %s",_("percent"));
    xfpm_spin_button_set_suffix(XFPM_SPIN_BUTTON(critical_spin),suffix);
	g_free(suffix);
    gtk_widget_show(critical_spin);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(critical_spin),
                              xfconf_channel_get_uint(channel,CRITICAL_BATT_CFG,10));
    g_signal_connect(critical_spin,"value-changed",
                    G_CALLBACK(set_battery_critical_charge_cb),channel);
    gtk_table_attach(GTK_TABLE(table),critical_spin,1,2,0,1,GTK_SHRINK,GTK_SHRINK,0,0);
    
    label = gtk_label_new(_("When battery charge level is critical do"));
    gtk_widget_show(label);
    if ( !ups_found )
        gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,1,2);
        
    action = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(action),_("Nothing"));
    if ( power_management != 0 )
    {
        gtk_combo_box_append_text(GTK_COMBO_BOX(action),_("Shutdown"));
    }
    if ( power_management & SYSTEM_CAN_HIBERNATE )
    {
        gtk_combo_box_append_text(GTK_COMBO_BOX(action),_("Hibernate"));
    }                           
    guint active = xfconf_channel_get_uint(channel,CRITICAL_BATT_ACTION_CFG,XFPM_DO_NOTHING);
    active = _combo_helper_function(channel,CRITICAL_BATT_ACTION_CFG,power_management,active);

    gtk_combo_box_set_active(GTK_COMBO_BOX(action),active);
    
    gtk_widget_show(action);
    g_signal_connect(action,"changed",G_CALLBACK(set_critical_action_cb),channel);
    
    !ups_found ? gtk_table_attach(GTK_TABLE(table),action,1,2,1,2,GTK_SHRINK,GTK_SHRINK,0,0) : gtk_widget_hide(action);

#ifdef HAVE_LIBNOTIFY
    GtkWidget *notify_bt;        
    label = gtk_label_new(ups_found ? _("Enable UPS charge notification") :_("Enable battery state notification"));
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,2,3);
    notify_bt = gtk_check_button_new();
    gtk_widget_show(notify_bt);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(notify_bt),
                                 xfconf_channel_get_bool(channel,BATT_STATE_NOTIFICATION_CFG,TRUE)); 
    g_signal_connect(notify_bt,"toggled",G_CALLBACK(set_battery_state_notification_cb),channel); 
    gtk_table_attach(GTK_TABLE(table),notify_bt,1,2,2,3,GTK_SHRINK,GTK_SHRINK,0,0);                             
#endif        
	if ( power_management & SYSTEM_CAN_POWER_SAVE )
	{
		// Power Save Profile
		GtkWidget *power_save;        
		label = gtk_label_new(ups_found ? _("Enable power save on UPS power") : _("Enable power save on battery power"));
		gtk_widget_show(label);
		gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,2,3);
		power_save = gtk_check_button_new();
		gtk_widget_show(power_save);
		
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(power_save),
									 power_management & SYSTEM_CAN_POWER_SAVE ?
									 xfconf_channel_get_bool(channel,POWER_SAVE_CFG,FALSE)
									 : FALSE);
									  
		g_signal_connect(power_save,"toggled",G_CALLBACK(set_power_save_cb),channel); 
		gtk_table_attach(GTK_TABLE(table),power_save,1,2,2,3,GTK_SHRINK,GTK_SHRINK,0,0); 
	}
	
    gtk_container_add(GTK_CONTAINER(align),table);
    return frame;
    
}

static GtkWidget *
xfpm_settings_cpu_on_ac_adapter(XfconfChannel *channel,guint8 govs,const gchar *label)
{
    GtkWidget *frame;
    GtkWidget *align;
    GtkWidget *vbox;
    
    GSList *list;
    guint current_governor = xfconf_channel_get_uint(channel,ON_AC_CPU_GOV_CFG,ONDEMAND);
    gboolean enable = xfconf_channel_get_bool(channel,CPU_FREQ_SCALING_CFG,TRUE);
    
    frame = xfce_create_framebox(label, &align);
    gtk_container_set_border_width(GTK_CONTAINER(frame),BORDER);
    gtk_widget_show(frame);
    
    vbox = gtk_vbox_new(FALSE,BORDER);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(align),vbox);
    
    performance_on_ac = gtk_radio_button_new_with_label(NULL,_("Performance"));
    gtk_box_pack_start (GTK_BOX (vbox), performance_on_ac, FALSE, FALSE, 0);
    
    list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(performance_on_ac));
    
    ondemand_on_ac = gtk_radio_button_new_with_label(list,_("Ondemand"));
    gtk_box_pack_start (GTK_BOX (vbox), ondemand_on_ac, FALSE, FALSE, 0);
    list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(ondemand_on_ac));
    
    userspace_on_ac = gtk_radio_button_new_with_label(list,_("Userspace"));
    gtk_box_pack_start (GTK_BOX (vbox), userspace_on_ac, FALSE, FALSE, 0);
    list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(userspace_on_ac));
    
    powersave_on_ac = gtk_radio_button_new_with_label(list,_("Powersave"));
    gtk_box_pack_start (GTK_BOX (vbox), powersave_on_ac, FALSE, FALSE, 0); 
    list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(powersave_on_ac));
    
    conservative_on_ac = gtk_radio_button_new_with_label(list,_("Conservative"));
    gtk_box_pack_start (GTK_BOX (vbox), conservative_on_ac, FALSE, FALSE, 0); 
    
    if ( govs & POWERSAVE ) 
    {
        gtk_widget_set_sensitive(powersave_on_ac,enable);
        g_signal_connect(powersave_on_ac,"pressed",G_CALLBACK(set_powersave_on_ac_cb),channel);
        gtk_widget_show(powersave_on_ac);
        if ( current_governor == POWERSAVE )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(powersave_on_ac),TRUE);
        }
    }
    if ( govs & ONDEMAND ) 
    {
        gtk_widget_set_sensitive(ondemand_on_ac,enable);
        g_signal_connect(ondemand_on_ac,"pressed",G_CALLBACK(set_ondemand_on_ac_cb),channel);
        gtk_widget_show(ondemand_on_ac);
        if ( current_governor == ONDEMAND )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ondemand_on_ac),TRUE);
        }
    }
    if ( govs & PERFORMANCE ) 
    {
        gtk_widget_set_sensitive(performance_on_ac,enable);
        g_signal_connect(performance_on_ac,"pressed",G_CALLBACK(set_performance_on_ac_cb),channel);
        gtk_widget_show(performance_on_ac);
        if ( current_governor == PERFORMANCE )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(performance_on_ac),TRUE);
        }
    }
    if ( govs & CONSERVATIVE ) 
    {
        gtk_widget_set_sensitive(conservative_on_ac,enable);
        g_signal_connect(conservative_on_ac,"pressed",G_CALLBACK(set_conservative_on_ac_cb),channel);
        gtk_widget_show(conservative_on_ac);
        if ( current_governor == CONSERVATIVE )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(conservative_on_ac),TRUE);
        }
    }
    if ( govs & USERSPACE ) 
    {
        gtk_widget_set_sensitive(userspace_on_ac,enable);
        g_signal_connect(userspace_on_ac,"pressed",G_CALLBACK(set_userspace_on_ac_cb),channel);
        gtk_widget_show(userspace_on_ac);
        if ( current_governor == USERSPACE )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(userspace_on_ac),TRUE);
        }
    }
    
    return frame;
}

static GtkWidget *
xfpm_settings_cpu_on_battery_power(XfconfChannel *channel,guint8 govs,gboolean ups)
{
    GtkWidget *frame;
    GtkWidget *align;
    GtkWidget *vbox;
    
    GSList *list;
    guint current_governor = xfconf_channel_get_uint(channel,ON_BATT_CPU_GOV_CFG,POWERSAVE);
    gboolean enable = xfconf_channel_get_bool(channel,CPU_FREQ_SCALING_CFG,TRUE);
    
    frame = xfce_create_framebox(ups ? _("CPU governor settings on UPS power") : _("CPU governor settings on battery power"), &align);
    gtk_container_set_border_width(GTK_CONTAINER(frame),BORDER);
    gtk_widget_show(frame);
    
    vbox = gtk_vbox_new(FALSE,BORDER);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(align),vbox);
    
    performance_on_batt = gtk_radio_button_new_with_label(NULL,_("Performance"));
    gtk_box_pack_start (GTK_BOX (vbox), performance_on_batt, FALSE, FALSE, 0);
    
    list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(performance_on_batt));
    
    ondemand_on_batt = gtk_radio_button_new_with_label(list,_("Ondemand"));
    gtk_box_pack_start (GTK_BOX (vbox), ondemand_on_batt, FALSE, FALSE, 0);
    list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(ondemand_on_batt));
    
    userspace_on_batt = gtk_radio_button_new_with_label(list,_("Userspace"));
    gtk_box_pack_start (GTK_BOX (vbox), userspace_on_batt, FALSE, FALSE, 0);
    list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(userspace_on_batt));
    
    powersave_on_batt = gtk_radio_button_new_with_label(list,_("Powersave"));
    gtk_box_pack_start (GTK_BOX (vbox), powersave_on_batt, FALSE, FALSE, 0); 
    list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(powersave_on_batt));
    
    conservative_on_batt = gtk_radio_button_new_with_label(list,_("Conservative"));
    gtk_box_pack_start (GTK_BOX (vbox), conservative_on_batt, FALSE, FALSE, 0); 
    
    if ( govs & POWERSAVE ) 
    {
        gtk_widget_set_sensitive(powersave_on_batt,enable);
        g_signal_connect(powersave_on_batt,"pressed",G_CALLBACK(set_powersave_on_batt_cb),channel);
        gtk_widget_show(powersave_on_batt);
        if ( current_governor == POWERSAVE )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(powersave_on_batt),TRUE);
        }
    }
    if ( govs & ONDEMAND ) 
    {
        gtk_widget_set_sensitive(ondemand_on_batt,enable);
        g_signal_connect(ondemand_on_batt,"pressed",G_CALLBACK(set_ondemand_on_batt_cb),channel);
        gtk_widget_show(ondemand_on_batt);
        if ( current_governor == ONDEMAND )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ondemand_on_batt),TRUE);
        }
    }
    if ( govs & PERFORMANCE ) 
    {
        gtk_widget_set_sensitive(performance_on_batt,enable);
        g_signal_connect(performance_on_batt,"pressed",G_CALLBACK(set_performance_on_batt_cb),channel);
        gtk_widget_show(performance_on_batt);
        if ( current_governor == PERFORMANCE )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(performance_on_batt),TRUE);
        }
    }
    if ( govs & CONSERVATIVE ) 
    {
        gtk_widget_set_sensitive(conservative_on_batt,enable);
        g_signal_connect(conservative_on_batt,"pressed",G_CALLBACK(set_conservative_on_batt_cb),channel);
        gtk_widget_show(conservative_on_batt);
        if ( current_governor == CONSERVATIVE )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(conservative_on_batt),TRUE);
        }
    }
    if ( govs & USERSPACE ) 
    {
        gtk_widget_set_sensitive(userspace_on_batt,enable);
        g_signal_connect(userspace_on_batt,"pressed",G_CALLBACK(set_userspace_on_batt_cb),channel);
        gtk_widget_show(userspace_on_batt);
        if ( current_governor == USERSPACE )
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(userspace_on_batt),TRUE);
        }
    }
    
    return frame;
    
}

static GtkWidget *
xfpm_settings_cpu_freq(XfconfChannel *channel,guint8 govs,gboolean laptop,gboolean ups)
{
    GtkWidget *hbox;
    hbox = gtk_hbox_new(FALSE,2);
    gtk_widget_show(hbox);
    
    GtkWidget *vbox;
    vbox = gtk_vbox_new(FALSE,0);
    gtk_widget_show(vbox);

    gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);
    
    if ( !govs ) 
    {
        GtkWidget *label;
        label = gtk_label_new(_("No CPU governor found"));
        gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
        gtk_widget_show(label);
        gtk_widget_set_sensitive(GTK_WIDGET(cpu_gov),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cpu_gov),FALSE);
    }
	
	if ( govs & CPU_FREQ_CANNOT_BE_USED )
	{
		GtkWidget *label;
        label = gtk_label_new(_("CPU frequency control cannot be used"));
        gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
        gtk_widget_show(label);
        gtk_widget_set_sensitive(GTK_WIDGET(cpu_gov),FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cpu_gov),FALSE);
	}
	
    else
    {
        GtkWidget *frame;
        frame = xfpm_settings_cpu_on_ac_adapter(channel,
                                                govs,
                                                laptop || ups ? _("CPU freq settings on electric power") : _("CPU freq settings"));
        gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,FALSE,0);
        
        if ( laptop || ups )
        {
            frame = xfpm_settings_cpu_on_battery_power(channel,govs,ups);
            gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,FALSE,0);
        }
    }    
    
    return vbox;
}

static GtkWidget *
xfpm_settings_lcd_brightness(XfconfChannel *channel,gboolean laptop,gboolean lcd)
{
    GtkWidget *hbox;
    hbox = gtk_hbox_new(FALSE,2);

    if ( laptop && lcd) 
    {
        GtkWidget *label;
        label = gtk_label_new(_("Enable LCD brightness control"));
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
        
        GtkWidget *brg;
        brg = gtk_check_button_new();
        gtk_widget_show(brg);
        gtk_box_pack_start(GTK_BOX(hbox),brg,FALSE,FALSE,0);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(brg),
                                    xfconf_channel_get_bool(channel,LCD_BRIGHTNESS_CFG,TRUE));
        g_signal_connect(brg,"toggled",G_CALLBACK(set_lcd_brightness_cb),channel);
        gtk_widget_show(hbox);
    }
    
    return hbox;
}

#ifdef HAVE_DPMS
static GtkWidget *
xfpm_settings_dpms_on_battery(XfconfChannel *channel,gboolean ups)
{
    GtkWidget *frame,*align;  
    frame = xfce_create_framebox(ups ? _("Monitor settings on UPS power") : _("Monitor settings on battery power"), &align);
    gtk_widget_show(frame);
    
    gtk_container_set_border_width(GTK_CONTAINER(frame),BORDER);
    
    on_batt_dpms = xfpm_dpms_spins_new();
    gtk_widget_show(on_batt_dpms);
    gtk_container_add(GTK_CONTAINER(align),on_batt_dpms);
    GPtrArray *arr;
    GValue *value;
    guint value1 = 3 ,value2 = 4 ,value3 = 5;
    arr = xfconf_channel_get_arrayv(channel,ON_BATT_DPMS_TIMEOUTS_CFG);
    if ( arr ) 
    {
        value = g_ptr_array_index(arr,0);
        value1 = g_value_get_uint(value);
        
        value = g_ptr_array_index(arr,1);
        value2 = g_value_get_uint(value);
        
        value = g_ptr_array_index(arr,2);
        value3 = g_value_get_uint(value);
        xfconf_array_free(arr);
    } 
    xfpm_dpms_spins_set_default_values(XFPM_DPMS_SPINS(on_batt_dpms),value1,value2,value3);
    gboolean dpms_enabled = xfconf_channel_get_bool(channel,DPMS_ENABLE_CFG,TRUE);
    xfpm_dpms_spins_set_active(XFPM_DPMS_SPINS(on_batt_dpms),dpms_enabled);                
    g_signal_connect(on_batt_dpms,"dpms-value-changed",
                     G_CALLBACK(set_dpms_on_battery_cb),channel);
    
    return frame;
    
}


static GtkWidget *
xfpm_settings_dpms_on_ac_adapter(XfconfChannel *channel,const gchar *label)
{
    GtkWidget *frame;
    GtkWidget *align;

    frame = xfce_create_framebox(label, &align);
    gtk_container_set_border_width(GTK_CONTAINER(frame),BORDER);
    gtk_widget_show(frame);
    
    on_ac_dpms = xfpm_dpms_spins_new();
    gtk_widget_show(on_ac_dpms);
    gtk_container_add(GTK_CONTAINER(align),on_ac_dpms);
    GPtrArray *arr;
    GValue *value;
    guint value1 = 30 ,value2 = 45 ,value3 = 60;
    arr = xfconf_channel_get_arrayv(channel,ON_AC_DPMS_TIMEOUTS_CFG);
    if ( arr ) 
    {
        value = g_ptr_array_index(arr,0);
        value1 = g_value_get_uint(value);
        
        value = g_ptr_array_index(arr,1);
        value2 = g_value_get_uint(value);
        
        value = g_ptr_array_index(arr,2);
        value3 = g_value_get_uint(value);
        xfconf_array_free(arr);
    } 
    xfpm_dpms_spins_set_default_values(XFPM_DPMS_SPINS(on_ac_dpms),value1,value2,value3);
    gboolean dpms_enabled = xfconf_channel_get_bool(channel,DPMS_ENABLE_CFG,TRUE);
    xfpm_dpms_spins_set_active(XFPM_DPMS_SPINS(on_ac_dpms),dpms_enabled);             
    g_signal_connect(on_ac_dpms,"dpms-value-changed",
                     G_CALLBACK(set_dpms_on_ac_cb),channel);
    
    return frame;   
}
#endif

static GtkWidget *
xfpm_settings_monitor(XfconfChannel *channel,gboolean laptop,
                      gboolean dpms_capable,gboolean lcd,gboolean ups)
{
    GtkWidget *hbox;
    hbox = gtk_hbox_new(FALSE,2);
    gtk_widget_show(hbox);
    GtkWidget *vbox;
    vbox = gtk_vbox_new(FALSE,0);
    
    gtk_widget_show(vbox);
#ifdef HAVE_DPMS
    GtkWidget *frame;
    frame = xfpm_settings_dpms_on_ac_adapter(channel,
                                            laptop || ups ? _("Monitor settings on electric power") : _("Monitor settings"));
    gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

    if ( laptop || ups )
    {
        frame = xfpm_settings_dpms_on_battery(channel,ups);
        gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,FALSE,0);
    }

    if (! dpms_capable ) 
    {
        GtkWidget *label;
        label = gtk_label_new(_("Your monitor doesn't support DPMS control"));
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);
        gtk_widget_set_sensitive(dpms_op,FALSE);
        xfpm_dpms_spins_set_active(XFPM_DPMS_SPINS(on_ac_dpms),FALSE); 
    
        if ( GTK_IS_WIDGET(on_batt_dpms) )
        {
            xfpm_dpms_spins_set_active(XFPM_DPMS_SPINS(on_batt_dpms),FALSE); 
        }
    }
#endif    
    GtkWidget *lcd_box = xfpm_settings_lcd_brightness(channel,laptop,lcd);
    gtk_box_pack_start(GTK_BOX(vbox),lcd_box,TRUE,TRUE,0);
    return vbox;
}


static GtkWidget *
xfpm_settings_keys(XfconfChannel *channel,
                   gboolean laptop,
                   guint8 power_management,
                   guint8 switch_buttons)
{
    GtkWidget *table;
    GtkWidget *label;
    GtkWidget *frame,*align;
    GtkWidget *vbox;
    vbox = gtk_vbox_new(FALSE,0);
    gtk_widget_show(vbox);
    
    frame = xfce_create_framebox(_("Keyboard shortcuts"), &align);
    gtk_widget_show(frame);
    table = gtk_table_new(3,2,TRUE);
    gtk_widget_show(table);
    
    guint active;
    /// Power Button
    if (  switch_buttons & POWER_SWITCH )
    {
        label = gtk_label_new(_("When power button is pressed do"));
        gtk_widget_show(label);
        gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,0,1);
        GtkWidget *power_button;
        power_button = gtk_combo_box_new_text();
        gtk_combo_box_append_text(GTK_COMBO_BOX(power_button),_("Nothing"));
        if ( power_management & SYSTEM_CAN_SUSPEND ) 
            gtk_combo_box_append_text(GTK_COMBO_BOX(power_button),_("Suspend"));
        if ( power_management & SYSTEM_CAN_HIBERNATE )
            gtk_combo_box_append_text(GTK_COMBO_BOX(power_button),_("Hibernate"));
        if ( power_management & SYSTEM_CAN_SHUTDOWN )    
            gtk_combo_box_append_text(GTK_COMBO_BOX(power_button),_("Shutdown"));        
         
        active = xfconf_channel_get_uint(channel,POWER_SWITCH_CFG,0);
        active = _combo_helper_function(channel,POWER_SWITCH_CFG,power_management,active);    
                                    
        gtk_combo_box_set_active(GTK_COMBO_BOX(power_button),active);
        gtk_table_attach(GTK_TABLE(table),power_button,1,2,0,1,GTK_SHRINK,GTK_SHRINK,0,0);
        g_signal_connect(power_button,"changed",G_CALLBACK(set_power_button_action_cb),channel);
       
        gtk_widget_show(power_button);
    }
    
    /// Sleep Button
    if ( switch_buttons & SLEEP_SWITCH )
    {    
        label = gtk_label_new(_("When sleep button is pressed do"));
        gtk_widget_show(label);
        gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,1,2);
        
        GtkWidget *sleep_button;
        active = xfconf_channel_get_uint(channel,SLEEP_SWITCH_CFG,0);
        sleep_button = gtk_combo_box_new_text();
        
        gtk_combo_box_append_text(GTK_COMBO_BOX(sleep_button),_("Nothing"));
        if ( power_management & SYSTEM_CAN_SUSPEND ) 
            gtk_combo_box_append_text(GTK_COMBO_BOX(sleep_button),_("Suspend"));
        if ( power_management & SYSTEM_CAN_HIBERNATE )
            gtk_combo_box_append_text(GTK_COMBO_BOX(sleep_button),_("Hibernate"));
        if ( power_management & SYSTEM_CAN_SHUTDOWN )
            gtk_combo_box_append_text(GTK_COMBO_BOX(sleep_button),_("Shutdown"));    
        active = _combo_helper_function(channel,POWER_SWITCH_CFG,power_management,active);   
        gtk_combo_box_set_active(GTK_COMBO_BOX(sleep_button),active);
        gtk_widget_show(sleep_button);
        gtk_table_attach(GTK_TABLE(table),sleep_button,1,2,1,2,GTK_SHRINK,GTK_SHRINK,0,0);
        g_signal_connect(sleep_button,"changed",G_CALLBACK(set_sleep_button_action_cb),channel);
    }
    
    /// Lid Button
    if ( laptop && ( switch_buttons & LID_SWITCH ) )
    {
        label = gtk_label_new(_("When laptop lid is closed"));
        gtk_widget_show(label);
        gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,2,3);
        GtkWidget *lid_button;
        active = xfconf_channel_get_uint(channel,LID_SWITCH_CFG,0);
        lid_button = gtk_combo_box_new_text();
        gtk_combo_box_append_text(GTK_COMBO_BOX(lid_button),_("Nothing"));
        if ( power_management & SYSTEM_CAN_SUSPEND ) 
            gtk_combo_box_append_text(GTK_COMBO_BOX(lid_button),_("Suspend"));
        if ( power_management & SYSTEM_CAN_SUSPEND )
            gtk_combo_box_append_text(GTK_COMBO_BOX(lid_button),_("Hibernate"));
        if ( power_management & SYSTEM_CAN_SHUTDOWN )    
            gtk_combo_box_append_text(GTK_COMBO_BOX(lid_button),_("Shutdown"));    
        active = _combo_helper_function(channel,POWER_SWITCH_CFG,power_management,active);       
        gtk_combo_box_set_active(GTK_COMBO_BOX(lid_button),active);
        gtk_widget_show(lid_button);
        gtk_table_attach(GTK_TABLE(table),lid_button,1,2,2,3,GTK_SHRINK,GTK_SHRINK,0,0);
        g_signal_connect(lid_button,"changed",G_CALLBACK(set_lid_button_action_cb),channel);
    }
    
    gtk_container_add(GTK_CONTAINER(align),vbox);    
    gtk_box_pack_start(GTK_BOX(vbox),table,TRUE,TRUE,0);
    if ( !switch_buttons )
    {
        label = gtk_label_new(_("No available keyboard shortcuts found"));
        gtk_widget_show(label);
        gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);
    }
    return frame;
    
}

static GtkWidget *
xfpm_settings_general(XfconfChannel *channel,gboolean battery_settings,gboolean ups)
{
    GtkWidget *table;
    GtkWidget *show_icon;
    GtkWidget *label;
    GtkWidget *frame,*align;
    
    frame = xfce_create_framebox(_("General Options"), &align);
    gtk_widget_show(frame);
    table = gtk_table_new(4,2,FALSE);
    gtk_widget_show(table);
    gtk_container_add(GTK_CONTAINER(align),table);
    
    /* Systray icon */
    label = gtk_label_new(_("System tray icon"));
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,0,1);
    
    guint icon_cfg = xfconf_channel_get_uint(channel,SHOW_TRAY_ICON_CFG,0);
    show_icon = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(show_icon),_("Always Display an icon"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(show_icon),_("When battery is present"));
    if ( battery_settings || ups )
    {
        gtk_combo_box_append_text(GTK_COMBO_BOX(show_icon),_("When battery is charging or discharging"));
                            
    } else if ( icon_cfg == 2 ) /* set to default value then */
    {
         if(!xfconf_channel_set_uint(channel,SHOW_TRAY_ICON_CFG,0))
         {
            g_critical("Cannot set value %s\n",SHOW_TRAY_ICON_CFG);
        
         }
         icon_cfg = 0;
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(show_icon),icon_cfg);
    g_signal_connect(show_icon,"changed",G_CALLBACK(set_show_tray_icon_cb),channel);
    gtk_widget_show(show_icon);
    gtk_table_attach(GTK_TABLE(table),show_icon,1,2,0,1,GTK_SHRINK,GTK_SHRINK,0,0);
    gboolean cpu_gov_enabled;
    label = gtk_label_new(_("Enable CPU freq scaling control"));
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,1,2);
    cpu_gov = gtk_check_button_new();
    gtk_widget_show(cpu_gov);
    cpu_gov_enabled = xfconf_channel_get_bool(channel,CPU_FREQ_SCALING_CFG,TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cpu_gov),
                                 cpu_gov_enabled);                    
    g_signal_connect(cpu_gov,"toggled",G_CALLBACK(set_cpu_freq_scaling_cb),channel);      
    gtk_table_attach(GTK_TABLE(table),cpu_gov,1,2,1,2,GTK_SHRINK,GTK_SHRINK,0,0); 
    
#ifdef HAVE_DPMS
    gboolean dpms_enabled;
    label = gtk_label_new(_("Enable monitor power management control"));
    gtk_widget_show(label);
    gtk_table_attach_defaults(GTK_TABLE(table),label,0,1,2,3);
    dpms_op = gtk_check_button_new();
    gtk_widget_show(dpms_op);
    dpms_enabled = xfconf_channel_get_bool(channel,DPMS_ENABLE_CFG,TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dpms_op),
                                 dpms_enabled);                    
    g_signal_connect(dpms_op,"toggled",G_CALLBACK(set_dpms_cb),channel);      
    gtk_table_attach(GTK_TABLE(table),dpms_op,1,2,2,3,GTK_SHRINK,GTK_SHRINK,0,0); 
#endif    
    return frame;
}


static void
_cursor_changed_cb(GtkIconView *view,gpointer data)
{
    GtkTreeSelection *sel;
	GtkTreeModel     *model;
	GtkTreeIter       selected_row;
	gint int_data = 0;
	
	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	
	gtk_tree_selection_get_selected(sel,&model,&selected_row);
		
    gtk_tree_model_get(model,
                       &selected_row,
                       2, 
                       &int_data,
                       -1);

    if ( int_data >= 0 && int_data <= 3 )
    {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(nt),int_data);
    }
    else
    {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(nt),0);
    }
}

static GtkWidget *
xfpm_settings_tree_view(gboolean is_laptop,gboolean ups)
{
    GdkPixbuf *pix;
    GtkWidget *view;
    GtkListStore *list_store;
    GtkTreeIter iter;
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    
    list_store = gtk_list_store_new(3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_INT);
    view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(view),TRUE);
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col,_("Advanced settings"));
    
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col,renderer,FALSE);
    gtk_tree_view_column_set_attributes(col,renderer,"pixbuf",0,NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col,renderer,FALSE);
    gtk_tree_view_column_set_attributes(col,renderer,"text",1,NULL);
    
    gtk_tree_view_append_column(GTK_TREE_VIEW(view),col);

    /// CPU Settings
    pix = xfpm_load_icon("xfpm-cpu",38);      
    gtk_list_store_append(list_store,&iter);
    if ( pix )
    {
        gtk_list_store_set(list_store,&iter,0,pix,1,_("CPU settings"),2,0,-1);
        g_object_unref(pix);
    }
    else
    {
        gtk_list_store_set(list_store,&iter,1,_("CPU settings"),2,0,-1);
    }
    
    /// Battery Settings
    if ( is_laptop || ups )
    {
        pix = xfpm_load_icon(ups ? "gpm-ups-100" : "gpm-primary-100",38);
        gtk_list_store_append(list_store,&iter);
        if ( pix )
        {
            gtk_list_store_set(list_store,&iter,0,pix,1,_("Battery settings"),2,1,-1);
            g_object_unref(pix);
        }
        else
        {
            gtk_list_store_set(list_store,&iter,1,_("Battery settings"),2,1,-1);
        }
   }
   
    /// Keyboard Settings
    if ( is_laptop )
    {
        pix = xfpm_load_icon("xfpm-keyboard",38);
        gtk_list_store_append(list_store,&iter);
        if ( pix )
        {
            gtk_list_store_set(list_store,&iter,0,pix,1,_("Shortcuts"),2,2,-1);
            g_object_unref(pix);
        }
        else
        {
            gtk_list_store_set(list_store,&iter,1,_("Shortcuts"),2,2,-1);
        } 
    }
    
    /// Dpms settings
#ifdef HAVE_DPMS    
    pix = xfpm_load_icon("xfpm-display",38);      
    gtk_list_store_append(list_store,&iter);
    if ( pix ) 
    {
        gtk_list_store_set(list_store,&iter,0,pix,1,_("Monitor Settings"),2,3,-1);
        g_object_unref(pix);
    }
    else
    {
        gtk_list_store_set(list_store,&iter,1,_("Monitor Settings"),2,3,-1);
    }
#endif
    
    GtkTreeSelection *sel;
    GtkTreePath *path;

    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
    path = gtk_tree_path_new_from_string("0");
    gtk_tree_selection_select_path(sel, path);
    gtk_tree_path_free(path);
    g_signal_connect(view,"cursor-changed",G_CALLBACK(_cursor_changed_cb),NULL);
    
    return view;
}

GtkWidget *
xfpm_settings_new(XfconfChannel *channel,
                  gboolean is_laptop,
                  guint8 power_management,
                  gboolean dpms_capable,
                  guint8 govs,
                  guint8 switch_buttons,
                  gboolean lcd,
                  gboolean ups_found)
{
    GtkWidget *Dialog;  /* Main dialog window */
    GtkWidget *mainbox; /* Box to get (Dialog)->vbox */
    GtkWidget *box;
    GtkWidget *table;   
    GtkWidget *view;
        
    Dialog = xfce_titled_dialog_new_with_buttons(_("Power Manager Preferences"),
                                                    NULL,
                                                    GTK_DIALOG_NO_SEPARATOR,
                                                    GTK_STOCK_CLOSE,
                                                    GTK_RESPONSE_CANCEL,
                                                    GTK_STOCK_HELP,
                                                    GTK_RESPONSE_HELP,
                                                    NULL);
    
    gtk_window_set_icon_name(GTK_WINDOW(Dialog),"gpm-ac-adapter");
    gtk_dialog_set_default_response(GTK_DIALOG(Dialog),GTK_RESPONSE_CLOSE);
    
    mainbox = GTK_DIALOG(Dialog)->vbox;

    gboolean ups;
    ups = is_laptop ? FALSE : ups_found;
        
    /// General Options Frame
    box = xfpm_settings_general(channel,is_laptop,ups);
    gtk_box_pack_start (GTK_BOX (mainbox), box, FALSE, FALSE, 0);
    
    /// Notebook container
    nt = gtk_notebook_new();
    gtk_widget_show(nt);
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nt),FALSE);
    table = gtk_table_new(1,2,FALSE);
    gtk_widget_show(table);
    gtk_box_pack_start (GTK_BOX (mainbox), table, FALSE, FALSE, 0);
    view = xfpm_settings_tree_view(is_laptop,ups_found);
    gtk_table_attach_defaults(GTK_TABLE(table),view,0,1,0,1);  
    gtk_widget_show(view);  
    gtk_table_attach_defaults(GTK_TABLE(table),nt,1,2,0,1); 
    
    /// Cpu freq settings
    box = xfpm_settings_cpu_freq(channel,govs,is_laptop,ups);
    gtk_notebook_append_page(GTK_NOTEBOOK(nt),box,NULL);

  
    /// Battery settings
    box = xfpm_settings_battery(channel,power_management,ups);
    gtk_notebook_append_page(GTK_NOTEBOOK(nt),box,NULL);

    /// Keyboard buttons settings
    box = xfpm_settings_keys(channel,is_laptop,power_management,switch_buttons);
    gtk_notebook_append_page(GTK_NOTEBOOK(nt),box,NULL); 
    
    /// Dpms settings && LCD brightness settings DPMS existence is checked above
    box = xfpm_settings_monitor(channel,is_laptop,dpms_capable,lcd,ups);
    gtk_notebook_append_page(GTK_NOTEBOOK(nt),box,NULL);
    
    return Dialog;
}
