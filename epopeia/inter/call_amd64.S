/*
! Canya!!!
! GCC rulez
! Este fichero ha sido pregenerado con GCC y se le han efectuado
! unos pocos cambios quirurgicos para aquello que no se podia hacer
! en C, a saber: enpilar los parametros (nada mas que el push)
! y restaurar la pila en la vuelta de la llamada a funcion
!
! Enlar/RGBA 2001
! Epopeia Scripting DemoSystem
!
! Codigo generador original:
! void Call(void* Funcion, int* ParamVector, int ParamCount)
! {
!    void (*Function)(void) = Funcion;
!    int ParamTemp = ParamCount;
!        
!    ParamVector+=ParamCount;
!    while(ParamTemp > 0)
!    {
!        ParamVector --;
!        ParamTemp --;
!        *ParamVector = 0;
!        // push *ParamVector to the stack
!    }
!    Function();
!    // Restore the stack
!}
*/
	.file	"call_amd64.s"
	.text
.globl Call
	.type	Call, @function
Call:
.LFB3:
	pushq	%rbp
.LCFI0:
	movq	%rsp, %rbp
.LCFI1:
	subq	$48, %rsp
.LCFI2:
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -32(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, -36(%rbp)
	movl	-20(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	-16(%rbp), %rax
	addq	%rdx, (%rax)
/* Bucle de enpilamiento de los parametros */
.L2:
	cmpl	$0, -36(%rbp)
	jg	.L4
	jmp	.L3
.L4:
	leaq	-16(%rbp), %rax
	subq	$4, (%rax)
	leaq	-36(%rbp), %rax
	decl	(%rax)
	movq	-16(%rbp), %rax
	pushq	(%rax)
	jmp	.L2
.L3:
	movq	-32(%rbp), %rax
/* Hacemos la llamada!! */
	call	*%rax
/* Restauramos la pila. Para eso hemos preservado el VectorCount original */
/* Cuidadin con el valor de retorno contenido el EAX */
	xchgq	-20(%rbp), %rax
	movl	%eax, %eax
	leaq	0(,%rax,4), %rdx
        addq    %rdx, %rsp
        movq    -20(%rbp), %rax
	leave
	ret
.LFE3:
	.size	Call, .-Call
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	""
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1
.LASFDE1:
	.long	.LASFDE1-.Lframe1
	.quad	.LFB3
	.quad	.LFE3-.LFB3
	.byte	0x4
	.long	.LCFI0-.LFB3
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE1:
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.3.5 (Debian 1:3.3.5-13)"
