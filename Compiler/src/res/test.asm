format PE console
entry start
include 'win32a.inc'
section '.code' code readable executable

start: 
	call __init
	ccall [getchar]
	stdcall [ExitProcess], 0

proc __init
	call main
ret
endp

proc __print_bool _arg
	 mov	ecx, dword [_arg]
	 jecxz	_label_false

	 ccall	[printf], __str_true
	 ret

	_label_false:
	 ccall [printf], __str_false

	 ret
endp

proc foo __arg_1
	local	_tr_0:DWORD
	push	dword [__arg_1]
	ccall [pqueue_size]
	mov	[_tr_0], eax
	ccall	[printf], __format_print_int, dword [_tr_0]
	ccall	[printf], __newline
	ret
endp

proc main 
	local	__local_3:DWORD
	ccall [pqueue_alloc]
	mov	[__local_3], eax
	push	dword [__const_4]
	push	dword [__const_5]
	push	dword [__local_3]
	ccall [pqueue_push]
	push	dword [__const_6]
	push	dword [__const_7]
	push	dword [__local_3]
	ccall [pqueue_push]
	push	dword [__const_8]
	push	dword [__const_9]
	push	dword [__local_3]
	ccall [pqueue_push]
	push	dword [__local_3]
	local	_tr_11:DWORD
	call	foo
	ret
endp

section '.data' readable 
__const_4	dd	3.0
__const_5	dd	30
__const_6	dd	2.0
__const_7	dd	20
__const_8	dd	1.0
__const_9	dd	10
__newline	db	10,0
__format_print_int	db	"%d ",0
__format_print_float	db	"%f ",0
__format_int	db	"%d",0
__format_float	db	"%f",0
__str_true	db	"true",0
__str_false	db	"false",0


section '.idata' import data readable
library kernel,'kernel32.dll',msvcrt,'msvcrt.dll', pqueue, 'libpqueue.dll'
import kernel, ExitProcess,'ExitProcess' 
import msvcrt, printf,'printf', getchar, '_fgetchar', scanf, 'scanf'
import pqueue, \
	pqueue_alloc, 'pqueue_alloc', \
	pqueue_size, 'pqueue_size', \
	pqueue_push, 'pqueue_push', \
	pqueue_pop, 'pqueue_pop', \
	pqueue_top_priority, 'pqueue_top_priority', \
	pqueue_top_value, 'pqueue_top_value'
