#LastName: Gorthy
#FirstName: Derek
#Email: derek.gorthy@colorado.edu


from __future__ import print_function
import sys

# We will use a class called my trie node
class MyTrieNode:
    # Initialize some fields 
  
    def __init__(self, isRootNode):
        #The initialization below is just a suggestion.
        #Change it as you will.
        # But do not change the signature of the constructor.
        self.isRoot = isRootNode
        self.isWordEnd = False # is this node a word ending node
        self.isRoot = False # is this a root node
        self.count = 0 # frequency count
        self.next = {} # Dictionary mappng each character from a-z to the child node

    # w is the word passed to addWord
    # pos letter position
    def addHelper(self,w,pos):
        # The current letter (next) we are evaluating
        current_letter = w[pos]

        # Check and see if the next letter is a valid node
        if current_letter not in self.next:
            self.next[current_letter] = MyTrieNode(False)

        # If we have reached the end of the word
        if (pos + 1 == len(w)):
            self.next[current_letter].isWordEnd = True # Flag node as end of word
            self.next[current_letter].count = self.next[current_letter].count + 1 # Increment count

        else:
            self.next[current_letter].addHelper(w,pos + 1) # Call again if not at end of word

        return True

    # w is the word passed to addWord
    # pos letter position
    # i is a counter we increment with every call
    def lookupHelper(self,w,i,pos):
        # If we have exceeded the bound of the word we are looking for
        if i < len(w):
            current_letter = w[i] # Check the letter
            # Check is the next letter is valid node
            if current_letter in self.next:
                pos = self.next[current_letter].lookupHelper(w,i+1,pos) # Recursively call same function
                # This will be true is we are at the end of the word
                if ((i == len(w) - 1) & (self.next[current_letter].isWordEnd == True)):
                    return pos + self.next[current_letter].count # Return the count
            return pos # If not at end of word, it will return the same as the return statement above

        else:
            return pos # Will return 0 if the word is not found


    def addWord(self,w):
        self.addHelper(w,0)
        return


    def lookupWord(self,w):
        f = self.lookupHelper(w,0,0)
        return f


    # Traverse tree to last inputted letter
    def locateLastNode(self,w,pos):
        # The current letter (next) we are evaluating
        current_letter = w[pos]

        # Return 0 if last node can't be found
        if current_letter not in self.next:
            return 0

        else: 
            # If we have found the end of the word
            if (pos + 1 == len(w)):
                return self.next[current_letter] # return the node of last letter we are given

            else:
                return self.next[current_letter].locateLastNode(w, pos + 1) # Otherwise recursively call


    # base is word we are given
    # myList is the list passed throughout the entire tree
    # cur is the updated string (path) in the tree until this point
    def autoCompleteDFS(self,base,myList,cur=None):
        # Save cur through recursive call
        rest = cur
        # Iterate through all next nodes
        for key in self.next.keys():
            cur = rest # Reset cur to initial cur passed (restored after recursive call)
            cur += key # Add the current letter to pass
            # If it is the end of a word, add to the list
            if self.next[key].count > 0:
                myList.append((cur,self.next[key].count))
            # If there are still paths to follow, recursively call
            if len(self.next.keys()) > 0:
                cur, myList = self.next[key].autoCompleteDFS(base,myList,cur)
        return cur, myList


    def autoComplete(self,w):
        autoCompleteList = [] # Initialize the list to return
        lastNode = self.locateLastNode(w,0) # Find the last node cooresponding to last letter
        # Check if the last node is valid
        if lastNode != 0:
            # Check to see if the word given is a valid word
            if lastNode.count > 0:
                autoCompleteList.append((w, lastNode.count))

            w, autoCompleteList = lastNode.autoCompleteDFS(w,autoCompleteList,w) # Call the helper function

        return autoCompleteList
    
    
            

if (__name__ == '__main__'):
    t= MyTrieNode(True)
    lst1=['test','testament','testing','ping','pin','pink','pine','pint','testing','pinetree']

    for w in lst1:
        t.addWord(w)

    j = t.lookupWord('testy') # should return 0
    j2 = t.lookupWord('telltale') # should return 0
    j3 = t.lookupWord ('testing') # should return 2
    lst3 = t.autoComplete('pi')
    print('Completions for \"pi\" are : ')
    print(lst3)
    
    lst4 = t.autoComplete('tes')
    print('Completions for \"tes\" are : ')
    print(lst4)
 
    
    
     
