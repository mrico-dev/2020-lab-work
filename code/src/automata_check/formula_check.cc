#include "formula_check.hh"

bool verifies_formula(const twa_ptr twa, const std::string& formula)
{
    auto f = spot::parse_formula(formula);

    return spot::contains(f, twa);
}

int verifies_formula_on_models(const twa_ptr sub_twa,
                             const twa_ptr over_twa,
                             const spot::formula& formula)
{
    if (!spot::is_stutter_invariant(formula))
        return -1;

    if (spot::contains(formula, over_twa))
        return 1;

    if (not spot::contains(formula, sub_twa))
        return 0;

    return 2;
}

int verifies_formula_on_models(const twa_ptr sub_twa,
                             const twa_ptr over_twa,
                             const std::string& formula)
{
    auto f = spot::parse_formula(formula);
    std::cout << "Verifying fomula: " << f << '\n';

    return verifies_formula_on_models(sub_twa, over_twa, f);
}
