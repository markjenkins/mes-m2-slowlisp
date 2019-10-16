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

/* Imported Functions */
// src/gc.mes
struct scm* gc_check ();
struct scm* gc ();
// src/hash.mes
struct scm* hashq (struct scm* x, struct scm* size);
struct scm* hash (struct scm* x, struct scm* size);
struct scm* hashq_get_handle (struct scm* table, struct scm* key, struct scm* dflt);
struct scm* hashq_ref (struct scm* table, struct scm* key, struct scm* dflt);
struct scm* hash_ref_ (struct scm* table, struct scm* key, struct scm* dflt);
struct scm* hashq_set_x_ (struct scm* table, struct scm* key, struct scm* value);
struct scm* hash_set_x (struct scm* table, struct scm* key, struct scm* value);
struct scm* hash_table_printer (struct scm* table);
struct scm* make_hash_table_(long size);
struct scm* make_hash_table (struct scm* x);
// src/lib.mes
struct scm* display_ (struct scm* x);
struct scm* display_error_ (struct scm* x);
struct scm* display_port_ (struct scm* x, struct scm* p);
struct scm* write_ (struct scm* x);
struct scm* write_error_ (struct scm* x);
struct scm* write_port_ (struct scm* x, struct scm* p);
struct scm* exit_ (struct scm* x);
struct scm* frame_printer (struct scm* frame);
struct scm* make_stack ();
struct scm* stack_length (struct scm* stack);
struct scm* stack_ref (struct scm* stack, struct scm* index);
struct scm* xassq (struct scm* x, struct scm* a);
struct scm* memq (struct scm* x, struct scm* a);
struct scm* equal2_p (struct scm* a, struct scm* b);
struct scm* last_pair (struct scm* x);
struct scm* pair_p (struct scm* x);
// src/math.mes
struct scm* greater_p (struct scm* x);
struct scm* less_p (struct scm* x);
struct scm* is_p (struct scm* x);
struct scm* minus (struct scm* x);
struct scm* plus (struct scm* x);
struct scm* divide (struct scm* x);
struct scm* modulo (struct scm* a, struct scm* b);
struct scm* multiply (struct scm* x);
struct scm* logand (struct scm* x);
struct scm* logior (struct scm* x);
struct scm* lognot (struct scm* x);
struct scm* logxor (struct scm* x);
struct scm* ash (struct scm* n, struct scm* count);
// src/mes.mes
struct scm* acons (struct scm* key, struct scm* value, struct scm* alist);
struct scm* add_formals (struct scm* formals, struct scm* x);
struct scm* append2 (struct scm* x, struct scm* y);
struct scm* arity_ (struct scm* x);
struct scm* assoc (struct scm* x, struct scm* a);
struct scm* assq (struct scm* x, struct scm* a);
struct scm* call (struct scm* fn, struct scm* x);
struct scm* car (struct scm* x);
struct scm* cdr (struct scm* x);
struct scm* cons(struct scm* x, struct scm* y);
struct scm* eq_p (struct scm* x, struct scm* y);
struct scm* error (struct scm* key, struct scm* x);
struct scm* eval_apply ();
struct scm* length (struct scm* x);
struct scm* list (struct scm* x);
struct scm* macro_get_handle (struct scm* name);
struct scm* make_cell_(struct scm* type, struct scm* car, struct scm* cdr);
struct scm* make_number(SCM n);
struct scm* null_p (struct scm* x);
struct scm* pairlis (struct scm* x, struct scm* y, struct scm* a);
struct scm* reverse_x_ (struct scm* x, struct scm* t);
struct scm* set_car_x (struct scm* x, struct scm* e);
struct scm* set_cdr_x (struct scm* x, struct scm* e);
struct scm* set_env_x (struct scm* x, struct scm* e, struct scm* a);
struct scm* type_ (struct scm* x);
struct scm* values (struct scm* x);
struct scm* builtin_printer(struct scm* builtin);

