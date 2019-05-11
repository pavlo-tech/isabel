import numpy as np
import sys
import ctypes
import re


# delete this later
#fileName='/scratch2/ptriant/100x500x500/QCLOUDf48.bin.f32'
#TOL=1E-1
#ITER=100
#compressor_path='./compressors/SZ-2.0.2.1/sz_cycle.dll'

fileName=sys.argv[1]
TOL=float(sys.argv[2])
ITER=int(sys.argv[3])
compressor_path=sys.argv[4]

compressor_dll = ctypes.cdll.LoadLibrary(compressor_path)
ZFP=re.search("(ZFP|zfp)",compressor_path) != None
cycle=compressor_dll.cycle_float


# copy data into 3d array
arr = np.fromfile(fileName, dtype=np.dtype('<f'))
iArr=np.reshape(arr,(100,500,500))

# create output array
oArr = np.zeros((100, 500, 500), dtype=np.dtype('<f'))


if ZFP:
	print( 'not yet implemented')

else:
	print( 'running SZ')

	#sz_config='./compressors/SZ-2.0.2.1/sz.config'
	#mode=0
	sz_config=sys.argv[5]
	mode=int(sys.argv[6])

	SZ_Init = compressor_dll.SZ_Init
	SZ_Finalize = compressor_dll.SZ_Finalize	

	SZ_Init(ctypes.c_char_p(sz_config.encode('utf-8')))

	cycle(iArr.ctypes.data_as(ctypes.POINTER(ctypes.c_float)),
		500,500,100, ctypes.c_double(TOL), ITER,
		oArr.ctypes.data_as(ctypes.POINTER(ctypes.c_float)),
		ctypes.c_int(mode)) 

	SZ_Finalize()

	print(np.max(np.abs(iArr.flatten() - oArr.flatten())))

exit(0)

