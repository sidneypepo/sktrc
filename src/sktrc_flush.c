/**
 * sktrc_flush.c - A module to test Simple Kernel Trace and flush its data
 * Copyright (C) 2025  Sidney PEPO <sidneypepo@disroot.org>
 *
 * This file is part of Simple Kernel Trace.
 *
 * Simple Kernel Trace is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Simple Kernel Trace is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Simple Kernel Trace; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  US
 */

#include <trace/sktrc.h>
#include <linux/module.h>
#include <linux/init.h>

static int __init sktrc_flush_init(void)
{
	sktrc(SKTRC_HASH(0, 1, 0, 3), "SKTRC flushed!");
	return 0;
}

static void __exit sktrc_flush_exit(void)
{
	return;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A module to test Simple Kernel Trace and flush its data");
MODULE_AUTHOR("Sidney PEPO <sidneypepo@disroot.org>");

module_init(sktrc_flush_init);
module_exit(sktrc_flush_exit);
