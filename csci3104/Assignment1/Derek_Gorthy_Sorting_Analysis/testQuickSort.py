from __future__ import print_function
import sys
import random
import time

def swap(A, pos1, pos2):
    print ('Swapping', A[pos1], ' for ', A[pos2])
    tempVal1 = A[pos1]
    tempVal2 = A[pos2]
    A[pos1] = tempVal2
    A[pos2] = tempVal1
    print ('the array is now: ', A)
    return A

def qSort(A, first, last):
    print("entered qSort")
    if first < last:
        q = partition(A, first, last)
        qSort(A, first, q-1)
        qSort(A, q+1, last)
    return A


def partition(A, first, last):
    print("entered partition")
    pivot = random.randint(first, last)
    print("the pivot index is: ", pivot)
    pivotVal = A[pivot]
    print("pivot value is: ", pivotVal)
    swap(A, last, pivot)

    i = first-1
    for j in range(first, last):
        if A[j] <= pivotVal:
            i = i + 1
            swap(A, i, j)
            print(A)
    swap(A, i+1, last)

    print('the array is now: ', A[first:last])
    return i+1

def quickSort(lst):
    print ('entered quickSort')
    ending = len(lst) - 1
    print ('the ending index is: ', ending)
    qSort(lst, 0, ending)
    return lst

def main():
    print ('Compiled!')
    testArray = [1,5,7,0,-15,-16,-17,8, 3,2 ,23,4,5,345,3,34,23,5,45,32,43,23,22,2,21,24,56,6,57,8,7,89,89]
    print(testArray)
    quickSort(testArray)
    print(testArray)



main()