// src/module.mes
struct scm* make_module_type_ ();
struct scm* module_printer (struct scm* module);
struct scm* module_variable_ (struct scm* module, struct scm* name);
struct scm* module_ref_ (struct scm* module, struct scm* name);
struct scm* module_define_x (struct scm* module, struct scm* name, struct scm* value);
// src/posix.mes
struct scm* peek_byte ();
struct scm* read_byte ();
struct scm* unread_byte (struct scm* i);
struct scm* peek_char ();
struct scm* read_char (struct scm* port);
struct scm* unread_char (struct scm* i);
struct scm* write_char (struct scm* i);
struct scm* write_byte (struct scm* x);
struct scm* getenv_ (struct scm* s);
struct scm* setenv_ (struct scm* s, struct scm* v);
struct scm* access_p (struct scm* file_name, struct scm* mode);
struct scm* current_input_port ();
struct scm* open_input_file (struct scm* file_name);
struct scm* open_input_string (struct scm* string);
struct scm* set_current_input_port (struct scm* port);
struct scm* current_output_port ();
struct scm* current_error_port ();
struct scm* open_output_file (struct scm* x);
struct scm* set_current_output_port (struct scm* port);
struct scm* set_current_error_port (struct scm* port);
struct scm* chmod_ (struct scm* file_name, struct scm* mode);
struct scm* isatty_p (struct scm* port);
struct scm* primitive_fork ();
struct scm* execl_ (struct scm* file_name, struct scm* args);
struct scm* waitpid_ (struct scm* pid, struct scm* options);
struct scm* current_time ();
struct scm* gettimeofday_ ();
struct scm* get_internal_run_time ();
struct scm* getcwd_ ();
struct scm* dup_ (struct scm* port);
struct scm* dup2_ (struct scm* old, struct scm* new);
struct scm* delete_file (struct scm* file_name);
// src/reader.mes
struct scm* read_input_file_env_ (struct scm* e, struct scm* a);
struct scm* read_input_file_env ();
struct scm* read_env (struct scm* a);
struct scm* reader_read_sexp (struct scm* c, struct scm* a);
struct scm* reader_read_character ();
struct scm* reader_read_binary ();
struct scm* reader_read_octal ();
struct scm* reader_read_hex ();
struct scm* reader_read_string ();
// src/strings.mes
struct scm* string_equal_p_ (struct scm* a, struct scm* b);
struct scm* symbol_to_string_ (struct scm* symbol);
struct scm* symbol_to_keyword_ (struct scm* symbol);
struct scm* keyword_to_string (struct scm* keyword);
struct scm* string_to_symbol (struct scm* string);
struct scm* make_symbol_ (struct scm* string);
struct scm* string_to_list (struct scm* string);
struct scm* list_to_string (struct scm* list);
struct scm* read_string (struct scm* port);
struct scm* string_append (struct scm* x);
struct scm* string_length (struct scm* string);
struct scm* string_ref (struct scm* str, struct scm* k);
// src/struct.mes
struct scm* make_struct_(struct scm* type, struct scm* fields, struct scm* printer);
struct scm* struct_length (struct scm* x);
struct scm* struct_ref (struct scm* x, struct scm* i);
struct scm* struct_set_x (struct scm* x, struct scm* i, struct scm* e);
// src/vector.mes
struct scm* make_vector_ (struct scm* n);
struct scm* vector_length (struct scm* x);
struct scm* vector_ref (struct scm* x, struct scm* i);
struct scm* vector_entry (struct scm* x);
struct scm* vector_set_x (struct scm* x, struct scm* i, struct scm* e);
struct scm* list_to_vector (struct scm* x);
struct scm* vector_to_list (struct scm* v);
struct scm* init_time(struct scm* a);

/* Internal functions required*/
struct scm* make_string(char const* s, int length);
struct scm* make_struct (struct scm* type, struct scm* fields, struct scm* printer);
struct scm* make_string_(char const* s);
struct scm* cstring_to_symbol(char const *s);
struct scm* symbol_to_string (struct scm* symbol);
struct scm* struct_ref_(struct scm* x, SCM i);
int fdputc (int c, int fd);
int fdputs (char const* s, int fd);
int eputs (char const* s);
void init_symbol(struct scm* x, SCM type, char const* name)
{
	struct scm* y = x;
	y->type = type;
	int l = strlen(name);
	struct scm* string = make_string(name, l);
	y->length = l;
	y->string = string->string;
	hash_set_x(g_symbols, string, x);
}

