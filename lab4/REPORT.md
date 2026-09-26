# Lab 4 - The tests you cannot write

## Part A: Five Rules

1. The snake dies if its head moves out of the bounds of the game board.
2. The snake dies if its head collides with its own body, an obstacle, or a bomb.
3. Eating a regular fruit increases the score by exactly 10 points and increases the snake's length by exactly one segment.
4. Eating a power fruit increases the score by exactly 50 points and increases the snake's length by exactly one segment.
5. The snake cannot directly reverse its current movement direction (e.g., if it is moving RIGHT, it ignores any input to move LEFT).

## Part B: Try to test all five (90 min)

| Rule | Test written? | Blocking dependency (file:line in lab4-base) |
|------|---------------|------------------------------------------------|
| 1. The snake dies if its head moves out of the bounds of the game board. | No | `Snake.cpp:236` (and 158) — `_getch()` called in constructor, preventing instantiation without a real keyboard. |
| 2. The snake dies if its head collides with its own body, an obstacle, or a bomb. | No | `Snake.cpp:236` — `_getch()` in constructor blocks instantiation. `Snake.cpp:23` — `snake` vector is private, cannot forcefully set up a collision state. |
| 3. Eating a regular fruit increases score by 10 and length by one. | No | `Snake.cpp:86` — `rand()` used to place fruit (non-deterministic). `Snake.cpp:36` — `score` is private, cannot assert the result. |
| 4. Eating a power fruit increases score by 50 and length by one. | No | `Snake.cpp:98` — `rand()` used to place power fruit. `Snake.cpp:23` — `snake` vector is private, cannot assert length change. |
| 5. The snake cannot directly reverse its current movement direction. | No | `Snake.cpp:261` — `_getch()` called in `Input()`, meaning there is no way to supply simulated movement keys programmatically. |

**How many of five could you test without changing the source?**  
**0**

Because all the game logic is tightly coupled with console I/O (`_getch()`, `cin`), non-deterministic functions (`rand()`), and because all internal state (`score`, `snake`, `dir`) is strictly `private` with no getter methods, it is completely impossible to write automated assertions for this codebase without refactoring it first.

## Part C: Coverage (30 min)

**Line %:** 0%  
**Branch %:** 0%  
*(Since 0 tests could be written without refactoring, we have a 0% baseline. Any higher number here would be propped up by "tests that assert nothing".)*

**Which of your five rules runs during a test but is never checked by one?**  
Since no tests could be written, technically no rules run. However, if we bypassed `_getch()` to write a test for eating a fruit (Rule 3), the game would still evaluate bounds and collisions on every movement.

- **Rule:** The snake dies if its head moves out of the bounds of the game board (Rule 1).
- **Line:** `Snake.cpp:121` (`if (p.x<0 || p.x>=WIDTH || p.y<0 || p.y>=HEIGHT) return true;`) inside `CheckCollision()`.
- **Missing assertion:** When testing fruit eating, this line runs continuously to verify the snake is safe, but the test suite would be missing an `assert(!game.IsGameOver())` or `EXPECT_FALSE(game.IsGameOver())` on every single tick, completely taking it for granted.
## Part D: Refactoring and Testing (Rule 3)

I chose to make **Rule 3** testable: *Eating a regular fruit increases score by 10 and length by one.*
To achieve this, I broke the dependency on and() and _getch() by introducing two seams: IRandomSource and IInputSource. These interfaces are injected into the constructor of SnakeGame, allowing my test file to use a DeterministicFakeRandomSource and FakeInputSource.

| Commit SHA | Commit Message | Files Modified | Nature of Changes |
| :--- | :--- | :--- | :--- |
| **`df9ca40`** | `refactor: add seams to bypass cin and rand for rule 3` | `Snake.cpp` | **Production seam only.** Added `IRandomSource` and `IInputSource`, injected them, and made state public. No tests added. |
| **`2299f3b`** | `test: add unit test for rule 3` | `test.cpp`, `lab4/REPORT.md` | **Tests only.** Unit tests asserting score and length increases. No production source changes. |

