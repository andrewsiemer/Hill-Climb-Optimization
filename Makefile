.KEEP_STATE:

all:
	gcc -fopenmp hill.c -O3 -o hill -lm -mno-avx -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -O1
clean:
	rm hill
	rm hill-cuda
	rm *.o

# ------------------------------------------------------------------------------
# CUDA Version
DFLAGS      =
OPTFLAGS    = -g -pg -O2
INCFLAGS    = -I.
CFLAGS      = $(OPTFLAGS) $(DFLAGS) $(INCFLAGS)
NVCCFLAGS   = $(CFLAGS) -Xcompiler -fopenmp
LDFLAGS     = $(OPTFLAGS) -lcurand -Xcompiler -fopenmp
LIBS        =
NVCC        = nvcc

%.o : %.cu
	$(NVCC) $(NVCCFLAGS) -o $@ -c $<

CUDA_CU_SRC = hill_cuda.cu
CUDA_CU_OBJ = $(CUDA_CU_SRC:%.cu=%.o)

cuda: $(CUDA_CU_OBJ)
	$(NVCC) $(LDFLAGS) -o hill-cuda $(CUDA_CU_OBJ)
#---------------------------------------------------------------------
