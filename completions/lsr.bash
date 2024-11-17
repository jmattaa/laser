_lsr_completion() {
    local cur opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    opts="--all --Files --Directories --Symlinks --Git --long --recursive "
    COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
}
complete -F _lsr_completion lsr
