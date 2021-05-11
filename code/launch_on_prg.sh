# - - - - - - - - - - - - - - - - - -
# Launch property verification on all files in prg/*.c
# - - - - - - - - - - - - - - - - - -

[ $# -eq 1 ] || echo "Usage: ./launch_on_prg.sh [LTL FORMULA]"

# Quick and ugly way to see if exe exists
ls exe

rm raw_dot/.*.dot
rm input/.*.dot

for file in prg/*.c; do 
    clang_cfg/get_clang_cfg.sh ${file} main raw_dot/ c
done

clang_cfg/simplify.sh raw_dot/ input/	
./exe "$1" input/.*.dot
