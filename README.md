# KomfyDB
<<<<<<< HEAD
<<<<<<< HEAD
The best database we've ever made
=======
>>>>>>> ec35cc6 (Initial commit)
=======

Simple database C++ implemenatation based on MIT SimpleDB.

## Requierments

- clang-13
- bazel
- openjdk-11-jdk

On debian-based:

```
sudo apt update && sudo apt install clang-13 bazel openjdk-11-jdk
```

Also, add this to your `.bashrc`:

```
export CC=clang-13
```

Or, everytime you issue a bazel command, add this as a prefix:

```
CC=clang-13 bazel ...
```
<<<<<<< HEAD
>>>>>>> ed34509 (Updated readme)
=======

## Using bazel

I highly encourage you to take a look on 
[this tutorial](https://docs.bazel.build/versions/main/tutorial/cpp.html) as
it's really short and shows almost everything we need here.

To simply see if bazel is working run (anywhere in the source tree!):

```
CC=clang-13 bazel run //komfydb:main
```

>>>>>>> d6d0c0e (Updated readme)
