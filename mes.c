/* -*-comment-start: "//";comment-end:""-*-
 * GNU Mes --- Maxwell Equations of Software
 * Copyright © 2016,2017,2018 Jan (janneke) Nieuwenhuizen <janneke@gnu.org>
 * Copyright © 2019 Jeremiah Orians
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
#include <fcntl.h>

#define TYPE(x) g_cells[x].type
#define CAR(x) g_cells[x].rac
#define CDR(x) g_cells[x].rdc
#define VALUE(x) g_cells[x].rdc
#define VARIABLE(x) g_cells[x].rac
#define STRING(x) g_cells[x].rdc
#define LENGTH(x) g_cells[x].rac
#define VECTOR(x) g_cells[x].rdc
#define MAKE_NUMBER(n) make_cell__ (TNUMBER, 0, (long)n)
#define MAKE_STRING0(x) make_string (x, strlen (x))
#define MAKE_MACRO(name, x) make_cell__ (TMACRO, x, STRING (name))
#define MAKE_CHAR(n) make_cell__ (TCHAR, 0, n)
#define MAKE_CONTINUATION(n) make_cell__ (TCONTINUATION, n, g_stack)
#define CAAR(x) CAR (CAR (x))
#define CADR(x) CAR (CDR (x))
#define CDAR(x) CDR (CAR (x))
#define CDDR(x) CDR (CDR (x))
#define CADAR(x) CAR (CDR (CAR (x)))
#define CADDR(x) CAR (CDR (CDR (x)))
#define CDADAR(x) CAR (CDR (CAR (CDR (x))))
#define MACRO(x) g_cells[x].rac
#define CLOSURE(x) g_cells[x].rdc
#define CONTINUATION(x) g_cells[x].rdc

SCM ARENA_SIZE;
SCM MAX_ARENA_SIZE;
SCM STACK_SIZE;

SCM JAM_SIZE;
SCM GC_SAFETY;

int MAX_STRING;
char *g_buf = 0;

char *g_arena = 0;
int g_debug;
SCM g_free = 0;

/* Imported Functions */
char *itoa (int number);
int fdputc (int c, int fd);
int fdputs (char const* s, int fd);
int eputs (char const* s);
struct scm* make_string(char const* s, int length);
struct scm* mes_builtins(struct scm* a);
struct scm* apply_builtin(SCM fn, SCM x);
struct scm* cstring_to_symbol(char const *s);
struct scm* make_hash_table_(SCM size);
struct scm* make_initial_module(SCM a);
struct scm* gc_check ();
SCM gc ();
struct scm* hashq_get_handle (SCM table, SCM key, SCM dflt);
struct scm* hashq_set_x (SCM table, SCM key, SCM value);
struct scm* hash_set_x (SCM table, SCM key, SCM value);
struct scm* display_ (SCM x);
struct scm* display_error_ (SCM x);
struct scm* write_error_ (SCM x);
SCM equal2_p (SCM a, SCM b);
SCM reverse_x_ (SCM x, SCM t);
struct scm* builtin_arity (SCM builtin);
SCM builtin_p (SCM x);
struct scm* module_printer (SCM module);
struct scm* module_variable (SCM module, SCM name);
struct scm* module_ref (SCM module, SCM name);
struct scm* module_define_x (SCM module, SCM name, SCM value);
SCM open_input_file (SCM file_name);
SCM set_current_input_port (SCM port);
SCM read_input_file_env ();
struct scm* string_equal_p (SCM a, SCM b);
struct scm* symbol_to_string (SCM symbol);
SCM init_time(SCM a);
SCM make_cell__(long type, SCM car, SCM cdr);
SCM eval_apply();

/* M2-Planet Imports */
int numerate_string(char *a);

SCM make_cell_(SCM type, SCM car, SCM cdr)
{
	assert(TYPE(type) == TNUMBER);
	SCM t = VALUE(type);

	if(t == TCHAR || t == TNUMBER)
	{
		return make_cell__(t, car ? CAR(car) : 0, cdr ? CDR(cdr) : 0);
	}

	return make_cell__(t, car, cdr);
}

