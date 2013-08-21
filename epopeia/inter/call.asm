;
; EPOPEIA Caller [nasm compatible]
;
bits 32


global _Call
 
section .text
align 16

_Call
	push 	ebp
	mov 	ebp,	esp
	sub 	esp,	8
	mov 	eax,	DWORD [8+ebp]
	mov 	DWORD [-4+ebp],	eax
	mov 	eax,	DWORD [16+ebp]
	mov 	DWORD [-8+ebp],	eax
	mov 	eax,	DWORD [16+ebp]
	mov 	eax,	eax
	lea 	edx,	[0+eax*4]
	lea 		eax,	[12+ebp]
	add 	DWORD [eax],	edx

_L4:
	cmp 	DWORD [-8+ebp],	0
	jg 	_L6
	jmp 	_L5

_L6:
	lea 	eax,	[12+ebp]
	sub 	DWORD [eax],	4
	lea 	eax,	[-8+ebp]
	dec 	DWORD [eax]
	mov 	eax,	DWORD [12+ebp]
	push 	DWORD [eax]
	jmp 	_L4

_L5:
	mov 	eax,	DWORD [-4+ebp]
	
        call 	eax

	xchg 	eax,	DWORD [16+ebp]
	mov 	eax,	eax
	lea 	edx,	[0+eax*4]
	add 	esp,	edx
	mov 	eax,	DWORD [16+ebp]
	leave
	ret
