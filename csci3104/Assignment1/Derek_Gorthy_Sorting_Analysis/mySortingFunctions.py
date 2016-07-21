# Name: Derek Gorthy
# Email:derek.gorthy@colorado.edu   
# SUID: 102359021
# I wrote both mergeSort and quickSort referencing psuedocode from the Algorithms textbook 
# I wrote the code for generating graphs referencing: 
# http://www.thetechrepo.com/main-articles/465-how-to-create-a-graph-in-python.html
# After attempting to export data to excel, I decided to generate using Python package

# By submitting this file as my own work, I declare that this
# code has been written on my own with no unauthorized help. I agree to the
# CU Honor Code. I am also aware that plagiarizing code may result in
# a failing grade for this class.
from __future__ import print_function
import sys
import random
import time
import matplotlib.pyplot as plt
import numpy as np 



# --------- Insertion Sort -------------
# Implementation of getPosition
# Helper function for insertionSort
def getPosition(rList, elt):
    # Find the position where element occurs in the list
    #
    for (i,e) in enumerate(rList):
        if (e >= elt):
            return i
    return len(rList)

# Implementation of Insertion Sort 
def insertionSort(lst):
    n = len(lst)
    retList = []
    for i in lst:
        pos = getPosition(retList,i)
        retList.insert(pos,i)    
    return retList