SCM assoc_string(SCM x, SCM a)  ///((internal))
{
	while(a != cell_nil && (TYPE(CAAR(a)) != TSTRING || GetSCM2(bad2good(string_equal_p(x, CAAR(a)), g_cells), g_cells) == cell_f))
	{
		a = CDR(a);
	}

	return a != cell_nil ? CAR(a) : cell_f;
}

SCM type_(SCM x)
{
	return MAKE_NUMBER(TYPE(x));
}

SCM car_(SCM x)
{
	return (TYPE(x) != TCONTINUATION
	        && (TYPE(CAR(x)) == TPAIR   // FIXME: this is weird
	            || TYPE(CAR(x)) == TREF
	            || TYPE(CAR(x)) == TSPECIAL
	            || TYPE(CAR(x)) == TSYMBOL
	            || TYPE(CAR(x)) == TSTRING)) ? CAR(x) : MAKE_NUMBER(CAR(x));
}

SCM cdr_(SCM x)
{
	return (TYPE(x) != TCHAR
	        && TYPE(x) != TNUMBER
	        && TYPE(x) != TPORT
	        && (TYPE(CDR(x)) == TPAIR
	            || TYPE(CDR(x)) == TREF
	            || TYPE(CDR(x)) == TSPECIAL
	            || TYPE(CDR(x)) == TSYMBOL
	            || TYPE(CDR(x)) == TSTRING)) ? CDR(x) : MAKE_NUMBER(CDR(x));
}

SCM cons(SCM x, SCM y)
{
	return make_cell__(TPAIR, x, y);
}

struct scm* cons2(struct scm* x, struct scm* y)
{
	return Getstructscm2(make_cell__(TPAIR, GetSCM2(x, g_cells), GetSCM2(y, g_cells)), g_cells);
}

SCM cons3(struct scm* x, struct scm* y)
{
	return make_cell__(TPAIR, GetSCM2(x, g_cells), GetSCM2(y, g_cells));
}


SCM car(SCM x)
{
	return CAR(x);
}

SCM cdr(SCM x)
{
	return CDR(x);
}

SCM list(SCM x)  ///((arity . n))
{
	return x;
}

SCM null_p(SCM x)
{
	return x == cell_nil ? cell_t : cell_f;
}

SCM eq_p(SCM x, SCM y)
{
	return (x == y
	        || ((TYPE(x) == TKEYWORD && TYPE(y) == TKEYWORD
	             && GetSCM2(bad2good(string_equal_p(x, y), g_cells), g_cells) == cell_t))
	        || (TYPE(x) == TCHAR && TYPE(y) == TCHAR
	            && VALUE(x) == VALUE(y))
	        || (TYPE(x) == TNUMBER && TYPE(y) == TNUMBER
	            && VALUE(x) == VALUE(y)))
	       ? cell_t : cell_f;
}

SCM values(SCM x)  ///((arity . n))
{
	SCM v = cons(0, x);
	TYPE(v) = TVALUES;
	return v;
}

SCM acons(SCM key, SCM value, SCM alist)
{
	return cons(cons(key, value), alist);
}

struct scm* acons2(struct scm* key, struct scm* value, struct scm* alist)
{
	return cons2(cons2(key, value), alist);
}

SCM acons3(struct scm* key, struct scm* value, struct scm* alist)
{
	return cons3(cons2(key, value), alist);
}


SCM length__(SCM x)  ///((internal))
{
	SCM n = 0;

	while(x != cell_nil)
	{
		n++;

		if(TYPE(x) != TPAIR)
		{
			return -1;
		}

		x = CDR(x);
	}

	return n;
}

SCM length(SCM x)
{
	return MAKE_NUMBER(length__(x));
}

SCM apply(SCM, SCM);

SCM error(SCM key, SCM x)
{
	SCM throw= GetSCM2(bad2good(module_ref(r0, cell_symbol_throw), g_cells), g_cells);

	if(throw != cell_undefined)
	{
		return apply(throw, cons(key, cons(x, cell_nil)));
	}

	display_error_(key);
	eputs(": ");
	write_error_(x);
	eputs("\n");
	assert(0);
	exit(EXIT_FAILURE);
}

