L_filters = {
    recent = function(entry) return os.time() - entry.mtime < 86400 end,
    large = function(entry) return entry.size > 1024 * 1024 end,
    small = function(entry) return entry.size < 1024 * 1024 end,
}