#------ Merge Sort --------------
def mergeSort(lst):
    if len(lst) > 1:
        #break down list into left and right
        lefthalf = lst[:len(lst)//2]
        righthalf = lst[len(lst)//2:]
        #recursively call to break down entire array
        mergeSort(lefthalf)
        mergeSort(righthalf)

        #build array back up
        a = 0 
        b = 0
        c = 0
        #while both lists still have elements to check
        while a < len(lefthalf) and b < len(righthalf):
            #add the smallest element of either left or right to our finished list
            if lefthalf[a] < righthalf[b]:
                lst[c] = lefthalf[a]
                a = a + 1
            else:
                lst[c] = righthalf[b]
                b = b + 1
            c = c + 1

        #if only left half has elements to check
        while a < len(lefthalf):
            lst[c] = lefthalf[a]
            a = a + 1
            c = c + 1

        #if only right half has elements to check
        while b < len(righthalf):
            lst[c] = righthalf[b]
            b = b + 1
            c = c + 1


    return lst 

#------ Quick Sort --------------

#simple function to swap two elements
def swap(A, pos1, pos2):
    tempVal1 = A[pos1]
    tempVal2 = A[pos2]
    A[pos1] = tempVal2
    A[pos2] = tempVal1
    return A

#main quicksort function
def qSort(A, first, last):
    #if the size of array is greater than 1
    if first < last:
        #find value of the partition and sort elements to left or right of partition
        q = partition(A, first, last)
        #sort left side
        qSort(A, first, q-1)
        #sort right side
        qSort(A, q+1, last)
    return A

#determine the value of the partition and sort all values to left or right 
def partition(A, first, last):
    #determine random value for pivot index
    pivot = random.randint(first, last)
    pivotVal = A[pivot]
    #move pivot to back of the array
    swap(A, last, pivot)


    i = first-1
    for j in range(first, last):
        #move value to left of the pivot
        if A[j] <= pivotVal:
            i = i + 1
            swap(A, i, j)
    #move pivot to correct place
    swap(A, i+1, last)
    return i+1

#quicksort function that grading script calls
def quickSort(lst):
    ending = len(lst) - 1
    #my function requires more arguments, passing it index 0 and last index
    qSort(lst, 0, ending)
    return lst


# ------ Timing Utility Functions ---------

# Code below is given only for demonstration purposes


# Function: generateRandomList
# Generate a list of n elements from 0 to n-1
# Shuffle these elements at random
#this function was given
def generateRandomList(n):
   # Generate a random shuffle of n elements
   lst = list(range(0,n))
   random.shuffle(lst)
   return lst


#this function was given
def measureRunningTimeComplexity(sortFunction,lst):
    t0 = time.clock()
    sortFunction(lst)
    t1 = time.clock() # A rather crude way to time the process.
    return (t1 - t0)



def calculateData(sortMethod, averageList, maxList):

    for i in range(1,101):
        # check each list size 50 times
        sizeOfList = 5*i
        numTrials = sizeOfList*20
        timeList = [[None]]*(numTrials)
        
        for j in range(0, numTrials):
            lst = generateRandomList(sizeOfList)
            time = measureRunningTimeComplexity(sortMethod, lst)
            timeList[j] = time

        # these are used to test that each size of array is being calculated correctly    
        #print('using method: insertionSort')
        #print('currently testing list of size:', sizeOfList)
        #print ('the max time is: ', max(timeList))
        #print ('the average time is: ', sum(timeList)/len(timeList))

        #Add the maximum to list of maximums
        maxList[i-1] = max(timeList)
        #Add the average to list of averages
        averageList[i-1] = sum(timeList)/len(timeList)

        print('Completed running trials of size: ', sizeOfList)
        print('Ran this many trials: ', numTrials)

    return(averageList, maxList)





def main():

    #Declare each list needed for graphing
    insertionSortMaxTimeList = [[None]]*100
    insertionSortAvTimeList = [[None]]*100
    mergeSortTimeMaxList = [[None]]*100
    mergeSortAvTimeList = [[None]]*100
    quickSortTimeMaxList = [[None]]*100
    quickSortAvTimeList = [[None]]*100

    insertionSortAvTimeList, insertionSortMaxTimeList = calculateData(insertionSort, insertionSortAvTimeList, insertionSortMaxTimeList)
    mergeSortAvTimeList, mergeSortTimeMaxList = calculateData(mergeSort, mergeSortAvTimeList, mergeSortTimeMaxList)
    quickSortAvTimeList, quickSortTimeMaxList = calculateData(quickSort, quickSortAvTimeList, quickSortTimeMaxList)

    #Print out the results of each
    print ('insertionSort averages: ', insertionSortAvTimeList)
    print ('')
    print ('insertionSort maximums: ', insertionSortMaxTimeList)
    print ('')
    print ('mergeSort averages:' , mergeSortAvTimeList)
    print ('')
    print ('mergeSort maximums: ', mergeSortTimeMaxList)
    print ('')
    print ('quickSort averages: ', quickSortAvTimeList)
    print ('')
    print ('quickSort maximums: ', quickSortTimeMaxList)

    #THE FOLLOWING COMMENTS ARE MY METHODS FOR GRAPH GENERATION
    #BECAUSE OF MY VARIABLE USE, EACH MUST BE GENERATED IN DIFFERENT TESTS
    #THE REUSE OF x_series AND y_series PLOTS TOO MANY LINES IF ALL ARE GENERATED AT THE SAME TIME
    #TO TEST, UNCOMMENT ONE BLOCK. THEN RECOMMENT THE BLOCK AND UNCOMMENT THE NEXT BLOCK
    #NOTE, Python3 DOES NOT SUPPORT THIS PACKAGE, MUST BE RUN WITH Python
    ###############################################################################################

########## GENERATE GRAPH FOR INSERTION SORT ##########     
#    x_series = [[None]]*100
#    for i in range(1,101):
#         x_series[i-1] = (5*i)

#    x_series = np.array(x_series)
#    y_series_1 = np.array(insertionSortMaxTimeList)
#    y_series_2 = np.array(insertionSortAvTimeList)

    #plot data
#    plt.plot(x_series, y_series_1, label="maximum")
#    plt.plot(x_series, y_series_2, label="average")
     
    #add in labels and title
#    plt.xlabel("Size of List")
#    plt.ylabel("Calculated Time")
#    plt.title("Insertion Sort Calculations")
     
    #add limits to the x and y axis
#    plt.xlim(0, 500)
#    plt.ylim(0, .1) 
     
    #create legend
#    plt.legend(loc="upper left")
     
    #save figure to png
#    plt.savefig("insertionSort.jpg")




########## GENERATE GRAPH FOR MERGE SORT ##########
#    x_series = [[None]]*100
#    for i in range(1,101):
#        x_series[i-1] = (5*i)

#    x_series = np.array(x_series)
#    y_series_1 = np.array(mergeSortTimeMaxList)
#    y_series_2 = np.array(mergeSortAvTimeList)

    #plot data
#    plt.plot(x_series, y_series_1, label="maximum")
#    plt.plot(x_series, y_series_2, label="average")
     
    #add in labels and title
#    plt.xlabel("Size of List")
#    plt.ylabel("Calculated Time")
#    plt.title("Merge Sort Calculations")
     
    #add limits to the x and y axis
#    plt.xlim(0, 500)
#    plt.ylim(0, .08) 
     
    #create legend
#    plt.legend(loc="upper left")
     
    #save figure to png
#    plt.savefig("mergeSort.jpg")




########## GENERATE GRAPH FOR QUICK SORT ##########
    x_series = [[None]]*100
    for i in range(1,101):
        x_series[i-1] = (5*i)

    x_series = np.array(x_series)
    y_series_1 = np.array(quickSortTimeMaxList)
    y_series_2 = np.array(quickSortAvTimeList)

    #plot data
    plt.plot(x_series, y_series_1, label="maximum")
    plt.plot(x_series, y_series_2, label="average")
     
    #add in labels and title
    plt.xlabel("Size of List")
    plt.ylabel("Calculated Time")
    plt.title("Quick Sort Calculations")
     
    #add limits to the x and y axis
    plt.xlim(0, 500)
    plt.ylim(0, .08) 
     
    #create legend
    plt.legend(loc="upper left")
     
    #save figure to png
    plt.savefig("quickSort.jpg")



main()

