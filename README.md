# 7zip_fix


I found 7-Zip's memory leak in extract rar files, this is a fix version of it.

The repository is base on 7-Zip 16.04, you can download it from

https://sourceforge.net/projects/sevenzip/files/7-Zip/16.04/7z1604-src.7z/download

Changes files include:

```
CPP/7zip/Archive/DllExports2.cpp
CPP/7zip/Crypto/MyAes.h 
CPP/7zip/Crypto/RarAes.h 
```

Discussion:

https://sourceforge.net/p/sevenzip/bugs/2030/

git diff:

see file ./diff.patch