from __future__ import print_function
import sys
import random
import time



def mergeSort(lst):
    # TODO: Implement mergesort here
    # You can add additional utility functions to help you out.
    # But the function to do mergesort should be called mergeSort
    if len(lst) > 1:
        lefthalf = lst[:len(lst)//2]
        righthalf = lst[len(lst)//2:]

        mergeSort(lefthalf)
        mergeSort(righthalf)

        a=0 
        b=0
        c = 0
        while a < len(lefthalf) and b < len(righthalf):
            if lefthalf[a] < righthalf[b]:
                lst[c]=lefthalf[a]
                a = a+1
            else:
                lst[c]=righthalf[b]
                b = b+1
            c=c+1

        while a < len(lefthalf):
            lst[c]=lefthalf[a]
            a=a+1
            c=c+1

        while b < len(righthalf):
            lst[c]=righthalf[b]
            b=b+1
            c=c+1


    return lst

def main():
    testArray = [2,-19,0,0,4,13,15,20,4,2]
    print (testArray)
    mergeSort(testArray)
    print (testArray)

main()