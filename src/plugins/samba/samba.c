/*  XMMS2 - X Music Multiplexer System
 *  Copyright (C) 2003-2006 XMMS2 Team
 *
 *  PLUGINS ARE NOT CONSIDERED TO BE DERIVED WORK !!!
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 */

#include "xmms/xmms_defs.h"
#include "xmms/xmms_xformplugin.h"
#include "xmms/xmms_log.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "libsmbclient.h"

extern int errno;

/*
 * Type definitions
 */

typedef struct {
	gint fd;
} xmms_samba_data_t;

/*
 * Function prototypes
 */

static gboolean xmms_samba_init (xmms_xform_t *xform);
static void xmms_samba_destroy (xmms_xform_t *xform);
static gint xmms_samba_read (xmms_xform_t *xform, void *buffer, gint len,
                             xmms_error_t *error);
static gint64 xmms_samba_seek (xmms_xform_t *xform, gint64 offset,
                               xmms_xform_seek_mode_t whence,
                               xmms_error_t *error);
static gboolean xmms_samba_plugin_setup (xmms_xform_plugin_t *xform_plugin);

/*
 * Plugin header
 */
XMMS_XFORM_PLUGIN("smb", "SMB/CIFS Transport", XMMS_VERSION,
                  "Access SMB/CIFS fileshares over a network",
                  xmms_samba_plugin_setup);

static gboolean
xmms_samba_plugin_setup (xmms_xform_plugin_t *xform_plugin)
{
	xmms_xform_methods_t methods;

	XMMS_XFORM_METHODS_INIT (methods);

	methods.init = xmms_samba_init;
	methods.destroy = xmms_samba_destroy;
	methods.read = xmms_samba_read;
	methods.seek = xmms_samba_seek;

	xmms_xform_plugin_methods_set (xform_plugin, &methods);

	xmms_xform_plugin_indata_add (xform_plugin, XMMS_STREAM_TYPE_MIMETYPE,
	                              "application/x-url", XMMS_STREAM_TYPE_URL,
	                              "smb://*", XMMS_STREAM_TYPE_END);

	return TRUE;
}


/*
 * Member functions
 */
static void 
xmms_samba_auth_fn (const gchar *server, const gchar *share,
                    gchar *workgroup, gint wgmaxlen, gchar *username,
                    gint unmaxlen, gchar *password, gint pwmaxlen)
{
	return;
}


static gboolean
xmms_samba_init (xmms_xform_t *xform)
{
	xmms_samba_data_t *data;
	const gchar *url;
	struct stat st;
	gint fd, err;

	g_return_val_if_fail (xform, FALSE);

	url = xmms_xform_indata_get_str (xform, XMMS_STREAM_TYPE_URL);
	g_return_val_if_fail (url, FALSE);

	err = smbc_init (xmms_samba_auth_fn, 0);
	if (err < 0) {
		xmms_log_error ("%s", strerror (errno));
		return FALSE;
	}

	/* smbc is drunk and url here, evil smbc */
	err = smbc_stat (url, &st);
	if (err < 0) {
		xmms_log_error ("%s", strerror (errno));
		return FALSE;
	}

	if (!S_ISREG (st.st_mode)) {
		xmms_log_error ("%s is not a regular file.", url);
		return FALSE;
	}

	/* need to reget the url thanks to evil smbc */
	url = xmms_xform_indata_get_str (xform, XMMS_STREAM_TYPE_URL);
	fd = smbc_open (url, O_RDONLY | O_NONBLOCK, 0);
	if (fd == -1) {
		xmms_log_error ("%s", strerror (errno));
		return FALSE;
	}

	data = g_new0 (xmms_samba_data_t, 1);
	data->fd = fd;

	xmms_xform_private_data_set (xform, data);

	xmms_xform_outdata_type_add (xform, XMMS_STREAM_TYPE_MIMETYPE,
	                             "application/octet-stream",
	                             XMMS_STREAM_TYPE_END);

	xmms_xform_metadata_set_int (xform, XMMS_MEDIALIB_ENTRY_PROPERTY_SIZE,
	                             st.st_size);
	xmms_xform_metadata_set_int (xform, XMMS_MEDIALIB_ENTRY_PROPERTY_LMOD,
	                             st.st_mtime);

	return TRUE;
}


static void
xmms_samba_destroy (xmms_xform_t *xform)
{
	gint err;
	xmms_samba_data_t *data;

	g_return_if_fail (xform);
	data = xmms_xform_private_data_get (xform);
	g_return_if_fail (data);

	if (data->fd != -1) {
		err = smbc_close (data->fd);
		if (err < 0) {
			xmms_log_error ("%s", strerror (errno));
		}
	}

	g_free (data);
}


static gint
xmms_samba_read (xmms_xform_t *xform, void *buffer, gint len, 
                 xmms_error_t *error)
{
	xmms_samba_data_t *data;
	gint ret;

	g_return_val_if_fail (xform, -1);
	g_return_val_if_fail (buffer, -1);
	g_return_val_if_fail (error, -1);

	data = xmms_xform_private_data_get (xform);
	g_return_val_if_fail (data, -1);

	ret = smbc_read (data->fd, buffer, len); 

	if (ret == 0) {
		xmms_error_set (error, XMMS_ERROR_EOS, "End of file reached");
	} else if (ret < 0) {
		xmms_error_set (error, XMMS_ERROR_GENERIC, strerror (errno));
	}

	return ret;
}


static gint64
xmms_samba_seek (xmms_xform_t *xform, gint64 offset,
                 xmms_xform_seek_mode_t whence, xmms_error_t *error)
{
	xmms_samba_data_t *data;
	gint w = 0;
	off_t res;

	g_return_val_if_fail (xform, -1);
	data = xmms_xform_private_data_get (xform);
	g_return_val_if_fail (data, -1);

	switch (whence) {
		case XMMS_XFORM_SEEK_SET:
			w = SEEK_SET;
			break;
		case XMMS_XFORM_SEEK_END:
			w = SEEK_END;
			break;
		case XMMS_XFORM_SEEK_CUR:
			w = SEEK_CUR;
			break;
	}

	res = smbc_lseek (data->fd, offset, w);
	if (res == -1) {
		xmms_error_set (error, XMMS_ERROR_INVAL, "Couldn't seek");
	}

	return res;
}
