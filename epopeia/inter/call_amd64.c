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
*/
 void Call(void* Funcion, int* ParamVector, int ParamCount)
 {
    void (*Function)(void) = Funcion;
    int ParamTemp = ParamCount;

    ParamVector+=ParamCount;
    while(ParamTemp > 0)
    {
        ParamVector --;
        ParamTemp --;
        *ParamVector = 0;
        // push *ParamVector to the stack
    }
    Function();
    // Restore the stack
}

