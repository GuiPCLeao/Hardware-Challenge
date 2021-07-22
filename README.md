FFT Library:
[fftw-3.3.9](http://www.fftw.org/download.html)

Installation section: [link](http://www.fftw.org/fftw3_doc/Installation-and-Customization.html)

Compiled and executed under Ubuntu 20.04.

``` shell
$ g++ -std=c++11 main.cpp -lm -lfftw3 -o main
$ ./main
```

Execution creates `output.txt`.

----
Author: Guilherme P.C. Leão