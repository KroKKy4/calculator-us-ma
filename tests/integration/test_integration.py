import subprocess
import pytest

def run_app(args, input_str):
    return subprocess.run(
        ["build/app.exe"] + args,
        input=input_str,
        text=True,
        capture_output=True
    )


def test_int_basic_operations():
    cases = [
        ("3+5*2", "13"),
        ("(20-4)/3*2", "10"),
        ("7-10/3+4", "8"),
        ("-5+9*2", "13"),
        ("2*(3+4*(5-2))", "30")
    ]
    for expr, expected in cases:
        res = run_app([], expr)
        assert res.returncode == 0
        assert res.stdout.strip() == expected

def test_float_precision():
    cases = [
        ("10/3", "3.3333"),
        ("1.0/3.0", "0.3333"),
        ("5/2*3.5", "8.7500"),
        ("(1.5+2.5)*3", "12.0000"),
        ("-4.2*3", "-12.6000")
    ]
    for expr, expected in cases:
        res = run_app(["--float"], expr)
        assert res.returncode == 0
        assert res.stdout.strip() == expected

def test_invalid_characters():
    cases = [
        "12a+5",
        "7.2*3",
        "15$*3",
        "2^3",
        "5 + '3'"
    ]
    for expr in cases:
        res = run_app([], expr)
        assert res.returncode != 0

def test_syntax_errors():
    cases = [
        "5++3",
        "12--4",
        "*5+3",
        "5*",
        "(4+3*2",
        "4+3)*2",
        "5 4 + 3"
    ]
    for expr in cases:
        res = run_app([], expr)
        assert res.returncode != 0

def test_division_errors():
    cases = [
        ("5/0", []),
        ("5/(2-2)", []),
        ("5/(0.0)", ["--float"]),
        ("5/(1-1.0)", ["--float"])
    ]
    for expr, args in cases:
        res = run_app(args, expr)
        assert res.returncode != 0

def test_whitespace_handling():
    cases = [
        ("  12   +   4  ", "16"),
        ("\t5\n*\n3\r", "15"),
        ("( 2 + 3 ) * 4", "20")
    ]
    for expr, expected in cases:
        res = run_app([], expr)
        assert res.returncode == 0
        assert res.stdout.strip() == expected

def test_unary_operators():
    cases = [
        ("+-5", "-5"),
        ("--5", "5"),
        ("-+-+5", "-5"),
        ("++5", "5")
    ]
    for expr, expected in cases:
        res = run_app([], expr)
        assert res.returncode == 0
        assert res.stdout.strip() == expected

def test_empty_input():
    res = run_app([], "")
    assert res.returncode != 0

def test_only_whitespace():
    res = run_app([], "   \t\n\r   ")
    assert res.returncode != 0
