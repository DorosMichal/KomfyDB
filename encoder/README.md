# Heap file encoder

This is taken and patched from the MIT SimpleDB. Simple usage:

- Create a test file `test.txt`:
  ```
    1,lol,2,xD
    3,lmao,4,siema
  ```
- Then compile and run the encoder:
  ```
  javac HeapFileEncoder.java
  java HeapFileEncoder test.txt 4 int,string,int,string
  ```
