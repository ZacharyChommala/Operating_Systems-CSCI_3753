from __future__ import print_function
import sys
import random
import time


def quickSort(lst):
	less = []
	greater = []
	lesscounter = 0
	greatercounter = 0
	final = []

	if len(lst) > 1:  
		pivotPos = random.randint(0, len(lst)-1)
		pivotValue = lst[pivotPos]	
		counter = 0
		while counter < (len(lst) -1):
			if counter == pivotPos:
				print('we found the pivot, moving on')
				counter = counter
			elif lst[counter] <= pivotValue:
				print('The current value: ', lst[counter], 'is less than: ', pivotValue)
				less += lst[counter]
				lesscounter = lesscounter + 1
				print(less)
			elif lst[counter] > pivotValue:
				print('The current value: ', lst[counter], 'is greater than: ', pivotValue)
				greater += lst[counter]
				greatercounter = greatercounter + 1
				print (greater)
			counter = counter + 1

		quickSort(less)
		quickSort(greater)

		final = less
		final.append(pivotValue)
		final.append(greater)

	elif len(lst) == 1:
		final = lst

	return final




def main():
	print ('Compiled!')
	testArray = [1,5,7,0,-15,-16,-17,8,2,6,-15,6,7,-20,20,123,-30,40,1,2,4,4]
	print(testArray)
	quickSort(testArray)
	print(testArray)



main()