echo off
echo You must specify which version of abered you want to make
echo REMEMBER to delete *.o before you change versions that you
echo want to build.
echo arguments:
echo pcc286: A limited zone size, but will run on 80286
echo gcc386: Will only run on 80386+ and you must have go32.exe
echo unix  : Builds the UNIX version of abered with gcc.
echo usage:
echo "make [pcc286 | gcc386 | unix]"