struct scm* mes_symbols()  ///((internal))
{
	g_symbol_max = g_free;
	g_symbols = make_hash_table_(500);
	init_symbol(cell_nil, TSPECIAL, "()");
	init_symbol(cell_f, TSPECIAL, "#f");
	init_symbol(cell_t, TSPECIAL, "#t");
	init_symbol(cell_dot, TSPECIAL, ".");
	init_symbol(cell_arrow, TSPECIAL, "=>");
	init_symbol(cell_undefined, TSPECIAL, "*undefined*");
	init_symbol(cell_unspecified, TSPECIAL, "*unspecified*");
	init_symbol(cell_closure, TSPECIAL, "*closure*");
	init_symbol(cell_circular, TSPECIAL, "*circular*");
	init_symbol(cell_begin, TSPECIAL, "*begin*");
	init_symbol(cell_call_with_current_continuation, TSPECIAL, "*call/cc*");
	init_symbol(cell_vm_apply, TSPECIAL, "core:apply");
	init_symbol(cell_vm_apply2, TSPECIAL, "*vm-apply2*");
	init_symbol(cell_vm_begin, TSPECIAL, "*vm-begin*");
	init_symbol(cell_vm_begin_eval, TSPECIAL, "*vm:begin-eval*");
	init_symbol(cell_vm_begin_expand, TSPECIAL, "core:eval");
	init_symbol(cell_vm_begin_expand_eval, TSPECIAL, "*vm:begin-expand-eval*");
	init_symbol(cell_vm_begin_expand_macro, TSPECIAL, "*vm:begin-expand-macro*");
	init_symbol(cell_vm_begin_expand_primitive_load, TSPECIAL, "*vm:core:begin-expand-primitive-load*");
	init_symbol(cell_vm_begin_primitive_load, TSPECIAL, "*vm:core:begin-primitive-load*");
	init_symbol(cell_vm_begin_read_input_file, TSPECIAL, "*vm-begin-read-input-file*");
	init_symbol(cell_vm_call_with_current_continuation2, TSPECIAL, "*vm-call-with-current-continuation2*");
	init_symbol(cell_vm_call_with_values2, TSPECIAL, "*vm-call-with-values2*");
	init_symbol(cell_vm_eval, TSPECIAL, "core:eval-expanded");
	init_symbol(cell_vm_eval2, TSPECIAL, "*vm-eval2*");
	init_symbol(cell_vm_eval_check_func, TSPECIAL, "*vm-eval-check-func*");
	init_symbol(cell_vm_eval_define, TSPECIAL, "*vm-eval-define*");
	init_symbol(cell_vm_eval_macro_expand_eval, TSPECIAL, "*vm:eval-macro-expand-eval*");
	init_symbol(cell_vm_eval_macro_expand_expand, TSPECIAL, "*vm:eval-macro-expand-expand*");
	init_symbol(cell_vm_eval_pmatch_car, TSPECIAL, "*vm-eval-pmatch-car*");
	init_symbol(cell_vm_eval_pmatch_cdr, TSPECIAL, "*vm-eval-pmatch-cdr*");
	init_symbol(cell_vm_eval_set_x, TSPECIAL, "*vm-eval-set!*");
	init_symbol(cell_vm_evlis, TSPECIAL, "*vm-evlis*");
	init_symbol(cell_vm_evlis2, TSPECIAL, "*vm-evlis2*");
	init_symbol(cell_vm_evlis3, TSPECIAL, "*vm-evlis3*");
	init_symbol(cell_vm_if, TSPECIAL, "*vm-if*");
	init_symbol(cell_vm_if_expr, TSPECIAL, "*vm-if-expr*");
	init_symbol(cell_vm_macro_expand, TSPECIAL, "core:macro-expand");
	init_symbol(cell_vm_macro_expand_car, TSPECIAL, "*vm:core:macro-expand-car*");
	init_symbol(cell_vm_macro_expand_cdr, TSPECIAL, "*vm:macro-expand-cdr*");
	init_symbol(cell_vm_macro_expand_define, TSPECIAL, "*vm:core:macro-expand-define*");
	init_symbol(cell_vm_macro_expand_define_macro, TSPECIAL, "*vm:core:macro-expand-define-macro*");
	init_symbol(cell_vm_macro_expand_lambda, TSPECIAL, "*vm:core:macro-expand-lambda*");
	init_symbol(cell_vm_macro_expand_set_x, TSPECIAL, "*vm:core:macro-expand-set!*");
	init_symbol(cell_vm_return, TSPECIAL, "*vm-return*");
	init_symbol(cell_symbol_dot, TSYMBOL, "*dot*");
	init_symbol(cell_symbol_lambda, TSYMBOL, "lambda");
	init_symbol(cell_symbol_begin, TSYMBOL, "begin");
	init_symbol(cell_symbol_if, TSYMBOL, "if");
	init_symbol(cell_symbol_quote, TSYMBOL, "quote");
	init_symbol(cell_symbol_define, TSYMBOL, "define");
	init_symbol(cell_symbol_define_macro, TSYMBOL, "define-macro");
	init_symbol(cell_symbol_quasiquote, TSYMBOL, "quasiquote");
	init_symbol(cell_symbol_unquote, TSYMBOL, "unquote");
	init_symbol(cell_symbol_unquote_splicing, TSYMBOL, "unquote-splicing");
	init_symbol(cell_symbol_syntax, TSYMBOL, "syntax");
	init_symbol(cell_symbol_quasisyntax, TSYMBOL, "quasisyntax");
	init_symbol(cell_symbol_unsyntax, TSYMBOL, "unsyntax");
	init_symbol(cell_symbol_unsyntax_splicing, TSYMBOL, "unsyntax-splicing");
	init_symbol(cell_symbol_set_x, TSYMBOL, "set!");
	init_symbol(cell_symbol_sc_expand, TSYMBOL, "sc-expand");
	init_symbol(cell_symbol_macro_expand, TSYMBOL, "macro-expand");
	init_symbol(cell_symbol_portable_macro_expand, TSYMBOL, "portable-macro-expand");
	init_symbol(cell_symbol_sc_expander_alist, TSYMBOL, "*sc-expander-alist*");
	init_symbol(cell_symbol_call_with_values, TSYMBOL, "call-with-values");
	init_symbol(cell_symbol_call_with_current_continuation, TSYMBOL, "call-with-current-continuation");
	init_symbol(cell_symbol_boot_module, TSYMBOL, "boot-module");
	init_symbol(cell_symbol_current_module, TSYMBOL, "current-module");
	init_symbol(cell_symbol_primitive_load, TSYMBOL, "primitive-load");
	init_symbol(cell_symbol_read_input_file, TSYMBOL, "read-input-file");
	init_symbol(cell_symbol_write, TSYMBOL, "write");
	init_symbol(cell_symbol_display, TSYMBOL, "display");
	init_symbol(cell_symbol_car, TSYMBOL, "car");
	init_symbol(cell_symbol_cdr, TSYMBOL, "cdr");
	init_symbol(cell_symbol_not_a_number, TSYMBOL, "not-a-number");
	init_symbol(cell_symbol_not_a_pair, TSYMBOL, "not-a-pair");
	init_symbol(cell_symbol_system_error, TSYMBOL, "system-error");
	init_symbol(cell_symbol_throw, TSYMBOL, "throw");
	init_symbol(cell_symbol_unbound_variable, TSYMBOL, "unbound-variable");
	init_symbol(cell_symbol_wrong_number_of_args, TSYMBOL, "wrong-number-of-args");
	init_symbol(cell_symbol_wrong_type_arg, TSYMBOL, "wrong-type-arg");
	init_symbol(cell_symbol_buckets, TSYMBOL, "buckets");
	init_symbol(cell_symbol_builtin, TSYMBOL, "<builtin>");
	init_symbol(cell_symbol_frame, TSYMBOL, "<frame>");
	init_symbol(cell_symbol_hashq_table, TSYMBOL, "<hashq-table>");
	init_symbol(cell_symbol_module, TSYMBOL, "<module>");
	init_symbol(cell_symbol_procedure, TSYMBOL, "procedure");
	init_symbol(cell_symbol_record_type, TSYMBOL, "<record-type>");
	init_symbol(cell_symbol_size, TSYMBOL, "size");
	init_symbol(cell_symbol_stack, TSYMBOL, "<stack>");
	init_symbol(cell_symbol_argv, TSYMBOL, "%argv");
	init_symbol(cell_symbol_mes_prefix, TSYMBOL, "%prefix");
	init_symbol(cell_symbol_mes_version, TSYMBOL, "%version");
	init_symbol(cell_symbol_internal_time_units_per_second, TSYMBOL, "internal-time-units-per-second");
	init_symbol(cell_symbol_compiler, TSYMBOL, "%compiler");
	init_symbol(cell_symbol_arch, TSYMBOL, "%arch");
	init_symbol(cell_symbol_pmatch_car, TSYMBOL, "pmatch-car");
	init_symbol(cell_symbol_pmatch_cdr, TSYMBOL, "pmatch-cdr");
	init_symbol(cell_type_bytes, TSYMBOL, "<cell:bytes>");
	init_symbol(cell_type_char, TSYMBOL, "<cell:char>");
	init_symbol(cell_type_closure, TSYMBOL, "<cell:closure>");
	init_symbol(cell_type_continuation, TSYMBOL, "<cell:continuation>");
	init_symbol(cell_type_function, TSYMBOL, "<cell:function>");
	init_symbol(cell_type_keyword, TSYMBOL, "<cell:keyword>");
	init_symbol(cell_type_macro, TSYMBOL, "<cell:macro>");
	init_symbol(cell_type_number, TSYMBOL, "<cell:number>");
	init_symbol(cell_type_pair, TSYMBOL, "<cell:pair>");
	init_symbol(cell_type_port, TSYMBOL, "<cell:port>");
	init_symbol(cell_type_ref, TSYMBOL, "<cell:ref>");
	init_symbol(cell_type_special, TSYMBOL, "<cell:special>");
	init_symbol(cell_type_string, TSYMBOL, "<cell:string>");
	init_symbol(cell_type_struct, TSYMBOL, "<cell:struct>");
	init_symbol(cell_type_symbol, TSYMBOL, "<cell:symbol>");
	init_symbol(cell_type_values, TSYMBOL, "<cell:values>");
	init_symbol(cell_type_variable, TSYMBOL, "<cell:variable>");
	init_symbol(cell_type_vector, TSYMBOL, "<cell:vector>");
	init_symbol(cell_type_broken_heart, TSYMBOL, "<cell:broken-heart>");
	init_symbol(cell_symbol_test, TSYMBOL, "%%test");
	struct scm* a = cell_nil;
	a = acons(cell_symbol_call_with_values, cell_symbol_call_with_values, a);
	a = acons(cell_symbol_boot_module, cell_symbol_boot_module, a);
	a = acons(cell_symbol_current_module, cell_symbol_current_module, a);
	a = acons(cell_symbol_call_with_current_continuation, cell_call_with_current_continuation, a);
	a = acons(cell_symbol_mes_version, make_string_("git"), a); // FIXME
	a = acons(cell_symbol_mes_prefix, make_string_("mes"), a);  // FIXME
	a = acons(cell_type_bytes, make_number((long)TBYTES), a);
	a = acons(cell_type_char, make_number(TCHAR), a);
	a = acons(cell_type_closure, make_number(TCLOSURE), a);
	a = acons(cell_type_continuation, make_number(TCONTINUATION), a);
	a = acons(cell_type_keyword, make_number(TKEYWORD), a);
	a = acons(cell_type_macro, make_number(TMACRO), a);
	a = acons(cell_type_number, make_number(TNUMBER), a);
	a = acons(cell_type_pair, make_number(TPAIR), a);
	a = acons(cell_type_port, make_number(TPORT), a);
	a = acons(cell_type_ref, make_number(TREF), a);
	a = acons(cell_type_special, make_number(TSPECIAL), a);
	a = acons(cell_type_string, make_number(TSTRING), a);
	a = acons(cell_type_struct, make_number(TSTRUCT), a);
	a = acons(cell_type_symbol, make_number(TSYMBOL), a);
	a = acons(cell_type_values, make_number(TVALUES), a);
	a = acons(cell_type_variable, make_number(TVARIABLE), a);
	a = acons(cell_type_vector, make_number(TVECTOR), a);
	a = acons(cell_type_broken_heart, make_number(TBROKEN_HEART), a);
	a = acons(cell_closure, a, a);
	return a;
}

