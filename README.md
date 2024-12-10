<div align="center">

# Laser

###### (lsr) Basically `ls` but readable 

![demo](./assets/demo.png)

</div>

## Table of contents
   1. [Installation](#installation)
      1. [From Homebrew](#from-homebrew)
      2. [Building from source](#building-from-source)
   2. [Usage](#usage)
      1. [Configure](#configure)
      2. [Command-line options](#command-line-options)
1. [Contributing](#contributing)
2. [Authors](#authors)

**lsr** basically `ls` but with colorization and sorting for better readability.
It offers filtering options, making it easy to
locate specific files and folders (with grep you'll find exactly what you need)
It can even display the directory contents in a tree-like structure! :fire:
If that wasn't enough you can even extend the program with lua! 
(I'm too proud of that )

## Installation

### From Homebrew

[Homebrew](https://brew.sh/) (MacOS and Linux)

```sh
brew tap jmattaa/laser
brew install --formula laser
```

By installing with Homebrew, you get the shell completions for the cli as well!

### Building from source

Clone the repository:

```sh
git clone https://github.com/jmattaa/laser.git
cd laser
```

Install system-wide:

```sh
cmake -S . -B build
sudo cmake --build build
sudo cmake --install build
```

To uninstall you can run the following while being in the `laser` directory:

```sh
sudo cmake --build build --target uninstall
```

> [!NOTE] 
> This dosen't install the shell completions for the cli but you can add them
> by placing the files from the `completions` directory in a directory that is
> in your `$PATH`. Or source the file from your `bashrc`, `zshrc` or
> `config.fish` file.

## Usage

After installing you can run laser in your current directory by simply
running:
```sh
lsr
# or for a specific directory run:
lsr some-directory
```

### Configure

If you'd like to configure the program's behavior, or change the default colors,
add icons and more. You can configure the program with lua :fire:. A default 
configuration will be installed at `/usr/local/.lsr/`. But if you want to 
write some own configuration you can place it in `~/.config/lsr/lsr.lua`.

Copy the files from `/usr/local/.lsr/` to `~/.config/lsr/` and you can edit
them (do not change in the files in the `/usr/local/lsr/` directory
because they will be used
as default if there are missing values in the `.config`).

Check out the [configuration guide](/CONFIGURATION.md)

### Command-line options

The command-line options can be added by passing flags. These flags can be put 
before or after the directory you want to search, if there is no directory
you can directly pass in the flags.

- `-a` or `--all` shows hidden files
- `-D` or `--Directories` shows only directories
- `-F` or `--Files` shows only files
- `-S` or `--Symlinks` shows only symlinks
- `-G` or `--Git` shows the entries which are tracked by git and gives a status
  for modified, added and renamed files (accepts an optional value for the git
      repository path) by default the current directory is used.
      `-G/path/to/git/repo` or `--Git=/path/to/git/repo`
- `-r` or `--recursive` shows directory tree structure
    - `-r` or `--recursive` has an optional value for max depth and  it can be
      used by using `-r2` or `--recursive=2` by default the depth is until last
      level
- `-l` or `--long` displays in long format (permissions, last modified, size
  and owner)
- `-fmyfilter` or `--filter=myfilter` applies a user defined filter from lua
  check the [CONFIGURATION.md](/CONFIGURATION.md)

# Contributing

Feel free to contribute to this project to make it better :rocket: check the 
[CONTRIBUTING.md](/CONTRIBUTING.md) and follow the guidlines from there!

# Authors
- [@jmattaa](https://github.com/jmattaa)
