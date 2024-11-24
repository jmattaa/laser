require "colors"
local utils = require "utils"

function L_compare_entries(name_a, name_b, is_dir_a, is_dir_b)
    if is_dir_a and not is_dir_b then return -1 end
    if not is_dir_a and is_dir_b then return 1 end

    return name_a < name_b and -1 or (name_a > name_b and 1 or 0)
end

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
