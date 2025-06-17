local M = {}

function M.getPerms(mode)
    local perms = string.format("%s%s%s%s%s%s%s%s%s",
        (mode & 0000400) ~= 0 and "r" or "-",
        (mode & 0000200) ~= 0 and "w" or "-",
        (mode & 0000100) ~= 0 and "x" or "-",
        (mode & 0000040) ~= 0 and "r" or "-",
        (mode & 0000020) ~= 0 and "w" or "-",
        (mode & 0000010) ~= 0 and "x" or "-",
        (mode & 0000004) ~= 0 and "r" or "-",
        (mode & 0000002) ~= 0 and "w" or "-",
        (mode & 0000001) ~= 0 and "x" or "-")

    return perms
end

function LASER_BUILTIN_formatSize(size)
    if size == -1 then
        return "      ---"
    end

    if size < 1024 then
        return string.format("%6d  B", size)
    elseif size < 1024 * 1024 then
        return string.format("%6.2f KB", size / 1024)
    elseif size < 1024 * 1024 * 1024 then
        return string.format("%6.2f MB", size / (1024 * 1024))
    else
        return string.format("%6.2f GB", size / (1024 * 1024 * 1024))
    end
end

return M