//  extra lib
SCM assert_defined(SCM x, SCM e)  ///((internal))
{
	if(e == cell_undefined)
	{
		return error(cell_symbol_unbound_variable, x);
	}

	return e;
}

SCM check_formals(SCM f, SCM formals, SCM args)  ///((internal))
{
	SCM flen = (TYPE(formals) == TNUMBER) ? VALUE(formals) : length__(formals);
	SCM alen = length__(args);

	if(alen != flen && alen != -1 && flen != -1)
	{
		char *s = "apply: wrong number of arguments; expected: ";
		eputs(s);
		eputs(itoa(flen));
		eputs(", got: ");
		eputs(itoa(alen));
		eputs("\n");
		write_error_(f);
		SCM e = GetSCM2(bad2good(MAKE_STRING0(s), g_cells), g_cells);
		return error(cell_symbol_wrong_number_of_args, cons(e, f));
	}

	return cell_unspecified;
}

SCM check_apply(SCM f, SCM e)  ///((internal))
{
	char* type = 0;

	if(f == cell_f || f == cell_t)
	{
		type = "bool";
	}

	if(f == cell_nil)
	{
		type = "nil";
	}

	if(f == cell_unspecified)
	{
		type = "*unspecified*";
	}

	if(f == cell_undefined)
	{
		type = "*undefined*";
	}

	if(TYPE(f) == TCHAR)
	{
		type = "char";
	}

	if(TYPE(f) == TNUMBER)
	{
		type = "number";
	}

	if(TYPE(f) == TSTRING)
	{
		type = "string";
	}

	if(TYPE(f) == TSTRUCT && builtin_p(f) == cell_f)
	{
		type = "#<...>";
	}

	if(TYPE(f) == TBROKEN_HEART)
	{
		type = "<3";
	}

	if(type)
	{
		char *s = "cannot apply: ";
		eputs(s);
		eputs(type);
		eputs("[");
		write_error_(e);
		eputs("]\n");
		SCM e = GetSCM2(bad2good(MAKE_STRING0(s), g_cells), g_cells);
		return error(cell_symbol_wrong_type_arg, cons(e, f));
	}

	return cell_unspecified;
}

SCM gc_push_frame()  ///((internal))
{
	if(g_stack < 5)
	{
		assert(!"STACK FULL");
	}

	g_stack_array[--g_stack] = (struct scm*) cell_f;
	g_stack_array[--g_stack] = (struct scm*)r0;
	g_stack_array[--g_stack] = (struct scm*)r1;
	g_stack_array[--g_stack] = (struct scm*)r2;
	g_stack_array[--g_stack] = (struct scm*)r3;
	return g_stack;
}

SCM gc_peek_frame()  ///((internal))
{
	r3 = (SCM) g_stack_array[g_stack];
	r2 = (SCM) g_stack_array[g_stack + 1];
	r1 = (SCM) g_stack_array[g_stack + 2];
	r0 = (SCM) g_stack_array[g_stack + 3];
	return (SCM) g_stack_array[g_stack + FRAME_PROCEDURE];
}

SCM gc_pop_frame()  ///((internal))
{
	SCM x = gc_peek_frame();
	g_stack += 5;
	return x;
}

SCM append2(SCM x, SCM y)
{
	if(x == cell_nil)
	{
		return y;
	}

	if(TYPE(x) != TPAIR)
	{
		error(cell_symbol_not_a_pair, cons(x, GetSCM2(cstring_to_symbol("append2"), g_cells)));
	}

	SCM r = cell_nil;

	while(x != cell_nil)
	{
		r = cons(CAR(x), r);
		x = CDR(x);
	}

	return reverse_x_(r, y);
}

SCM append_reverse(SCM x, SCM y)
{
	if(x == cell_nil)
	{
		return y;
	}

	if(TYPE(x) != TPAIR)
	{
		error(cell_symbol_not_a_pair, cons(x, GetSCM2(cstring_to_symbol("append-reverse"), g_cells)));
	}

	while(x != cell_nil)
	{
		y = cons(CAR(x), y);
		x = CDR(x);
	}

	return y;
}

