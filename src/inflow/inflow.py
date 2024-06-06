import sys
from collections.abc import Iterable

Block = tuple[list[str], int, str]
State = tuple[int, float, int, int]

# characters allowed to be in a prefix
PREFIX = set(" >:-*|#$%'\"")


def get_lines(par: list[str], width: int) -> list[int]:
    """Compute optimal line lengths with forward greedy."""
    lines, count, i = [0], 0, 0
    while i < len(par):
        # break onto a new line
        line_width = len(par[i])
        count += 1
        while i + 1 < len(par) and line_width <= width:
            i += 1
            lines.append(count)
            line_width += 1 + len(par[i])
        if line_width <= width:
            i += 1
            lines.append(count)

    return lines


def vardp(par: list[str], lines: list[int], width: int) -> list[State]:
    """Computes the minimum variance, constrained to use optimal lines."""
    # state (index, variance, sum of x^2 terms, sum of x)
    dp: list[State] = [(0, 0.0, 0, 0)] * (len(par) + 1)
    for i in range(1, len(par) + 1):
        k, best, sum_x2, sum_x, x = -1, float("inf"), 0, 0, 0
        for j in range(i - 1, -1, -1):
            # add 1 for space, if the current line isn't empty
            line_width = x + (x != 0) + len(par[j])
            if line_width <= width:
                x = line_width
                _, _, sum_x2j, sum_xj = dp[j]
                n = 1 + lines[j]
                sum_x2j += x * x
                sum_xj += x
                # Var[X] = E[X^2] - E[X]^2
                mean = sum_xj / n
                var = sum_x2j / n - mean * mean
                if var < best and n == lines[i]:
                    k, best, sum_x2, sum_x = j, var, sum_x2j, sum_xj
            else:
                break
        dp[i] = (k, best, sum_x2, sum_x)

    return dp


def process(par: list[str], width: int, prefix: str) -> str:
    """Takes in a paragraph and returns a string with a new line width."""
    if len(par) == 0:
        return prefix
    assert max(map(len, par)) <= width, "line too long"

    lines = get_lines(par, width)
    dp = vardp(par, lines, width)
    # if the paragraph is less than 3 lines long, don't ignore the last line
    if lines[-1] <= 3:
        k = len(par)
    else:
        best, k, x = [float("inf")] * 2, [0] * 2, 0
        for i in range(len(par) - 1, -1, -1):
            x += (x != 0) + len(par[i])
            if x > width:
                break
            b = x <= dp[i][-1] / lines[i]
            if lines[i] + 1 == lines[-1] and dp[i][1] < best[b]:
                best[b], k[b] = dp[i][1], i
        k = k[1] if best[1] != float("inf") else k[0]

    out, i = [], k
    if k < len(par):
        out.append(" ".join(par[k:]))
    while i > 0:
        j = dp[i][0]
        out.append(" ".join(par[j:i]))
        i = j
    # add prefix to each line
    return "\n".join(map(lambda x: prefix + x, out[::-1]))


def get_prefix(lines: list[str]) -> str:
    """Finds the common prefix for the lines."""
    # find prefix, where a prefix is defined as a series
    # of the same character, if the character is in PREFIX
    prefix = []
    smallest = min(map(len, lines))
    for ch in range(smallest):
        for line in lines:
            if line[ch] != lines[0][ch] or line[ch] not in PREFIX:
                break
        else:
            prefix.append(lines[0][ch])
            continue
        break
    return "".join(prefix)


def parse_prefix(lines: list[str], width: int) -> list[Block]:
    """Parses lines into a list of tokens, taking into account prefixes."""
    prefix = get_prefix(lines)
    if len(prefix) == 0:
        par = []
        for line in lines:
            par += line.split()
        return [(par, width, prefix)]
    else:
        lines = [line[len(prefix) :] for line in lines]
        return [
            (par, width, prefix)
            for par, width, _ in parse_lines(lines, width - len(prefix))
        ]


def parse_lines(lines: Iterable[str], width: int) -> list[Block]:
    """Read input lines into paragraphs, making empty lines []."""
    pars, par = [], []
    for line in lines:
        if line != "\n":
            par.append(line)
        else:
            if len(par) > 0:
                pars += parse_prefix(par, width)
            pars.append(([], width, ""))
            par, par = [], []
    if len(par) > 0:
        pars += parse_prefix(par, width)
    return pars


def main() -> None:
    """Command-line interface."""
    # read command line arguments - one parameter, width
    width = int(sys.argv[1]) if len(sys.argv) > 1 else 79
    print("\n".join(map(lambda x: process(*x), parse_lines(sys.stdin, width))))


if __name__ == "__main__":
    main()
