/**
 *  @file szToHDF5.c
 *  @author Sheng Di
 *  @date July, 2017
 *  @brief This is an example of using compression interface (HDF5)
 *  (C) 2017 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "hdf5.h"
#include "H5Z_SZ.h"

#define DATASET "testdata_compressed"

int main(int argc, char * argv[])
{
	size_t r5=0,r4=0,r3=0,r2=0,r1=0;
	char outDir[640], oriFilePath[640], outputFilePath[640];
	size_t cd_nelmts, nbEle; 
	unsigned int *cd_values = NULL;
	//unsigned int cd_values[7];
	
	herr_t status;
	htri_t avail;
	unsigned filter_config;

	hid_t sid, idsid, cpid, fid;

	if(argc < 4)
	{
		printf("Test case: szToHDF5 [dataType] [config_file] [srcFilePath] [dimension sizes...]\n");
		printf("Example: szToHDF5 -f/-d sz.config testdata/x86/testfloat_8_8_128.dat 8 8 128\n");
		exit(0);
	}

	printf("config file = %s\n", argv[2]);
	int dataType = strcmp(argv[1],"-f")==0?SZ_FLOAT:SZ_DOUBLE;
	strcpy(cfgFile, argv[2]);
	sprintf(oriFilePath, "%s", argv[3]);
	if(argc>=5)
		r1 = atoi(argv[4]); //8
	if(argc>=6)
		r2 = atoi(argv[5]); //8
	if(argc>=7)
		r3 = atoi(argv[6]); //128
	if(argc>=8)
		r4 = atoi(argv[7]);
	if(argc>=9)
		r5 = atoi(argv[8]);

	printf("cfgFile=%s\n", cfgFile); 
	sprintf(outputFilePath, "%s.sz.h5", oriFilePath);

//	printf("argv[1]=%s, dataType=%d\n", argv[1], dataType);
	nbEle = computeDataLength(r5, r4, r3, r2, r1);
		
//	printf("nbEle=%u\n", nbEle);

	//Create cd_values
	printf("Dimension sizes: n5=%u, n4=%u, n3=%u, n2=%u, n1=%u\n", r5, r4, r3, r2, r1); 
	SZ_metaDataToCdArray(&cd_nelmts, &cd_values, dataType, r5, r4, r3, r2, r1);
	/*cd_nelmts = 5;
	cd_values[0] = 3;
	cd_values[1] = 0;
	cd_values[2] = 128;
	cd_values[3] = 8;
	cd_values[4] = 8;
	cd_values[5] = 0;				
	cd_values[6] = 0;*/
	
	int i = 0;
//	for(i=0;i<cd_nelmts;i++)
//		printf("cd_values[%d]=%u\n", i, cd_values[i]);

	//compute dimension
	int dim = computeDimension(r5, r4, r3, r2, r1);

	hsize_t dims[5] = {0,0,0,0,0}, chunk[5] = {0,0,0,0,0};
	init_dims_chunk(dim, dims, chunk, nbEle, r5, r4, r3, r2, r1);

	/* create HDF5 file */
	if (0 > (fid = H5Fcreate(outputFilePath, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT))) ERROR(H5Fcreate);

	/*Create dataspace. Setting maximum size */
	if (0 > (sid = H5Screate_simple(dim, dims, NULL))) ERROR(H5Screate_simple);

	/* setup dataset creation properties */
	if (0 > (cpid = H5Pcreate(H5P_DATASET_CREATE))) ERROR(H5Pcreate);
	
	/* Add the SZ compression filter and set the chunk size */
	if (0 > H5Pset_filter(cpid, H5Z_FILTER_SZ, H5Z_FLAG_MANDATORY, cd_nelmts, cd_values)) ERROR(H5Pset_filter);	
	avail = H5Zfilter_avail(H5Z_FILTER_SZ);
	if(avail)
	{
		status = H5Zget_filter_info(H5Z_FILTER_SZ, &filter_config);
		if((filter_config & H5Z_FILTER_CONFIG_ENCODE_ENABLED) && (filter_config & H5Z_FILTER_CONFIG_ENCODE_ENABLED))
			printf("sz filter is available for encoding and decoding.\n");
	}
	if (0 > H5Pset_chunk(cpid, dim, chunk)) ERROR(H5Pset_chunk);

	//Initialize the configuration for SZ
	//You can also use the global variable conf_params to set the configuratoin for sz without cfgFile.
	//Example of setting an absolute error bound:
	//			sz_params* params = H5Z_SZ_Init_Default();
	//			params->errorBoundMode = ABS;
    //			params->absErrBound = 1E-4;
	
	//H5Z_SZ_Init(cfgFile);
	
	printf("....Writing SZ compressed data.............\n");
    	
	if(dataType == SZ_FLOAT)
	{
		float *data = readFloatData(oriFilePath, &nbEle, &status);

		printf("original data = ");
		for(i=0;i<20;i++)
			printf("%f ", data[i]);	
		printf("....\n");	

		if(dataEndianType == LITTLE_ENDIAN_DATA)
		{
			if (0 > (idsid = H5Dcreate(fid, DATASET, H5T_IEEE_F32LE, sid, H5P_DEFAULT, cpid, H5P_DEFAULT))) ERROR(H5Dcreate);
			if (0 > H5Dwrite(idsid, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data)) ERROR(H5Dwrite);			
		}
		else //BIG_ENDIAN_DATA
		{
			if (0 > (idsid = H5Dcreate(fid, DATASET, H5T_IEEE_F32BE, sid, H5P_DEFAULT, cpid, H5P_DEFAULT))) ERROR(H5Dcreate);
			if (0 > H5Dwrite(idsid, H5T_IEEE_F32BE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data)) ERROR(H5Dwrite);						
		}

		if (0 > H5Dclose(idsid)) ERROR(H5Dclose);
	}
	else
	{
		double *data = readDoubleData(oriFilePath, &nbEle, &status);
		
		printf("original data = ");
		for(i=0;i<20;i++)
			printf("%f ", data[i]);	
		printf("....\n");			
		
		if(dataEndianType == LITTLE_ENDIAN_DATA)
		{
			if (0 > (idsid = H5Dcreate(fid, DATASET, H5T_IEEE_F64LE, sid, H5P_DEFAULT, cpid, H5P_DEFAULT))) ERROR(H5Dcreate);
			if (0 > H5Dwrite(idsid, H5T_IEEE_F64LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data)) ERROR(H5Dwrite);			
		}
		else //BIG_ENDIAN_DATA
		{
			if (0 > (idsid = H5Dcreate(fid, DATASET, H5T_IEEE_F64BE, sid, H5P_DEFAULT, cpid, H5P_DEFAULT))) ERROR(H5Dcreate);
			if (0 > H5Dwrite(idsid, H5T_IEEE_F64BE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data)) ERROR(H5Dwrite);				
		}

		if (0 > H5Dclose(idsid)) ERROR(H5Dclose);
	}   

	/*Close and release reosurces*/
	if (0 > H5Sclose(sid)) ERROR(H5Sclose);
	if (0 > H5Pclose(cpid)) ERROR(H5Pclose);
	if (0 > H5Fclose(fid)) ERROR(H5Fclose);

	printf("Output hdf5 file: %s\n", outputFilePath);
	H5Z_SZ_Finalize();
	H5close();
    
	return 0;
}
