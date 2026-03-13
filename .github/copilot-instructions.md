# Copilot Instructions

## General Guidelines
- First general instruction
- Second general instruction
- Avoid baseless assertions, stopgap measures, and evasions; seek permanent solutions that are valid as a TSF implementation.

## Code Style
- Use specific formatting rules
- Follow naming conventions

## Project-Specific Rules
- The user's project is based on a TSF implementation of SKK IME, utilizing Q-ELF input method.
- Maintain a feature to visualize the current state during Q-ELF input (conversion in progress) using a dedicated window class (e.g., CStateWindow) rather than reusing the candidate window.
- Prioritize displaying detailed information about the Q-ELF input state (multiple statuses) in the CStateWindow.
- Simplify the state display without increasing character count, using symbols for clarity (e.g., Shift=Åü, Youon=Åû, Mode=J). Ensure that a legend explaining each symbol is provided to prioritize clarity for the user.
- Format the state display as labeled indicators: `Shift:Å†/Åü`, `Youon:Å†/Å°`, `Sokuon:Å†/Å°`, `Mode:JÇ–Å©1`.