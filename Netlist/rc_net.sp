********** rc_net **********

R1  1 2 1
R2  2 3 2
R3  1 4 5
R4  2 5 10
R5  3 6 2
R6  4 5 1
R7  5 6 2
R8  4 7 5
R9  5 8 8
R10 6 9 5
R11 7 8 5
R12 8 9 1
R13 7 10 1
R14 10 11 2
R15 9 gnd 10
C1 2 5 10000p
C2 3 6 2p
C3 4 5 5p
C4 5 8 3p
C5 7 10 10p
C6 9 gnd 1p
*VIN 11 gnd pulse(-5, 5, 1.5e-10,  1e-10,  1e-10,  1e-11,  3e-09)
VIN 11 gnd 1 
*.tran 1e-11 1e-8
*.PROBE V(11) V(10)
.end
