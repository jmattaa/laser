<div align="center">

# Laser

###### (lsr) Basically `ls` but readable 

![demo](./assets/demo.png)

</div>

## Table of contents
   1. [Installation](#installation)
      1. [From Homebrew](#from-homebrew)
      2. [From AUR](#from-aur)
      3. [Building from source](#building-from-source)
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

### From AUR

[AUR](https://aur.archlinux.org/packages/laser-git) (Arch Linux)

```sh
yay -S laser-git
```

### Building from source

Clone the repository:

```sh
git clone https://github.com/jmattaa/laser.git
cd laser
```

Install system-wide:

```sh
cmake -S . -B build
cmake --build build
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
configuration will be installed at `~/.lsr/`. But if you want to 
write some own configuration you can place it in `~/.config/lsr/lsr.lua`.

Copy the files from `~/.lsr/` to `~/.config/lsr/` and you can edit them (do not
change in the files in the `~/.lsr/` directory because they will be used as
default if there are missing values in the config files).

Check out the [configuration guide](/CONFIGURATION.md) for more information.

### Command-line options

The command-line options can be added by passing flags. These flags can be put 
before or after the directory you want to search, if there is no directory
you can directly pass in the flags.

- `-a` / `--all`  Displays hidden files.

- `-D` / `--Directories`  Displays only directories.

- `-F` / `--Files`  Displays only files.

- `-S` / `--Symlinks`  Displays only symbolic links.

- `-G` / `--Git`  Combines the `--git-status` and `--git-ignore` flags.  
  - Defaults to use the current directory as git repo.
  - To specify a Git repository: `-G/path/to/git/repo` or
    `--Git=/path/to/git/repo`.

- `-g` / `--git-status`  Displays the Git status of files.  
  - Defaults to use the current directory as git repo.
  - To specify a Git repository: `-g/path/to/git/repo` or
    `--git-status=/path/to/git/repo`.
  - Lowercase letters as a status means that the file is not staged. And 
    uppercase letters mean that the file is staged.
  - Status letters:  
    - `A` = added  
    - `M` = modified  
    - `R` = renamed  
    - `T` = type changed

- `-i` / `--git-ignore`  Displays Git-ignored files.  
  - Defaults to use the current directory as git repo.
  - To specify a Git repository: `-i/path/to/git/repo` or
    `--git-ignore=/path/to/git/repo`.

- `-r` / `--recursive`  Displays the directory tree structure.  
  - Optional depth: Specify a maximum depth with `-r2` or `--recursive=2`.  
  - Default: Expands to the last level.

- `-l` / `--long`  Displays detailed file information, including permissions,
  last modified date, size, and owner.

- `-c` / `--ensure-colors`  Ensures that the output is colorized even if output
  is redirected to another file than stdout (e.g. `lsr > file.txt` or `lsr |
  less`).

- `-fmyfilter` / `--filter=myfilter`  Applies a user-defined filter written in
  Lua.  
  - For more information, see [CONFIGURATION.md](CONFIGURATION.md#L_filters).

- `-!` / `--no-lua` Ignores the `~/.lsr/lsr.lua` file and runs without user 
  config and user defined filters.

# Contributing

Feel free to contribute to this project to make it better :rocket: check the 
[CONTRIBUTING.md](/CONTRIBUTING.md) and follow the guidlines from there!

# Authors
- [@jmattaa](https://github.com/jmattaa)
