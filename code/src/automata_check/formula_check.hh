#pragma once

#include <spot/tl/formula.hh>
#include <spot/tl/print.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/contains.hh>
#include <spot/twaalgos/stutter.hh>

#include "automata_builder.hh"

// Unused
bool verifies_formula(const twa_ptr twa, const std::string& formula);

int verifies_formula_on_models(const twa_ptr sub_twa,
                             const twa_ptr over_twa,
                             const spot::formula& formula);

// Returns 0 if False, 1 if True, 2 if not sure, -1 if not stutter invariant
int verifies_formula_on_models(const twa_ptr sub_twa,
                             const twa_ptr over_twa,
                             const std::string& formula);

