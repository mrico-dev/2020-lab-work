#!/bin/sh
[ $# -eq 4 ] || (echo "Usage: ./get_clang_cfg file function_name output_folder c|cc" && exit 1)

# Generate .dot file
if [ "$4" = c ]; then  
    clang -S -emit-llvm "$1" -o - | opt -analyze -dot-cfg
else
    clang++ -S -emit-llvm "$1" -o - | opt -analyze -dot-cfg
fi

# CallGraph command line
#clang++ -### -fsyntax-only -S -emit-llvm "$1" -o - | opt -analyze -dot-callgraph

# Old way to unmangle
#echo $(llvm-cxxfilt < callgraph.dot) > callgraph.dot

# Generate .png file
#dot -Tpng callgraph.dot -o res.png

# TODO Unmangle everthing
# Unmangle and generate png
cat .$2.dot |
   c++filt | # Other way to unmangle
   llvm-cxxfilt | # Unmangle
   sed 's,>,\\>,g; s,-\\>,->,g; s,<,\\<,g' | # for lecture purpose
   gawk '/external node/{id=$1} $1 != id' | # Removing useless nodes
   #dot -Tpng -o res.png # Getting png

#rm "$3"/.*.dot
mv .*.dot "$3"/

# Display and clean
# feh res.png
#rm res.png
