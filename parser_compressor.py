import numpy as np
import sys
import ctypes
import re

nz=100; ny=500; nx=500;
#nz=128; ny=8; nx=8;



fileName=sys.argv[1]
TOL=float(sys.argv[2])
ITER=int(sys.argv[3])
compressor_path=sys.argv[4]

compressor_dll = ctypes.cdll.LoadLibrary(compressor_path)
ZFP=re.search("(ZFP|zfp)",compressor_path) != None
cycle=compressor_dll.cycle_float


# copy data into 3d array
arr = np.fromfile(fileName, dtype=np.dtype('<f'))

'''
with open("input.txt","w") as f:
	for i in range(nz*nx*ny):
		f.write(str(arr[i]) + "\n")
'''

iArr=np.reshape(arr,(nz,ny,nx))

# create output array
oArr = np.zeros((nz,ny,nx), dtype=np.dtype('<f'))


if ZFP:
	print( 'not yet implemented')

else:
	print( 'running SZ')

	sz_config=sys.argv[5]
	mode=int(sys.argv[6])

	SZ_Init = compressor_dll.SZ_Init
	SZ_Finalize = compressor_dll.SZ_Finalize	

	SZ_Init(ctypes.c_char_p(sz_config.encode('utf-8')))

	cycle(iArr.ctypes.data_as(ctypes.POINTER(ctypes.c_float)),
		nx,ny,nz, ctypes.c_double(TOL), ITER,
		oArr.ctypes.data_as(ctypes.POINTER(ctypes.c_float)),
		ctypes.c_int(mode)) 

	#for testing, delete later
	'''
	oarr=oArr.flatten()
	with open("output.txt","w") as f:
		for i in range(nz*nx*ny):
			f.write(str(oarr[i]) + "\n")
	'''
	SZ_Finalize()


	print(np.max(np.abs(iArr.flatten() - oArr.flatten())))

exit(0)

