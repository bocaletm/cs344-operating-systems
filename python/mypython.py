#####################
# Mario Bocaletti
# cs344 
# 02/23/19
# Python Exploration
# ###################

import random

#########
# randChars
#   Returns string of n random characters
#   lc alpha with no whitespace
#########
def randChars(n):
  string = ""
  for i in range(n):
    randAscii = random.randint(97,122)
    string += chr(randAscii)
  return string  

#########
# randInt
# returns a random in between
# the arg ints inclusive
########
def randInt(a,b):
  return random.randint(a,b)

#########
# main
#########
numFiles = 3
numChars = 10
rangeLow = 1
rangeHigh = 42
for i in range(numFiles):
  filename = "file" + str(i)
  content = randChars(numChars)
  f = open(filename,"w+")
  f.write(content)
  f.write("\n")
  print(content)
a = randInt(rangeLow,rangeHigh)
b = randInt(rangeLow,rangeHigh)
print(a)
print(b)
print(a*b)


