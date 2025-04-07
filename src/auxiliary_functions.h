#ifndef AUXILIARY_FUNCTIONS_H
#define AUXILIARY_FUNCTIONS_H

template <typename F>
struct FinalAction {
    // Performs an action `act` when destructor called
    explicit FinalAction(F f) : act(f) {}
    ~FinalAction() { act(); }
    F act;
};

template <typename F>
[[nodiscard]] auto Finally(F f)
// Wrapper for FinalAction
{
    return FinalAction{f};
}

#endif // AUXILIARY_FUNCTIONS_H
