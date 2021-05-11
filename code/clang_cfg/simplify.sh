#!/bin/sh

for dotfile in "$1"/.*.dot; do

    filename=$(basename ${dotfile})
    dest="$2/${filename}"

    # Removing nodes with no calls
    sed -E 's/\{[^@]*\}/\{\}/g' "${dotfile}" > "${dest}"

    #sed -i -E 's/\{.*call .* @([a-zA-Z0-9_]*)\(.*\}/\{call \1\}/g' "${dest}"
    #sed -i -E 's/\{.*call .* @([a-zA-Z0-9_]*)\(.*\}/\{call \1\}/g' "${dest}"

    # Erasing content between { and \l
    #sed -i -E 's/\{[^@]*\\l/\{/g' "${dest}"
    # Erasing content betweene { and call
    sed -i -E 's/\{[^@]*call/\{call/g' "${dest}"
    # Erasing content between \l and }
    #sed -i -E 's/\\l[^@]*\}/\}/g' "${dest}"

    # Erasing content between \l and \l
    #sed -i -E 's/\\l[^@]*\\l//g' "${dest}"

    # Replacing call ... @function( by call function(
    sed -i -E 's/call [^@]* @([a-zA-Z0-9_]*)\(/call \1\(/g' "${dest}"

    #Removing spaces
    sed -i -E 's/\{ *call/\{call/g' "${dest}"
    sed -i -E 's/\\l *call/\\l call/g' "${dest}"
    #Putting spaces
    sed -i -E 's/([a-zA-Z0-9_]*)\\l/\1 \\l/g' "${dest}"

    # Removing sub-nodes
    sed -i -E 's/^(.*Node0x[0-9a-f]*):s0(.*)$/\1\2/g' "${dest}"
    sed -i -E 's/^(.*Node0x[0-9a-f]*):s1(.*)$/\1\2/g' "${dest}"

done
