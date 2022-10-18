# KomfyDB

Simple database C++ implemenatation based on MIT SimpleDB.

## Requierments

- clang-13
- bazel
- openjdk-11-jdk

On debian-based:

```
sudo apt update && sudo apt install clang-13 bazel openjdk-11-jdk
```

## Using bazel

I highly encourage you to take a look on 
[this tutorial](https://docs.bazel.build/versions/main/tutorial/cpp.html) as
it's really short and shows almost everything we need here.

First run:
```
source ./env.sh
```

To see if bazel is working run (anywhere in the source tree!):

```
bazel run //komfydb:komfydb -- --logtostderr=1
```

To cleanup simply run
```
bazel clean --expunge
```