struct scm* make_builtin(struct scm* builtin_type, struct scm* name, struct scm* arity, struct scm* function)
{
	struct scm* v = cell_nil;
	v = cons(function, v);
	v = cons(arity, v);
	v = cons(name, v);
	v = cons(cell_symbol_builtin, v);
	return make_struct(builtin_type, v, cstring_to_symbol("builtin-printer"));
}

struct scm* make_builtin_type()  ///(internal))
{
	struct scm* fields = cell_nil;
	fields = cons(cstring_to_symbol("address"), fields);
	fields = cons(cstring_to_symbol("arity"), fields);
	fields = cons(cstring_to_symbol("name"), fields);
	fields = cons(fields, cell_nil);
	fields = cons(cell_symbol_builtin, fields);
	return make_struct(cell_symbol_record_type, fields, cell_unspecified);
}

struct scm* init_builtin(struct scm* builtin_type, char const* name, int arity, struct scm*(*function)(), struct scm* a)
{
	struct scm* s = cstring_to_symbol(name);
	return acons(s, make_builtin(builtin_type, symbol_to_string(s), make_number(arity), make_number((SCM)function)), a);
}

struct scm* builtin_name(struct scm* builtin)
{
	return struct_ref_(builtin, 3);
}

struct scm* builtin_arity(struct scm* builtin)
{
	return struct_ref_(builtin, 4);
}

