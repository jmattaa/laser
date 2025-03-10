# Fish completions for lsr
complete -c lsr -s a -l all -d "Show all entries, including hidden"
complete -c lsr -s F -l Files -d "Show files only"
complete -c lsr -s D -l Directories -d "Show directories only"
complete -c lsr -s S -l Symlinks -d "Show symlinks only"
complete -c lsr -s G -l Git -d "Do not show ignored git files" -r
complete -c lsr -s g -l git-status -d "Show git status for entries" -r
complete -c lsr -s i -l git-ignored -d "Ignore git ignored files" -r
complete -c lsr -s l -l long -d "Use long format"
complete -c lsr -s r -l recursive -d "Show in tree format" -r
complete -c lsr -s f -l filter -d "Filter out files using lua filters (L_filters in lsr.lua)" -r
complete -c lsr -s ! -l no-lua -d "Don't use user defined configuration from lsr.lua"
complete -c lsr -s v -l version -d "Print the current version"
complete -c lsr -s h -l help -d "Print help message"
complete -c lsr -l completions -d "Generate shell completions" -r
