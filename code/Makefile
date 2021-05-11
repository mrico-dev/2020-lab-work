CC=g++
FLAGS= -std=c++17 -g -fsanitize=address -Wall -Werror -Wextra -pedantic 
SRC= src/cfg_linker/dot_reader.cc src/cfg_linker/graph_linker.cc src/main.cc src/automata_check/automata_builder.cc src/cfg_linker/cfg_utility.cc src/automata_check/formula_check.cc src/automata_check/automata_print.cc

# You can tune the formula from here
FORMULA=G("open"->F("close"))

# Choose a c file
CFILE=clang_cfg/infinite_func.c

all: exe pipeline

clean:
	-rm exe res_*.dot res_*.png raw_dot/.*.dot input/.*.dot

re: clean all

pipeline:
	-rm raw_dot/.*.dot input/.*.dot 
	clang_cfg/get_clang_cfg.sh "$(CFILE)" main raw_dot/ c
	
	clang_cfg/simplify.sh raw_dot/ input/
	
	./exe "$(FORMULA)" input/.*.dot
	@-dot -Tpng res_sub.dot -o res_sub.png
	@-dot -Tpng res_over.dot -o res_over.png
	@-dot -Tpng aut_sub.dot -o aut_sub.png
	@-dot -Tpng aut_over.dot -o aut_over.png

exe:
	$(CC) $(FLAGS) -o exe $(SRC) -lboost_graph -lboost_regex -lspot -lbddx
