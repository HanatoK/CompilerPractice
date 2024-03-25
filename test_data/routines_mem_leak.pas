PROGRAM RoutinesTest(input, output);

TYPE
    enum = (alpha, beta, gamma);

VAR
    e : enum;

FUNCTION func(x : real; i : real; n : integer) : real;
    BEGIN {func}
        func := 0.3;
    END {func};

BEGIN {RoutinesTest}
    e := beta;
END {RoutinesTest}.
