<div align="center">

# Configure laser!

</div>

## Table of contents
   1. [Creating Your Own Configuration File](#creating-your-own-configuration-file)
   2. [Writing Your Own Configuration](#writing-your-own-configuration)
      1. [Constants](#constants)
         1. [`L_colors`](#l_colors)
         2. [`L_recursive_max_depth`](#l_recursive_max_depth)
         3. [`L_default_args`](#l_default_args)
         4. [`L_filters`](#l_filters)
      2. [Functions](#functions)
         1. [`L_compare_entries()`](#l_compare_entries())
         2. [`L_long_format()`](#l_long_format())
      3. [Using Defaults](#using-defaults)

Laser is highly customizable through Lua scripts. The default configuration
file is located at `/usr/local/share/lsr/lsr.lua`. This file is loaded by the
program and sets the default behavior for laser. You can modify this file or
create your own configuration file to change the behavior of laser.

## Creating Your Own Configuration File

To create your own configuration all that you need to do is to create a lua
file at `/usr/local/share/lsr/`. This will be the default file where laser will
look for your configuration. You can of course add more files to this directory
(check the `lua` directory in the repo). Other files in this directory will not
be loaded by laser but you can do a `require` to them to load them into the
`lsr.lua` file.

## Writing Your Own Configuration

The default configuration file is located at `/usr/local/share/lsr/lsr.lua`.
You can copy these files and edit them to your liking. But here comes a small
guide on what you can do with a blank `lsr.lua` file.

> [!NOTE] 
> All laser-specific functions and constants are prefixed with `L_`.

### Constants

#### `L_colors`

You can configure the colors of the different entry types by modifying the
global `L_colors` table. You can even add glyphs or text as it is just a normal
string. The definition of the table should be placed in the `lsr.lua` file
(check the [Writing Your Own Configuration](#writing-your-own-configuration)).
But if you check the `lua` directory in the repo you will find the default
colors are placed in a different file. You can do this by doing a
`require("colors")` in the `lsr.lua` (assuming that the `colors.lua` file is in
the same directory).

The default configuration of `L_colors` is:
```lua
L_colors = {
    DIR = "\x1b[34m",
    SYMLINK = "\x1b[36m",
    FILE = "\x1b[0m",
    HIDDEN = "\x1b[90m",
    EXEC = "\x1b[32;4m",
    ARCHIVE = "\x1b[31m",
    MEDIA = "\x1b[33m",
    DOCUMENT = "\x1b[35;3m",
}
```
#### `L_recursive_max_depth`

You can configure the maximum depth of the directory tree by modifying the
global `L_recursive_max_depth` variable. It defaults to -1 (which means
infinite). This is the default value that will be used when run `-r`. By doing
a `-rN` you can set the maximum depth to `N` and override the default value.

```lua
L_recursive_max_depth = -1
```

#### `L_default_args`

You can configure the default arguments that should be used when running `lsr`
by creating the global `L_default_args` table. 

The default configuration of `L_default_args` is:
```lua
L_default_args = {
    all = false, -- show hidden files
    files = true, -- show files
    directories = true, -- show directories
    symlinks = true, -- show symlinks
    git = false, -- show git status and entries tracked by git
    long = false, -- show long format

    filters = {}, -- apply filters defined in `L_filters` 
                    -- (NOTE: the name should be a string)
}

```

#### `L_filters`

You can configure the default filters by modifying the global table named
`L_filters`. You can create your own filters by adding a key with the filter name
that will be accesed by the `-f` flag. (eg. `-fmyfilter`).

The default table looks like this:

```lua
L_filters = {
    recent = function(entry) return os.time() - entry.mtime < 24 * 60 * 60 end,
    large = function(entry) return entry.size > 1024 * 1024 end,
    small = function(entry) return entry.size < 1024 * 1024 end,
    me = function(entry) 
        return entry.owner == user -- the user variable is gotten from os.getenv
    end
}
```

The function of the filter gets a entry table as an argument check the
definition of the entry table at the [`L_long_format`](#l_long_format)
function. Every filter must return a boolean.

> [!NOTE]
> If you add your own filters all the default filters will be overridden. You
> can still use the default filters by adding them to the `L_filters` table.

If you want to add your on filters but do not want to override the default values
you could define them like this:

```lua
L_filters.myfilter = function(entry) return true end
L_filters.myotherfilter = function(entry) return false end
```

### Functions

There are a few functions you can use to modify the behavior of the program.

#### `L_compare_entries()`

This function is used to sort the entries in the directory. This is by default
used to add "gravity" to files so that they fall at the bottom of the list. And
directories being placed at the top.

This fcuntion have the following signature: 

```lua
L_compare_entries(entry1, entry2, entry1_is_dir, entry2_is_dir)
```

```lua
-- files should get "gravity" and fall to the bottom of the list
-- then the entries are sorted alphabetically
function L_compare_entries(entry1, entry2, entry1_is_dir, entry2_is_dir)
    if entry1_is_dir and not entry2_is_dir then return -1 end
    if not entry1_is_dir and entry2_is_dir then return 1 end

    return entry1 < entry2 and -1 or (entry1 > entry2 and 1 or 0)
end
```

#### `L_long_format()`

This function is used to format the entries in long format. This is by default
used to show the permissions, the size, last modified and owner of the entry.

This fcuntion have the following signature:

```lua
L_long_format(entry, longest_name)
```

Where `entry` is a table with the following keys:

- `name`: The name of the entry
- `mode`: The c stat mode of the entry (this can be used to get the permissions
  and do much more, there is a default util function called `getPerms(mode)`
  that can be found in the lua directory of this repo, under `utils.lua`)
- `size`: The size of the entry (there is a util function called
  `formatSize(size)` to make it human readable)
- `mtime`: The last modified time of the entry you can pass it through
  `os.date` to format it
- `owner`: The name of the owner of the entry
- `type`: The type of the entry in one character
    - `d`: Directory
    - `l`: Symlink
    - `c`: Character device
    - `b`: Block device
    - `p`: Named pipe (fifo)
    - `s`: Socket
    - `-`: File

The `longest_name` is the length of the longest name in the directory. This
could be used to align the output.

The default function is:

```lua
function L_long_format(entry, longest_name)
    local perms = string.format("%s%s", entry.type, utils.getPerms(entry.mode))

    local last_modified = os.date("%b %d %H:%M", entry.mtime)
    local size = utils.formatSize(entry.size)
    local owner = string.format("%-" .. longest_name .. "s ", entry.owner)

    return string.format("%s %s%s %s%s %s%s ",
        perms,
        L_colors.SYMLINK,
        last_modified, L_colors.MEDIA, size,
        L_colors.SYMLINK, owner)
end
```

#### `L_pre_print_entries()`

This function is called before the entries are printed. It can be used to add
anything before the entries are printed. The function takes no arguments, and it
should not return anything.

The default implementation is that it does nothing. But an idea to use this 
could be running a command like `clear` before printing the entries.

```lua
function L_pre_print_entries()
    os.execute("clear")
end
```

### Using Defaults

If you do not define these functions in your configuration file, the default
functions from the default configuration file will be used automatically. If
there is a missing constant or function the default configuration values will
be
used.
