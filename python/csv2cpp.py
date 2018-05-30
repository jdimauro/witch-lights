#!/usr/bin/python

#Given a csv file on the command line, convert it to a sequence of strcpy/strcat commands
#that can be pasted into an arduino program

# usage: csv2cpp.py [-h] --csv CSV

# Convert csv files describing animations into C++ code

# optional arguments:
#   -h, --help  show this help message and exit
#   --csv CSV   path to csv file

import csv
import argparse
import os.path

def load_file(f_name, anim_name):
	with open(f_name, 'rb') as infile:
		reader = csv.reader(infile, delimiter=',')

		#Build up a list of C++ code strings
		code = []

		#Get the header and format it as a comment
		try:
			#Get the header
			line = reader.next() 
			#Get a line of spaces to indent the comment
			spaces = " " * (len("strcpy(afc_{0}, \"".format(anim_name)) - 2 )
			#Compose the line 
			code.append("//{0}{1}".format(spaces, ''.join(line)))

			#Set up for the main loop
			line = reader.next()
		except StopIteration:
			line = None
		
		#Set a flag for the first line (which uses strcpy instead of strcat)
		op = "cpy"

		while line is not None:
			#replace empty spots with spaces
			line = [' ' if x == '' else x for x in line]
			
			#Add a line of C++ code to the list
			code.append("str{0}(afc_{1}, \"{2}\");".format(op, anim_name, ''.join(line)))
			
			#Change the flag if needed
			if op == "cpy":
				op = "cat"
			#Try to get the next line
			try:
				line = reader.next()
			except StopIteration:
				line = None

		#Now we have all the code in the list, join it with newlines and return that
		return '\n'.join(code)

if __name__=="__main__":
	#Create an argument parser, which has a --csv option, and parse the arguments from that
	parser = argparse.ArgumentParser(description='Convert csv files describing animations into C++ code')
	parser.add_argument('--csv', help="path to csv file", nargs=1, required=True)
	args = parser.parse_args()
	filename = args.csv[0]
	
	#If this is a legit file, load and convert it
	if os.path.isfile(filename):
		#The animation name is everything before the first period in the base file name. 
		#Note that this is not well-behaved for file names with spaces in them
		anim_name = os.path.basename(filename).split('.')[0]
		ccode = load_file(filename, anim_name)

		#Write it to a text file, scribbling over any previous text file with that name
		with open("{0}.txt".format(anim_name), 'w') as outfile:
			outfile.write(ccode)