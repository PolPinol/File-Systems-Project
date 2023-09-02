# File Systems Project

## Build
The following command will build the executable `./fsutils`.
```bash
$ make
```

The following command will clean the binaries generated.
```bash
$ make clean
```

## Commands
### Phase 1
The flag `--info` will show the metadata of the file system.
```bash
$ ./fsutils --info <file system>
```
### Phase 2
The flag `--tree` will show a listing of the files and directories in the file system.
```bash
$./fsutils --tree <file system>
```

### Phase 3
The flag `--cat` is able to print the data of the specified file if it has been located inside the provided FAT16 file system.
```bash
$./fsutils --cat <FAT16 file system> <file>
```

## Resources
There are some resources that have been used to test the code ubicated in the `res` folder:
- fat1.fs: FAT16 file system.
- libfat: FAT16 file system.
- mangaFat: FAT16 file system.
- pokeFat: FAT16 file system.
- datasetFat: FAT16 file system.
- bigFileExt: ext2 file system.
- datasetExt: ext2 file system.
- mongoExt: ext2 file system.
- lolext: ext2 file system.
- yakofs: ext2 file system.


## Authors
> Pol Piñol Castuera &nbsp;&middot;&nbsp;
> Ángel Garcia Gascón &nbsp;&middot;&nbsp;
