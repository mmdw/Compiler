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

proc main 
	local	__local_1:DWORD
	lea	eax, [__local_1]
	push	dword eax
	ccall [scanf], __format_int
	ccall [getchar]
	ccall	[printf], __format_print_int, dword [__local_1]
	ccall	[printf], __newline
	ret
endp

section '.data' readable 
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
