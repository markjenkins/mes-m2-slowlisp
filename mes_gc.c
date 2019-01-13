/* -*-comment-start: "//";comment-end:""-*-
 * GNU Mes --- Maxwell Equations of Software
 * Copyright © 2016,2017 Jan (janneke) Nieuwenhuizen <janneke@gnu.org>
 *
 * This file is part of GNU Mes.
 *
 * GNU Mes is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * GNU Mes is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Mes.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mes.h"
#include "mes_constants.h"
#include <errno.h>
#include <strings.h>

#define TYPE(x) g_cells[x].type
#define NTYPE(x) g_news[x].type
#define NVECTOR(x) g_news[x].cdr
#define LENGTH(x) g_cells[x].car
#define NLENGTH(x) g_news[x].car
#define VECTOR(x) g_cells[x].cdr
#define CBYTES(x) (char*)&g_cells[x].cdr
#define NCBYTES(x) (char*)&g_news[x].cdr
#define CAR(x) g_cells[x].car
#define NVALUE(x) g_news[x].cdr

size_t bytes_cells(size_t length);
int eputs(char const* s);
char *itoa (int number);
SCM error(SCM key, SCM x);
SCM cstring_to_symbol(char const *s);
SCM write_error_ (SCM x);
SCM gc_push_frame();
SCM gc_pop_frame();

SCM gc_up_arena()  ///((internal))
{
	long old_arena_bytes = (ARENA_SIZE + JAM_SIZE) * sizeof(struct scm);

	if(ARENA_SIZE >> 1 < MAX_ARENA_SIZE >> 2)
	{
		ARENA_SIZE <<= 1;
		JAM_SIZE <<= 1;
		GC_SAFETY <<= 1;
	}
	else
	{
		ARENA_SIZE = MAX_ARENA_SIZE - JAM_SIZE;
	}

	long arena_bytes = (ARENA_SIZE + JAM_SIZE) * sizeof(struct scm);
	void *p = realloc(g_cells - 1, arena_bytes + STACK_SIZE * sizeof(SCM));

	if(!p)
	{
		eputs("realloc failed, g_free=");
		eputs(itoa(g_free));
		eputs(":");
		eputs(itoa(ARENA_SIZE - g_free));
		eputs("\n");
		assert(0);
		exit(1);
	}

	g_cells = (struct scm*)p;
	memcpy(p + arena_bytes, p + old_arena_bytes, STACK_SIZE * sizeof(SCM));
	g_cells++;
	return 0;
}

void gc_flip()  ///((internal))
{
	if(g_debug > 2)
	{
		eputs(";;;   => jam[");
		eputs(itoa(g_free));
		eputs("]\n");
	}

	if(g_free > JAM_SIZE)
	{
		JAM_SIZE = g_free + g_free / 2;
	}

	memcpy(g_cells - 1, g_news - 1, (g_free + 2)*sizeof(struct scm));
}

SCM gc_copy(SCM old)  ///((internal))
{
	if(TYPE(old) == TBROKEN_HEART)
	{
		return g_cells[old].car;
	}

	SCM new = g_free++;
	g_news[new] = g_cells[old];

	if(NTYPE(new) == TSTRUCT || NTYPE(new) == TVECTOR)
	{
		NVECTOR(new) = g_free;

		for(long i = 0; i < LENGTH(old); i++)
		{
			g_news[g_free++] = g_cells[VECTOR(old) + i];
		}
	}
	else if(NTYPE(new) == TBYTES)
	{
		char const *src = CBYTES(old);
		char *dest = NCBYTES(new);
		size_t length = NLENGTH(new);
		memcpy(dest, src, length + 1);
		g_free += bytes_cells(length) - 1;

		if(g_debug > 4)
		{
			eputs("gc copy bytes: ");
			eputs(src);
			eputs("\n");
			eputs("    length: ");
			eputs(itoa(LENGTH(old)));
			eputs("\n");
			eputs("    nlength: ");
			eputs(itoa(NLENGTH(new)));
			eputs("\n");
			eputs("        ==> ");
			eputs(dest);
			eputs("\n");
		}
	}

	TYPE(old) = TBROKEN_HEART;
	CAR(old) = new;
	return new;
}

SCM gc_relocate_car(SCM new, SCM car)  ///((internal))
{
	g_news[new].car = car;
	return cell_unspecified;
}

SCM gc_relocate_cdr(SCM new, SCM cdr)  ///((internal))
{
	g_news[new].cdr = cdr;
	return cell_unspecified;
}

void gc_loop(SCM scan)  ///((internal))
{
	SCM car;
	SCM cdr;

	while(scan < g_free)
	{
		if(NTYPE(scan) == TBROKEN_HEART)
		{
			error(cell_symbol_system_error, cstring_to_symbol("gc"));
		}

		if(NTYPE(scan) == TMACRO
		        || NTYPE(scan) == TPAIR
		        || NTYPE(scan) == TREF
		        || scan == 1 // null
		        || NTYPE(scan) == TVARIABLE)
		{
			car = gc_copy(g_news[scan].car);
			gc_relocate_car(scan, car);
		}

		if((NTYPE(scan) == TCLOSURE
		        || NTYPE(scan) == TCONTINUATION
		        || NTYPE(scan) == TKEYWORD
		        || NTYPE(scan) == TMACRO
		        || NTYPE(scan) == TPAIR
		        || NTYPE(scan) == TPORT
		        || NTYPE(scan) == TSPECIAL
		        || NTYPE(scan) == TSTRING
		        || NTYPE(scan) == TSYMBOL
		        || scan == 1 // null
		        || NTYPE(scan) == TVALUES)
		        && g_news[scan].cdr) // allow for 0 terminated list of symbols
		{
			cdr = gc_copy(g_news[scan].cdr);
			gc_relocate_cdr(scan, cdr);
		}

		if(NTYPE(scan) == TBYTES)
		{
			scan += bytes_cells(NLENGTH(scan)) - 1;
		}

		scan++;
	}

	gc_flip();
}

void gc();
SCM gc_check()
{
	if(g_free + GC_SAFETY > ARENA_SIZE)
	{
		gc();
	}

	return cell_unspecified;
}

SCM gc_init_news()  ///((internal))
{
	g_news = g_cells + g_free;
	NTYPE(0) = TVECTOR;
	NLENGTH(0) = 1000;
	NVECTOR(0) = 0;
	g_news++;
	NTYPE(0) = TCHAR;
	NVALUE(0) = 'n';
	return 0;
}

void gc_()  ///((internal))
{
	gc_init_news();

	if(g_debug == 2)
	{
		eputs(".");
	}

	if(g_debug > 2)
	{
		eputs(";;; gc[");
		eputs(itoa(g_free));
		eputs(":");
		eputs(itoa(ARENA_SIZE - g_free));
		eputs("]...");
	}

	g_free = 1;
	if(ARENA_SIZE < MAX_ARENA_SIZE && g_news > 0)
	{
		if(g_debug == 2)
		{
			eputs("+");
		}

		if(g_debug > 2)
		{
			eputs(" up[");
			eputs(itoa((unsigned long)g_cells));
			eputs(",");
			eputs(itoa((unsigned long)g_news));
			eputs(":");
			eputs(itoa(ARENA_SIZE));
			eputs(",");
			eputs(itoa(MAX_ARENA_SIZE));
			eputs("]...");
		}

		gc_up_arena();
	}

	for(long i = g_free; i < g_symbol_max; i++)
	{
		gc_copy(i);
	}

	g_symbols = gc_copy(g_symbols);
	g_macros = gc_copy(g_macros);
	g_ports = gc_copy(g_ports);
	m0 = gc_copy(m0);

	for(long i = g_stack; i < STACK_SIZE; i++)
	{
		g_stack_array[i] = gc_copy(g_stack_array[i]);
	}

	gc_loop(1);
}

void gc()
{
	if(g_debug > 4)
	{
		eputs("symbols: ");
		write_error_(g_symbols);
		eputs("\n");
		eputs("R0: ");
		write_error_(r0);
		eputs("\n");
	}

	gc_push_frame();
	gc_();
	gc_pop_frame();

	if(g_debug > 4)
	{
		eputs("symbols: ");
		write_error_(g_symbols);
		eputs("\n");
		eputs("R0: ");
		write_error_(r0);
		eputs("\n");
	}
}