SCM reverse_x_(SCM x, SCM t)
{
	if(x != cell_nil && TYPE(x) != TPAIR)
	{
		error(cell_symbol_not_a_pair, cons(x, GetSCM2(cstring_to_symbol("core:reverse!"), g_cells)));
	}

	SCM r = t;

	while(x != cell_nil)
	{
		t = CDR(x);
		CDR(x) = r;
		r = x;
		x = t;
	}

	return r;
}

SCM pairlis(SCM x, SCM y, SCM a)
{
	if(x == cell_nil)
	{
		return a;
	}

	if(TYPE(x) != TPAIR)
	{
		return cons(cons(x, y), a);
	}

	return cons(cons(car(x), car(y)), pairlis(cdr(x), cdr(y), a));
}

SCM assq(SCM x, SCM a)
{
	if(TYPE(a) != TPAIR)
	{
		return cell_f;
	}

	int t = TYPE(x);

	if(t == TSYMBOL || t == TSPECIAL)
	{
		while(a != cell_nil && x != CAAR(a))
		{
			a = CDR(a);
		}
	}
	else if(t == TCHAR || t == TNUMBER)
	{
		SCM v = VALUE(x);

		while(a != cell_nil && v != VALUE(CAAR(a)))
		{
			a = CDR(a);
		}
	}
	else if(t == TKEYWORD)
	{
		while(a != cell_nil && GetSCM2(bad2good(string_equal_p(x, CAAR(a)), g_cells), g_cells) == cell_f)
		{
			a = CDR(a);
		}
	}
	else
	{
		/* pointer equality, e.g. on strings. */
		while(a != cell_nil && x != CAAR(a))
		{
			a = CDR(a);
		}
	}

	return a != cell_nil ? CAR(a) : cell_f;
}

SCM assoc(SCM x, SCM a)
{
	if(TYPE(x) == TSTRING)
	{
		return assoc_string(x, a);
	}

	while(a != cell_nil && equal2_p(x, CAAR(a)) == cell_f)
	{
		a = CDR(a);
	}

	return a != cell_nil ? CAR(a) : cell_f;
}

SCM set_car_x(SCM x, SCM e)
{
	if(TYPE(x) != TPAIR)
	{
		error(cell_symbol_not_a_pair, cons(x, GetSCM2(cstring_to_symbol("set-car!"), g_cells)));
	}

	CAR(x) = e;
	return cell_unspecified;
}

SCM set_cdr_x(SCM x, SCM e)
{
	if(TYPE(x) != TPAIR)
	{
		error(cell_symbol_not_a_pair, cons(x, GetSCM2(cstring_to_symbol("set-cdr!"), g_cells)));
	}

	CDR(x) = e;
	return cell_unspecified;
}

SCM set_env_x(SCM x, SCM e, SCM a)
{
	SCM p;

	if(TYPE(x) == TVARIABLE)
	{
		p = VARIABLE(x);
	}
	else
	{
		p = assert_defined(x, GetSCM2(bad2good(module_variable(a, x), g_cells), g_cells));
	}

	if(TYPE(p) != TPAIR)
	{
		error(cell_symbol_not_a_pair, cons(p, x));
	}

	return set_cdr_x(p, e);
}

SCM call_lambda(SCM e, SCM x)  ///((internal))
{
	SCM cl = cons(cons(cell_closure, x), x);
	r1 = e;
	r0 = cl;
	return cell_unspecified;
}

SCM make_closure_(SCM args, SCM body, SCM a)  ///((internal))
{
	return make_cell__(TCLOSURE, cell_f, cons(cons(cell_circular, a), cons(args, body)));
}

SCM make_variable_(SCM var)  ///((internal))
{
	return make_cell__(TVARIABLE, var, 0);
}

SCM macro_get_handle(SCM name)
{
	if(TYPE(name) == TSYMBOL)
	{
		return GetSCM2(bad2good(hashq_get_handle(g_macros, name, cell_nil), g_cells), g_cells);
	}

	return cell_f;
}

SCM get_macro(SCM name)  ///((internal))
{
	SCM m = macro_get_handle(name);

	if(m != cell_f)
	{
		return MACRO(CDR(m));
	}

	return cell_f;
}

