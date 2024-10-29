<div align="center">

# Laser

###### (lsr) Basically `ls` but readable

![demo](./assets/demo.png)

</div>

**lsr** basically `ls` but with colorization and sorting for better readability.
It offers filtering options, making it easy to
locate specific files and folders (with grep you'll find exactly what you need)
It can even display the directory contents in a tree-like structure!

## Installation

### Building from source

> [!NOTE] 
> If you want to disable the nerdfonts symbols remove the 
`-DLASER_NF_SYMBOLS` flag from the `Makefile` under the variable `LASER_DEFINES`
before compiling.

Clone the repository:

```sh
git clone https://github.com/jmattaa/laser.git
cd laser
```

Install system-wide:

```sh
sudo make install
```

To uninstall you can run

```sh
sudo make uninstall
```

## Usage

After installing you can run laser in your current directory by simply
running:
```sh
lsr
# or for a specific directory run:
lsr some-directory
```

### Command-line options

The command-line options can be added by passing flags. These flags can be put 
before or after the directory you want to search, if there is no directory
you can directly pass in the flags.

- `-a` or `--all` shows hidden files
- `-D` or `--Directories` shows only directories
- `-F` or `--Files` shows only files
- `-S` or `--Symlinks` shows only symlinks
- `-G` or `--Git` shows only the entries that are not named in `.gitignore` (works only if there exists a `.gitignore`)
- `-r` or `--recursive` shows directory tree structure


The flags can be chained for example using:
```sh
lsr -Dar # this will show all directories including hidden ones recursivly
lsr -GDS # this will only show directories (not hidden) and symlinks that are not mentioned in .gitignore
```

# Authors
- [@jmattaa](https://github.com/jmattaa)
