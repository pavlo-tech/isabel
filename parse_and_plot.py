import sys
import os
import numpy as np
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import seaborn as sns

def parse_output(compressorName, tols, mode=-1):

	modeDir = "" if mode == -1 else str(mode) + "/"

	CompressionValues = np.ones((tolRange, numVars))

	for tolIdx, tol_str in enumerate(tols):
	
		inputFileName = "results/" + compressorName + "/" + modeDir + tol_str + "/100.txt"

		if os.path.getsize(inputFileName)==0:
			print 'skipping: ' + inputFileName
			continue

	
		#allocat mem for avg		

		with open(inputFileName) as f:
			