void* builtin_function(struct scm* builtin)
{
	return (void*)struct_ref_(builtin, 5)->cdr;
}

struct scm* builtin_p(struct scm* x)
{
	struct scm* y = x;
	if (y->type == TSTRUCT && struct_ref_(x, 2) == cell_symbol_builtin) return cell_t;
	return cell_f;
}

struct scm* builtin_printer(struct scm* builtin)
{
	fdputs("#<procedure ", __stdout);
	display_(builtin_name(builtin));
	fdputc(' ', __stdout);
	int arity = builtin_arity(builtin)->value;

	if(arity == -1)
	{
		fdputc('_', __stdout);
	}
	else
	{
		fdputc('(', __stdout);

		for(int i = 0; i < arity; i++)
		{
			if(i)
			{
				fdputc(' ', __stdout);
			}

			fdputc('_', __stdout);
		}
	}

	fdputc('>', __stdout);
	return cell_unspecified;
}

struct scm* apply_builtin(struct scm* fn, struct scm* x)  ///((internal))
{
	int arity = builtin_arity(fn)->value;
	struct scm* y = x;

	if((arity > 0 || arity == -1) && x != cell_nil && y->car->type == TVALUES)
	{
		y = cons(y->car->cdr->car, y->cdr);
	}

	if(arity > 1 || arity == -1)
	{
		if(x != cell_nil && y->cdr->type == TPAIR)
		{
			if(x->cdr->car->type == TVALUES)
			{
				y = cons(y->car, cons(y->cdr->car->cdr->car, y->cdr));
			}
		}
	}

