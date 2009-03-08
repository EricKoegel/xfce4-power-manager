/*
 * * Copyright (C) 2008-2009 Ali <aliov@xfce.org>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
#include <libxfce4util/libxfce4util.h>

#include <libnotify/notify.h>

#include "libxfpm/xfpm-common.h"

#include "xfpm-notify.h"

/* Init */
static void xfpm_notify_class_init (XfpmNotifyClass *klass);
static void xfpm_notify_init       (XfpmNotify *notify);
static void xfpm_notify_finalize   (GObject *object);

#define XFPM_NOTIFY_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE((o), XFPM_TYPE_NOTIFY, XfpmNotifyPrivate))

struct XfpmNotifyPrivate
{
    NotifyNotification *notification;
};

G_DEFINE_TYPE(XfpmNotify, xfpm_notify, G_TYPE_OBJECT)

static void
xfpm_notify_class_init(XfpmNotifyClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->finalize = xfpm_notify_finalize;


    g_type_class_add_private(klass,sizeof(XfpmNotifyPrivate));
}

static void
xfpm_notify_init(XfpmNotify *notify)
{
    notify->priv = XFPM_NOTIFY_GET_PRIVATE(notify);
    
    notify->priv->notification = NULL;
        
    notify_init ("xfce4-power-manager");
}

static void
xfpm_notify_finalize(GObject *object)
{
    XfpmNotify *notify;

    notify = XFPM_NOTIFY(object);
    
    G_OBJECT_CLASS(xfpm_notify_parent_class)->finalize(object);
}

static NotifyNotification *
xfpm_notify_new_notification_internal (XfpmNotify *notify, const gchar *title, const gchar *message)
{
    NotifyNotification *n;
    
    n = notify_notification_new (title, message, NULL, NULL);
    
    return n;
}

static void
xfpm_notify_set_icon (XfpmNotify *notify, NotifyNotification *n, const gchar *icon_name )
{
    GdkPixbuf *pix = xfpm_load_icon (icon_name, 48);
    
    if ( pix )
    {
	notify_notification_set_icon_from_pixbuf (n, 
						  pix);
	g_object_unref ( G_OBJECT(pix));
    }
}

static void
xfpm_notify_closed_cb (NotifyNotification *n, XfpmNotify *notify)
{
    notify->priv->notification = NULL;
    g_object_unref (G_OBJECT(n));
}

static void
xfpm_notify_close_notification (XfpmNotify *notify )
{
    if ( notify->priv->notification )
    {
    	if (!notify_notification_close (notify->priv->notification, NULL))
	    g_warning ("Failed to close notification\n");
	
	g_object_unref (G_OBJECT(notify->priv->notification) );
	notify->priv->notification  = NULL;
    }
}

XfpmNotify *
xfpm_notify_new(void)
{
    XfpmNotify *notify = NULL;
    notify = g_object_new (XFPM_TYPE_NOTIFY, NULL);
    return notify;
}

void xfpm_notify_show_notification (XfpmNotify *notify, const gchar *title,
				    const gchar *text,  const gchar *icon_name,
				    gint timeout, XfpmNotifyUrgency urgency, 
				    GtkStatusIcon *icon)
{
    xfpm_notify_close_notification (notify);
    
    notify->priv->notification = 
    	xfpm_notify_new_notification_internal (notify, title, text);
    
    if ( icon_name )
    	xfpm_notify_set_icon (notify, notify->priv->notification, icon_name);
	
    if ( icon )
    	notify_notification_attach_to_status_icon (notify->priv->notification, icon);

    g_signal_connect (G_OBJECT(notify->priv->notification),"closed",
		      G_CALLBACK(xfpm_notify_closed_cb), notify);
		      
    notify_notification_show (notify->priv->notification, NULL);
    
}
