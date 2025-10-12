-------- PROJECT GENERATOR --------
PROJECT NAME :	rx62n_sci_async
PROJECT DIRECTORY :	C:\WorkSpace\rx62n_sci_async\rx62n_sci_async
CPU SERIES :	RX600
CPU TYPE :	RX62N
TOOLCHAIN NAME :	Renesas RX Standard Toolchain
TOOLCHAIN VERSION :	1.2.1.0
GENERATION FILES :
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\dbsct.c
        Setting of B,R Section
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\typedefine.h
        Aliases of Integer Type
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\sbrk.c
        Program of sbrk
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\iodefine.h
        Definition of I/O Register
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\intprg.c
        Interrupt Program
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\vecttbl.c
        Initialize of Vector Table
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\vect.h
        Definition of Vector
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\resetprg.c
        Reset Program
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\rx62n_sci_async.c
        Main Program
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\sbrk.h
        Header file of sbrk file
    C:\WorkSpace\rx62n_sci_async\rx62n_sci_async\stacksct.h
        Setting of Stack area
START ADDRESS OF SECTION :
 H'1000	B_1,R_1,B_2,R_2,B,R,SU,SI
 H'FFFF8000	PResetPRG
 H'FFFF8100	C_1,C_2,C,C$*,D_1,D_2,D,P,PIntPRG,W*,L
 H'FFFFFFD0	FIXEDVECT

* When the user program is executed,
* the interrupt mask has been masked.
* 
* Program start 0xFFFF8000.
* RAM start 0x1000.

SELECT TARGET :
    RX E1/E20 SYSTEM
DATE & TIME : 2013/04/17 14:17:22
