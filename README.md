# Dots and Boxes
For a game of size M x N (M rows and N columns), line IDs will be calculated as follows:
- Horizontal lines: from 0 to (m + 1) * n - 1, indexed from left to right, from top to bottom.
- Vertical lines: from (m + 1) * n to (m + 1) * n + m * (n + 1), indexed from left to right, from top to bottom.
