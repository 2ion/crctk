#!/bin/bash

_crctk(){
  local cur=${COMP_WORDS[COMP_CWORD]}
  local prev=${COMP_WORDS[COMP_CWORD-1]}

  if [[ $cur =~ ^-$ ]] ; then
    COMPREPLY=( $(compgen -W '-X -x -t -n -v -V -h -s -r -C -c -d -e -p -a -m' -- $cur) )
    return
  elif [[ $cur =~ ^-- ]] ; then
    COMPREPLY=( $(compgen -W '--verify --verify-db --prefer-hexstring --calc --numerical --create-db --delete --append --print --tag --strip-tag --remove-tag --tag-regex --help --version --merge' -- $cur) )
    return
  fi
}

complete -F _crctk crctk