SCM macro_set_x(SCM name, SCM value)  ///((internal))
{
	return GetSCM2(bad2good(hashq_set_x(g_macros, name, value), g_cells), g_cells);
}

SCM push_cc(SCM p1, SCM p2, SCM a, SCM c)  ///((internal))
{
	SCM x = r3;
	r3 = c;
	r2 = p2;
	gc_push_frame();
	r1 = p1;
	r0 = a;
	r3 = x;
	return cell_unspecified;
}

SCM add_formals(SCM formals, SCM x)
{
	while(TYPE(x) == TPAIR)
	{
		formals = cons(CAR(x), formals);
		x = CDR(x);
	}

	if(TYPE(x) == TSYMBOL)
	{
		formals = cons(x, formals);
	}

	return formals;
}

int formal_p(SCM x, SCM formals)  /// ((internal))
{
	if(TYPE(formals) == TSYMBOL)
	{
		if(x == formals)
		{
			return x;
		}
		else
		{
			return cell_f;
		}
	}

	while(TYPE(formals) == TPAIR && CAR(formals) != x)
	{
		formals = CDR(formals);
	}

	if(TYPE(formals) == TSYMBOL)
	{
		return formals == x;
	}

	return TYPE(formals) == TPAIR;
}

SCM expand_variable_(SCM x, SCM formals, int top_p)  ///((internal))
{
	while(TYPE(x) == TPAIR)
	{
		if(TYPE(CAR(x)) == TPAIR)
		{
			if(CAAR(x) == cell_symbol_lambda)
			{
				SCM f = CAR(CDAR(x));
				formals = add_formals(formals, f);
			}
			else if(CAAR(x) == cell_symbol_define || CAAR(x) == cell_symbol_define_macro)
			{
				SCM f = CAR(CDAR(x));
				formals = add_formals(formals, f);
			}

			if(CAAR(x) != cell_symbol_quote)
			{
				expand_variable_(CAR(x), formals, 0);
			}
		}
		else
		{
			if(CAR(x) == cell_symbol_lambda)
			{
				SCM f = CADR(x);
				formals = add_formals(formals, f);
				x = CDR(x);
			}
			else if(CAR(x) == cell_symbol_define || CAR(x) == cell_symbol_define_macro)
			{
				SCM f = CADR(x);

				if(top_p && TYPE(f) == TPAIR)
				{
					f = CDR(f);
				}

				formals = add_formals(formals, f);
				x = CDR(x);
			}
			else if(CAR(x) == cell_symbol_quote)
			{
				return cell_unspecified;
			}
			else if(TYPE(CAR(x)) == TSYMBOL
			        && CAR(x) != cell_symbol_boot_module
			        && CAR(x) != cell_symbol_current_module
			        && CAR(x) != cell_symbol_primitive_load
			        && !formal_p(CAR(x), formals))
			{
				SCM v = GetSCM2(bad2good(module_variable(r0, CAR(x)), g_cells), g_cells);

				if(v != cell_f)
				{
					CAR(x) = make_variable_(v);
				}
			}
		}

		x = CDR(x);
		top_p = 0;
	}

	return cell_unspecified;
}

SCM expand_variable(SCM x, SCM formals)  ///((internal))
{
	return expand_variable_(x, formals, 1);
}

SCM apply(SCM f, SCM x)  ///((internal))
{
	push_cc(cons(f, x), cell_unspecified, r0, cell_unspecified);
	r3 = cell_vm_apply;
	return eval_apply();
}

// Jam Collector
SCM g_symbol_max;

