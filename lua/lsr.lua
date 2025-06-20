require "laser_default_colors"
require "laser_default_filters"
local utils = require "laser_default_utils"

L_recursive_max_depth = -1

function L_compare_entries(entry1, entry2, entry1_is_dir, entry2_is_dir)
    if entry1_is_dir and not entry2_is_dir then return -1 end
    if not entry1_is_dir and entry2_is_dir then return 1 end

    return entry1 < entry2 and -1 or (entry1 > entry2 and 1 or 0)
end

function L_long_format(entry, longest_name)
    local perms = string.format("%s%s", entry.type, utils.getPerms(entry.mode))

    local last_modified = os.date("%b %d %H:%M", entry.mtime)
    local size = LASER_BUILTIN_formatSize(entry.size)
    local owner = string.format("%-" .. longest_name .. "s ", entry.owner)

    return string.format("%s %s%s %s%s %s%s ",
        perms,
        L_colors.SYMLINK,
        last_modified, L_colors.MEDIA, size,
        L_colors.SYMLINK, owner)
end

function L_pre_print_entries()
end

L_default_args = {
    all = false,
    files = true,
    directories = true,
    symlinks = true,
    git = { status = false, ignore = false },
    recursive = false,
    long = false,
    directory_size = false, -- it be sad to set this to true and not set the long to true
    ensure_colors = false,
    sort = true,
    filters = {},
}
