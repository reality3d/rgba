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
	.file	"call.c"
	.version	"01.01"
.globl Call
	.type	 Call,@function

Call:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	16(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	16(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	12(%ebp), %eax
	addl	%edx, (%eax)
/* Bucle de enpilamiento de los parametros */
	.p2align 2
.L4:
	cmpl	$0, -8(%ebp)
	jg	.L6
	jmp	.L5
	.p2align 2
.L6:
	leal	12(%ebp), %eax
	subl	$4, (%eax)
	leal	-8(%ebp), %eax
	decl	(%eax)
        movl    12(%ebp), %eax
        pushl   (%eax)
	jmp	.L4
	.p2align 2
.L5:
	movl	-4(%ebp), %eax
/* Hacemos la llamada!! */
	call	*%eax
/* Restauramos la pila. Para eso hemos preservado el VectorCount original */
/* Cuidadin con el valor de retorno contenido el EAX */
	xchgl	16(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
        addl    %edx, %esp
        movl    16(%ebp), %eax
	leave
	ret
.Lfe2:
	.size	 Call,.Lfe2-Call
	.align 4