int open_boot(char *prefix, char const *boot, char const *location);
void read_boot()  ///((internal))
{
	__stdin = -1;
	char prefix[1024];
	char boot[1024];

	if(getenv("MES_BOOT"))
	{
		strcpy(boot, getenv("MES_BOOT"));
	}
	else
	{
		strcpy(boot, "boot-0.scm");
	}

	if(getenv("MES_PREFIX"))
	{
		strcpy(prefix, getenv("MES_PREFIX"));
		strcpy(prefix + strlen(prefix), "/module");
		strcpy(prefix + strlen(prefix), "/mes/");
		__stdin = open_boot(prefix, boot, "MES_PREFIX");
	}

	if(__stdin < 0)
	{
		char const *p = "module/mes/";
		strcpy(prefix, p);
		__stdin = open_boot(prefix, boot, "module");
	}

	if(__stdin < 0)
	{
		strcpy(prefix, "mes/module/mes/");
		__stdin = open_boot(prefix, boot, ".");
	}

	if(__stdin < 0)
	{
		prefix[0] = 0;
		__stdin = open_boot(prefix, boot, "<boot>");
	}

	if(__stdin < 0)
	{
		eputs("mes: boot failed: no such file: ");
		eputs(boot);
		eputs("\n");
		exit(EXIT_FAILURE);
	}

	r2 = read_input_file_env();
	__stdin = STDIN;
}

int get_env_value(char* c, int alt)
{
	char* s = getenv(c);
	if(NULL == s) return alt;
	return numerate_string(s);
}

SCM mes_environment(int argc, char *argv[]);
int main(int argc, char *argv[])
{
	__ungetc_buf = calloc((RLIMIT_NOFILE + 1), sizeof(int));
	g_continuations = 0;
	g_symbols = 0;
	g_stack = 0;
	r0 = 0;
	r1 = 0;
	r2 = 0;
	r3 = 0;
	m0 = 0;
	g_macros = 0;
	g_ports = 1;
	g_cells = 0;
	g_news = 0;
	__stdin = STDIN;
	__stdout = STDOUT;
	__stderr = STDERR;

	g_debug = get_env_value("MES_DEBUG", 0);

	if(g_debug > 1)
	{
		eputs(";;; MODULEDIR=");
		eputs("module");
		eputs("\n");
	}

	MAX_ARENA_SIZE = get_env_value("MES_MAX_ARENA", 100000000);
	ARENA_SIZE = get_env_value("MES_ARENA", 10000000);
	JAM_SIZE = ARENA_SIZE / 10;
	JAM_SIZE = get_env_value("MES_JAM", 20000);
	GC_SAFETY = ARENA_SIZE / 100;
	GC_SAFETY = get_env_value("MES_SAFETY", 2000);
	STACK_SIZE = get_env_value("MES_STACK", 20000);
	MAX_STRING = get_env_value("MES_MAX_STRING", 524288);

	SCM a = mes_environment(argc, argv);
	a = GetSCM2(mes_builtins(Getstructscm2(a, g_cells)), g_cells);
	a = init_time(a);
	m0 = GetSCM2(bad2good(make_initial_module(a), g_cells), g_cells);
	g_macros = GetSCM2(bad2good(make_hash_table_(0), g_cells), g_cells);

	if(g_debug > 4)
	{
		module_printer(m0);
	}

	read_boot();
	push_cc(r2, cell_unspecified, r0, cell_unspecified);

	if(g_debug > 2)
	{
		eputs("\ngc stats: [");
		eputs(itoa(g_free));
		eputs("]\n");
	}

	if(g_debug > 3)
	{
		eputs("program: ");
		write_error_(r1);
		eputs("\n");
	}

	r3 = cell_vm_begin_expand;
	r1 = eval_apply();

	if(g_debug)
	{
		write_error_(r1);
		eputs("\n");
	}

	if(g_debug)
	{
		if(g_debug > 4)
		{
			module_printer(m0);
		}

		eputs("\ngc stats: [");
		eputs(itoa(g_free));
		MAX_ARENA_SIZE = 0;
		gc(g_stack);
		eputs(" => ");
		eputs(itoa(g_free));
		eputs("]\n");

		if(g_debug > 4)
		{
			module_printer(m0);
		}

		eputs("\n");
		gc(g_stack);
		eputs(" => ");
		eputs(itoa(g_free));
		eputs("]\n");

		if(g_debug > 4)
		{
			module_printer(m0);
		}

		eputs("\n");
		gc(g_stack);
		eputs(" => ");
		eputs(itoa(g_free));
		eputs("]\n");

		if(g_debug > 4)
		{
			module_printer(m0);
		}

		if(g_debug > 3)
		{
			eputs("ports:");
			write_error_(g_ports);
			eputs("\n");
		}

		eputs("\n");
	}

	return 0;
}
