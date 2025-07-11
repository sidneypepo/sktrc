/**
 * sktrc_debug.c - A module to help at Simple Kernel Trace's debugging
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
#include <linux/module.h> // module info
#include <linux/init.h> // init macros

#include <linux/types.h> // bit-defined types
#include <linux/stddef.h> // null
#include <linux/fs.h> // r/w kernel
#include <uapi/asm-generic/fcntl.h> // fs flags
#include <linux/err.h> // error macros
#include <linux/string.h> // strlen
#include <linux/slab.h> // mem alloc
#include <linux/printk.h> // self-explainatory
#include <linux/preempt.h> // in_atomic
#include <linux/irqflags.h> // irqs_disabled
#include <linux/gfp.h> // alloc flags
#include <linux/kernel.h> // mini-libc (snprintf)
#include <linux/mutex.h> // mutex
#include <stdarg.h> // var args

static void sktrc_debug(const char *str)
{
	// Put your code snippet here
	return;
}

static int __init debug_init(void)
{
	pr_info("SKTRC debug init\n");

	// Write your debugging test cases here
	pr_info("SKTRC debug start\n");
	sktrc_debug("SKTRC test 1\n");
	sktrc_debug("SKTRC test 2\n");
	sktrc_debug("SKTRC test 3\n");
	pr_info("SKTRC debug end\n");
	return 0;
}

static void __exit debug_exit(void)
{
	pr_info("SKTRC debug exit\n");
	return;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A module to help at Simple Kernel Trace's debugging");
MODULE_AUTHOR("Sidney PEPO <sidneypepo@disroot.org>");

module_init(debug_init);
module_exit(debug_exit);
