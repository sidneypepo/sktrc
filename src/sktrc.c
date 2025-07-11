/**
 * sktrc.c - Simple Kernel Trace's main file
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

#define SKTRC_BFFR_MAX 0x200
#define SKTRC_SIZE_CHUNK 0x1000

static const char *const SKTRC_LOG = "/FULL/PATH/TO/sktrc.log";

static const char SKTRC_SIGNS[5] = "*+-?";

// DON'T touch the first array: it's reserved for this module ONLY!
static const char SKTRC_CTX[0x10][0x100][0x2a] = {
	{			// 0x0000  XXXX
		"sktrc",		// 0000
		"sktrc_init",		// 0001
		"sktrc_exit",		// 0002
		"sktrc_flush"		// 0003
	},
	{			// 0x0001  XXXX
		"your_first_module",	// 0000
		"module1_init",		// 0001
		"module1_exit",
		"func_a",
		"func_b"
	},
	{			// 0x0002  XXXX
		"your_second_module",	// 0000
		"func_c",
		"func_d"
	},
	{			// 0x0003  XXXX
		"your_third_module",	// 0000
		"func_e"	 	// 0001
	}
};

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

static const char *const SKTRC_EOPEN = "fail to open ";
static const char *const SKTRC_EALLOC = "fail to alloc memory";

static u_int32_t chunks = 0;
static char *flush = NULL;
static char *tmp = NULL;

static void sktrc_file_write(char *const ctx, const char *const msg,
			     const size_t msg_size)
{
	const char sign = ctx[1];
	struct file *log_filp = filp_open(SKTRC_LOG, O_WRONLY | O_APPEND |
					  O_LARGEFILE | O_CREAT, 0644);
	if (!IS_ERR_OR_NULL(log_filp)) {
		if (flush != NULL) {
			kernel_write(log_filp, flush, strlen(flush),
				     log_filp->f_pos);
			kfree(flush);
			flush = NULL;
		}
		kernel_write(log_filp, msg, strlen(msg), log_filp->f_pos);
		filp_close(log_filp, NULL);
		return;
	}

	ctx[1] = SKTRC_SIGNS[2];
	pr_err("%s%s%s (%ld)!\n", ctx, SKTRC_EOPEN, SKTRC_LOG,
	       PTR_ERR(log_filp));

	if (flush == NULL) {
		++chunks;
		tmp = kzalloc(SKTRC_SIZE_CHUNK, (in_atomic() || irqs_disabled())
			      ? GFP_ATOMIC : GFP_KERNEL);
	} else if (strlen(flush) + msg_size >= chunks * SKTRC_SIZE_CHUNK) {
		++chunks;
		tmp = krealloc(flush, chunks * SKTRC_SIZE_CHUNK, __GFP_ZERO |
			       (in_atomic() || irqs_disabled()) ? GFP_ATOMIC :
			       GFP_KERNEL);
	}

	if (!IS_ERR_OR_NULL(tmp)) {
		flush = tmp;
		snprintf(flush + ((flush[0] == 0) ? 0 : strlen(flush)),
			 msg_size, "%s", msg);
	} else {
		--chunks;
		pr_err("%s%s (flush)!\n", ctx, SKTRC_EALLOC);
	}

	ctx[1] = sign;
	return;
}

static DEFINE_MUTEX(lock);

u_int8_t sktrc(const u_int32_t hash, const char *fmt, ...)
{
	size_t msg_size = 0;
	char *msg = NULL;
	va_list args;

	u_int8_t flags = (hash >> 30) & 0x3;
	u_int8_t sign = (hash >> 28) & 0x3;
	u_int16_t mod = (hash >> 16) & 0xfff;
	u_int16_t func = hash & 0xffff;

	size_t ctx_size = strlen(SKTRC_CTX[mod][0]) +
		strlen(SKTRC_CTX[mod][func]) + 10;
	char ctx[ctx_size];
	snprintf(ctx, ctx_size, "[%c] [%s:%s]: ", SKTRC_SIGNS[sign],
		 SKTRC_CTX[mod][0], SKTRC_CTX[mod][func]);

	ctx_size = strlen(ctx);
	msg_size = ctx_size + strlen(fmt) + SKTRC_BFFR_MAX + 2;
	msg = kzalloc(msg_size, GFP_KERNEL);
	if (IS_ERR_OR_NULL(msg)) {
		ctx[1] = SKTRC_SIGNS[2];
		pr_err("%s%s (msg)!\n", ctx, SKTRC_EALLOC);
		return 1;
	}

	mutex_lock(&lock);

	snprintf(msg, ctx_size, "%s", ctx);

	va_start(args, fmt);
	vsnprintf(msg + ctx_size, msg_size - ctx_size - 2, fmt, args);
	va_end(args);

	msg[ctx_size - 1] = ' ';
	msg_size = strlen(msg);
	if (msg[msg_size - 1] != '\n')
		msg[msg_size] = '\n';
	msg_size += 2;

	pr_info("%s", msg);

	if (flags != 1)
		sktrc_file_write(ctx, msg, msg_size);

	kfree(msg);
	mutex_unlock(&lock);
	return 0;
}
EXPORT_SYMBOL(sktrc);

static int __init sktrc_init(void)
{
	// pr_info("SKTRC loaded!");
	sktrc(SKTRC_HASH(0, 1, 0, 1), "SKTRC loaded!"); // 0x10000001
	return 0;
}

static void __exit sktrc_exit(void)
{
	// pr_info("SKTRC unloaded!");
	sktrc(SKTRC_HASH(0, 1, 0, 2), "SKTRC unloaded!"); // 0x10000002
	return;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SKTRC - Simple Kernel Trace");
MODULE_AUTHOR("Sidney PEPO <sidneypepo@disroot.org>");

module_init(sktrc_init);
module_exit(sktrc_exit);
