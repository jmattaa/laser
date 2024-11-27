require "colors"
local utils = require "utils"

function L_compare_entries(entry1, entry2, entry1_is_dir, entry2_is_dir)
    if entry1_is_dir and not entry2_is_dir then return -1 end
    if not entry1_is_dir and entry2_is_dir then return 1 end

    return entry1 < entry2 and -1 or (entry1 > entry2 and 1 or 0)
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

function L_pre_print_entries()
end
