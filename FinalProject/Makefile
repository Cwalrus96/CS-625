ProjectSerial: ParSet.c Project-serial.c
        export LD_LIBRARY_PATH=/homes/cwalrus96/project
        gcc -I/homes/cwalrus96/project -c ParSet.c Project-serial.c
        gcc -L/homes/cwalrus96/project -Wl,-rpath=/homes/cwalrus96/project ParSet.o Project-serial.o \
        -llammps -lfftw -lmpi -lpthread -lstdc++ -lm -o ProjectSerial.o

Test: ParSet.c Project-serial-test.c
        export LD_LIBRARY_PATH=/homes/cwalrus96/project
        gcc -I/homes/cwalrus96/project -c ParSet.c Project-serial-test.c
        gcc -L/homes/cwalrus96/project ParSet.o Project-serial-test.o  -llammps -lfftw -lmpi -lpthread -lstdc++ -lm -o ProjectTest
        
MPI: ParSet.c Project-MPI.c
        gcc -I/homes/cwalrus96/project -c ParSet.c Project-MPI.c
        gcc -L/homes/cwalrus96/project -Wl,-rpath=/homes/cwalrus96/project ParSet.o Project-MPI.o \
        -llammps -lfftw -lmpi -lpthread -lstdc++ -lm -o ProjectMPI.o
