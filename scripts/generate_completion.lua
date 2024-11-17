-- THIS SCRIPT SHOULDN'T BE RUN BY INSTALL RATHER IT SHOULD BE USED MANUALLY
-- BEFORE PUSHING TO THE REPO
-- THIS IS SO TO AVOID DEPENDENCIES FROM THE INSTALLER

local options = {
    { long = "all",         description = "Show all entries, including hidden" },
    { long = "Files",       description = "Show files only" },
    { long = "Directories", description = "Show directories only" },
    { long = "Symlinks",    description = "Show symlinks only" },
    { long = "Git",         description = "Show entries that are not defined in .gitignore" },
    { long = "long",        description = "Use long format" },
    { long = "recursive",   description = "Recursively list files (accepts an optional depth)" }
}

local function generate_bash()
    local script = "_lsr_completion() {\n"
    script = script .. "    local cur opts\n"
    script = script .. "    COMPREPLY=()\n"
    script = script .. "    cur=\"${COMP_WORDS[COMP_CWORD]}\"\n"
    script = script .. "    opts=\""

    for _, opt in ipairs(options) do
        script = script .. "--" .. opt.long .. " "
    end

    script = script .. "\"\n"
    script = script .. "    COMPREPLY=( $(compgen -W \"${opts}\" -- \"${cur}\") )\n"
    script = script .. "}\n"
    script = script .. "complete -F _lsr_completion lsr\n"
    return script
end

local function generate_zsh()
    local script = "#compdef lsr\n\n_arguments \\\n"
    for _, opt in ipairs(options) do
        script = script .. "    '--" .. opt.long .. "[" .. opt.description .. "]' \\\n"
    end
    return script:sub(1, -3)
end

local function generate_fish()
    local script = ""
    for _, opt in ipairs(options) do
        script = script .. "complete -c lsr -l " .. opt.long .. " -d \"" .. opt.description .. "\"\n"
    end
    return script
end

local function write_to_file(filename, content)
    local file = io.open(filename, "w")
    if file then
        file:write(content)
        file:close()
        print("Generated " .. filename)
    else
        print("Error: Could not write to " .. filename)
    end
end

local function generate_completion(shell)
    if shell == "bash" then
        write_to_file("completions/lsr.bash", generate_bash())
    elseif shell == "zsh" then
        write_to_file("completions/lsr.zsh", generate_zsh())
    elseif shell == "fish" then
        write_to_file("completions/lsr.fish", generate_fish())
    else
        return "Unsupported shell: " .. shell
    end
end

local function generate_all()
    for _, shell in ipairs({ "bash", "zsh", "fish" }) do
        generate_completion(shell)
    end
end

local shell = arg[1]
if not shell then
    generate_all()
    return
end

local script = generate_completion(shell)
print(script)
