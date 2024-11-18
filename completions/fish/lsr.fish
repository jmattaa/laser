# Fish completions for lsr
complete -c lsr -s a -l all -d 'Show all entries, including hidden'
complete -c lsr -s F -l Files -d 'Show files only'
complete -c lsr -s D -l Directories -d 'Show directories only'
complete -c lsr -s S -l Symlinks -d 'Show symlinks only'
complete -c lsr -s G -l Git -d 'Show entries that are not defined in .gitignore'
complete -c lsr -s l -l long -d 'Use long format'
complete -c lsr -s r -l recursive -d 'Show in tree format'
complete -c lsr -l completions -d 'Generate shell completions'
