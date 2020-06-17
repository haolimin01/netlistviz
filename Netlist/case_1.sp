********
R1 1 0 1
R2 2 n3 2
R3 n3 0 1
C1 n3 0 1p
VIN 1 2 1

*.dc VIN 0 10 1
*.plot V(2, 1) V(2, 0)
.end