	if(arity == 0)
	{
		//function0_t fp = f->function;
		FUNCTION0* fp = builtin_function(fn);
		return fp();
	}
	else if(arity == 1)
	{
		//function1_t fp = f->function;
		FUNCTION1* fp = builtin_function(fn);
		return fp(y->car);
	}
	else if(arity == 2)
	{
		//function2_t fp = f->function;
		FUNCTION2* fp = builtin_function(fn);
		return fp(y->car, y->cdr->car);
	}
	else if(arity == 3)
	{
		//function3_t fp = f->function;
		FUNCTION3* fp = builtin_function(fn);
		return fp(y->car, y->cdr->car, y->cdr->cdr->car);
	}
	else if(arity == -1)
	{
		//functionn_t fp = f->function;
		FUNCTION1* fp = builtin_function(fn);
		return fp(y);
	}

	return cell_unspecified;
}


struct scm* mes_builtins(struct scm* a)  ///((internal))
{
	// TODO minimal: cons, car, cdr, list, null_p, eq_p minus, plus,  display_, display_error_, getenv
	struct scm* builtin_type = make_builtin_type();
	/* src/gc.mes */
	a = init_builtin(builtin_type, "prim:gc-check", 0, &gc_check, a);
	a = init_builtin(builtin_type, "prim:gc", 0, &gc, a);
	/* src/hash.mes */
	a = init_builtin(builtin_type, "prim:hashq", 2, &hashq, a);
	a = init_builtin(builtin_type, "prim:hash", 2, &hash, a);
	a = init_builtin(builtin_type, "prim:hashq-get-handle", 3, &hashq_get_handle, a);
	a = init_builtin(builtin_type, "prim:hashq-ref", 3, &hashq_ref, a);
	a = init_builtin(builtin_type, "prim:hash-ref", 3, &hash_ref_, a);
	a = init_builtin(builtin_type, "prim:hashq-set!", 3, &hashq_set_x_, a);
	a = init_builtin(builtin_type, "prim:hash-set!", 3, &hash_set_x, a);
	a = init_builtin(builtin_type, "prim:hash-table-printer", 1, &hash_table_printer, a);
	a = init_builtin(builtin_type, "prim:make-hash-table", 1, &make_hash_table, a);
	/* src/lib.mes */
	a = init_builtin(builtin_type, "prim:display", 1, &display_, a);
	a = init_builtin(builtin_type, "prim:display-error", 1, &display_error_, a);
	a = init_builtin(builtin_type, "prim:display-port", 2, &display_port_, a);
	a = init_builtin(builtin_type, "prim:write", 1, &write_, a);
	a = init_builtin(builtin_type, "prim:write-error", 1, &write_error_, a);
	a = init_builtin(builtin_type, "prim:write-port", 2, &write_port_, a);
	a = init_builtin(builtin_type, "prim:exit", 1, &exit_, a);
	a = init_builtin(builtin_type, "prim:frame-printer", 1, &frame_printer, a);
	a = init_builtin(builtin_type, "prim:make-stack", -1, &make_stack, a);
	a = init_builtin(builtin_type, "prim:stack-length", 1, &stack_length, a);
	a = init_builtin(builtin_type, "prim:stack-ref", 2, &stack_ref, a);
	a = init_builtin(builtin_type, "prim:xassq", 2, &xassq, a);
	a = init_builtin(builtin_type, "prim:memq", 2, &memq, a);
	a = init_builtin(builtin_type, "prim:equal2?", 2, &equal2_p, a);
	a = init_builtin(builtin_type, "prim:last-pair", 1, &last_pair, a);
	a = init_builtin(builtin_type, "prim:pair?", 1, &pair_p, a);
	/* src/math.mes */
	a = init_builtin(builtin_type, "prim:greater_p", -1, &greater_p, a);
	a = init_builtin(builtin_type, "prim:less_p", -1, &less_p, a);
	a = init_builtin(builtin_type, "prim:is_p", -1, &is_p, a);
	a = init_builtin(builtin_type, "prim:minus", -1, &minus, a);
	a = init_builtin(builtin_type, "prim:plus", -1, &plus, a);
	a = init_builtin(builtin_type, "prim:divide", -1, &divide, a);
	a = init_builtin(builtin_type, "prim:modulo", 2, &modulo, a);
	a = init_builtin(builtin_type, "prim:multiply", -1, &multiply, a);
	a = init_builtin(builtin_type, "prim:logand", -1, &logand, a);
	a = init_builtin(builtin_type, "prim:logior", -1, &logior, a);
	a = init_builtin(builtin_type, "prim:lognot", 1, &lognot, a);
	a = init_builtin(builtin_type, "prim:logxor", -1, &logxor, a);
	a = init_builtin(builtin_type, "prim:ash", 2, &ash, a);
	/* src/mes.mes */
	a = init_builtin(builtin_type, "prim:make-cell", 3, &make_cell_, a);
	a = init_builtin(builtin_type, "prim:type", 1, &type_, a);
	a = init_builtin(builtin_type, "prim:car", 1, &car, a);
	a = init_builtin(builtin_type, "prim:cdr", 1, &cdr, a);
	a = init_builtin(builtin_type, "prim:cons", 2, &cons, a);
	a = init_builtin(builtin_type, "prim:list", -1, &list, a);
	a = init_builtin(builtin_type, "prim:null?", 1, &null_p, a);
	a = init_builtin(builtin_type, "prim:eq?", 2, &eq_p, a);
	a = init_builtin(builtin_type, "prim:values", -1, &values, a);
	a = init_builtin(builtin_type, "prim:acons", 3, &acons, a);
	a = init_builtin(builtin_type, "prim:length", 1, &length, a);
	a = init_builtin(builtin_type, "prim:error", 2, &error, a);
	a = init_builtin(builtin_type, "prim:append2", 2, &append2, a);
	a = init_builtin(builtin_type, "prim:reverse!", 2, &reverse_x_, a);
	a = init_builtin(builtin_type, "prim:pairlis", 3, &pairlis, a);
	a = init_builtin(builtin_type, "prim:assq", 2, &assq, a);
	a = init_builtin(builtin_type, "prim:assoc", 2, &assoc, a);
	a = init_builtin(builtin_type, "prim:set-car!", 2, &set_car_x, a);
	a = init_builtin(builtin_type, "prim:set-cdr!", 2, &set_cdr_x, a);
	a = init_builtin(builtin_type, "prim:set-env!", 3, &set_env_x, a);
	a = init_builtin(builtin_type, "prim:macro-get-handle", 1, &macro_get_handle, a);
	a = init_builtin(builtin_type, "prim:add-formals", 2, &add_formals, a);
	a = init_builtin(builtin_type, "prim:eval-apply", 0, &eval_apply, a);
	a = init_builtin(builtin_type, "prim:make-builtin-type", 0, &make_builtin_type, a);
	a = init_builtin(builtin_type, "prim:make-builtin", 4, &make_builtin, a);
	a = init_builtin(builtin_type, "prim:builtin-name", 1, &builtin_name, a);
	a = init_builtin(builtin_type, "prim:builtin-arity", 1, &builtin_arity, a);
	a = init_builtin(builtin_type, "prim:builtin?", 1, &builtin_p, a);
	a = init_builtin(builtin_type, "prim:builtin-printer", 1, &builtin_printer, a);
	/* src/module.mes */
	a = init_builtin(builtin_type, "prim:make-module-type", 0, &make_module_type_, a);
	a = init_builtin(builtin_type, "prim:module-printer", 1, &module_printer, a);
	a = init_builtin(builtin_type, "prim:module-variable", 2, &module_variable_, a);
	a = init_builtin(builtin_type, "prim:module-ref", 2, &module_ref_, a);
	a = init_builtin(builtin_type, "prim:module-define!", 3, &module_define_x, a);
	/* src/posix.mes */
	a = init_builtin(builtin_type, "prim:peek-byte", 0, &peek_byte, a);
	a = init_builtin(builtin_type, "prim:read-byte", 0, &read_byte, a);
	a = init_builtin(builtin_type, "prim:unread-byte", 1, &unread_byte, a);
	a = init_builtin(builtin_type, "prim:peek-char", 0, &peek_char, a);
	a = init_builtin(builtin_type, "prim:read-char", -1, &read_char, a);
	a = init_builtin(builtin_type, "prim:unread-char", 1, &unread_char, a);
	a = init_builtin(builtin_type, "prim:write-char", -1, &write_char, a);
	a = init_builtin(builtin_type, "prim:write-byte", -1, &write_byte, a);
	a = init_builtin(builtin_type, "prim:getenv", 1, &getenv_, a);
	a = init_builtin(builtin_type, "prim:setenv", 2, &setenv_, a);
	a = init_builtin(builtin_type, "prim:access?", 2, &access_p, a);
	a = init_builtin(builtin_type, "prim:current-input-port", 0, &current_input_port, a);
	a = init_builtin(builtin_type, "prim:open-input-file", 1, &open_input_file, a);
	a = init_builtin(builtin_type, "prim:open-input-string", 1, &open_input_string, a);
	a = init_builtin(builtin_type, "prim:set-current-input-port", 1, &set_current_input_port, a);
	a = init_builtin(builtin_type, "prim:current-output-port", 0, &current_output_port, a);
	a = init_builtin(builtin_type, "prim:current-error-port", 0, &current_error_port, a);
	a = init_builtin(builtin_type, "prim:open-output-file", -1, &open_output_file, a);
	a = init_builtin(builtin_type, "prim:set-current-output-port", 1, &set_current_output_port, a);
	a = init_builtin(builtin_type, "prim:set-current-error-port", 1, &set_current_error_port, a);
	a = init_builtin(builtin_type, "prim:chmod", 2, &chmod_, a);
	a = init_builtin(builtin_type, "prim:isatty?", 1, &isatty_p, a);
	a = init_builtin(builtin_type, "prim:primitive-fork", 0, &primitive_fork, a);
	a = init_builtin(builtin_type, "prim:execl", 2, &execl_, a);
	a = init_builtin(builtin_type, "prim:waitpid", 2, &waitpid_, a);
	a = init_builtin(builtin_type, "prim:current-time", 0, &current_time, a);
	a = init_builtin(builtin_type, "prim:gettimeofday", 0, &gettimeofday_, a);
	a = init_builtin(builtin_type, "prim:get-internal-run-time", 0, &get_internal_run_time, a);
	a = init_builtin(builtin_type, "prim:getcwd", 0, &getcwd_, a);
	a = init_builtin(builtin_type, "prim:dup", 1, &dup_, a);
	a = init_builtin(builtin_type, "prim:dup2", 2, &dup2_, a);
	a = init_builtin(builtin_type, "prim:delete-file", 1, &delete_file, a);
	/* src/reader.mes */
	a = init_builtin(builtin_type, "prim:read-input-file-env", 2, &read_input_file_env_, a);
	a = init_builtin(builtin_type, "prim:read-env", 1, &read_env, a);
	a = init_builtin(builtin_type, "prim:reader-read-sexp", 3, &reader_read_sexp, a);
	a = init_builtin(builtin_type, "prim:reader-read-character", 0, &reader_read_character, a);
	a = init_builtin(builtin_type, "prim:reader-read-binary", 0, &reader_read_binary, a);
	a = init_builtin(builtin_type, "prim:reader-read-octal", 0, &reader_read_octal, a);
	a = init_builtin(builtin_type, "prim:reader-read-hex", 0, &reader_read_hex, a);
	a = init_builtin(builtin_type, "prim:reader-read-string", 0, &reader_read_string, a);
	/* src/strings.mes */
	a = init_builtin(builtin_type, "prim:string=?", 2, &string_equal_p_, a);
	a = init_builtin(builtin_type, "prim:symbol->string", 1, &symbol_to_string_, a);
	a = init_builtin(builtin_type, "prim:symbol->keyword", 1, &symbol_to_keyword_, a);
	a = init_builtin(builtin_type, "prim:keyword->string", 1, &keyword_to_string, a);
	a = init_builtin(builtin_type, "prim:string->symbol", 1, &string_to_symbol, a);
	a = init_builtin(builtin_type, "prim:make-symbol", 1, &make_symbol_, a);
	a = init_builtin(builtin_type, "prim:string->list", 1, &string_to_list, a);
	a = init_builtin(builtin_type, "prim:list->string", 1, &list_to_string, a);
	a = init_builtin(builtin_type, "prim:read-string", -1, &read_string, a);
	a = init_builtin(builtin_type, "prim:string-append", -1, &string_append, a);
	a = init_builtin(builtin_type, "prim:string-length", 1, &string_length, a);
	a = init_builtin(builtin_type, "prim:string-ref", 2, &string_ref, a);
	/* src/struct.mes */
	a = init_builtin(builtin_type, "prim:make-struct", 3, &make_struct_, a);
	a = init_builtin(builtin_type, "prim:struct-length", 1, &struct_length, a);
	a = init_builtin(builtin_type, "prim:struct-ref", 2, &struct_ref, a);
	a = init_builtin(builtin_type, "prim:struct-set!", 3, &struct_set_x, a);
	/* src/vector.mes */
	a = init_builtin(builtin_type, "prim:make-vector", 1, &make_vector_, a);
	a = init_builtin(builtin_type, "prim:vector-length", 1, &vector_length, a);
	a = init_builtin(builtin_type, "prim:vector-ref", 2, &vector_ref, a);
	a = init_builtin(builtin_type, "prim:vector-entry", 1, &vector_entry, a);
	a = init_builtin(builtin_type, "prim:vector-set!", 3, &vector_set_x, a);
	a = init_builtin(builtin_type, "prim:list->vector", 1, &list_to_vector, a);
	a = init_builtin(builtin_type, "prim:vector->list", 1, &vector_to_list, a);
	return a;
}
