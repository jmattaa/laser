function L_compare_entries(name_a, name_b, is_dir_a, is_dir_b)
    if is_dir_a and not is_dir_b then return -1 end
    if not is_dir_a and is_dir_b then return 1 end
    return name_a < name_b and -1 or (name_a > name_b and 1 or 0)
end
