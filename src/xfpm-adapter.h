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

#ifndef __XFPM_ADAPTER_H
#define __XFPM_ADAPTER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define XFPM_TYPE_ADAPTER        (xfpm_adapter_get_type () )
#define XFPM_ADAPTER(o)          (G_TYPE_CHECK_INSTANCE_CAST((o), XFPM_TYPE_ADAPTER, XfpmAdapter))
#define XFPM_IS_ADAPTER(o)       (G_TYPE_CHECK_INSTANCE_TYPE((o), XFPM_TYPE_ADAPTER))

typedef struct XfpmAdapterPrivate XfpmAdapterPrivate;

typedef struct
{
    GObject		  parent;
    XfpmAdapterPrivate	 *priv;
    
} XfpmAdapter;

typedef struct
{
    GObjectClass 	  parent_class;
    
    void                 (*adapter_changed)    	      (XfpmAdapter *adapter,
    						       gboolean present);
    
} XfpmAdapterClass;

GType        	 	 xfpm_adapter_get_type        (void) G_GNUC_CONST;
XfpmAdapter      	*xfpm_adapter_new             (void);

gboolean 	  	 xfpm_adapter_get_present     (XfpmAdapter *adapter);
G_END_DECLS

#endif /* __XFPM_ADAPTER_H */
