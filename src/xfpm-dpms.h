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

#ifndef __XFPM_DPMS_H
#define __XFPM_DPMS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>

#include <xfconf/xfconf.h>

#ifdef HAVE_DPMS

G_BEGIN_DECLS

#define XFPM_TYPE_DPMS        (xfpm_dpms_get_type () )
#define XFPM_DPMS(o)          (G_TYPE_CHECK_INSTANCE_CAST((o), XFPM_TYPE_DPMS, XfpmDpms))
#define XFPM_IS_DPMS(o)       (G_TYPE_CHECK_INSTANCE_TYPE((o), XFPM_TYPE_DPMS))

typedef struct XfpmDpmsPrivate XfpmDpmsPrivate;

typedef struct
{
    GObject		  parent;
    XfpmDpmsPrivate	 *priv;
    
} XfpmDpms;

typedef struct
{
    GObjectClass 	  parent_class;
    
} XfpmDpmsClass;

GType           xfpm_dpms_get_type        (void) G_GNUC_CONST;
XfpmDpms       *xfpm_dpms_new             (XfconfChannel *channel);

void            xfpm_dpms_set_on_battery  (XfpmDpms *dpms,
					   gboolean on_battery);
gboolean        xfpm_dpms_capable         (XfpmDpms *dpms);


G_END_DECLS

#endif /* HAVE_DPMS */

#endif /* __XFPM_DPMS_H */
